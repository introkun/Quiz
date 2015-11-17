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
private:
    QQuestionsTableWidget * questionsTable;
    QGridLayout * layoutAnswers, * layoutCommands;
    QVBoxLayout * layoutMain;
    int table_width;//текущая ширина таблицы вопросов в режиме редактирования
};

#endif // QPARALLELQUIZ_H
