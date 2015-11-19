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
    layoutCommands -> setSpacing(1);
    layoutAnswers = new QGridLayout();
    layoutAnswers -> setContentsMargins(1,1,1,1);
    layoutAnswers -> setSpacing(1);
    setLayout(layoutMain);

    questionsTable = new QQuestionsTableWidget();

    layoutMain -> addWidget(questionsTable);
    layoutMain -> addLayout(layoutCommands);
    layoutMain -> addLayout(layoutAnswers);
    setEditable(false);
}

QParallelQuiz::~QParallelQuiz()
{
    delete questionsTable;
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
}




//Получить карту json кода игры
QVariantMap QParallelQuiz::getJsonData()
{
    QVariantMap game_map;
    game_map["type"] = _gameType;
    QList<QQuestion> questions = questionsTable -> questions();
    QVariantList questions_list;
    foreach(QQuestion question,questions)
        questions_list.append(QObjectHelper::qobject2qvariant(&question));
    game_map["questions"] = questions_list;
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
    return true;
}

