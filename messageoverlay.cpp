#include "messageoverlay.h"
#include "ui_messageoverlay.h"

MessageOverlay::MessageOverlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageOverlay)
{
    ui->setupUi(this);

    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(quite()));

    QFile styleF;
    styleF.setFileName(":/message_overlay_style.css");
    styleF.open(QFile::ReadOnly);
    QString qssStr = styleF.readAll();
    this->setStyleSheet(qssStr);

    // установка шрифтов
    QTimer::singleShot(0, this, SLOT(initFont())); //инициализация шрифтов по таймеру

    this->hide();
}

MessageOverlay::~MessageOverlay()
{
    delete ui;
}

void MessageOverlay::initFont() {
    QFont font;

    font = ui->titleLabel->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->titleLabel->setFont(font);

    font = ui->messageLabel->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->messageLabel->setFont(font);

    font = ui->okButton->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->okButton->setFont(font);
}

void MessageOverlay::showMessage(QString title, QString message) {
    this->ui->titleLabel->setText(title);
    this->ui->messageLabel->setText(message);
    this->show();
}

void MessageOverlay::quite() {
    this->hide();
    emit messageOverlayClosed();
}
