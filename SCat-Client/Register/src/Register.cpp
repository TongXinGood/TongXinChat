#include "../include/Register.h"

Register::Register(QObject* parent) :QObject(parent)
, registerWindow(nullptr)
, networkManager(nullptr)
{
}

Register::~Register()
{
}

void Register::setRegisterWindow(RegisterWindow* Window)
{
    this->registerWindow = Window;
    if (registerWindow) {
        connect(registerWindow, &RegisterWindow::sendRegisterClicked, this, &Register::handleRegisterRequest);
        connect(registerWindow, &RegisterWindow::sendReturnClicked,this, &Register::onReturnClicked);
    }
}

void Register::setNetworkManager(NetWorkManager* manager)
{
    this->networkManager = manager;
    if (networkManager) {
        connect(networkManager, &NetWorkManager::dataReceived,this, &Register::onDataReceived);
    }
}

bool Register::connectToServer(const QString& serverIP, quint16 port)
{
    if (!networkManager) {
        return false;
    }
    return networkManager->connectToServer(serverIP, port);
}

void Register::disconnectFromServer()
{
    if (networkManager) {
        networkManager->disconnectFromServer();
    }
}

void Register::handleRegisterRequest(const QString& account, const QString& pwd)
{
    if (!networkManager) {
        return;
    }

    if (!networkManager->isConnected()) {
        qDebug() << "Register: 检测到未连接，准备读取配置文件...";

        // 1. 获取路径
        QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";

        // 2. 读取配置
        QSettings settings(configPath, QSettings::IniFormat);
        QString serverIP = settings.value("Server/IP", "127.0.0.1").toString();
        int serverPort = settings.value("Server/Port", 8888).toInt();
        qDebug() << "Register: 读取配置连接 ->" << serverIP;
        networkManager->connectToServer(serverIP, serverPort);
        QMessageBox::information(registerWindow, "提示", "正在连接服务器，请连接成功后重试...");
        return;
    }
    sendRegisterRequest(account, pwd);
}

void Register::onReturnClicked()
{
    emit goBackToLogin();
}

void Register::onDataReceived(const QByteArray& data)
{
    processServerResponse(data);
}

void Register::sendRegisterRequest(const QString& account, const QString& pwd)
{
    QJsonObject dataObj;
    dataObj["account"] = account;
    dataObj["password"] = pwd; 
    QByteArray jsonData = QJsonDocument(dataObj).toJson(QJsonDocument::Compact);
    bool ret = networkManager->sendRequest("REGISTER", jsonData);
    if (ret) {
        qDebug() << "注册请求已发送 -> 账号:" << account;
    }
    else {
        QMessageBox::warning(registerWindow, "错误", "发送数据失败，请检查网络连接");
    }
}

void Register::processServerResponse(const QByteArray& data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return;
    }
    QJsonObject root = doc.object();
    QString type = root["type"].toString();
    if (type == "REGISTER_RES") {

        // 解析 data 部分 (完全复用 Login.cpp 的鲁棒性写法)
        QJsonObject resData;
        if (root["data"].isString()) {
            QString dataStr = root["data"].toString();
            resData = QJsonDocument::fromJson(dataStr.toUtf8()).object();
        }
        else {
            resData = root["data"].toObject();
        }
        bool isSuccess = resData["success"].toBool();
        QString msg = resData["msg"].toString();
        if (isSuccess) {
            QString newAccount = resData["account"].toString();
            qDebug() << "注册成功! 账号:" << newAccount;
            QMessageBox::information(registerWindow, "成功", "注册成功！\n请使用新账号登录。");
            emit registerSuccess(newAccount);
            onReturnClicked();
        }
        else {
            qDebug() << "注册失败:" << msg;
            QMessageBox::critical(registerWindow, "注册失败", msg);
            emit registerFailed(msg);
        }
    }
}



