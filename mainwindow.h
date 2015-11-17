#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpacerItem>
#include <QImage>
#include "game/TurnBasedQuiz/qtheme.h"
#include "interface_manager.h"
#include "qautoconnecttool.h"
#include "qregistrationdialog.h"
#include "game/TurnBasedQuiz/qturnbasedquiz.h"
#include "event_filter.h"
#define PROG_VERSION "1.01"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void saveJSON();
    void loadJSON();
    void selectWorkMode();
    void questionClicked(QQuestionWidget * widget,Qt::MouseButton button);
    void set_interface_status(bool value);
    void rs232_cmd_income(QByteArray buf);
    void start_autoconnect();
    void no_modem_on_port();
    void modem_on_port();
    void setPicture();
    void clearPicture();
    void changeFont();
    void registration();
    void reconnect();
    void showAbout();
    void formResize();
protected:
    virtual void showEvent (QShowEvent * event);
signals:
    void signal_start_autoconnect();
    void signalRCClicked(unsigned int);
private:
    typedef enum {
        FONT_CAPTION = 0,
        FONT_QUESTION,
        FONT_TIME,
        FONT_ANSWER,
        FONT_QUEUE,
        FONT_BUTTON,
        FONT_LAST
    } FONT_ACTIONS;
    Ui::MainWindow *ui;

    bool first_show;
    QString json_file;
    QInterface_Manager * im;
    QAutoConnectTool * auto_connect;
    QList <QFont> fonts;
    QList <QAction *> font_actions;
    QList<QRegistrationDialog::REG_DEVICE_T> reg_devices;
    QList<QGame *> games;
    QTabWidget * gamesTabWidget;
    Event_Filter * ev_fltr;
};

#endif // MAINWINDOW_H
