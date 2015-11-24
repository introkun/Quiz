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
    //Получить карту json кода игры
    virtual QVariantMap getJsonData() const;
    //Настроить игру из карты JSON
    virtual  bool setFromJsonData(const QVariantMap & map);
protected:
    virtual void showEvent(QShowEvent * event);
private:
    QQuestionsTableWidget * questionsTable;
    QGridLayout * layoutAnswers, * layoutCommands;
    QVBoxLayout * layoutMain;
    QSpacerItem * vspacer;
    QList<QTeamWidget *> teams;
    QPushButton * mainButton;
    QLabel * labelQuestion;
    QLabel * labelTime;
    QList<QLabel *> labelsAnswer;
};

#endif // QPARALLELQUIZ_H
