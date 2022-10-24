#ifndef PASSWORDOVERLAY_H
#define PASSWORDOVERLAY_H

#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QDebug>

namespace Ui {
class PasswordOverlay;
}

class PasswordOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordOverlay(QWidget *parent = 0);
    ~PasswordOverlay();

    int getSelectedItemID() {return selected_item_id;}

private:
    Ui::PasswordOverlay *ui;
    int selected_item_id;

private slots:
    void initFont();
    void initValidator();

public slots:
    void showPasswordDialog(int selected_item_id = -1);
    void okAction();
    void cancelAction();

    void showPassword();
    void hidePassword();

signals:
    void passwordOverlayOk(QString);
    void passwordOverlayCancel();
};

#endif // PASSWORDOVERLAY_H
