#ifndef FSDATA_H
#define FSDATA_H

#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>

class FileData {
public:
    QString full_filename;
    QString short_filename;
    qint64 file_size;
};

class FSUdpData {
public:
    // main data
    QString nickname;
    QString private_key;
    bool is_locked;
    // file data
    qint8 num_files;
    // host address
    QHostAddress address;

    FSUdpData() {nickname = ""; private_key = "", is_locked = false; num_files = 0;}

    bool setData(QByteArray data);
    QByteArray getData();
};

class FSDescriptionData {
public:
    // action key
    qint64 action_key;
    // main data
    QString nickname;
    QString description;
    QList<FileData> filedata;
    // host address
    QHostAddress address;

    FSDescriptionData() {description = "";}

    bool setData(QByteArray data);
    QByteArray getData();
};

class ConnectionListData {
public:
    FSUdpData data;
    int timeout;
};

#endif // FSDATA_H
