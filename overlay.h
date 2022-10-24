#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QScrollBar>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QString>
#include <QSettings>
#include <QRegularExpressionValidator>
#include <QRegExp>
#include <QCommonStyle>
#include "FSData.h"

class OverlayPreferencesData {
public:
    QString nickname;
    QString private_key;
    QString password;

    OverlayPreferencesData() {nickname = ""; private_key = ""; password = "";}
};

class OverlayDescriptionData {
public:
    QString nickname;
    QString description;
    QList<FileData> filedata;

    OverlayDescriptionData() {description = "";}
};

namespace Ui {
class Overlay;
}

class Overlay : public QWidget
{
    Q_OBJECT

public:
    explicit Overlay(QWidget *parent = 0);
    ~Overlay();

    void initValidator();
    void init(OverlayPreferencesData);
    void openOverlay(int index);
    OverlayPreferencesData getOverlayPreferencesData();
    void setOverlayPreferencesData(OverlayPreferencesData);
    void setOverlayDescriptionData(OverlayDescriptionData);
    void setListIconSize(QSize size);

private:
    Ui::Overlay *ui;

    QList<FileData> filedata;

    static constexpr const char* oDataPath = "overlay_data.ini";

    void saveData();
    bool loadData();

public slots:
    void initFont();
    void quitOverlay();

    void showPassword();
    void hidePassword();
    void downloadButtonAction();

signals:
    void overlayClosed();
    void downloadButtonClicked(QList<FileData> selected_filedata);
};

#endif // OVERLAY_H
