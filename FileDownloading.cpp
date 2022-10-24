#include "FileDownloading.h"

int FileDownloading::getActionKey(QByteArray &data) {
    QDataStream in(&data, QIODevice::ReadOnly);

    // check data length
    qint64 size = -1;
    if(in.device()->size() > sizeof(qint64)) {
        in >> size;
    }
    else {
        return -1;
    }
    if (in.device()->size() != size)
        return -1;

    qint64 action_key;
    in >> action_key;

    return action_key;
}

// data creation functions
QByteArray FileDownloading::getFileDescrRequest(const QString &filename) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << FILE_DESCR_REQUEST;

    out << filename;

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

QByteArray FileDownloading::setFileDescr(const QString &filename, quint16 hash, qint64 file_size, qint64 packet_count) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << FILE_DESCR;

    out << filename;
    out << hash;
    out << file_size;
    out << packet_count;

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

QByteArray FileDownloading::getPartFileRequest(const QString &filename, qint64 packet_num) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << PART_FILE_REQUEST;

    out << filename;
    out << packet_num;

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

QByteArray FileDownloading::setPartOfFile(const QByteArray &part_file_data, const QString &filename, qint64 packet_num) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << PART_OF_FILE;

    out << part_file_data;
    out << filename;
    out << packet_num;

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

QByteArray FileDownloading::setEndOfFile(const QString &filename) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << END_OF_FILE;

    out << filename;

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

// data reading functions
void FileDownloading::readFileDescrRequest(QByteArray &data, QString *filename) {
    QDataStream in(&data, QIODevice::ReadOnly);

    qint64 size;
    in >> size;

    qint64 action_key;
    in >> action_key;

    in >> *filename;
}

void FileDownloading::readFileDescr(QByteArray &data, QString *filename, quint16 *hash, qint64 *file_size, qint64 *packet_count) {
    QDataStream in(&data, QIODevice::ReadOnly);

    qint64 size;
    in >> size;

    qint64 action_key;
    in >> action_key;

    in >> *filename;
    in >> *hash;
    in >> *file_size;
    in >> *packet_count;
}

void FileDownloading::readPartFileRequest(QByteArray &data, QString *filename, qint64 *packet_num) {
    QDataStream in(&data, QIODevice::ReadOnly);

    qint64 size;
    in >> size;

    qint64 action_key;
    in >> action_key;

    in >> *filename;
    in >> *packet_num;
}

void FileDownloading::readPartOfFile(QByteArray &data, QByteArray *part_file_data, QString *filename, qint64 *packet_num) {
    QDataStream in(&data, QIODevice::ReadOnly);

    qint64 size;
    in >> size;

    qint64 action_key;
    in >> action_key;

    in >> *part_file_data;
    in >> *filename;
    in >> *packet_num;
}

void FileDownloading::readEndOfFile(QByteArray &data, QString *filename) {
    QDataStream in(&data, QIODevice::ReadOnly);

    qint64 size;
    in >> size;

    qint64 action_key;
    in >> action_key;

    in >> *filename;
}

// other functions
void FileDownloading::MakeCRC16Table() {
    quint16 r;
    for(int i=0; i<256; i++){
        r = ((quint16)i)<<8;
        for(char j=0; j<8; j++) {
            if(r&(1<<15))
                r=(r<<1)^0x8005;
            else
                r=r<<1;
        }
        crctable[i]=r;
    }
}

quint16 FileDownloading::GetCRC16FromFile(const char *filename) {
    quint16 crc;
    crc = 0xffff;

    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return 0x0000;

    char ch;
    file.read(&ch, 1);
    while(!file.eof()) {
        crc = crctable[static_cast<int>(((crc>>8)^ch)&0xFF)] ^ (crc<<8);
        file.read(&ch, 1);
    }
    crc ^= 0xffff;
    return crc;
}

//-------------------

FileDownloading::FileDownloading(QObject *parent) : QObject(parent)
{
    client_timeout = server_timeout = -1;

    server = new QTcpServer();
    save_file = new QFile();

    timeout_timer = new QTimer();
    timeout_timer->start(1000);

    MakeCRC16Table();

    connect(server, SIGNAL(newConnection()), SLOT(newConnection()));
    connect(timeout_timer, SIGNAL(timeout()), this, SLOT(timeoutCheck()));
}

void FileDownloading::init(int port) {
    this->port = port;
    server->listen(QHostAddress::Any, this->port);
}

void FileDownloading::timeoutCheck() {
    // client
    if (client_timeout > 0)
        client_timeout--;

    if (client_timeout == 0) {
        client_timeout = -1;
        // disconnection actions
        client_socket->disconnectFromHost();
        if (save_file->isOpen())
            save_file->close();
        emit downloadingError(TIMEOUT_ERROR);
    }

    //server
    QString key;
    QFile *read_file;
    for (int i = read_files.count() - 1; i >= 0; i--) {
        key = read_files.keys()[i];
        read_file = read_files[key].file;

        if (read_files[key].time > 0)
            read_files[key].time--;

        if (read_files[key].time == 0) {
            if (read_file->isOpen()) {
                read_file->close();
                delete read_file;
                read_files.remove(key);
            }
        }
    }
}

// client functions

void FileDownloading::clientRead() {
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(QObject::sender());

    QByteArray data;
    data.resize(socket->bytesAvailable());
    QHostAddress address;
    socket->read(data.data(), data.size());
    address = socket->peerAddress();

    int action_key = getActionKey(data);
    switch (action_key) {
        case FILE_DESCR:
            processFileDescr(data);
            break;

        case PART_OF_FILE:
            processPartFile(data);
            break;

        case END_OF_FILE:
            processEndFile(data);
            break;
    }
}

