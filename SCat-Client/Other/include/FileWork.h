#ifndef FILEWORK_H
#define FILEWORK_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include "../../NetWork/include/NetWorkManager.h"

class FileWork : public QObject
{
    Q_OBJECT

public:
    explicit FileWork(QObject* parent = nullptr);
    ~FileWork();
    void setNetworkManager(NetWorkManager* manager);
    // --- 发送端接口 ---
    // 1. 打开文件选择框，返回路径
    QString selectFile();
    // 2. 开始发送文件
    bool sendFile(const QString& filePath, const QString& receiverAccount);

signals:
    // --- 信号 ---
    // isSender: true=我是发送方, false=我是接收方
    void fileProgress(const QString& fileName, qint64 current, qint64 total, bool isSender);
    void fileSuccess(const QString& fileName, bool isSender);
    void fileError(const QString& fileName, const QString& error);

public slots:
    // 处理网络层传来的数据
    void onDataReceived(const QByteArray& data);

private slots:
    // 内部槽：发送下一个分片
    void sendNextChunk();

private:
    // --- 核心逻辑 ---
    void handleFileInfo(const QJsonObject& json); // 处理文件头
    void handleFileData(const QJsonObject& json); // 处理文件体

    // 工具函数：把字节数转为易读字符串 (如 "5.2 MB")
    QString formatFileSize(qint64 size);

private:
    NetWorkManager* networkManager;

    // --- 发送端状态变量 ---
    QFile* sendFileObj;          // 当前读取的文件对象
    QString sendFileName;        // 文件名
    QString targetAccount;       // 接收者账号
    qint64 sendTotalSize;        // 总大小
    qint64 sendCurrentSize;      // 已发送大小
    QTimer* transferTimer;       // 发送定时器

    // --- 接收端状态变量 ---
    QFile* recvFileObj;          // 当前写入的文件对象
    QString recvFileName;        // 文件名
    QString sourceAccount;       // 发送者账号
    qint64 recvTotalSize;        // 总大小
    qint64 recvCurrentSize;      // 已接收大小
};

#endif // FILEWORK_H