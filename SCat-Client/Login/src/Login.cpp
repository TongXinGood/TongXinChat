#include "../include/Login.h"

Login::Login(QObject* parent) :QObject(parent)
	, loginWindow(nullptr)
	,networkManager(nullptr)
{
}

Login::~Login()
{
}

void Login::setLoginWindow(LoginWindow* loginWindow)
{
	this->loginWindow = loginWindow;
	connect(loginWindow, &LoginWindow::sendLoginClicked,this, &Login::handleLoginRequest);
	connect(loginWindow, &LoginWindow::sendRegisterClicked,this, &Login::onRegisterButtonClicked);
	connect(loginWindow, &LoginWindow::sendForgotpwdClicked,this, &Login::handleForgotPwdRequest);
}

void Login::setNetworkManager(NetWorkManager* Manager)
{
	this->networkManager = Manager;
	connect(networkManager, &NetWorkManager::dataReceived,this, &Login::onDataReceived);

}

bool Login::connectToServer(const QString& serverIP, quint16 port)
{
	if (!networkManager) {
		return false;
	}
	return networkManager->connectToServer(serverIP, port);
}

void Login::disconnectFromServer()
{
	if (networkManager) {
		networkManager->disconnectFromServer();
	}
}

void Login::handleLoginRequest(const QString& account, const QString& password)
{
	if (!networkManager) {
		return;
	}

	// 如果还没有连接，就去读取配置文件并连接
	if (!networkManager->isConnected()) {
		qDebug() << "Login: 检测到未连接，准备读取配置文件...";
		QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
		if (!QFileInfo::exists(configPath)) {
			QMessageBox::critical(loginWindow, "错误", "找不到配置文件 config.ini\n请确保它和exe在同一目录！");
			return;
		}
		QSettings settings(configPath, QSettings::IniFormat);
		QString serverIP = settings.value("Server/IP", "127.0.0.1").toString();
		int serverPort = settings.value("Server/Port", 8888).toInt();

		qDebug() << "Login: 读取到服务器配置 ->" << serverIP << ":" << serverPort;
		networkManager->connectToServer(serverIP, serverPort);

		QMessageBox::information(loginWindow, "提示",
			QString("正在连接服务器 (%1)...\n如果卡住，请检查IP配置是否正确。").arg(serverIP));
		return;
	}
	sendLoginRequest(account, password);
}

void Login::onDataReceived(const QByteArray& data)
{
	processServerResponse(data);
}

void Login::onRegisterButtonClicked()
{
	emit showRgeisterWindow();
}

void Login::handleForgotPwdRequest()
{
	QMessageBox::information(
		loginWindow,  
		"忘记密码",   
		"请联系系统管理员处理密码重置问题\n",
		QMessageBox::Ok  
	);
}

void Login::sendLoginRequest(const QString& account, const QString& password)
{
	QJsonObject dataObj;
	dataObj["account"] = account;
	dataObj["password"] = password;

	// 2. 转成二进制数据
	QByteArray jsonData = QJsonDocument(dataObj).toJson(QJsonDocument::Compact);
	bool ret = networkManager->sendRequest("LOGIN", jsonData);

	if (ret) {
		qDebug() << "登录请求已发送 -> 账号:" << account;
	}
	else {
		QMessageBox::warning(loginWindow, "错误", "发送数据失败，请检查网络连接");
	}
}

void Login::processServerResponse(const QByteArray& response)
{
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(response, &error);

	if (error.error != QJsonParseError::NoError) {
		qDebug() << "JSON 解析错误:" << error.errorString();
		return;
	}

	QJsonObject root = doc.object();
	QString type = root["type"].toString();

	// 区分不同的回复类型
	if (type == "LOGIN_RES") {

		// 解析内层 data
		QJsonObject resData;
		if (root["data"].isString()) {
			// 如果服务端把 data 存成了字符串
			QString dataStr = root["data"].toString();
			resData = QJsonDocument::fromJson(dataStr.toUtf8()).object();
		}
		else {
			// 如果服务端直接存的是对象
			resData = root["data"].toObject();
		}

		bool isSuccess = resData["success"].toBool();
		QString msg = resData["msg"].toString();

		if (isSuccess) {
			qDebug() << "登录成功！UID:" << resData["uid"].toString();

			// 发送信号给 MainControl，切换到主聊天窗口
			emit loginSuccess(resData["account"].toString());

			// 隐藏登录窗口
			if (loginWindow) loginWindow->hide();
		}else {
			qDebug() << "登录失败:" << msg;
			QMessageBox::critical(loginWindow, "登录失败", msg);
		}
	}
	else if (type == "HEARTBEAT") {
	}
}
