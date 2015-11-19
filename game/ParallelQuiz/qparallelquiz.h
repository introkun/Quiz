#ifndef QPARALLELQUIZ_H
#define QPARALLELQUIZ_H

#include <QWidget>
#include <QLayout>
#include "../qgame.h"
#include "../../qquestionstablewidget.h"

class QParallelQuiz : public QGame
{
    Q_OBJECT
public:
    QParallelQuiz(QWidget *parent = 0);
    ~QParallelQuiz();
    virtual QString name() const;
    virtual void setEditable(bool value);
    virtual void changeSize();

    //Получить карту json кода игры
    virtual QVariantMap getJsonData();
    //Настроить игру из карты JSON
    virtual  bool setFromJsonData(const QVariantMap & map);
protected:
    virtual void showEvent(QShowEvent * event);
private:
    QQuestionsTableWidget * questionsTable;
    QGridLayout * layoutAnswers, * layoutCommands;
    QVBoxLayout * layoutMain;
};

#endif // QPARALLELQUIZ_H
