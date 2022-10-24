#include "FSCore.h"

FSCore::FSCore(QObject *parent) : QObject(parent) {
    file_loading_manager = new FileDownloading();
    connect(&sending_timer, SIGNAL(timeout()), this, SLOT(sendFileData()));
    connect(&connection_timer, SIGNAL(timeout()), this, SLOT(timeoutConnectionList()));
    connect(&connector, SIGNAL(readyRead(FSUdpData)), this, SLOT(updateConnectionList(FSUdpData)));
    connect(&tcp_connector, SIGNAL(readyReadDescriptionRequest(QHostAddress)), this, SLOT(sendDescription(QHostAddress)));
    connect(&tcp_connector, SIGNAL(readyReadDescription(FSDescriptionData)), this, SLOT(readDescription(FSDescriptionData)));
    connect(&tcp_connector, SIGNAL(readyReadDescriptionError(QString)), this, SLOT(readDescriptionError(QString)));
    connect(file_loading_manager, SIGNAL(connected()), this, SLOT(downloadNextFile()));
    connect(file_loading_manager, SIGNAL(downloadingSucceed()), this, SLOT(downloadNextFile()));
}

FSCore::~FSCore() {

}

void FSCore::init(int port, int tcp_port, int file_loading_port, int connection_timeout) {
    connector.init(port);
    this->port = port;
    this->connection_timeout = connection_timeout;

    tcp_connector.init(tcp_port);
    this->tcp_port = tcp_port;

    file_loading_manager->init(file_loading_port);
    this->file_loading_port = file_loading_port;
}

void FSCore::startSendFileData(int interval) {
    sending_timer.start(interval);
}

void FSCore::stopSendFileData() {
    sending_timer.stop();
}

void FSCore::sendFileData() {
    connector.send(send_data);
}

void FSCore::startConnectionTimer(int interval) {
    connection_timer.start(interval);
}

void FSCore::stopConnectionTimer() {
    connection_timer.stop();
}

void FSCore::updateConnectionList(FSUdpData read_data) {
    bool is_new = 1;
    ConnectionListData newData;
    for (int i = 0; i < connection_list.size(); i++) {
        if (connection_list[i].data.address.toString() == read_data.address.toString()) {
            is_new = 0;
            connection_list[i].timeout = connection_timeout;
            connection_list[i].data = read_data;
            emit readyUpdateConnections(2, i);
        }
    }
    if (is_new) {
        newData.data = read_data;
        newData.timeout = connection_timeout;
        connection_list.push_back(newData);
        emit readyUpdateConnections(1, connection_list.count() - 1);
    }
    emit readyUpdateConnections(3, -1);
}

void FSCore::timeoutConnectionList() {
    for (int i = 0; i < connection_list.size(); i++) {
        connection_list[i].timeout -= connection_timer.interval();
        //qDebug() << connection_list[i].timeout;
        if (connection_list[i].timeout <= 0) {
            connection_list.remove(i, 1);
            emit readyUpdateConnections(0, i);
        }
    }
}

bool FSCore::checkIndexPassword(int row) {
    if (!connection_list[row].data.is_locked) {
        return false;
    }
    else {
        return true;
    }
}

void FSCore::getDescriptionByIndex(int row, QString password) {
    if (connection_list.count() > row) {
        QString serverIP = connection_list[row].data.address.toString();
        getDescription(serverIP, password);
    }
}

void FSCore::addFiledata(QString key, FileData fdata) {
    this->filedata.insert(key, fdata);
}

void FSCore::deleteFiledata(QString key) {
    this->filedata.remove(key);
}

void FSCore::deleteFiledataByIndex(int index) {
    this->filedata.remove(filedata.keys()[index]);
}

void FSCore::updateEnableFiles() {
    QStringList enable_files;
    foreach(FileData fdata, this->filedata) {
        enable_files.append(fdata.full_filename);
    }
    file_loading_manager->setEnableFiles(enable_files);
}

QList<FileData> FSCore::getAllFiledata() {
    QList<FileData> list_filedata;
    for (int i = 0; i < filedata.count(); i++) {
        list_filedata.append(filedata[filedata.keys()[i]]);
    }
    return list_filedata;
}

void FSCore::downloadFiles(const QString &serverIP, const QList<FileData> &filenames, const QString &path) {
    downloadingFileIndex = -1;
    file_loading_manager->setPath(path);
    this->downloading_filenames = filenames;
    file_loading_manager->connectToHost(serverIP);
}

void FSCore::downloadNextFile() {
    downloadingFileIndex++;
    if (downloadingFileIndex < downloading_filenames.count())
        file_loading_manager->downloadFile(downloading_filenames[downloadingFileIndex].full_filename);
}

void FSCore::outputDebug(QString local_filename, float percent) {
    qDebug() << "Filename: " << local_filename << "Percent: " << percent;
}

//protected

void FSCore::getDescription(QString serverIP, QString password) {
    this->tcp_connector.setSendPassword(password);
    this->tcp_connector.sendDescriptionRequest(serverIP);
}

void FSCore::sendDescription(QHostAddress serverAddress) {
    this->tcp_connector.sendDescription(serverAddress.toString());
    emit readyDescriptionRequest();
}

void FSCore::readDescription(FSDescriptionData read_data) {
    this->current_connection_IP = read_data.address.toString();
    emit readyDescription(read_data);
}

void FSCore::readDescriptionError(QString error) {
    emit readyDescriptionError(error);
}
