#include "passwordoverlay.h"
#include "ui_passwordoverlay.h"

PasswordOverlay::PasswordOverlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PasswordOverlay)
{
    ui->setupUi(this);

    connect(ui->showPasswordButton, SIGNAL(pressed()), this, SLOT(showPassword()));
    connect(ui->showPasswordButton, SIGNAL(released()), this, SLOT(hidePassword()));
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(okAction()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelAction()));

    QFile styleF;
    styleF.setFileName(":/password_overlay_style.css");
    styleF.open(QFile::ReadOnly);
    QString qssStr = styleF.readAll();
    this->setStyleSheet(qssStr);

    // установка валидатора
    initValidator();

    // установка шрифтов
    QTimer::singleShot(0, this, SLOT(initFont())); //инициализация шрифтов по таймеру

    this->hide();
}

PasswordOverlay::~PasswordOverlay()
{
    delete ui;
}

void PasswordOverlay::initFont() {
    QFont font;

    font = ui->titleLabel->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->titleLabel->setFont(font);

    font = ui->passwordLineEdit->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->passwordLineEdit->setFont(font);

    font = ui->okButton->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->okButton->setFont(font);

    font = ui->cancelButton->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->cancelButton->setFont(font);
}

void PasswordOverlay::initValidator() {
    QRegExpValidator *validator;
    validator = new QRegExpValidator(QRegExp("\\w+"));
    ui->passwordLineEdit->setValidator(validator);
}

void PasswordOverlay::showPasswordDialog(int selected_item_id) {
    this->selected_item_id = selected_item_id;
    this->ui->passwordLineEdit->clear();
    this->show();
}

void PasswordOverlay::okAction() {
    this->hide();
    selected_item_id = -1;
    emit passwordOverlayOk(ui->passwordLineEdit->text());
}

void PasswordOverlay::cancelAction() {
    this->hide();
    selected_item_id = -1;
    emit passwordOverlayCancel();
}

void PasswordOverlay::showPassword() {
    ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
}

void PasswordOverlay::hidePassword() {
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}
