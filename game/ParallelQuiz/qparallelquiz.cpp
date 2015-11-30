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
    labelCaption = new QLabel();
    labelCaption -> setAlignment(Qt::AlignHCenter);
    labelQuestion = new QLabel();
    labelQuestion -> setAlignment(Qt::AlignHCenter);
    labelQuestion -> setWordWrap(true);
    labelTime  = new QLabel();
    labelTime -> setAlignment(Qt::AlignHCenter);
    questionsTable = new QQuestionsTableWidget();
    mainButton = new QPushButton(tr("Начать"));
    for (int i = 0; i < 4; i++)
    {
        labelsAnswer.append(new QLabel(tr("Ответ %1").arg(QString::number(i))));
        labelsAnswer.last() -> setAlignment(Qt::AlignCenter);
        labelsAnswer.last() -> setFixedHeight(120);
        labelsAnswer.last() -> setStyleSheet("QLabel{border:5px solid grey; border-radius: 15px;}");
        layoutAnswers -> addWidget(labelsAnswer.last(), i / 2, i % 2);
    }

    layoutMain -> addWidget(questionsTable);
    layoutMain -> addLayout(layoutCommands);
    layoutMain -> addWidget(labelCaption);
    layoutMain -> addWidget(labelQuestion);
    layoutMain -> addWidget(labelTime);
    layoutMain -> addSpacerItem(vspacer);
    layoutMain -> addLayout(layoutAnswers);
    layoutMain -> addWidget(mainButton);
    setEditable(false);
    GAME_FONT _font;
    QString s;
    for (int i = 0; i < FONT_LAST; i++)
    {
        switch (i)
        {
        case FONT_CAPTION:
            s = tr("Шрифт заголовка");
            break;
        case FONT_QUESTION:
            s = tr("Шрифт вопроса");
            break;
        case FONT_TIME:
            s = tr("Шрифт времени");
            break;
        case FONT_ANSWER:
            s = tr("Шрифт ответа");
            break;
        case FONT_BUTTON:
            s = tr("Шрифт кнопки");
            break;
        }
        _font.name = s;
        _font.font = font();
        _fonts.append(_font);
    }

}

QParallelQuiz::~QParallelQuiz()
{
    foreach (QTeamWidget * team,teams)
        delete team;
    foreach(QWidget * widget,labelsAnswer)
        delete widget;
    layoutMain -> removeItem(vspacer);
    delete vspacer;
    delete questionsTable;
    delete labelTime;
    delete labelQuestion;
    delete labelCaption;
    delete mainButton;
    delete layoutCommands;
    delete layoutAnswers;
    delete layoutMain;
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
    labelCaption -> setVisible(!value);
    labelQuestion -> setVisible(!value);
    labelTime -> setVisible(!value);
    adjustSize();
    if (!value )
    {
        QList<QQuestion> quest_list = questionsTable -> questions();
        if (quest_list.count())
        {
            labelQuestion -> setText(quest_list.first().question());
            for (int i = 0; i < quest_list.first().answers().count(); i++)
                labelsAnswer.at(i) -> setText(tr("%1) %2").arg(QString::number(i + 1),quest_list.first().answers().at(i)));
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


void QParallelQuiz::setFonts(const QList<GAME_FONT> & new_fonts)
{
    QGame::setFonts(new_fonts);
    labelCaption -> setFont(_fonts.at(FONT_CAPTION).font);
    labelQuestion -> setFont(_fonts.at(FONT_QUESTION).font);
    labelTime -> setFont(_fonts.at(FONT_TIME).font);
    foreach (QLabel * labelAnswer,labelsAnswer)
        labelAnswer -> setFont(_fonts.at(FONT_ANSWER).font);
    mainButton -> setFont(_fonts.at(FONT_BUTTON).font);
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

