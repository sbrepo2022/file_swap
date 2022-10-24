#include "overlay.h"
#include "ui_overlay.h"

Overlay::Overlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Overlay)
{
    ui->setupUi(this);

    connect(ui->overlay_quitButton, SIGNAL(clicked()), this, SLOT(quitOverlay()));
    connect(ui->overlay_quitButton2, SIGNAL(clicked()), this, SLOT(quitOverlay()));
    connect(ui->showPasswordButton, SIGNAL(pressed()), this, SLOT(showPassword()));
    connect(ui->showPasswordButton, SIGNAL(released()), this, SLOT(hidePassword()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(downloadButtonAction()));

    QFile styleF;
    styleF.setFileName(":/overlay_style.css");
    styleF.open(QFile::ReadOnly);
    QString qssStr = styleF.readAll();
    this->setStyleSheet(qssStr);

    // стиль полосы прокрутки
    ui->descriptionScrollArea->verticalScrollBar()->setStyle(new QCommonStyle);
    ui->filesListWidget->horizontalScrollBar()->setStyle(new QCommonStyle);

    // установка валидатора
    initValidator();

    // установка шрифтов
    QTimer::singleShot(0, this, SLOT(initFont())); //инициализация шрифтов по таймеру

    this->hide();
}

Overlay::~Overlay()
{
    delete ui;
}

void Overlay::initFont() {
    QFont font;

    font = ui->overlay_titleLabel->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->overlay_titleLabel->setFont(font);

    font = ui->overlay_titleLabel2->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->overlay_titleLabel2->setFont(font);

    font = ui->nicknameLabel->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->nicknameLabel->setFont(font);

    font = ui->privateKeyLabel->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->privateKeyLabel->setFont(font);

    font = ui->passwordLabel->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->passwordLabel->setFont(font);

    font = ui->nicknameLineEdit->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->nicknameLineEdit->setFont(font);

    font = ui->privateKeyLineEdit->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->privateKeyLineEdit->setFont(font);

    font = ui->passwordLineEdit->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->passwordLineEdit->setFont(font);

    font = ui->nicknameDescrLabel->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->nicknameDescrLabel->setFont(font);

    font = ui->descriptionLabel->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->descriptionLabel->setFont(font);

    font = ui->filesListWidget->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->filesListWidget->setFont(font);

    font = ui->filesLabel->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->filesLabel->setFont(font);

    font = ui->downloadButton->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->downloadButton->setFont(font);
}

void Overlay::initValidator() {
    QRegExpValidator *validator;
    validator = new QRegExpValidator(QRegExp("\\w+"));
    //ui->nicknameLineEdit->setValidator(validator);
    ui->privateKeyLineEdit->setValidator(validator);
    ui->passwordLineEdit->setValidator(validator);
}

void Overlay::init(OverlayPreferencesData oData) {
    if (! loadData())
        setOverlayPreferencesData(oData);
}

void Overlay::openOverlay(int index) {
    ui->overlay_mainStackedWidget->setCurrentIndex(index);
    this->show();
}

OverlayPreferencesData Overlay::getOverlayPreferencesData() {
    OverlayPreferencesData oData;
    oData.nickname = this->ui->nicknameLineEdit->text();
    oData.private_key = this->ui->privateKeyLineEdit->text();
    oData.password = this->ui->passwordLineEdit->text();

    return oData;
}

void Overlay::saveData() {
    OverlayPreferencesData oData = getOverlayPreferencesData();

    QFile settingsF(oDataPath);
    settingsF.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream text_stream(&settingsF);
    text_stream << "[personal_settings]\n";
    text_stream << "nickname=" << oData.nickname << "\n";
    text_stream << "private_key=" << oData.private_key << "\n";
    text_stream << "password=" << oData.password << "\n";
}

bool Overlay::loadData() {
    QSettings *settings;
    OverlayPreferencesData oData;

    if (QFile::exists(oDataPath)) {
        settings = new QSettings(oDataPath, QSettings::IniFormat);
        settings->beginGroup("personal_settings");
        oData.nickname = settings->value("nickname").toString();
        oData.private_key = settings->value("private_key").toString();
        oData.password = settings->value("password").toString();

        setOverlayPreferencesData(oData);
        return true;
    }
    return false;
}

void Overlay::setOverlayPreferencesData(OverlayPreferencesData oData) {
    this->ui->nicknameLineEdit->setText(oData.nickname);
    this->ui->privateKeyLineEdit->setText(oData.private_key);
    this->ui->passwordLineEdit->setText(oData.password);
}

void Overlay::setOverlayDescriptionData(OverlayDescriptionData oData) {
    this->ui->nicknameDescrLabel->setText(oData.nickname);
    this->ui->descriptionLabel->setText(oData.description);

    QString filename;
    QString filetype;
    int num_last_symbol;
    QListWidgetItem *item;

    this->ui->filesListWidget->clear();
    for (int i = 0; i < oData.filedata.count(); i++) {
        filename = oData.filedata[i].short_filename;
        for (int i = 0; filename[i] != '\0'; i++) {
            if (filename[i] == '.')
                num_last_symbol = i;
        }
        filetype = filename.remove(0, num_last_symbol + 1);

        // размер файлов
        float file_size_float = (float)oData.filedata[i].file_size / 1024;
        int weight_index;
        for (weight_index = 0; file_size_float > 1023; file_size_float /= 1024, weight_index++) {
            if (weight_index > 3) {
                weight_index = 3;
                break;
            }
        }
        QString prefixes[] = {" Kb", " Mb", " Gb", " Tb"};
        QString file_size_str = QString::number(file_size_float, 'f', 1) + prefixes[weight_index];
        //------

        item = new QListWidgetItem(oData.filedata[i].short_filename + "\n" + file_size_str);
        if (QFile::exists(":/textures/file_types/" + filetype + ".png"))
            item->setIcon(QIcon(":/textures/file_types/" + filetype + ".png"));
        else
            item->setIcon(QIcon(":/textures/file_types/unknown.png"));
        ui->filesListWidget->addItem(item);
    }

    if (oData.filedata.count()) {
        this->ui->filesListWidget->show();
        this->ui->downloadButton->show();
        this->ui->filesLabel->show();
    }
    else {
        this->ui->filesListWidget->hide();
        this->ui->downloadButton->hide();
        this->ui->filesLabel->hide();
    }
    this->filedata = oData.filedata;
}

void Overlay::setListIconSize(QSize size) {
    this->ui->filesListWidget->setIconSize(size);
}

void Overlay::quitOverlay() {
    this->hide();
    saveData();

    emit overlayClosed();
}

void Overlay::showPassword() {
    ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
}

void Overlay::hidePassword() {
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

void Overlay::downloadButtonAction() {
    QListWidgetItem *item;
    QList<FileData> selected_filedata;

    for (int i = ui->filesListWidget->count() - 1; i >= 0; i--) {
        item = ui->filesListWidget->item(i);
        if (item->isSelected()) {
            selected_filedata.append(this->filedata[i]);
        }
    }

    emit downloadButtonClicked(selected_filedata);
}
