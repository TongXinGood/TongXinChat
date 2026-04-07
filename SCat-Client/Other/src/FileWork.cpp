#include "../include/FileWork.h"
#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QApplication>

// 定义分片大小：32KB (既保证速度又不会阻塞网络)
const qint64 CHUNK_SIZE = 32 * 1024;

FileWork::FileWork(QObject* parent)
    : QObject(parent)
    , networkManager(nullptr)
    , sendFileObj(nullptr)
    , sendTotalSize(0)
    , sendCurrentSize(0)
    , transferTimer(new QTimer(this))
    , recvFileObj(nullptr)
    , recvTotalSize(0)
    , recvCurrentSize(0)
{
    // 配置定时器，每20ms触发一次发送，给UI留出喘息时间
    connect(transferTimer, &QTimer::timeout, this, &FileWork::sendNextChunk);
}

FileWork::~FileWork()
{
    // 安全清理发送文件对象
    if (sendFileObj) {
        if (sendFileObj->isOpen()) sendFileObj->close();
        delete sendFileObj;
    }
    // 安全清理接收文件对象
    if (recvFileObj) {
        if (recvFileObj->isOpen()) recvFileObj->close();
        delete recvFileObj;
    }
}

void FileWork::setNetworkManager(NetWorkManager* manager)
{
    this->networkManager = manager;
    // 监听网络数据，如果有数据包过来，交给 onDataReceived 处理
    if (networkManager) {
        connect(networkManager, &NetWorkManager::dataReceived, this, &FileWork::onDataReceived);
    }
}

// 1. 打开文件选择框
QString FileWork::selectFile()
{
    // 打开 Windows 文件选择窗口
    QString path = QFileDialog::getOpenFileName(nullptr, "选择发送文件", QDir::homePath());
    return path;
}

// 2. 开始发送文件
bool FileWork::sendFile(const QString& filePath, const QString& receiverAccount)
{
    if (!networkManager || !networkManager->isConnected()) {
        emit fileError("", "未连接服务器，无法发送文件");
        return false;
    }

    // 清理上一次可能没发完的残留对象
    if (sendFileObj) {
        delete sendFileObj;
        sendFileObj = nullptr;
    }

    sendFileObj = new QFile(filePath);
    if (!sendFileObj->open(QIODevice::ReadOnly)) {
        emit fileError(filePath, "无法读取文件，请检查权限");
        return false;
    }

    // 初始化发送状态
    sendFileName = QFileInfo(filePath).fileName();
    targetAccount = receiverAccount;
    sendTotalSize = sendFileObj->size();
    sendCurrentSize = 0;

    // --- 第一步：发送文件头 (FILE_INFO) ---
    QJsonObject json;
    json["fileName"] = sendFileName;
    json["fileSize"] = sendTotalSize;
    json["receiver"] = targetAccount; // 告诉服务器发给谁

    // 序列化 JSON -> 字节流 -> 通过 NetWorkManager 发送
    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);
    networkManager->sendRequest("FILE_INFO", data);

    qDebug() << "[File] 开始发送文件:" << sendFileName << "大小:" << formatFileSize(sendTotalSize);

    // --- 第二步：启动定时器，开始切片发送 ---
    transferTimer->start(20);

    return true;
}

// 定时器槽函数：发送下一个分片
void FileWork::sendNextChunk()
{
    // 如果文件没打开，或者已经读到末尾
    if (!sendFileObj || sendFileObj->atEnd()) {
        transferTimer->stop();
        if (sendFileObj) {
            sendFileObj->close();
            delete sendFileObj;
            sendFileObj = nullptr;
        }
        // 发送完毕信号
        emit fileSuccess(sendFileName, true); // true 表示我是发送者
        qDebug() << "[File] 文件发送完毕";
        return;
    }

    // 1. 读取一块数据 (32KB)
    QByteArray chunk = sendFileObj->read(CHUNK_SIZE);
    sendCurrentSize += chunk.size();

    // 2. 【关键】Base64 编码 (防止二进制数据在 JSON 中乱码)
    QString base64Str = chunk.toBase64();

    // 3. 组装数据包 (FILE_DATA)
    QJsonObject json;
    json["content"] = base64Str;
    json["receiver"] = targetAccount;
    // 也可以加一个 currentSize 字段用于校验，这里从简

    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);

    // 4. 发送
    networkManager->sendRequest("FILE_DATA", data);

    // 5. 更新 UI 进度条
    emit fileProgress(sendFileName, sendCurrentSize, sendTotalSize, true);
}

