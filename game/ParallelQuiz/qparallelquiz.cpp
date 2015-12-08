#include "../../qobjecthelper.h"
#include "qparallelquiz.h"
#define ATTENTION_TEXT "<h4>" +tr("Внимание, вопрос!") + "<h4>"
#define ANSWER_TIME_SEC 30
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
    vspacer1 = new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding);
    vspacer2 = new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding);
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
        labelsAnswer.last() -> setWordWrap(true);
        labelsAnswer.last() -> setFixedHeight(120);
        labelsAnswer.last() -> setStyleSheet("QLabel{border:5px solid grey; border-radius: 15px;}");
        layoutAnswers -> addWidget(labelsAnswer.last(), i / 2, i % 2);
    }

    layoutMain -> addWidget(questionsTable);
    layoutMain -> addLayout(layoutCommands);
    layoutMain -> addSpacerItem(vspacer1);
    layoutMain -> addWidget(labelCaption);
    layoutMain -> addWidget(labelQuestion);
    layoutMain -> addWidget(labelTime);
    layoutMain -> addSpacerItem(vspacer2);
    layoutMain -> addLayout(layoutAnswers);
    layoutMain -> addWidget(mainButton);

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
        case FONT_TEAM_NAME:
            s = tr("Шрифт названия команд");
            break;
        case FONT_TEAM_RESULTS:
            s = tr("Шрифт результатов команд");
            break;
        }
        _font.name = s;
        _font.font = font();
        _fonts.append(_font);
    }
    timer = new QTimer();
    timer -> start(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    setStage(STAGE_NONE);
    setEditable(false);
    main_sound = new QSound(":/sounds/time.wav");
    main_sound -> setLoops(-1);
    connect(mainButton,SIGNAL(clicked()),this,SLOT(startQuestion()));
    connect(this,SIGNAL(signalRCClicked(uint,unsigned short)),this,SLOT(rcClicked(uint,unsigned short)));
    connect(questionsTable,SIGNAL(signalNeedSave()),this,SIGNAL(signalNeedSave()));
}

QParallelQuiz::~QParallelQuiz()
{
    main_sound -> stop();
    delete main_sound;
    timer -> stop();
    delete timer;
    foreach (QTeamWidget * team,teams)
        delete team;
    foreach(QWidget * widget,labelsAnswer)
        delete widget;
    layoutMain -> removeItem(vspacer1);
    layoutMain -> removeItem(vspacer2);
    delete vspacer1;
    delete vspacer2;
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
    QGame::changeSize();
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
    {
        vspacer1 -> changeSize(1,1);
        vspacer2 -> changeSize(1,1);
    }
    else
    {
        vspacer1 -> changeSize(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding);
        vspacer2 -> changeSize(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding);
    }
    labelCaption -> setVisible(!value);
    labelQuestion -> setVisible(!value);
    labelTime -> setVisible(!value);
    adjustSize();
    if (!value)
        setStage(stage);
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

        teams.last() -> setFonts(_fonts.at(FONT_TEAM_NAME).font,_fonts.at(FONT_TEAM_RESULTS).font);
        //teams.last() -> prepareForQuestion();
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
    foreach (QTeamWidget * team,teams)
        team -> setFonts(_fonts.at(FONT_TEAM_NAME).font,_fonts.at(FONT_TEAM_RESULTS).font);
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
    if (!picture.isNull())
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        picture.save(&buffer, "PNG");
        game_map["picture"] = ba.toBase64();
    }
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
    this -> questions =  questionsTable -> questions();
    if (map["picture"].isValid())
        setImage(QImage::fromData(QByteArray::fromBase64(map["picture"].toByteArray()),"PNG"));
    else
        setImage(QImage());

    if (map["fonts"].isValid())
        setFromJSonFonts(map["fonts"].toList());
    return true;
}

