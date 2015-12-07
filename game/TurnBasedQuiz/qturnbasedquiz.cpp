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
        case FONT_QUEUE:
            s = tr("Шрифт очереди");
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
    {
        connect(theme,SIGNAL(signalQuestionClicked(QQuestionWidget*,Qt::MouseButton)),this,SLOT(questionClicked(QQuestionWidget*,Qt::MouseButton)));
        connect(theme,SIGNAL(signalNeedSave()),this,SIGNAL(signalNeedSave()));
    }
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

    QList<QFont> game_fonts;
    foreach(GAME_FONT game_font,_fonts)
        game_fonts.append(game_font.font);

    QQuestionDialog * qd = new QQuestionDialog(this,widget -> question(),widget -> answer(),image,game_fonts,_rc_list,QString("%1(%2)").arg(theme -> name(),QString::number(widget -> rating())));
    connect(this,SIGNAL(signalRCClicked(uint,unsigned short)),qd,SLOT(rcClicked(uint,unsigned short)));
    int result = qd -> exec();
    delete qd;
    if (result == QDialog::Accepted)
    {
        widget -> hide();
    }
    update();
}


//Получить карту json кода игры
QVariantMap QTurnBasedQuiz::getJsonData() const
{
    QVariantList th_list;
    QVariantMap game_map;
    game_map["type"] = _gameType;
    foreach (QTheme * theme, _themes)
        th_list.append(theme -> themeMap());
    game_map["themes"] = th_list;
    if (!picture.isNull())
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        picture.save(&buffer, "PNG");
        game_map["picture"] = ba.toBase64();
    }
    game_map["fonts"] = getJSonFonts();
    return game_map;
}

//Настроить игру из карты JSON
bool QTurnBasedQuiz::setFromJsonData(const QVariantMap & map)
{

   QVariantList themes_list = map["themes"].toList();
   QList<QTheme *> themes;
   foreach(QVariant value, themes_list)
   {
       QTheme * theme = new QTheme(parentWidget());
       theme -> setThemeMap(value.toMap());
       themes.append(theme);
   }
   setThemes(themes);

   if (map["picture"].isValid())
       picture = QImage::fromData(QByteArray::fromBase64(map["picture"].toByteArray()),"PNG");
   if (map["fonts"].isValid())
       setFromJSonFonts(map["fonts"].toList());
   return true;
}
