#include "../include/FileWork.h"

FileWork::FileWork(QObject* parent)
{
}

FileWork::~FileWork()
{
}

void FileWork::setNetworkManager(NetWorkManager* networkManager)
{
}

bool FileWork::sendFile(const QString& filePath, const QString& receiverAccount)
{
	return false;
}

bool FileWork::receiveFile(const QByteArray& fileData, const QString& fileName, const QString& savePath)
{
	return false;
}

QString FileWork::selectFile()
{
	return QString();
}

QString FileWork::getFileName(const QString& filePath)
{
	return QString();
}

qint64 FileWork::getFileSize(const QString& filePath)
{
	return qint64();
}

void FileWork::onDataReceived(const QByteArray& data)
{

}

QByteArray FileWork::readFile(const QString& filePath)
{
	return QByteArray();
}

bool FileWork::writeFile(const QByteArray& data, const QString& savePath)
{
	return false;
}

void FileWork::processFileTransferData(const QByteArray& data)
{
}
