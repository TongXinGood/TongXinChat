#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>

class NetWorkManager : public QObject
{
	Q_OBJECT
public:
	explicit NetWorkManager(QObject* parent = nullptr);
	~NetWorkManager();
	//用于连接服务器
	bool connectToServer(const QString& serverIP, quint16 port);
	void disconnectFromServer();
	bool isConnected() const; //判断是否连接服务器

	bool sendData(const QByteArray& data); //发送数据
	bool sendRequest(const QString& requestType, const QByteArray& data); //发送请求类型，方便服务器判断

protected:
    QTcpSocket* getSocket() const;

signals:
    void connectedsuccess();
    void disconnected();
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onHeartbeatTimeout();

private:
    void startHeartbeat();
    void stopHeartbeat();
    void sendHeartbeat();

    QTcpSocket* Tsocket;
    QString TserverIP;
    quint16 TserverPort;
    bool isConnect;
    QTimer* TheartbeatTimer;
    int TheartbeatInterval;
    QByteArray Hbuffer;

};

#endif