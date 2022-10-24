#include "UdpConnection.h"

UdpConnection::UdpConnection(QObject *parent) : QObject(parent) {
    connect(&socket, SIGNAL(readyRead()), SLOT(read()));
}

UdpConnection::~UdpConnection() {

}

void UdpConnection::init(int port) {
    this->port = port;
    socket.bind(QHostAddress::Any, port);
}

void UdpConnection::send(FSUdpData send_data) {
    socket.writeDatagram(send_data.getData(), QHostAddress::Broadcast, port);
}

void UdpConnection::read() {
    QByteArray datagram;
    datagram.resize(socket.pendingDatagramSize());
    QHostAddress address;
    socket.readDatagram(datagram.data(), datagram.size(), &address);

    if (read_data.setData(datagram)) {
        read_data.address = address;
        emit readyRead(read_data);
    }
}
