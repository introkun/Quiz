#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdbmanipulator.h"
#include "qchoicedialog.h"
#include "qaboutdialog.h"
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QList<QGame *> games;
    games.append(QGame::createGame(QGame::GAME_TURNBASEDQUIZ,this));
    games.append(QGame::createGame(QGame::GAME_PARALLELQUIZ,this));
    im = new QInterface_Manager();
    ui -> setupUi(this);
    ui -> centralWidget -> setLayout(ui -> mainLayout);
    ev_fltr = new Event_Filter();
    connect(ev_fltr,SIGNAL(form_resize()),this,SLOT(formResize()));
    installEventFilter(ev_fltr);
    gamesTabWidget = new QTabWidget();
    ui -> mainLayout -> addWidget(gamesTabWidget);
    uiSetupGames(games);
    setWindowTitle("Quiz");
    connect(ui -> actionSave,SIGNAL(triggered()),this,SLOT(saveJSON()));
    connect(ui -> actionMode,SIGNAL(triggered()),this,SLOT(selectWorkMode()));
    connect(ui -> actionQuit,SIGNAL(triggered()),this,SLOT(close()));
    connect(im,SIGNAL(signal_interface_status(bool)),this,SLOT(set_interface_status(bool)));
    connect(im,SIGNAL(signal_proc_incoming_message(QByteArray)),this,SLOT(rs232_cmd_income(QByteArray)));
    connect(this,SIGNAL(signal_start_autoconnect()),this,SLOT(start_autoconnect()),Qt::QueuedConnection);
    connect(ui -> actionSetPicture,SIGNAL(triggered()),this,SLOT(setPicture()));
    connect(ui -> actionClearPicture,SIGNAL(triggered()),this,SLOT(clearPicture()));
    connect(ui -> actionRegistration,SIGNAL(triggered()),this,SLOT(registration()));
    connect(ui -> actionModemReset,SIGNAL(triggered()),this,SLOT(reconnect()));
    connect(ui -> actionAbout,SIGNAL(triggered()),this,SLOT(showAbout()));
    ui -> actionSave -> setText(tr("Сохранить"));
    ui -> actionMode -> setText(tr("Сменить режим работы"));
    ui -> actionModemReset -> setText(tr("Переподключение к модему"));
    ui -> actionRegistration -> setText(tr("Регистрация команд"));
    ui -> actionQuit -> setText(tr("Выйти"));
    ui -> actionSetPicture -> setText(tr("Установить картинку"));
    ui -> actionClearPicture -> setText(tr("Сбросить картинку"));
    ui -> menuPicture -> setTitle(tr("Картинка"));
    ui -> menu -> setTitle(tr("Файл"));
    ui -> menuFonts -> setTitle(tr("Шрифты"));
    ui -> actionAbout -> setText(tr("О программе"));
    first_show = true;
    QString s;    
    QList <QFont> fonts;
    for (int i = 0; i < QTurnBasedQuiz::FONT_LAST; i++)
    {
        switch (i)
        {
        case QTurnBasedQuiz::FONT_CAPTION:
            s = tr("Шрифт заголовка");
            break;
        case QTurnBasedQuiz::FONT_QUESTION:
            s = tr("Шрифт вопроса");
            break;
        case QTurnBasedQuiz::FONT_TIME:
            s = tr("Шрифт времени");
            break;
        case QTurnBasedQuiz::FONT_ANSWER:
            s = tr("Шрифт ответа");
            break;
        case QTurnBasedQuiz::FONT_QUEUE:
            s = tr("Шрифт очереди");
            break;
        case QTurnBasedQuiz::FONT_BUTTON:
            s = tr("Шрифт кнопки");
            break;
        }
        fonts.append(font());      
        font_actions.append(new QAction(s,this));
        connect(font_actions.last(),SIGNAL(triggered()),this,SLOT(changeFont()));
    }
    games.at(0)-> setFonts(fonts);
    ui -> menuFonts -> addActions(font_actions);
    set_interface_status(false);
}

MainWindow::~MainWindow()
{
    if (auto_connect)
       {
           delete auto_connect;
           auto_connect = NULL;
       }
    delete im;
    while (games.count())
    {
        delete games.first();
        games.removeFirst();
    }
    while (font_actions.count())
    {
        delete font_actions.first();
        font_actions.removeFirst();
    }
    delete gamesTabWidget;
    delete ui;
}


void MainWindow::showEvent (QShowEvent * event)
{
    QMainWindow::showEvent(event);
    event -> accept();
    if (first_show)
    {
        QList<QFont> tmp_fonts;
        emit signal_start_autoconnect();
        first_show = false;
        if (QDbManipulator::loadConfiguration(reg_devices, tmp_fonts))
            games.at(0) -> setFonts(tmp_fonts);
        selectWorkMode();
    }
}


void MainWindow::uiSetupGames(const QList<QGame *> games)
{
    while(this -> games.count())
    {
        delete this -> games.first();
        this -> games.removeFirst();
    }
    foreach(QGame * game,games)
    {
        gamesTabWidget -> addTab(game,game -> name());
        connect(this,SIGNAL(signalRCClicked(uint,unsigned short)),game,SIGNAL(signalRCClicked(uint,unsigned short)));
    }
    this -> games = games;
}



void MainWindow::saveJSON()
{
    QString error;
    if (json_file.isEmpty())
        json_file = QFileDialog::getSaveFileName(this,tr("Сохранение файла базы данных"), "","*.json");
    if (json_file.isEmpty())
        return;
    QDbManipulator::save(json_file,games,error);
}


