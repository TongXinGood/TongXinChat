#include "../include/NetWorkManager.h"

NetWorkManager::NetWorkManager(QObject* parent):QObject(parent),
	Tsocket(new QTcpSocket(this))
    ,TserverPort(0)
	,isConnect(false)
	,TheartbeatTimer(new QTimer(this))
    ,TheartbeatInterval(5000)
{
	connect(Tsocket, &QTcpSocket::connected, this, &NetWorkManager::onConnected);
	connect(Tsocket, &QTcpSocket::disconnected, this, &NetWorkManager::onDisconnected);
	connect(Tsocket, &QTcpSocket::readyRead, this, &NetWorkManager::onReadyRead);
	connect(Tsocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),this, &NetWorkManager::onErrorOccurred);
	connect(TheartbeatTimer, &QTimer::timeout, this, &NetWorkManager::onHeartbeatTimeout);
}

NetWorkManager::~NetWorkManager()
{
	disconnectFromServer();
}

bool NetWorkManager::connectToServer(const QString& serverIP, quint16 port)
{
	if (Tsocket->state() == QAbstractSocket::ConnectedState) return true;

	Hbuffer.clear(); 
	TserverIP = serverIP;
	TserverPort = port;

	qDebug() << "[Net] 正在连接服务器:" << serverIP << port;
	Tsocket->connectToHost(serverIP, port);
	return true;
}

void NetWorkManager::disconnectFromServer()
{
	Tsocket->disconnectFromHost();
	Tsocket->close();
	stopHeartbeat();
}

bool NetWorkManager::isConnected() const
{
	return Tsocket->state() == QAbstractSocket::ConnectedState;
}

bool NetWorkManager::sendData(const QByteArray& data)
{
	if (!isConnected()) {
		return false;
	}
	qint64 len = Tsocket->write(data);
	return len != -1;
}

bool NetWorkManager::sendRequest(const QString& requestType, const QByteArray& data)
{
	if (!isConnected()) return false;

	// 1. 构造 JSON 包
	QJsonObject root;
	root["type"] = requestType;
	// 将二进制数据转为字符串放入 JSON (假设是UTF8文本)
	// 这样服务端解析时，取出来也是个字符串，再转JSON即可
	root["data"] = QString::fromUtf8(data);
	root["timestamp"] = QDateTime::currentMSecsSinceEpoch();

	QByteArray jsonBody = QJsonDocument(root).toJson(QJsonDocument::Compact);

	// 2. 添加包头 (4字节长度) 防止粘包
	QByteArray packet;
	QDataStream out(&packet, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);

	out << (quint32)0;
	out.writeRawData(jsonBody.data(), jsonBody.size());
	out.device()->seek(0);
	out << (quint32)(packet.size() - sizeof(quint32));

	// 3. 发送
	return sendData(packet);
}

QTcpSocket* NetWorkManager::getSocket() const
{
	return Tsocket;
}

void NetWorkManager::onConnected()
{
	isConnect = true;
	qDebug() << "[Net] 连接成功";
	emit connectedsuccess();
	startHeartbeat();
}

void NetWorkManager::onDisconnected()
{
	isConnect = false;
	qDebug() << "[Net] 连接断开";
	emit disconnected();
	stopHeartbeat();
}

void NetWorkManager::onReadyRead()
{
	Hbuffer.append(Tsocket->readAll());

	while (true) {
		// 2. 如果数据连个包头(4字节)都不够，跳出等待
		if (Hbuffer.size() < (int)sizeof(quint32)) {
			break;
		}

		// 3. 读取包头里的长度
		QDataStream stream(Hbuffer);
		stream.setVersion(QDataStream::Qt_5_0);
		quint32 packetSize = 0;
		stream >> packetSize;

		// 4. 如果缓冲区数据不够完整的一个包，跳出等待
		if ((quint32)Hbuffer.size() < sizeof(quint32) + packetSize) {
			break;
		}

		// 5. 提取完整的数据包 (跳过头部的4字节)
		QByteArray realData = Hbuffer.mid(sizeof(quint32), packetSize);

		// 6. 从缓冲区移除已处理的数据
		Hbuffer = Hbuffer.mid(sizeof(quint32) + packetSize);

		// 7. 发送信号给上层
		emit dataReceived(realData);
	}
}

void NetWorkManager::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
	Q_UNUSED(socketError);
	QString err = Tsocket->errorString();
	qDebug() << "[Net] 网络错误:" << err;
	emit errorOccurred(err);
}

void NetWorkManager::onHeartbeatTimeout()
{
	sendHeartbeat();
}

void NetWorkManager::startHeartbeat()
{
	if (!TheartbeatTimer->isActive()) {
		TheartbeatTimer->start(TheartbeatInterval);
	}
}

void NetWorkManager::stopHeartbeat()
{
	TheartbeatTimer->stop();
}

void NetWorkManager::sendHeartbeat()
{
	QByteArray emptyData;
	sendRequest("HEARTBEAT", emptyData);
}
