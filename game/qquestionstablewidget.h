#ifndef QQUESTIONSTABLEWIDGET_H
#define QQUESTIONSTABLEWIDGET_H

#include <QTableWidget>
#include <QMenu>
#include "qquestion.h"
class QQuestionsTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit QQuestionsTableWidget(QWidget *parent = 0,bool rating = false);
    ~QQuestionsTableWidget();
    void setQuestions(const QList<QQuestion> & questions);
    QList<QQuestion>  questions() const;
    void changeSize();
private slots:
    void menuPopUp();
    void addQuestionRow();
    void deleteQuestionRow();
private:
    void initMenu();
    void deleteMenu();

    QMenu * menu;
    int table_width;//текущая ширина таблицы вопросов в режиме редактирования
};

#endif // QQUESTIONSTABLEWIDGET_H
