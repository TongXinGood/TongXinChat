#ifndef LOGIN_H
#define LOGIN_H

#include <QObject>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <QJsonObject>    
#include <QJsonDocument>
#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>
#include "../include/LoginWindow.h"
#include "../../NetWork/include/NetWorkManager.h"

class Login : public QObject
{
    Q_OBJECT

public:
    explicit Login(QObject* parent = nullptr);
    ~Login();

    void setLoginWindow(LoginWindow* loginWindow);
    void setNetworkManager(NetWorkManager* Manager);

    bool connectToServer(const QString& serverIP, quint16 port);
    void disconnectFromServer();

signals:
    void loginSuccess(const QString& account);
    void loginFailed(const QString& errorMessage);
    void networkError(const QString& error);
    void showRgeisterWindow();
    void showSCatWindow();

public slots:
    void handleLoginRequest(const QString& account, const QString& password);
    void onDataReceived(const QByteArray& data);
    void onRegisterButtonClicked();
    void handleForgotPwdRequest();

private:
    void sendLoginRequest(const QString& account, const QString& password);
    void processServerResponse(const QByteArray& response);

    LoginWindow* loginWindow;
    NetWorkManager* networkManager;
};

#endif