#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include "FSData.h"

class TcpConnection : public QObject {
    Q_OBJECT
private:
    static const qint64 DESCR_REQUEST = 0, DESCR = 1, DESCR_ERROR = 2; //action keys

    QTcpServer server;
    FSDescriptionData read_data;
    FSDescriptionData send_data;
    int port;

    QByteArray getDescrResponse(); //get byte array of description request
    QByteArray getErrorResponse(QString error_message);
    int getActionKey(QByteArray data); //get type of data
    QString getPassword(QByteArray data);
    QString getErrorMessage(QByteArray data);

    bool sending_state;
    QString send_password, local_password;
    QString error_message; // temporary value

public:
    explicit TcpConnection(QObject *parent = 0);
    ~TcpConnection() {}
    void init(int port);
    void sendDescriptionRequest(QString serverIP);
    void sendDescription(QString serverIP);
    void sendError(QString serverIP, QString error_message);

    void setLocalPassword(QString local_password) {this->local_password = local_password;}
    void setSendPassword(QString send_password) {this->send_password = send_password;}
    void setSendData(FSDescriptionData send_data) {this->send_data = send_data;}
    void setSendingState(bool state) {sending_state = state;}

public slots:
    void newConnection();
    void read();
    void sendDescriptionRequestAction();
    void sendDescriptionAction();
    void sendErrorAction();

signals:
    void readyReadDescriptionRequest(QHostAddress);
    void readyReadDescription(FSDescriptionData);
    void readyReadDescriptionError(QString);
};

#endif // TCPCONNECTION_H
