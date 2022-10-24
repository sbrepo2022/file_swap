#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QListWidgetItem>
#include <QFont>
#include <QFontDatabase>
#include <QTimer>
#include <QCommonStyle>
#include "MoveTracker.h"
#include "FSCore.h"
#include "Overlay.h"
#include "messageoverlay.h"
#include "passwordoverlay.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // флаги ф-ии scaleInterface
    static const unsigned long int SF_SCALEINTERFACE = 0x0001;
    static const unsigned long int SF_SCALEFONT = 0x0002;
    static const unsigned long int SF_ALL = SF_SCALEINTERFACE | SF_SCALEFONT;

private:
    Ui::MainWindow *ui;
    MoveTracker mt;
    FSCore core;

    const int sending_interval = 1000;
    const int connection_timeout = 5000;
    const int connection_update = 5000;
    const int port = 5758;
    const int tcp_port = 5759;
    const int file_loading_port = 5760;
    const int max_num_symbols = 1000;
    const float interface_scale = 1.0f;

    QString open_file_path = "/";
    QString save_file_path = "/";
    int selected_row;

    // оверлеи
    Overlay *overlay;
    MessageOverlay *message_overlay;
    PasswordOverlay *password_overlay;

protected:
    void mousePressEvent(QMouseEvent *pe);
    void mouseMoveEvent(QMouseEvent *pe);
    void mouseReleaseEvent(QMouseEvent *pe);

public slots:
    // инициализация
    void initFont();
    void initOverlay();
    void initMessageOverlay();
    void initPasswordOverlay();
    void updateSendData();
    void updateDescriptionData();
    void scaleInterface(QWidget *widget, float factor, unsigned long int flags = SF_ALL);
    void resetFont(QWidget *widget);

    // оконнные функции
    void quitApp();
    void hideWindow();

    // перемещение по страницам
    void toSendPage();
    void fromSendPage();
    void toGetPage();
    void fromGetPage();
    void switchToDescription();
    void switchToFiles();
    void toPersonalSettings();
    void toDescriptionPage(FSDescriptionData data);
    void fromOverlay();
    void toTools();
    void fromTools() {}

    void showNetworkError(QString message);

    // взаимодействие с интерфейсом
    void changeSendingState(bool);
    void updateConnectionsList(int action, int index);
    void updateNumSymbols();
    void addFileToList();
    void deleteFileFromList();
    void downloadButtonAction(QList<FileData> selected_filedata);
    void getPasswordAction(QString password);

    //действия по событиям
    void onConnectionList(QListWidgetItem *item);

    //функции общих действий
    void MessageBox(QString title, QString message) {this->message_overlay->showMessage(title, message); this->mt.block();}
    void quiteMessage() {this->mt.unblock();}
};

#endif // MAINWINDOW_H
