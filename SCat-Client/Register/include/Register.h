#ifndef REGISTER_H
#define REGISTER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>
#include "RegisterWindow.h"
#include "../../NetWork/include/NetWorkManager.h"

class Register : public QObject
{
    Q_OBJECT

public:
    explicit Register(QObject* parent = nullptr);
    ~Register();

    // 依赖注入
    void setRegisterWindow(RegisterWindow* Window);
    void setNetworkManager(NetWorkManager* manager);

    // 接口封装
    bool connectToServer(const QString& serverIP, quint16 port);
    void disconnectFromServer();

signals:
    // 注册成功，返回账号（用于自动填充登录界面）
    void registerSuccess(const QString& account);
    // 注册失败
    void registerFailed(const QString& errorMessage);
    // 返回登录界面
    void goBackToLogin();

public slots:
    void handleRegisterRequest(const QString& account, const QString& pwd);
    void onReturnClicked();
    void onDataReceived(const QByteArray& data);
   

private:
    void sendRegisterRequest(const QString& account, const QString& pwd);
    void processServerResponse(const QByteArray& data);

private:
    RegisterWindow* registerWindow; // View
    NetWorkManager* networkManager; // Model/Service
};

#endif // REGISTER_H