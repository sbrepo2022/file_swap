#ifndef FILEDOWNLOADING_H
#define FILEDOWNLOADING_H

#include <fstream>
#include <QObject>
#include <QDebug>
#include <QMap>
#include <QTcpSocket>
#include <QTcpServer>
#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QTimer>

class FileTime {
public:
    QFile *file;
    qint64 time;

    FileTime() {}
    FileTime(QFile *file, qint64 time) {this->file = file; this->time = time;}
};

class CacheSettings {
public:
    QString local_filename;
    qint16 hash;
    qint64 file_size;
    qint64 current_packet;
};

class FileDownloading : public QObject {
    Q_OBJECT
private:
    static const qint64 PART_FILE_REQUEST = 0, FILE_DESCR_REQUEST = 10; // client action keys
    static const qint64 PART_OF_FILE = 0, END_OF_FILE = 1, FILE_DESCR = 10;// server action keys

    static const qint64 PART_SIZE = 1024; // size a part of file
    static const qint64 TIMEOUT_TIME = 5;

    QTcpSocket *client_socket;
    QTcpServer *server;
    int port;

    QString path;
    QFile *save_file;
    QMap<QString, FileTime> read_files;

    QTimer *timeout_timer;
    int client_timeout, server_timeout;

    QString curr_local_filename;
    qint64 curr_part_num;

    quint16 crctable[256];
    QStringList enable_files;
    QString cache_filename;

    // action key
    int getActionKey(QByteArray &data);

    // data creation functions
    QByteArray getFileDescrRequest(const QString &filename);
    QByteArray setFileDescr(const QString &filename, quint16 hash, qint64 file_size, qint64 packet_count);
    QByteArray getPartFileRequest(const QString &filename, qint64 packet_num);
    QByteArray setPartOfFile(const QByteArray &part_file_data, const QString &filename, qint64 packet_num);
    QByteArray setEndOfFile(const QString &filename);

    // data reading functions
    void readFileDescrRequest(QByteArray &data, QString *filename);
    void readFileDescr(QByteArray &data, QString *filename, quint16 *hash, qint64 *file_size, qint64 *packet_count);
    void readPartFileRequest(QByteArray &data, QString *filename, qint64 *packet_num);
    void readPartOfFile(QByteArray &data, QByteArray *part_file_data, QString *filename, qint64 *packet_num);
    void readEndOfFile(QByteArray &data, QString *filename);

    // other functions
    void MakeCRC16Table();
    quint16 GetCRC16FromFile(const char *filename);

public:
    explicit FileDownloading(QObject *parent = nullptr);

    static const qint64 TIMEOUT_ERROR = 0, FILE_ERROR = 1; // downloading error keys

    void setEnableFiles(QStringList enable_files) {this->enable_files = enable_files;}
    void setCacheFilename(QString cache_filename) {this->cache_filename = cache_filename;}
    QStringList getEnableFiles() {return enable_files;}
    QString getCacheFilename() {return cache_filename;}

public slots:
    void init(int port);
    void setPath(const QString &path) {this->path = path;}
    void timeoutCheck();

    // client functions
    void clientRead();
    void connectToHost(const QString &serverIP);
    void downloadFile(const QString &filename);
    void processFileDescr(QByteArray &data); // дописать!!!
    void processPartFile(QByteArray &data);
    void processEndFile(QByteArray &data);

    // server functions
    void serverRead();
    void newConnection();
    void sendFileDescr(QTcpSocket *socket, QByteArray &data);
    void sendPartFile(QTcpSocket *socket, QByteArray &data);

signals:
    void connected();
    void connectionFailed(QAbstractSocket::SocketError);
    void downloadingSucceed();
    void downloadingError(qint64); // error key
};

#endif // FILEDOWNLOADING_H
