#include "qturnbasedquiz.h"

QTurnBasedQuiz::QTurnBasedQuiz(QWidget *parent) :
    QGame(parent)
{
    _gameType = GAME_TURNBASEDQUIZ;
    layoutMain = new QGridLayout();
    layoutMain -> setContentsMargins(1,1,1,1);
    layoutMain -> setSpacing(1);
    setLayout(layoutMain);
}

QTurnBasedQuiz::~QTurnBasedQuiz()
{
    //TODO:проверить, вызывается ли delete QGame
    while (_themes.count())
    {
        delete _themes.first();
        _themes.removeFirst();
    }
    delete layoutMain;
}


void QTurnBasedQuiz::fillThemes()
{
    int i = 1;
    foreach (QSpacerItem * spacer,spacer_items)
    {
        layoutMain -> removeItem(spacer);
        delete spacer;
    }
    spacer_items.clear();

    foreach (QTheme * theme, _themes)
        theme -> fillLayout(layoutMain, i++);
    //первый рядок заполняем равномерными spacers
    for (int j = 0; j < layoutMain -> columnCount(); j++)
    {
        QSpacerItem * spacer = new QSpacerItem(10,1,QSizePolicy::Expanding,QSizePolicy::Fixed);
        layoutMain -> addItem(spacer,0,j);
        spacer_items.append(spacer);
    }
}


void QTurnBasedQuiz::setThemes(const QList<QTheme *> & themes)
{
    foreach (QTheme * theme,_themes )
        delete theme;
    if (themes.count())
        _themes = themes;
    else
    {
        _themes.clear();
        for (int i = 0; i < THEMES_COUNT; i++)
            _themes.append(new QTheme(parentWidget()));
    }
    //связуем нажатие по виджету с открытием вопроса
    foreach (QTheme * theme,_themes )
        connect(theme,SIGNAL(signalQuestionClicked(QQuestionWidget*,Qt::MouseButton)),this,SIGNAL(signalQuestionClicked(QQuestionWidget*,Qt::MouseButton)));
    fillThemes();
}


void QTurnBasedQuiz::setEditable(bool value)
{
    QGame::setEditable(value);
    foreach (QTheme * theme, _themes)
        theme -> setEditable(value);
}

QString QTurnBasedQuiz::name() const
{
    return tr("Основная электронная викторина");
}
