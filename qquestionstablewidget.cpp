#include <QComboBox>
#include "qquestionstablewidget.h"

QQuestionsTableWidget::QQuestionsTableWidget(QWidget *parent) : QTableWidget(parent)
{
    QStringList sl;
    sl << tr("Вопрос") << tr("Ответ №1") << tr("Ответ №2") << tr("Ответ №3") << tr("Ответ №4") << tr("Правильный ответ");
    setColumnCount(6);
    setHorizontalHeaderLabels(sl);
    initMenu();
    setSelectionBehavior(SelectRows);

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
    QComboBox * combobox = new QComboBox();
    for (int i = 1; i < 5; i++)
        combobox -> addItem(QString::number(i));
    setCellWidget(rowCount() - 1,5,combobox);
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

}

QList<QQuestion> QQuestionsTableWidget::questions() const
{

}


