#include <QMessageBox>
#include "qturnbasedquiz.h"
#include "qquestiondialog.h"
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
        connect(theme,SIGNAL(signalQuestionClicked(QQuestionWidget*,Qt::MouseButton)),this,SLOT(questionClicked(QQuestionWidget*,Qt::MouseButton)));
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


void QTurnBasedQuiz::setFonts(const QList<QFont> & new_fonts)
{
    _fonts = new_fonts;
    //наращиваем количество шрифтов до количества, необходимого для игры
    while(_fonts.size() < FONT_LAST)
        _fonts.append(font());
}


void QTurnBasedQuiz::questionClicked(QQuestionWidget * widget,Qt::MouseButton button)
{
    QTheme * theme;
    //закрытие виджета по правой кнопке
    if (button == Qt::RightButton)
    {
        if (QMessageBox::warning(this,tr("Предупреждение"), tr("Вопрос будет закрыт. Продолжить?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
            widget -> hide();
        return;
    }
    QImage image;
    foreach (theme, _themes)
        if (theme -> isChild(widget))
        {
            image = theme -> image();
            break;
        }
    QQuestionDialog * qd = new QQuestionDialog(this,widget -> question(),widget -> answer(),image,_fonts,_rc_list,QString("%1(%2)").arg(theme -> name(),QString::number(widget -> rating())));
    connect(this,SIGNAL(signalRCClicked(uint,unsigned short)),qd,SLOT(rcClicked(uint,unsigned short)));
    int result = qd -> exec();
    delete qd;
    if (result == QDialog::Accepted)
    {
        widget -> hide();
    }
    update();
}
