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


void QGame::setFonts(const QList<GAME_FONT> & new_fonts)
{
    for (int i = 0; i < _fonts.count(); i++)
        for (int j = 0; j < new_fonts.count(); j++)
            if (_fonts.at(i).name == new_fonts.at(j).name)
                _fonts.replace(i,new_fonts.at(j));
}

QVariantList QGame::getJSonFonts() const
{
    QVariantList font_list;
    foreach (GAME_FONT font,_fonts)
    {
        QVariantMap font_map;
        font_map["name"] = font.name;
        font_map["font"] = font.font.toString();
        font_list.append(font_map);
    }
    return font_list;
}


void QGame::setFromJSonFonts(const QVariantList & font_list)
{
    foreach(QVariant value, font_list)
    {
        QVariantMap font_map = value.toMap();
        if (!font_map["name"].isValid() || !font_map["font"].isValid())
            continue;
        GAME_FONT font;
        font.name = font_map["name"].toString();
        font.font.fromString(font_map["font"].toString());
        for (int i = 0; i < _fonts.count(); i++)
            if (_fonts.at(i).name == font.name)
                _fonts.replace(i,font);
    }
    setFonts(_fonts);
}
