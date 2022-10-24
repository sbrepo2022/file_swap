#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // ui
    ui->setupUi(this);

    initOverlay();
    initMessageOverlay();
    initPasswordOverlay();
    updateSendData();

    //----------------------------------------------------------------
    // сигналы и слоты
    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(quitApp()));
    connect(ui->hideButton, SIGNAL(clicked()), this, SLOT(hideWindow()));

    // переход по страницам
    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(toSendPage()));
    connect(ui->getButton, SIGNAL(clicked()), this, SLOT(toGetPage()));
    connect(ui->descriptionSwitchButton, SIGNAL(clicked()), this, SLOT(switchToDescription()));
    connect(ui->filesSwitchButton, SIGNAL(clicked()), this, SLOT(switchToFiles()));
    connect(ui->back1Button, SIGNAL(clicked()), this, SLOT(fromSendPage()));
    connect(ui->back2Button, SIGNAL(clicked()), this, SLOT(fromGetPage()));

    // взаимодействие с интерфейсом
    connect(ui->onlineCheckBox, SIGNAL(clicked(bool)), this, SLOT(changeSendingState(bool)));
    connect(ui->descriptionEdit, SIGNAL(textChanged()), this, SLOT(updateNumSymbols()));
    connect(ui->personalSettingsButton, SIGNAL(clicked()), this, SLOT(toPersonalSettings()));
    connect(ui->toolsButton, SIGNAL(clicked()), this, SLOT(toTools()));
    connect(&core, SIGNAL(readyDescriptionRequest()), this, SLOT(updateDescriptionData()));
    connect(&core, SIGNAL(readyDescription(FSDescriptionData)), this, SLOT(toDescriptionPage(FSDescriptionData)));
    connect(&core, SIGNAL(readyDescriptionError(QString)), this, SLOT(showNetworkError(QString)));
    connect(overlay, SIGNAL(overlayClosed()), this, SLOT(fromOverlay()));
    connect(&core, SIGNAL(readyUpdateConnections(int, int)), this, SLOT(updateConnectionsList(int, int)));
    connect(ui->connectionsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onConnectionList(QListWidgetItem*)));
    connect(ui->addFileButton, SIGNAL(clicked()), this, SLOT(addFileToList()));
    connect(ui->deleteFileButton, SIGNAL(clicked()), this, SLOT(deleteFileFromList()));
    connect(overlay, SIGNAL(downloadButtonClicked(QList<FileData>)), this, SLOT(downloadButtonAction(QList<FileData>)));
    connect(message_overlay, SIGNAL(messageOverlayClosed()), this, SLOT(quiteMessage()));
    connect(password_overlay, SIGNAL(passwordOverlayOk(QString)), this, SLOT(getPasswordAction(QString)));



    // инициализация ядра
    core.init(port, tcp_port, file_loading_port, connection_timeout);
    core.startConnectionTimer(1000);

    // установка отслеживания мыши внутри окна
    setMouseTracking(true);

    // стиль рамки окна
    setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *bodyShadow = new QGraphicsDropShadowEffect;
    bodyShadow->setBlurRadius(18.0);
    bodyShadow->setColor(QColor(0, 0, 0, 160));
    bodyShadow->setOffset(0.0);
    ui->mainWidget->setGraphicsEffect(bodyShadow);

    // стиль полосы прокрутки
    ui->descriptionEdit->verticalScrollBar()->setStyle(new QCommonStyle);
    ui->connectionsListWidget->verticalScrollBar()->setStyle(new QCommonStyle);
    ui->connectionsListWidget->horizontalScrollBar()->setStyle(new QCommonStyle);
    ui->filesListWidget->verticalScrollBar()->setStyle(new QCommonStyle);
    ui->filesListWidget->horizontalScrollBar()->setStyle(new QCommonStyle);

    // загрузка шрифтов
    int font_id;
    QString family;

    font_id = QFontDatabase::addApplicationFont(":/fonts/SF-UI-Display-Regular.ttf");
    family = QFontDatabase::applicationFontFamilies(font_id).at(0);
    //qDebug() << "Font" << family << "was succesfully loaded!";

    font_id = QFontDatabase::addApplicationFont(":/fonts/SF-UI-Text-Regular.ttf"); //путь к шрифту
    family = QFontDatabase::applicationFontFamilies(font_id).at(0); //имя шрифта
    //qDebug() << "Font" << family << "was succesfully loaded!";

    // установка шрифтов
    QTimer::singleShot(0, this, SLOT(initFont())); //инициализация шрифтов по таймеру

    // обновление счетчика символов
    updateNumSymbols();

    // сброс шрифта
    resetFont(ui->centralWidget);
    resetFont(overlay);
    resetFont(message_overlay);

    // маштабирование интерфейса
    this->setGeometry(this->geometry().x(), this->geometry().y(), this->geometry().width() * interface_scale, this->geometry().height() * interface_scale);
    scaleInterface(ui->centralWidget, interface_scale, SF_ALL);

    overlay->setGeometry(this->geometry());
    scaleInterface(overlay, interface_scale, SF_ALL);

    message_overlay->setGeometry(this->geometry());
    scaleInterface(message_overlay, interface_scale, SF_ALL);

    // размеры значков
    ui->filesListWidget->setIconSize(QSize(35 * interface_scale, 35 * interface_scale));
    this->overlay->setListIconSize(QSize(35 * interface_scale, 35 * interface_scale));
}

