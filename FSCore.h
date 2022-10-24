#ifndef FSCORE_H
#define FSCORE_H

#include <QTimer>
#include <QVector>
#include <QMap>
#include "UdpConnection.h"
#include "TcpConnection.h"
#include "FileDownloading.h"

class FSCore : public QObject {
    Q_OBJECT
public:
    explicit FSCore(QObject *parent = 0);
    ~FSCore();
    void init(int port, int tcp_port, int file_loading_port, int connection_timeout);

private:
    UdpConnection connector;
    TcpConnection tcp_connector;
    FileDownloading *file_loading_manager;
    FSUdpData send_data;
    QVector<ConnectionListData> connection_list;
    QMap<QString, FileData> filedata;

    QList<FileData> downloading_filenames;
    int downloadingFileIndex;
    QString current_connection_IP;

    QTimer sending_timer;
    QTimer connection_timer; // таймер обновления списка подключений

    int port, tcp_port, file_loading_port;
    int connection_timeout; // таймаут элемента в списке

public:
    QVector<ConnectionListData> getConnectionList() {return connection_list;}

protected slots:
    void getDescription(QString serverIP, QString password);
    void sendDescription(QHostAddress serverAddress);
    void readDescription(FSDescriptionData read_data);
    void readDescriptionError(QString error);

public slots:
    void startSendFileData(int interval);
    void stopSendFileData();
    void sendFileData();
    void updateFileData(FSUdpData send_data) {this->send_data = send_data;}

    void startConnectionTimer(int interval);
    void stopConnectionTimer();
    void updateConnectionList(FSUdpData read_data);
    void timeoutConnectionList();

    bool checkIndexPassword(int row);
    QString getCurrentConnectionIP() {return current_connection_IP;}
    void getDescriptionByIndex(int row, QString password);
    void updateDescriptionData(FSDescriptionData description_data) {this->tcp_connector.setSendData(description_data);}
    void setSendingState(bool state) {this->tcp_connector.setSendingState(state);}
    void setLocalPassword(QString local_password) {this->tcp_connector.setLocalPassword(local_password);}

    void addFiledata(QString key, FileData fdata);
    void deleteFiledata(QString key);
    void deleteFiledataByIndex(int index);
    void updateEnableFiles();
    QList<FileData> getAllFiledata();
    FileData getFiledata(QString key) {return this->filedata[key];}
    FileData getFiledataByIndex(int index) {return this->filedata[this->filedata.keys()[index]];}

    void downloadFiles(const QString &serverIP, const QList<FileData> &filenames, const QString &path);
    void downloadNextFile();
    void outputDebug(QString local_filename, float percent);

signals:
    void readyUpdateConnections(int action, int index = -1);

    void readyDescriptionRequest();
    void readyDescription(FSDescriptionData);
    void readyDescriptionError(QString);
};

#endif // FSCORE_H
