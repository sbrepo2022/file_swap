#ifndef UDPCONNECTION_H
#define UDPCONNECTION_H

#include <QString>
#include <QUdpSocket>
#include <QDebug>
#include "FSData.h"

class UdpConnection : public QObject {
    Q_OBJECT
private:
    QUdpSocket socket;
    FSUdpData read_data;
    int port;

public:
    explicit UdpConnection(QObject *parent = 0);
    ~UdpConnection();
    void init(int port);
    void send(FSUdpData send_data);

public slots:
    void read();

signals:
    void readyRead(FSUdpData);
};

#endif // UDPCONNECTION_H