void MainWindow::initFont() {
    QFont font;

    font = ui->onlineCheckBox->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->onlineCheckBox->setFont(font);

    font = ui->descriptionEdit->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->descriptionEdit->setFont(font);

    font = ui->connectionsListWidget->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->connectionsListWidget->setFont(font);

    font = ui->filesListWidget->font();
    font.setFamily(QStringLiteral("SF UI Text"));
    ui->filesListWidget->setFont(font);

    font = ui->maxSumbolsLabel->font();
    font.setFamily(QStringLiteral("SF UI Display"));
    ui->maxSumbolsLabel->setFont(font);
}

void MainWindow::initOverlay() {
    // создание оверлея
    overlay = new Overlay(this);

    overlay->setGeometry(this->geometry());
    overlay->hide();

    OverlayPreferencesData oData;
    oData.nickname = "user";
    oData.private_key = "";
    oData.password = "";

    overlay->init(oData);
}

void MainWindow::initMessageOverlay() {
    message_overlay = new MessageOverlay(this);

    message_overlay->setGeometry(this->geometry());
}

void MainWindow::initPasswordOverlay() {
    password_overlay = new PasswordOverlay(this);

    password_overlay->setGeometry(this->geometry());
}

void MainWindow::updateSendData() {
    FSUdpData send_data;

    OverlayPreferencesData oData = overlay->getOverlayPreferencesData();

    send_data.nickname = oData.nickname;
    send_data.private_key = oData.private_key;
    if (oData.password.count() > 0) {
        send_data.is_locked = true;
    }
    else {
        send_data.is_locked = false;
    }
    core.setLocalPassword(oData.password);

    core.updateFileData(send_data);
}

void MainWindow::updateDescriptionData() {
    FSDescriptionData send_data;

    OverlayPreferencesData oData = overlay->getOverlayPreferencesData();

    send_data.nickname = oData.nickname;
    send_data.description = ui->descriptionEdit->toPlainText();
    send_data.filedata = core.getAllFiledata();

    core.updateDescriptionData(send_data);
}

void MainWindow::scaleInterface(QWidget *widget, float factor, unsigned long flags) {
    QFont font;

    if (widget != nullptr && qobject_cast<QWidget*>(widget)) { //qobject_cast<QVBoxLayout*>(widget) != nullptr
        //qDebug() << widget->property("objectName").toString();

        if (flags & SF_SCALEINTERFACE) {
            if (widget->maximumWidth() * factor < 16777215)
                widget->setMaximumWidth(widget->maximumWidth() * factor);

            if (widget->maximumHeight() * factor < 16777215)
                widget->setMaximumHeight(widget->maximumHeight() * factor);

            if (widget->minimumWidth() * factor < 16777215)
                widget->setMinimumWidth(widget->minimumWidth() * factor);

            if (widget->minimumHeight() * factor < 16777215)
                widget->setMinimumHeight(widget->minimumHeight() * factor);
        }

        if (flags & SF_SCALEFONT) {
            font = widget->font();
            font.setPointSizeF(font.pointSizeF() * factor);
            widget->setFont(font);
        }
    }

    QObjectList children_objects = widget->children();
    for (int i = 0; i < children_objects.size(); i++) {
        scaleInterface(reinterpret_cast<QWidget*>(children_objects[i]), factor, flags);
    }
}

