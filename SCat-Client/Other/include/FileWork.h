#ifndef FILEWORK_H
#define FILEWORK_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QFileInfo>
#include "../../NetWork/include/NetworkManager.h"

class FileWork : public QObject
{
    Q_OBJECT

public:
    explicit FileWork(QObject* parent = nullptr);
    ~FileWork();

    void setNetworkManager(NetWorkManager* networkManager);

    bool sendFile(const QString& filePath, const QString& receiverAccount);
    bool receiveFile(const QByteArray& fileData, const QString& fileName, const QString& savePath);
    QString selectFile();
    QString getFileName(const QString& filePath);
    qint64 getFileSize(const QString& filePath);

signals:
    void fileSendProgress(qint64 bytesSent, qint64 bytesTotal);
    void fileSendSuccess(const QString& fileName);
    void fileSendFailed(const QString& fileName, const QString& error);

    void fileReceiveProgress(qint64 bytesReceived, qint64 bytesTotal);
    void fileReceiveSuccess(const QString& fileName);
    void fileReceiveFailed(const QString& fileName, const QString& error);

public slots:
    void onDataReceived(const QByteArray& data);

private:
    QByteArray readFile(const QString& filePath);
    bool writeFile(const QByteArray& data, const QString& savePath);
    void processFileTransferData(const QByteArray& data);

    NetWorkManager* networkManager;
    QString currentFilePath;
    qint64 fileSize;
    qint64 bytesTransferred;
};

#endif
