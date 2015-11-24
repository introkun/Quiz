#include "../../qobjecthelper.h"
#include "qparallelquiz.h"

QParallelQuiz::QParallelQuiz(QWidget *parent) :
    QGame(parent)
{
    _gameType = GAME_PARALLELQUIZ;
    layoutMain = new QVBoxLayout();
    layoutMain -> setContentsMargins(1,1,1,1);
    layoutMain -> setSpacing(5);
    layoutCommands = new QGridLayout();
    layoutCommands -> setContentsMargins(1,1,1,1);
    layoutCommands -> setSpacing(5);
    layoutAnswers = new QGridLayout();
    layoutAnswers -> setContentsMargins(1,1,1,1);
    layoutAnswers -> setSpacing(1);
    vspacer = new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding);
    setLayout(layoutMain);
    labelQuestion = new QLabel();
    labelQuestion -> setAlignment(Qt::AlignHCenter);
    labelQuestion -> setWordWrap(true);
    questionsTable = new QQuestionsTableWidget();
    for (int i = 0; i < 4; i++)
    {
        labelsAnswer.append(new QLabel("<h2>" + tr("Ответ %1").arg(QString::number(i))+ "</h2>"));
        labelsAnswer.last() -> setAlignment(Qt::AlignCenter);
        labelsAnswer.last() -> setFixedHeight(120);
        labelsAnswer.last() -> setStyleSheet("QLabel{border:5px solid grey; border-radius: 15px;}");
        layoutAnswers -> addWidget(labelsAnswer.last(), i / 2, i % 2);
    }

    layoutMain -> addWidget(questionsTable);
    layoutMain -> addLayout(layoutCommands);
    layoutMain -> addSpacerItem(vspacer);
    layoutMain -> addWidget(labelQuestion);
    layoutMain -> addLayout(layoutAnswers);
    setEditable(false);
}

QParallelQuiz::~QParallelQuiz()
{
    layoutMain -> removeItem(vspacer);
    delete vspacer;
    delete questionsTable;
    delete layoutCommands;
    delete layoutAnswers;
    delete layoutMain;
    foreach (QTeamWidget * team,teams)
        delete team;
}

void QParallelQuiz::showEvent(QShowEvent * event)
{
    changeSize();
    QGame::showEvent(event);

}

void QParallelQuiz::changeSize()
{
    questionsTable -> changeSize();
}

QString QParallelQuiz::name() const
{
    return tr("Командная эстафета");
}


void QParallelQuiz::setEditable(bool value)
{
    QGame::setEditable(value);
    questionsTable -> setVisible(value);
    foreach (QTeamWidget * team,teams)
        team -> setVisible(!value);
    foreach(QLabel * labelAnswer,labelsAnswer)
        labelAnswer -> setVisible(!value);
    if (value)
        vspacer -> changeSize(1,1);
    else
        vspacer -> changeSize(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding);
    labelQuestion -> setVisible(!value);
    adjustSize();
    if (!value )
    {
        QList<QQuestion> quest_list = questionsTable -> questions();
        if (quest_list.count())
        {
            labelQuestion -> setText("<h1>" + quest_list.first().question() + "</h1>");
            for (int i = 0; i < quest_list.first().answers().count(); i++)
                labelsAnswer.at(i) -> setText("<h2>" + tr("%1) %2").arg(QString::number(i + 1),quest_list.first().answers().at(i))+ "</h2>");
        }
    }
}


void QParallelQuiz::setRCList(const QList<QRegistrationDialog::REG_DEVICE_T> & rc_list)
{
    QGame::setRCList(rc_list);
    //пока что при смене регистрации тупо удаляем все виджеты и создаем снова
    foreach (QTeamWidget * team,teams)
        delete team;
    teams.clear();
    int i = 0;
    foreach(QRegistrationDialog::REG_DEVICE_T rc,rc_list)
    {
        teams.append(new QTeamWidget(this,rc.mac,rc.name));
        layoutCommands -> addWidget(teams.last(),i / 4, i % 4);
        teams.last() -> prepareForQuestion();
        i++;
    }
    setEditable(edit_mode);
}


//Получить карту json кода игры
QVariantMap QParallelQuiz::getJsonData() const
{
    QVariantMap game_map;
    game_map["type"] = _gameType;
    QList<QQuestion> questions = questionsTable -> questions();
    QVariantList questions_list;
    foreach(QQuestion question,questions)
        questions_list.append(QObjectHelper::qobject2qvariant(&question));

    game_map["questions"] = questions_list;
    game_map["fonts"] = getJSonFonts();
    return game_map;
}

//Настроить игру из карты JSON
bool QParallelQuiz::setFromJsonData(const QVariantMap & map)
{
    QQuestion question;
    QVariantList question_list = map["questions"].toList();
    QList<QQuestion> questions;
    foreach(QVariant var_question, question_list)
    {
        QObjectHelper::qvariant2qobject(var_question.toMap(),&question);
        questions.append(question);
    }
    questionsTable -> setQuestions(questions);

    if (map["picture"].isValid())
        picture = QImage::fromData(QByteArray::fromBase64(map["picture"].toByteArray()),"PNG");

    if (map["fonts"].isValid())
        setFromJSonFonts(map["fonts"].toList());
    return true;
}