// 处理收到的网络数据
void FileWork::onDataReceived(const QByteArray& data)
{
    // 解析最外层协议 (参考 NetWorkManager 协议)
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) return;

    QJsonObject root = doc.object();
    QString type = root["type"].toString();

    // 取出内部数据 (注意 NetWorkManager 把 data 存成了 String，所以要二次解析)
    QJsonObject contentJson;
    if (root["data"].isString()) {
        contentJson = QJsonDocument::fromJson(root["data"].toString().toUtf8()).object();
    }
    else {
        contentJson = root["data"].toObject();
    }

    // 分发处理
    if (type == "FILE_INFO") {
        handleFileInfo(contentJson);
    }
    else if (type == "FILE_DATA") {
        handleFileData(contentJson);
    }
}

// 处理：收到文件头通知
void FileWork::handleFileInfo(const QJsonObject& json)
{
    recvFileName = json["fileName"].toString();
    recvTotalSize = json["fileSize"].toVariant().toLongLong();
    sourceAccount = json["sender"].toString(); // 假设服务器转发时补上了发送者ID
    recvCurrentSize = 0;

    // 准备保存目录 (默认保存在 exe 同级目录下的 ReceivedFiles)
    QString saveDir = QCoreApplication::applicationDirPath() + "/ReceivedFiles";
    QDir dir;
    if (!dir.exists(saveDir)) {
        dir.mkpath(saveDir);
    }

    QString savePath = saveDir + "/" + recvFileName;

    // 清理旧对象
    if (recvFileObj) {
        if (recvFileObj->isOpen()) recvFileObj->close();
        delete recvFileObj;
    }

    // 打开文件准备写入
    recvFileObj = new QFile(savePath);
    if (!recvFileObj->open(QIODevice::WriteOnly)) {
        qDebug() << "[File] 无法写入文件:" << savePath;
        emit fileError(recvFileName, "无法保存文件");
        return;
    }

    qDebug() << "[File] 准备接收文件:" << recvFileName << "来自:" << sourceAccount;
}

// 处理：收到文件数据块
void FileWork::handleFileData(const QJsonObject& json)
{
    if (!recvFileObj || !recvFileObj->isOpen()) return;

    // 1. 获取 Base64 字符串
    QString base64Str = json["content"].toString();

    // 2. 【关键】Base64 解码 -> 还原为二进制
    QByteArray rawData = QByteArray::fromBase64(base64Str.toLatin1());

    // 3. 写入硬盘
    recvFileObj->write(rawData);
    recvCurrentSize += rawData.size();

    // 4. 更新 UI 进度
    emit fileProgress(recvFileName, recvCurrentSize, recvTotalSize, false); // false 表示我是接收者

    // 5. 判断是否接收完毕
    if (recvCurrentSize >= recvTotalSize) {
        recvFileObj->close();
        delete recvFileObj;
        recvFileObj = nullptr;

        emit fileSuccess(recvFileName, false);
        qDebug() << "[File] 文件接收完毕";
    }
}

// 工具：格式化文件大小
QString FileWork::formatFileSize(qint64 size)
{
    if (size < 1024)
        return QString("%1 B").arg(size);
    else if (size < 1024 * 1024)
        return QString::number(size / 1024.0, 'f', 1) + " KB";
    else
        return QString::number(size / (1024.0 * 1024.0), 'f', 1) + " MB";
}