#include "qquestionstablewidget.h"

QQuestionsTableWidget::QQuestionsTableWidget(QWidget *parent) : QTableWidget(parent)
{
    QStringList sl;
    sl << tr("Вопрос") << tr("Ответ №1") << tr("Ответ №2") << tr("Ответ №3") << tr("Ответ №4") << tr("Праильный ответ");
    setColumnCount(6);
    setHorizontalHeaderLabels(sl);
}

QQuestionsTableWidget::~QQuestionsTableWidget()
{

}

