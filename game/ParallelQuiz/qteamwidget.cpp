#include "qteamwidget.h"

QTeamWidget::QTeamWidget(QWidget *parent, uint id, const QString & team_name) :
    QFrame(parent)
{
    _id = id;

    layout = new QVBoxLayout(this);
    layout_main = new QHBoxLayout();
    layout_info = new QVBoxLayout();
    label_name = new QLabel(team_name);//Название команды
    label_quest_status = new QLabel();//метка отображения ответа на текущий вопрос
    label_quest_true = new QLabel();//метка отображения количества правильных ответов
    label_quest_false = new QLabel();//метка отображения количества неправильных ответов
    label_quest_unknown = new QLabel();//метка отображения количества неотвеченных

    layout -> addWidget(label_name);
    layout -> addLayout(layout_main);
    layout_main -> addLayout(layout_info);
    label_quest_true -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    label_quest_false -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    label_quest_unknown -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    layout_main -> addWidget(label_quest_status);
    layout_info -> addWidget(label_quest_true,0,Qt::AlignTop);
    layout_info -> addWidget(label_quest_false,0,Qt::AlignTop);
    layout_info -> addWidget(label_quest_unknown,0,Qt::AlignTop);
    label_quest_status -> setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    label_quest_status -> setAlignment(Qt::AlignCenter);
    label_name -> setAlignment(Qt::AlignHCenter);
    layout -> setSpacing(1);
    layout_info -> setSpacing(1);
    layout_main -> setSpacing(1);
    clearResults();
    setStyleSheet("QTeamWidget {border: 3px solid black;border-radius: 6px;background-color: rgba(56,174,203,127);}");
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
    label_quest_false -> setText("<font color = ""red"">" + tr("Неправильно: %1").arg(QString::number(quest_false)) + "</font>");
    label_quest_true -> setText("<font color = ""green"">" + tr("Правильно: %1").arg(QString::number(quest_true)) + "</font>");
    label_quest_unknown -> setText("<font color = ""black"">" + tr("Не отвечено: %1").arg(QString::number(quest_unknown)) + "</font>");
}

void QTeamWidget::increaseTrue()
{
    label_quest_status -> setPixmap(QPixmap(":/images/good_answer.png").scaled(label_quest_status -> size(),Qt::KeepAspectRatio));
    unknown = false;
    quest_true++;
    updateInfo();
}

void QTeamWidget::increaseFalse()
{
    label_quest_status -> setPixmap(QPixmap(":/images/bad_answer.png").scaled(label_quest_status -> size(),Qt::KeepAspectRatio));
    unknown = false;
    quest_false++;
    updateInfo();
}

void QTeamWidget::prepareForQuestion()
{
    if (unknown)
        quest_unknown++;
    unknown = true;
    label_quest_status -> setPixmap(QPixmap(":/images/unknown_answer.png").scaled(QSize(width(),label_quest_status -> height()),Qt::KeepAspectRatio));
    updateInfo();
}


void QTeamWidget::setFonts(const QFont & team_font, const QFont & results_font)
{
    label_name -> setFont(team_font);
    label_quest_false -> setFont(results_font);
    label_quest_true -> setFont(results_font);
    label_quest_unknown -> setFont(results_font);
}
