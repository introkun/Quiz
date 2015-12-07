#ifndef QPARALLELQUIZ_H
#define QPARALLELQUIZ_H

#include <QWidget>
#include <QLayout>
#include <QSpacerItem>
#include "../qgame.h"
#include "../qquestionstablewidget.h"
#include "qteamwidget.h"
class QParallelQuiz : public QGame
{
    Q_OBJECT
public:
    QParallelQuiz(QWidget *parent = 0);
    ~QParallelQuiz();
    virtual QString name() const;
    virtual void setEditable(bool value);
    virtual void changeSize();
    virtual void setRCList(const QList<QRegistrationDialog::REG_DEVICE_T> & rc_list);
    virtual void setFonts(const QList<GAME_FONT> & new_fonts);

    //Получить карту json кода игры
    virtual QVariantMap getJsonData() const;
    //Настроить игру из карты JSON
    virtual  bool setFromJsonData(const QVariantMap & map);
protected:
    virtual void showEvent(QShowEvent * event);
private:
    typedef enum {
        FONT_CAPTION = 0,
        FONT_QUESTION,
        FONT_TIME,
        FONT_ANSWER,
        FONT_BUTTON,
        FONT_TEAM_NAME,
        FONT_TEAM_RESULTS,
        FONT_LAST
    } FONT_ACTIONS;

    QQuestionsTableWidget * questionsTable;
    QGridLayout * layoutAnswers, * layoutCommands;
    QVBoxLayout * layoutMain;
    QSpacerItem * vspacer;
    QList<QTeamWidget *> teams;
    QPushButton * mainButton;
    QLabel * labelCaption;
    QLabel * labelQuestion;
    QLabel * labelTime;
    QList<QLabel *> labelsAnswer;
};

#endif // QPARALLELQUIZ_H
