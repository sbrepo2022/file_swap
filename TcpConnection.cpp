#include "TcpConnection.h"

TcpConnection::TcpConnection(QObject *parent) : QObject(parent) {
    connect(&server, SIGNAL(newConnection()), SLOT(newConnection()));
}

void TcpConnection::init(int port) {
    sending_state = false;

    this->port = port;
    server.listen(QHostAddress::Any, port);
}

void TcpConnection::sendDescriptionRequest(QString serverIP) {
    QTcpSocket *socket = new QTcpSocket();
    connect(socket, SIGNAL(connected()), SLOT(sendDescriptionRequestAction()));
    socket->connectToHost(serverIP, port);
    //qDebug() << serverIP;
}

void TcpConnection::sendDescriptionRequestAction() {
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(QObject::sender());
    disconnect(socket, SIGNAL(connected()), this, SLOT(sendDescriptionRequestAction()));

    socket->write(getDescrResponse());

    socket->disconnectFromHost();
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void TcpConnection::sendDescription(QString serverIP) {
    QTcpSocket *socket = new QTcpSocket();
    connect(socket, SIGNAL(connected()), SLOT(sendDescriptionAction()));
    socket->connectToHost(serverIP, port);
}

void TcpConnection::sendDescriptionAction() {
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(QObject::sender());
    disconnect(socket, SIGNAL(connected()), this, SLOT(sendDescriptionAction()));

    send_data.action_key = DESCR;
    socket->write(send_data.getData());

    socket->disconnectFromHost();
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void TcpConnection::sendError(QString serverIP, QString error_message) {
    this->error_message = error_message;

    QTcpSocket *socket = new QTcpSocket();
    connect(socket, SIGNAL(connected()), SLOT(sendErrorAction()));
    socket->connectToHost(serverIP, port);
}

void TcpConnection::sendErrorAction() {
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(QObject::sender());
    disconnect(socket, SIGNAL(connected()), this, SLOT(sendErrorAction()));


    socket->write(getErrorResponse(error_message));

    socket->disconnectFromHost();
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void TcpConnection::newConnection() {
    QTcpSocket *socket = server.nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), SLOT(read()));
}

void TcpConnection::read() {
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(QObject::sender());

    QByteArray data;
    data.resize(socket->bytesAvailable());
    QHostAddress address;
    socket->read(data.data(), data.size());
    address = socket->peerAddress();

    int action_key = getActionKey(data);

    switch (action_key) {
        case DESCR_REQUEST: //send description request
            // ДОДЕЛАТЬ!!!!!!!! Пароль получен, дальше сравнить с локальным паролем, (отпр. описание ИЛИ сразу вернуть ошибку)
            if (sending_state) {
                if (local_password == getPassword(data)) {
                    emit readyReadDescriptionRequest(address);
                }
                else {
                    sendError(address.toString(), "Incorrect password");
                }
            }
            else {
                sendError(address.toString(), "Cannot connect to host");
            }
            break;

        case DESCR: //description
            if (read_data.setData(data)) {
                read_data.address = address;
                emit readyReadDescription(read_data);
            }
            break;

        case DESCR_ERROR:
            emit readyReadDescriptionError(getErrorMessage(data));
            break;
    }
}

QByteArray TcpConnection::getDescrResponse() {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << DESCR_REQUEST;

    out << send_password;

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

QByteArray TcpConnection::getErrorResponse(QString error_message) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << DESCR_ERROR;

    out << error_message;

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

int TcpConnection::getActionKey(QByteArray data) {
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

QString TcpConnection::getPassword(QByteArray data) {
    QDataStream in(&data, QIODevice::ReadOnly);

    qint64 size;
    in >> size;

    qint64 action_key;
    in >> action_key;

    QString password;
    in >> password;

    return password;
}

QString TcpConnection::getErrorMessage(QByteArray data) {
    QDataStream in(&data, QIODevice::ReadOnly);

    qint64 size;
    in >> size;

    qint64 action_key;
    in >> action_key;

    QString error_message;
    in >> error_message;

    return error_message;
}
