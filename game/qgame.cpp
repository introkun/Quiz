#include "qgame.h"
#include "TurnBasedQuiz/qturnbasedquiz.h"
#include "ParallelQuiz/qparallelquiz.h"
QGame::QGame(QWidget *parent) :
    QWidget(parent)
{
    setEditable(false);
}

QGame::~QGame()
{
}


QGame * QGame::createGame(GAME_TYPE gameType, QWidget * parent)
{
    switch (gameType)
    {
    case GAME_TURNBASEDQUIZ:
        return new QTurnBasedQuiz(parent);
    case GAME_PARALLELQUIZ:
        return new QParallelQuiz(parent);
    case GAME_CAPTAINQUIZ:
        return 0;
    }
}


void QGame::setImage(const QImage & picture)
{
    this -> picture = picture;
    QPalette pal = palette();
    if (picture.isNull())
        pal.setBrush(QPalette::Background,QBrush(Qt::color0));
    else
        pal.setBrush(QPalette::Background,QBrush(QPixmap::fromImage(picture).scaled(QSize(width(),height()))));
    setPalette(pal);
}