void MainWindow::resetFont(QWidget *widget) {
    QFont font;

    if (widget != nullptr && qobject_cast<QWidget*>(widget)) {
        font = widget->font();
        font.setPointSizeF(font.pointSizeF());
        widget->setFont(font);
    }

    QObjectList children_objects = widget->children();
    for (int i = 0; i < children_objects.size(); i++) {
        resetFont(reinterpret_cast<QWidget*>(children_objects[i]));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *pe) {
    QPoint mt_pos1;
    mt_pos1.setX(ui->windowToolsWidget->pos().x());
    mt_pos1.setY(ui->windowToolsWidget->pos().y());
    QPoint mt_pos2;
    mt_pos2.setX(ui->windowToolsWidget->size().width() + ui->windowToolsWidget->pos().x());
    mt_pos2.setY(ui->windowToolsWidget->size().height() + ui->windowToolsWidget->pos().y());
    this->mt.init(mt_pos1.x(), mt_pos1.y(), mt_pos2.x(), mt_pos2.y());

    this->mt.start(pe->globalX(), pe->globalY(), this->pos().x() + ui->mainWidget->pos().x(), this->pos().y() + ui->mainWidget->pos().y());
}

void MainWindow::mouseMoveEvent(QMouseEvent *pe) {
    int xoffset = 0, yoffset = 0;
    QPoint w_pos = this->pos();
    if (this->mt.tracking(pe->globalX(), pe->globalY(), xoffset, yoffset)) {
        this->move(xoffset + w_pos.x(), yoffset + w_pos.y());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *pe) {
    this->mt.end();
}

void MainWindow::quitApp() {
    QFile settingsF("settings.ini");
    settingsF.open(QFile::WriteOnly);
    settingsF.write("[position]\n");
    settingsF.write(QString("x=" + QString::number(this->pos().x()) + "\n").toUtf8());
    settingsF.write(QString("y=" + QString::number(this->pos().y()) + "\n").toUtf8());

    qApp->quit();
}

void MainWindow::hideWindow() {
    this->showMinimized();
}

void MainWindow::toSendPage() {
    ui->contentStackedWidget->setCurrentIndex(1);
}

void MainWindow::fromSendPage() {
    ui->contentStackedWidget->setCurrentIndex(0);
}

void MainWindow::switchToDescription() {
    ui->sendingContentStackedWidget->setCurrentIndex(0);
    ui->descriptionSwitchButton->setChecked(true);
    ui->filesSwitchButton->setChecked(false);
}

void MainWindow::switchToFiles() {
    ui->sendingContentStackedWidget->setCurrentIndex(1);
    ui->descriptionSwitchButton->setChecked(false);
    ui->filesSwitchButton->setChecked(true);
}

void MainWindow::toGetPage() {
    ui->contentStackedWidget->setCurrentIndex(2);
}

void MainWindow::fromGetPage() {
    ui->contentStackedWidget->setCurrentIndex(0);
}

void MainWindow::toPersonalSettings() {
    this->mt.block();
    overlay->openOverlay(0);
}

void MainWindow::toDescriptionPage(FSDescriptionData data) {
    OverlayDescriptionData oData;
    oData.nickname = data.nickname;
    oData.description = data.description;
    oData.filedata = data.filedata;

    overlay->setOverlayDescriptionData(oData);

    this->mt.block();
    overlay->openOverlay(1);
}

void MainWindow::fromOverlay() {
    this->mt.unblock();

    updateSendData();
}

void MainWindow::toTools() {
    MessageBox("Tools", "There is no content, but will appear soon...");
}

void MainWindow::showNetworkError(QString message) {
    MessageBox("Oops...", message);
}

void MainWindow::changeSendingState(bool state) {
    if (state) {
        core.startSendFileData(sending_interval);
        core.setSendingState(true);
    }
    else {
        core.stopSendFileData();
        core.setSendingState(false);
    }
}

void MainWindow::updateConnectionsList(int action, int index) {
    QVector<ConnectionListData> connections = core.getConnectionList();
    if (action == 0) {
        if (password_overlay->getSelectedItemID() == index) {
            password_overlay->cancelAction();
            MessageBox("Oops...", "Cannot connect to host");
        }
        delete ui->connectionsListWidget->item(index);
    }

    if (action == 1) {
        if (this->overlay->getOverlayPreferencesData().private_key == connections[index].data.private_key || connections[index].data.private_key == "") {
            ui->connectionsListWidget->addItem(connections[index].data.nickname);
        }
    }

    if (action == 2) {
        if (! (this->overlay->getOverlayPreferencesData().private_key == connections[index].data.private_key || connections[index].data.private_key == "")) {
            delete ui->connectionsListWidget->item(index);
        }
        else {
            if (ui->connectionsListWidget->item(index)->text() != connections[index].data.nickname) {
                ui->connectionsListWidget->item(index)->setText(connections[index].data.nickname);
            }
        }
    }
}

void MainWindow::updateNumSymbols() {
    int num_symbols = ui->descriptionEdit->toPlainText().count();
    ui->maxSumbolsLabel->setText(QString::number(num_symbols) + "/" + QString::number(max_num_symbols));

    QString str;
    QTextCursor cursor;
    int cursorPos;
    if (num_symbols > max_num_symbols){
        cursor = ui->descriptionEdit->textCursor();
        cursorPos = cursor.position();
        qDebug() << cursorPos;
        str = ui->descriptionEdit->toPlainText().remove(cursorPos - 1, 1);
        ui->descriptionEdit->setPlainText(str);
        ui->descriptionEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    }
}

void MainWindow::addFileToList() {
    QStringList files_list = QFileDialog::getOpenFileNames(this, tr("Select files"), this->open_file_path, tr("*"));

    QString filename;
    int num_last_symbol;

    // получение пути до папки с файлом
    if (files_list.count() > 0) {
        filename = files_list[0];
        for (int i = 0; filename[i] != '\0'; i++) {
            if (filename[i] == '/')
                num_last_symbol = i;
        }
        open_file_path = filename.remove(num_last_symbol + 1, filename.size());
    }

    for (int i = 0; i < files_list.count(); i++) {
        QString full_filename = files_list[i];

        if (this->core.getFiledata(full_filename).full_filename != "") {
            continue;
        }

        // получение короткого имени файла
        filename = full_filename;
        for (int i = 0; filename[i] != '\0'; i++) {
            if (filename[i] == '/')
                num_last_symbol = i;
        }
        QString short_filename = filename.remove(0, num_last_symbol + 1);

        // получение типа файла
        filename = short_filename;
        for (int i = 0; filename[i] != '\0'; i++) {
            if (filename[i] == '.')
                num_last_symbol = i;
        }
        QString filetype = filename.remove(0, num_last_symbol + 1);

        // размер файлов
        QFileInfo file_info(full_filename);
        qint64 file_size = file_info.size();

        float file_size_float = (float)file_size / 1024;
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

        QListWidgetItem *item = new QListWidgetItem(short_filename + "\n" + file_size_str);
        if (QFile::exists(":/textures/file_types/" + filetype + ".png"))
            item->setIcon(QIcon(":/textures/file_types/" + filetype + ".png"));
        else
            item->setIcon(QIcon(":/textures/file_types/unknown.png"));
        item->setToolTip(full_filename);
        ui->filesListWidget->addItem(item);

        FileData filedata;
        filedata.full_filename = full_filename;
        filedata.short_filename = short_filename;
        filedata.file_size = file_size;

        this->core.addFiledata(full_filename, filedata);
    }
}

void MainWindow::deleteFileFromList() {
    QListWidgetItem *item;

    for (int i = ui->filesListWidget->count() - 1; i >= 0; i--) {
        item = ui->filesListWidget->item(i);
        if (item->isSelected()) {
            this->core.deleteFiledataByIndex(i);
            delete item;
        }
    }

}

void MainWindow::downloadButtonAction(QList<FileData> selected_filedata) {
    QString save_path = "";
    if (!selected_filedata.count()) {
        MessageBox("Oops...", "Select at least one file.");
    }
    else {
        for (int i = 0; i < selected_filedata.count(); i++)
            qDebug() << selected_filedata[i].full_filename;

        save_path = QFileDialog::getExistingDirectory(this, tr("Open Directory"), this->save_file_path, QFileDialog::ShowDirsOnly);
        if (save_path != "") {
            this->save_file_path = save_path;
            this->core.downloadFiles(core.getCurrentConnectionIP(), selected_filedata, save_path);
        }
    }
}

void MainWindow::getPasswordAction(QString password) {
    this->core.getDescriptionByIndex(selected_row, password);
}

void MainWindow::onConnectionList(QListWidgetItem *item) {
    this->selected_row = this->ui->connectionsListWidget->row(item);
    if (!this->core.checkIndexPassword(selected_row)) {
        this->core.getDescriptionByIndex(selected_row, "");
    }
    else {
        password_overlay->showPasswordDialog(selected_row);
    }
}
