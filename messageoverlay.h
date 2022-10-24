#ifndef MESSAGEOVERLAY_H
#define MESSAGEOVERLAY_H

#include <QWidget>
#include <QString>
#include <QTimer>
#include <QFile>

namespace Ui {
class MessageOverlay;
}

class MessageOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit MessageOverlay(QWidget *parent = 0);
    ~MessageOverlay();

private:
    Ui::MessageOverlay *ui;

private slots:
    void initFont();

public slots:
    void showMessage(QString title, QString message);
    void quite();

signals:
    void messageOverlayClosed();
};

#endif // MESSAGEOVERLAY_H
