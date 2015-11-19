#include <QComboBox>
#include "qquestionstablewidget.h"

QQuestionsTableWidget::QQuestionsTableWidget(QWidget *parent,bool rating) : QTableWidget(parent)
{
    table_width = -1;
    QStringList sl;
    sl << tr("Вопрос") << tr("Ответ №1") << tr("Ответ №2") << tr("Ответ №3") << tr("Ответ №4") << tr("Правильный ответ") << tr("Сложность");
    setColumnCount(7);
    setHorizontalHeaderLabels(sl);
    initMenu();
    setSelectionBehavior(SelectRows);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (!rating)
        setColumnHidden(6,true);

}

QQuestionsTableWidget::~QQuestionsTableWidget()
{
    deleteMenu();
}


void QQuestionsTableWidget::initMenu()
{
    QAction * action;
    menu = new QMenu(this);

    action = menu -> addAction(QIcon(":/images/plus.png"),tr("Добавить вопрос"));
    action -> setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    action -> setShortcutContext(Qt::WidgetShortcut);
    connect(action,SIGNAL(triggered()),this,SLOT(addQuestionRow()));

    action = menu -> addAction(QIcon(":/images/minus.png"),tr("Удалить вопрос"));
    action -> setShortcut(QKeySequence(Qt::Key_Delete));
    action -> setShortcutContext(Qt::WidgetShortcut);
    connect(action,SIGNAL(triggered()),this,SLOT(deleteQuestionRow()));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(menuPopUp()));
}



void QQuestionsTableWidget::deleteMenu()
{
    QList<QAction *> actions = menu -> actions();
    for (int i = 0; i < actions.count(); i++)
        delete actions.at(i);
    delete menu;
}

void QQuestionsTableWidget::menuPopUp()
{
    QList<QAction *> actions = menu -> actions();
    actions.at(1) -> setVisible(selectedItems().size());
    actions.at(0) -> setText(tr("Добавить вопрос"));
    actions.at(1) -> setText(tr("Удалить вопрос"));
    menu -> popup(QCursor::pos());
}

void QQuestionsTableWidget::addQuestionRow()
{
    insertRow(rowCount());
    for (int i = 0; i < 5; i++)
        setItem(rowCount() - 1,i,new QTableWidgetItem());
    QComboBox * combobox = new QComboBox();
    for (int i = 1; i < 5; i++)
        combobox -> addItem(QString::number(i));
    setCellWidget(rowCount() - 1,5,combobox);
    combobox = new QComboBox();
    for (int i = 1; i < 15; i++)
        combobox -> addItem(QString::number(i));
    setCellWidget(rowCount() - 1,6,combobox);
}


void QQuestionsTableWidget::deleteQuestionRow()
{
    QList<int> rows;
    QList<QTableWidgetItem *> sil = selectedItems();
    for (int row = rowCount() - 1; row >= 0 ; row--)
        foreach (QTableWidgetItem * twi,sil)
            if (item(row,0) == twi)
            {
                rows.append(row);
                break;
            }

    foreach(int row,rows)
        removeRow(row);
}


void QQuestionsTableWidget::setQuestions(const QList<QQuestion> & questions)
{
    QStringList answers;
    clearContents();
    setSortingEnabled(false);
    blockSignals(true);
    model() -> blockSignals(true);
    foreach(QQuestion quest,questions)
    {
        answers = quest.answers();
        if (answers.count() != 4)
            continue;
        addQuestionRow();
        item(rowCount() - 1,0) -> setText(quest.question());
        item(rowCount() - 1,1) -> setText(answers.at(0));
        item(rowCount() - 1,2) -> setText(answers.at(1));
        item(rowCount() - 1,3) -> setText(answers.at(2));
        item(rowCount() - 1,4) -> setText(answers.at(3));
        dynamic_cast<QComboBox *>(cellWidget(rowCount() - 1,5)) -> setCurrentText(QString::number(quest.goodAnswerIndex()));
        dynamic_cast<QComboBox *>(cellWidget(rowCount() - 1,6)) -> setCurrentText(QString::number(quest.rating()));
    }
    setSortingEnabled(true);
    blockSignals(false);
    model() -> blockSignals(false);

}

QList<QQuestion> QQuestionsTableWidget::questions() const
{
    QList<QQuestion> result_questions;
    QQuestion question;
    int i,j;
    for (i = 0; i < rowCount(); i++)
    {
        //проверяем на правильную заполненность
        for (j = 0; j < 5; j++)
            if (item(i,j) -> text().isEmpty())
                break;
        if (j >= 5)
        {
            QStringList answers;
            question.setGoodAnswerIndex(dynamic_cast<QComboBox *>(cellWidget(i,5)) -> currentText().toInt());
            question.setQuestion(item(i,0) -> text());
            for (int k = 0; k < 4; k++)
                answers.append(item(i,k) -> text());
            question.setAnswers(answers);
            question.setRating(dynamic_cast<QComboBox *>(cellWidget(i,5)) -> currentText().toInt());
            result_questions.append(question);
        }
    }
    return result_questions;
}


void QQuestionsTableWidget::changeSize()
{
    //перерисовка таблиц
    if (table_width != viewport() -> width())
    {
        setColumnWidth(0, (viewport() -> width()) * (isColumnHidden(6) ? 0.5 : 0.4));
        setColumnWidth(1, viewport() -> width() * 0.1);
        setColumnWidth(2, viewport() -> width() * 0.1);
        setColumnWidth(3, viewport() -> width() * 0.1);
        setColumnWidth(4, viewport() -> width() * 0.1);
        setColumnWidth(5, viewport() -> width() * 0.1);
        if (!isColumnHidden(6))
            setColumnWidth(6, viewport() -> width() * 0.1);
        table_width = viewport() -> width();
    }
}