void QParallelQuiz::setStage(STAGE_T stage)
{
    this -> stage = stage;
    switch (stage)
    {
    case STAGE_NONE:
        labelCaption -> hide();
        labelQuestion -> hide();
        labelTime -> hide();
        foreach (QLabel * label,labelsAnswer)
            label -> hide();
        mainButton -> setEnabled(true && !isEditable());
        break;
    case STAGE_PREPARE:
        ticks = 0;
        labelCaption -> show();
        labelQuestion -> hide();
        foreach (QLabel * label,labelsAnswer)
            label -> hide();
        foreach (QTeamWidget * widget,teams)
            widget -> prepareForQuestion();
        mainButton -> setEnabled(false);
        break;
    case STAGE_QUESTION:
        ticks = -1;
        next_tick = 0;
        question_parts = 0;
        labelQuestion -> show();
        break;
    case STAGE_TIMING:
        ticks = -1;
        for (int i = 0; i < labelsAnswer.count() ; i++)
        {
            labelsAnswer.at(i) -> show();
            labelsAnswer.at(i) -> setStyleSheet("QLabel{border:5px solid gray; border-radius: 15px;}");
            labelsAnswer.at(i) -> setText(tr("%1) %2").arg(QString::number(i + 1),questions.first().answers().at(i)));
        }
        main_sound -> play();
        labelTime -> show();
        break;
    case STAGE_ANSWERING:
        if (questions.count() > 1)
        {
            mainButton -> setEnabled(true);
            mainButton -> setText(tr("Следующий вопрос"));
        }
        main_sound -> stop();
        QSound::play(":/sounds/answer.wav");
        labelTime -> hide();
        showAnswer();
        break;
    default:
        break;
    }
}

void QParallelQuiz::showAnswer()
{
    for (int i = 0; i < labelsAnswer.count(); i++)
        if (questions.first().goodAnswerIndex() - 1 == i)
            labelsAnswer.at(i) -> setStyleSheet("QLabel{border:5px solid green; border-radius: 15px;}");
        else
            labelsAnswer.at(i) -> setStyleSheet("QLabel{border:5px solid red; border-radius: 15px;}");
}

void QParallelQuiz::timerTick()
{

    switch (stage)
    {
    case STAGE_NONE:
        return;

    case STAGE_PREPARE:
        if (ticks % 10 == 0)
        {
            QString color = "red";
            if (ticks % 20 == 0)
                color = "black";
            labelCaption -> setText(QString("<font color=""%1"">").arg(color) + ATTENTION_TEXT + "</font>");
        }
        if (ticks >= 40)
            setStage(STAGE_QUESTION);
        break;

    case STAGE_QUESTION:

        if (next_tick == ticks)
        {
            QStringList sl = questions.first().question().split(" ");
            question_parts++;
            if (question_parts > (unsigned int)sl.count())
            {
                setStage(STAGE_TIMING);
                break;
            }
            while (question_parts < (unsigned int)sl.count())
                sl.removeLast();
            labelQuestion -> setText(sl.join(" "));
            next_tick = sl.join(" ").count();
        }
        break;

    case STAGE_TIMING:
    {
        bool next_stage = true;
        //а вдруг все ответили? чего тогда ждать
        foreach (QTeamWidget * widget, teams)
        {
            if (widget -> isUnknownAnswer())
            {
                next_stage = false;
                break;
            }
        }
        if (next_stage)
            ticks = 10 * ANSWER_TIME_SEC;

        if (ticks % 10 == 0)
        {
            labelTime -> setText(tr("Время для размышления: %1 секунд").arg(QString::number(ANSWER_TIME_SEC - ticks / 10)));
            if (ANSWER_TIME_SEC == ticks / 10)
                setStage(STAGE_ANSWERING);
        }



        break;
    }
    case STAGE_ANSWERING:            
        break;
    }
    ticks++;
}


void QParallelQuiz::startQuestion()
{
    if (!questions.count())
        return;
    if (stage == STAGE_ANSWERING)
        questions.removeFirst();

    if (!questions.count())
        return;
    setStage(STAGE_PREPARE);
}


void QParallelQuiz::rcClicked(unsigned int mac,unsigned short group)
{
    if (stage != STAGE_TIMING)
        return;
    foreach (QTeamWidget * widget,teams)
        if (widget -> id() == mac)
        {
            if (!widget -> isUnknownAnswer())
                return;
            if (questions.first().goodAnswerIndex() == group)
            {
                QSound::play(":/sounds/truestart.wav");
                widget -> increaseTrue();
            }
            else
            {
                QSound::play(":/sounds/falsestart.wav");
                widget -> increaseFalse();
            }
            return;
        }
}