void MainWindow::loadJSON()
{
    QString error;
    QString json_file = QFileDialog::getOpenFileName(this,tr("Загрузка файла базы данных"), "","*.json");
    if (json_file.isEmpty())
        return;
    QList<QGame *> games = QDbManipulator::load(json_file,error,this);
    uiSetupGames(games);
    if (error.isEmpty())
        this -> json_file = json_file;
}


void MainWindow::selectWorkMode()
{
    QStringList choices;
    choices << tr("Режим игры") << tr("Создание базы вопросов") << tr("Редактирование базы вопросов");
    QChoiceDialog * cd = new QChoiceDialog(this,QChoiceDialog::CHOICE_SINGLE,choices,tr("Выбор режима работы"),tr("Выберите режим работы викторины"));
    cd -> exec();
    QList<int> result = cd -> choiceResult();
    delete cd;
    if (result.isEmpty())
    {
        ui -> actionSave -> setVisible(false);
        return;
    }
    switch (result.at(0))
    {
    case 0:
        setWindowState(Qt::WindowFullScreen);
        loadJSON();
        foreach (QGame * game,games)
            game -> setEditable(false);
        ui -> actionSave -> setVisible(false);
        break;

    case 1:
    {
        setWindowState(Qt::WindowMaximized);
        dynamic_cast<QTurnBasedQuiz *>(games.at(0)) -> setThemes(QList<QTheme *>());
        foreach (QGame * game, games)
            game -> setEditable(true);
        json_file.clear();
        ui -> actionSave -> setVisible(true);
        games.at(0) -> setImage(QImage());
        break;
    }

    case 2:
        setWindowState(Qt::WindowMaximized);
        loadJSON();
        foreach (QGame * game,games)
            game -> setEditable(true);
        ui -> actionSave -> setVisible(true);
        break;
    }
}




void MainWindow::set_interface_status(bool value)
{

}


void MainWindow::rs232_cmd_income(QByteArray buf)
{
    QRS232Message mess(NULL,buf);
    //если не смогли распарсить
    if (mess.parse() != QRS232Message::PARSE_OK)
    {
        return;
    }
    if (mess.getCommand() == 0x1509)
    {
        QByteArray cmd_data = mess.netroCommand();
        if (cmd_data.count() > sizeof(uint32) + sizeof(uint16))
        {
            uint32 mac;
            uint16 group;
            memcpy(&mac,cmd_data.data(),sizeof(uint32));
            memcpy(&group,cmd_data.data() + sizeof(uint32),sizeof(uint16));
            emit signalRCClicked(mac,group);
        }
    }
}


void MainWindow::start_autoconnect()
{
    //отключаем возможность нового коннекта
    disconnect(this,SIGNAL(signal_start_autoconnect()),this,SLOT(start_autoconnect()));
    //ui_autoconnect_label_set_visible(true);
    //modem.setCurrentTransaction(QModem::MODEM_AUTOCONNECT_VERSION);
    auto_connect = new QAutoConnectTool(this,im);
    connect(auto_connect,SIGNAL(signalNoPortFound()),this,SLOT(no_modem_on_port()),Qt::QueuedConnection);
    connect(auto_connect,SIGNAL(signalPortFound(QString)),this,SLOT(modem_on_port()),Qt::QueuedConnection);
    auto_connect -> findDevicePort(true);
    ui -> actionModemReset -> setEnabled(false);
}


//модем на портах не обнаружен
void MainWindow::no_modem_on_port()
{
    if (auto_connect)
        auto_connect -> findDevicePort(true);
    ui -> menuModemOnline -> setTitle(tr("Модем отключен"));
    ui -> actionModemReset -> setEnabled(false);
}


//модем  обнаружен
void MainWindow::modem_on_port()
{
    uint16 soft,hard;
    if (auto_connect)
    {
        soft = auto_connect -> getVersionSoft();
        hard = auto_connect -> getVersionHard();
        delete auto_connect;
        auto_connect = NULL;
        ui -> menuModemOnline -> setTitle(tr("Модем подключен"));
        ui -> actionModemReset -> setEnabled(true);
    }

}


void MainWindow::setPicture()
{
    QString pic_file = QFileDialog::getOpenFileName(this,tr("Загрузка файла картинки"), "","*.jpg;*.png;*.jpeg");
    if (pic_file.isEmpty())
        return;
    QImage picture;
    picture.load(pic_file);
    games.at(0) -> setImage(picture);
}


void MainWindow::clearPicture()
{
    games.at(0) -> setImage(QImage());
}


void MainWindow::changeFont()
{
    bool ok;
    QFont new_font;
    QList<QFont> fonts;
    for (int i = 0; i < font_actions.size(); i++)
        if (sender() == font_actions.at(i))
        {
            fonts = games.at(0) -> fonts();
            new_font = QFontDialog::getFont(&ok,fonts.at(i),this);
            if (ok)
            {

                fonts.replace(i,new_font);
                games.at(0) -> setFonts(fonts);
                QDbManipulator::saveConfiguration(reg_devices,fonts);
            }
            break;
        }
}


void MainWindow::registration()
{
    QRegistrationDialog * rd;
    rd = new QRegistrationDialog(this,reg_devices);
    connect(this,SIGNAL(signalRCClicked(uint)),rd,SLOT(rcLicked(uint)));
    if (rd -> exec() == QDialog::Accepted)
    {
        reg_devices = rd -> getRegDevices();
        foreach(QGame * game,games)
            game -> setRCList(reg_devices);
        QDbManipulator::saveConfiguration(reg_devices,games.at(0) -> fonts());
    }
    delete rd;
}


void MainWindow::reconnect()
{
    start_autoconnect();
}

void MainWindow::showAbout()
{
    QAboutDialog * ad = new QAboutDialog(this,PROG_VERSION);
    ad -> exec();
    delete ad;
}

void MainWindow::formResize()
{
    foreach(QGame * game,games)
        game -> changeSize();
}
