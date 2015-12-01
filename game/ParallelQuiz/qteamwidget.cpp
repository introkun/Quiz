#include "qteamwidget.h"

QTeamWidget::QTeamWidget(QWidget *parent, uint id, const QString & team_name) :
    QFrame(parent)
{
    _id = id;

    layout = new QVBoxLayout(this);
    layout_main = new QHBoxLayout();
    layout_info = new QVBoxLayout();
    label_name = new QLabel("<h2>" + team_name + "</h2>");//Название команды
    label_quest_status = new QLabel();//метка отображения ответа на текущий вопрос
    label_quest_true = new QLabel();//метка отображения количества правильных ответов
    label_quest_false = new QLabel();//метка отображения количества неправильных ответов
    label_quest_unknown = new QLabel();//метка отображения количества неотвеченных

    layout -> addWidget(label_name);
    layout -> addLayout(layout_main);
    layout_main -> addLayout(layout_info);
    layout_main -> addWidget(label_quest_status);
    layout_info -> addWidget(label_quest_true);
    layout_info -> addWidget(label_quest_false);
    layout_info -> addWidget(label_quest_unknown);
    label_quest_status -> setFixedSize(100,100);
    label_name -> setAlignment(Qt::AlignHCenter);
    setFrameShape(QFrame::Box);
    clearResults();
}


QTeamWidget::~QTeamWidget()
{
    delete label_quest_status;
    delete label_quest_true;
    delete label_quest_false;
    delete label_quest_unknown;
    delete label_name;
    delete layout_info;
    delete layout_main;
    delete layout;
}


void QTeamWidget::clearResults()
{
    quest_true = 0;
    quest_false = 0;
    quest_unknown = 0;
    unknown = false;
    updateInfo();
}


void QTeamWidget::updateInfo()
{
    label_quest_false -> setText("<h3><font color = ""red"">" + tr("Неправильно: %1").arg(QString::number(quest_false)) + "</font></h3>");
    label_quest_true -> setText("<h3><font color = ""green"">" + tr("Правильно: %1").arg(QString::number(quest_true)) + "</font></h3>");
    label_quest_unknown -> setText("<h3><font color = ""black"">" + tr("Не отвечено: %1").arg(QString::number(quest_unknown)) + "</font></h3>");
}

void QTeamWidget::increaseTrue()
{
    label_quest_status -> setPixmap(QPixmap(":/images/good_answer.png").scaled(label_quest_status -> size()));
    unknown = false;
    quest_true++;
    updateInfo();
}

void QTeamWidget::increaseFalse()
{
    label_quest_status -> setPixmap(QPixmap(":/images/bad_answer.png").scaled(label_quest_status -> size()));
    unknown = false;
    quest_false++;
    updateInfo();
}

void QTeamWidget::prepareForQuestion()
{
    if (unknown)
        quest_unknown++;
    unknown = true;
    label_quest_status -> setPixmap(QPixmap(":/images/unknown_answer.png").scaled(label_quest_status -> size()));
    updateInfo();
}
