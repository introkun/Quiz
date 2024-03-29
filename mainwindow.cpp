#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdbmanipulator.h"
#include "qchoicedialog.h"
#include "qaboutdialog.h"
#include "version.h"
#include "game/qgamesettingsdialog.h"
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
    connect(ui -> actionRegistration,SIGNAL(triggered()),this,SLOT(registration()));
    connect(ui -> actionModemReset,SIGNAL(triggered()),this,SLOT(reconnect()));
    connect(ui -> actionGameSettings,SIGNAL(triggered()),this,SLOT(changeGamesSettings()));
    connect(ui -> actionAbout,SIGNAL(triggered()),this,SLOT(showAbout()));
    ui -> actionSave -> setText(tr("Сохранить"));
    ui -> actionMode -> setText(tr("Сменить режим работы"));
    ui -> actionModemReset -> setText(tr("Переподключение к модему"));
    ui -> actionRegistration -> setText(tr("Регистрация команд"));
    ui -> actionQuit -> setText(tr("Выйти"));
    ui -> actionSetPicture -> setText(tr("Установить картинку"));
    ui -> actionClearPicture -> setText(tr("Сбросить картинку"));
    ui -> menu -> setTitle(tr("Файл"));
    ui -> menuSettings -> setTitle(tr("Настройки"));
    ui -> actionGameFonts -> setText(tr("Игровые шрифты"));
    ui -> actionGameSettings -> setText(tr("Игровые настройки"));
    ui -> actionAbout -> setText(tr("О программе"));
    first_show = true;
    need_save = false;
    set_interface_status(false);
    updateWindowTitle();
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
        emit signal_start_autoconnect();
        first_show = false;
        QDbManipulator::loadConfiguration(reg_devices);
        selectWorkMode();
    }
}


void MainWindow::closeEvent(QCloseEvent * event)
{
    while (need_save)
    {
        if (QMessageBox::warning(this,tr("Внимание"),tr("Текущие изменения в играх не сохранены. Сохранить?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
            saveJSON();
        else
            break;
    }
    QMainWindow::closeEvent(event);
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
    if (QDbManipulator::save(json_file,games,error))
    {
        need_save = false;
        updateWindowTitle();
    }

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
    while (need_save)
    {
        if (QMessageBox::warning(this,tr("Внимание"),tr("Текущие изменения в играх не сохранены. Сохранить?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
            saveJSON();
        else
            break;
    }
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
        {
            game -> setEditable(false);
            game -> setRCList(reg_devices);
        }
        ui -> actionSave -> setVisible(false);
        break;

    case 1:
    {
        setWindowState(Qt::WindowMaximized);
        dynamic_cast<QTurnBasedQuiz *>(games.at(0)) -> setThemes(QList<QTheme *>());
        foreach (QGame * game, games)
        {
            game -> setFromJsonData(QVariantMap());
            game -> setEditable(true);
        }
        json_file.clear();
        ui -> actionSave -> setVisible(true);
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
    need_save = false;
    updateWindowTitle();
    foreach (QGame * game, games)
    {
        disconnect(game,SIGNAL(signalNeedSave()),this,SLOT(needSaveGames()));
        connect(game,SIGNAL(signalNeedSave()),this,SLOT(needSaveGames()));
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
        if (cmd_data.count() > sizeof(uint32) + sizeof(uint16) + sizeof(uint8) + sizeof(uint8))
        {
            uint32 mac;
            uint16 group;
            uint8 operation;
            memcpy(&mac,cmd_data.data(),sizeof(uint32));
            memcpy(&group,cmd_data.data() + sizeof(uint32),sizeof(uint16));
            operation = cmd_data.at(sizeof(uint32) + sizeof(uint16) + sizeof(uint8));
            //моделируем группу четырехканального пульта с двухканального
            switch (operation)
            {
            case 2://вверх
                group = (group - 1) * 2 + 1;
                break;
            case 3://вниз
                group *= 2;
                break;
            case 6://сценарий
            default:
                //для пультов 8501-4М
                if (group == 3) group = 4;
                else if (group == 4) group = 3;
                break;
            }

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


void MainWindow::changeGamesSettings()
{
    QGameSettingsDialog * gfd = new QGameSettingsDialog(this,games);
    connect(gfd,SIGNAL(signalNeedSave()),this,SLOT(needSaveGames()));
    gfd -> exec();
    delete gfd;
}


void MainWindow::registration()
{
    QRegistrationDialog * rd;
    rd = new QRegistrationDialog(this,reg_devices);
    connect(this,SIGNAL(signalRCClicked(uint,unsigned short)),rd,SLOT(rcLicked(uint)));
    if (rd -> exec() == QDialog::Accepted)
    {
        reg_devices = rd -> getRegDevices();
        foreach(QGame * game,games)
            game -> setRCList(reg_devices);
        QDbManipulator::saveConfiguration(reg_devices);
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


void MainWindow::needSaveGames()
{
    need_save = true;
    updateWindowTitle();
}


void MainWindow::updateWindowTitle()
{
    setWindowTitle(QString("Quiz v.%1.%2 %3%4").arg(QString::number(VERSION_MAJOR),VERSION_MINOR,json_file,need_save ? QString("*") : QString()));
}
