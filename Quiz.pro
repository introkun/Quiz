#-------------------------------------------------
#
# Project created by QtCreator 2015-10-05T14:37:24
#
#-------------------------------------------------

QT       += core gui serialport
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Quiz
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qobjecthelper.cpp \
    qdbmanipulator.cpp \
    qchoicedialog.cpp \
    qclickedlabel.cpp \
    interface_manager.cpp \
    qautoconnecttool.cpp \
    rs232_port.cpp \
    message/qinterfacemessage.cpp \
    message/qslipmessage.cpp \
    message/rs232message.cpp \
    qregistrationdialog.cpp \
    qaboutdialog.cpp \
    game/qgame.cpp \
    game/TurnBasedQuiz/qquestiondialog.cpp \
    game/TurnBasedQuiz/qquestionwidget.cpp \
    game/TurnBasedQuiz/qtheme.cpp \
    game/TurnBasedQuiz/qthemewidget.cpp \
    game/TurnBasedQuiz/qturnbasedquiz.cpp \
    game/ParallelQuiz/qparallelquiz.cpp \
    event_filter.cpp \
    game/ParallelQuiz/qteamwidget.cpp \
    game/qquestion.cpp \
    game/qquestionstablewidget.cpp \
    game/qgamesettingsdialog.cpp \
    game/qgamesettingswidget.cpp

HEADERS  += mainwindow.h \
    qobjecthelper.h \
    qdbmanipulator.h \
    qchoicedialog.h \
    qclickedlabel.h \
    interface_manager.h \
    qautoconnecttool.h \
    rs232_port.h \
    typedefs.h \
    message/qinterfacemessage.h \
    message/qslipmessage.h \
    message/rs232message.h \
    qregistrationdialog.h \
    qaboutdialog.h \
    game/qgame.h \
    game/TurnBasedQuiz/qquestiondialog.h \
    game/TurnBasedQuiz/qquestionwidget.h \
    game/TurnBasedQuiz/qtheme.h \
    game/TurnBasedQuiz/qthemewidget.h \
    game/TurnBasedQuiz/qturnbasedquiz.h \
    game/ParallelQuiz/qparallelquiz.h \
    qquestionstablewidget.h \
    event_filter.h \
    game/ParallelQuiz/qteamwidget.h \
    game/qquestion.h \
    game/qquestionstablewidget.h \
    game/qgamesettingsdialog.h \
    game/qgamesettingswidget.h \
    version.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