void FileDownloading::connectToHost(const QString &serverIP) {
    client_socket = new QTcpSocket();
    connect(client_socket, SIGNAL(connected()), SIGNAL(connected()));
    connect(client_socket, SIGNAL(error(QAbstractSocket::SocketError)), SIGNAL(connectionFailed(QAbstractSocket::SocketError)));
    connect(client_socket, SIGNAL(disconnected()), SLOT(deleteLater()));

    connect(client_socket, SIGNAL(readyRead()), SLOT(clientRead()));
    client_socket->connectToHost(serverIP, this->port);
}

void FileDownloading::downloadFile(const QString &filename) {
    client_socket->write(getFileDescrRequest(filename));
}

void FileDownloading::processFileDescr(QByteArray &data) {
    QString filename;
    quint16 hash;
    qint64 file_size;
    qint64 packet_count;
    readFileDescr(data, &filename, &hash, &file_size, &packet_count);

    // file init
    std::fstream file_settings;
    if (! QFile::exists(cache_filename)) {
        file_settings.open(cache_filename.toStdString(), std::ios_base::out);
        file_settings.close();
    }

    QVector<CacheSettings> file_cache;
    CacheSettings read_cache;
    qint64 str_size;
    char *cStr;
    file_settings.open(cache_filename.toStdString(), std::ios_base::in | std::ios_base::binary);

    while (! file_settings.eof()) {
        file_settings.read((char*)&str_size, sizeof(str_size));

        cStr = new char[str_size + 1];
        file_settings.read(cStr, sizeof(str_size));
        cStr[str_size] = '\0';
        read_cache.local_filename = cStr;
        delete [] cStr;

        file_settings.read((char*)&(read_cache.hash), sizeof(read_cache.hash));
        file_settings.read((char*)&(read_cache.file_size), sizeof(read_cache.file_size));
        file_settings.read((char*)&(read_cache.current_packet), sizeof(read_cache.current_packet));
        if (! file_settings.eof()) {
            file_cache.push_back(read_cache);
        }
    }
    file_settings.close();
}

void FileDownloading::processPartFile(QByteArray &data) {
    QByteArray part_file_data;
    QString filename;
    qint64 part_num;
    readPartOfFile(data, &part_file_data, &filename, &part_num);

    curr_part_num = part_num;
    if (! save_file->isOpen()) {
        curr_local_filename = this->path + "/" + filename.section('/', -1, -1); // переписать (после файлов иниц.)
        save_file->setFileName(curr_local_filename);
        if (save_file->exists())
            save_file->open(QIODevice::Append);
        else
            save_file->open(QIODevice::WriteOnly);
    }
    client_socket->write(getPartFileRequest(filename, part_num + 1));

    save_file->write(part_file_data);

    client_timeout = TIMEOUT_TIME;
    // переписать подсчет процентов (после файлов иниц.)
}

void FileDownloading::processEndFile(QByteArray &data) {
    client_timeout = -1;
    if (save_file->isOpen())
        save_file->close();
    emit downloadingSucceed();
}

// server functions

void FileDownloading::serverRead() {
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(QObject::sender());

    QByteArray data;
    data.resize(socket->bytesAvailable());
    QHostAddress address;
    socket->read(data.data(), data.size());
    address = socket->peerAddress();

    int action_key = getActionKey(data);
    switch (action_key) {
        case FILE_DESCR_REQUEST:
            sendFileDescr(socket, data);
            break;

        case PART_FILE_REQUEST:
            sendPartFile(socket, data);
            break;
    }
}


void FileDownloading::newConnection() {
    QTcpSocket *server_socket = server->nextPendingConnection();
    connect(server_socket, SIGNAL(disconnected()), SLOT(deleteLater()));

    connect(server_socket, SIGNAL(readyRead()), SLOT(serverRead()));
}

void FileDownloading::sendFileDescr(QTcpSocket *socket, QByteArray &data) {
    QString filename;
    readFileDescrRequest(data, &filename);

    if (enable_files.contains(filename)) {
        quint16 hash;
        qint64 file_size;
        qint64 packet_count;

        hash = GetCRC16FromFile(filename.toStdString().c_str());
        QFileInfo file_info(filename);
        if (file_info.exists()) {
            file_size = file_info.size();
            packet_count = file_size / PART_SIZE + (file_size % PART_SIZE) && 1;
            socket->write(setFileDescr(filename, hash, file_size, packet_count));
        }
    }
}

void FileDownloading::sendPartFile(QTcpSocket *socket, QByteArray &data) {
    QString filename;
    qint64 part_num;
    readPartFileRequest(data, &filename, &part_num);

    if (enable_files.contains(filename)) {
        if (! read_files.contains(filename)) {
            read_files.insert(filename, FileTime(new QFile(filename), TIMEOUT_TIME));
            read_files[filename].file->open(QIODevice::ReadOnly);
        }

        QFile *read_file = read_files[filename].file;
        if (PART_SIZE * part_num < read_file->size()) {
            read_file->seek(PART_SIZE * part_num);
            QByteArray part_file_data = read_file->read(PART_SIZE);
            socket->write(setPartOfFile(part_file_data, filename, part_num));
            read_files[filename].time = TIMEOUT_TIME;
        }
        else {
            if (read_file->isOpen()) {
                read_file->close();
                delete read_file;
                read_files.remove(filename);
            }
            socket->write(setEndOfFile(filename));
        }
    }
}

