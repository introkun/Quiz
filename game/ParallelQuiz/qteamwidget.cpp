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
    spacer = new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Preferred);

    layout -> addWidget(label_name);
    layout -> addLayout(layout_main);
    layout_main -> addLayout(layout_info);
    label_quest_true -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    label_quest_false -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    label_quest_unknown -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    layout_main -> addWidget(label_quest_status,0,Qt::AlignCenter);
    layout_info -> addWidget(label_quest_true,0,Qt::AlignTop);
    layout_info -> addWidget(label_quest_false,0,Qt::AlignTop);
    layout_info -> addWidget(label_quest_unknown,0,Qt::AlignTop);
    layout_info -> addSpacerItem(spacer);
    label_quest_status -> setMinimumSize(100,100);
    label_quest_status -> setSizeIncrement(1,1);
    label_name -> setAlignment(Qt::AlignHCenter);
    setFrameShape(QFrame::Box);
    clearResults();
}


QTeamWidget::~QTeamWidget()
{
    layout_info -> removeItem(spacer);
    delete spacer;
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
    label_quest_status -> setPixmap(QPixmap(":/images/unknown_answer.png").scaled(label_quest_status -> size(),Qt::KeepAspectRatio));
    updateInfo();
}


void QTeamWidget::setFonts(const QFont & team_font, const QFont & results_font)
{
    label_name -> setFont(team_font);
    label_quest_false -> setFont(results_font);
    label_quest_true -> setFont(results_font);
    label_quest_unknown -> setFont(results_font);
}
