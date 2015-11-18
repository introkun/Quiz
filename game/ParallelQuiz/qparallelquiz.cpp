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
    table_width = -1;
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

QString QParallelQuiz::name() const
{
    return tr("Командная эстафета");
}


void QParallelQuiz::setEditable(bool value)
{
    QGame::setEditable(value);
    questionsTable -> setVisible(value);
}


void QParallelQuiz::changeSize()
{
    //перерисовка таблиц
    if (table_width != questionsTable -> viewport() -> width())
    {
        questionsTable -> setColumnWidth(0, (questionsTable -> viewport() -> width()) * 0.5);
        questionsTable -> setColumnWidth(1, (questionsTable -> viewport() -> width()) * 0.1);
        questionsTable -> setColumnWidth(2, (questionsTable -> viewport() -> width()) * 0.1);
        questionsTable -> setColumnWidth(3, (questionsTable -> viewport() -> width()) * 0.1);
        questionsTable -> setColumnWidth(4, (questionsTable -> viewport() -> width()) * 0.1);
        questionsTable -> setColumnWidth(5, (questionsTable -> viewport() -> width()) * 0.1);
        table_width = questionsTable -> viewport() -> width();
    }
}


