#ifndef QTURNBASEDQUIZ_H
#define QTURNBASEDQUIZ_H

#include "../qgame.h"
#include "qtheme.h"
#define THEMES_COUNT 10
class QTurnBasedQuiz : public QGame
{
    Q_OBJECT
public:
    typedef enum {
        FONT_CAPTION = 0,
        FONT_QUESTION,
        FONT_TIME,
        FONT_ANSWER,
        FONT_QUEUE,
        FONT_BUTTON,
        FONT_LAST
    } FONT_ACTIONS;
    explicit QTurnBasedQuiz(QWidget *parent = 0);
    virtual ~QTurnBasedQuiz();
    void setThemes(const QList<QTheme *> & themes);
    QList<QTheme *> themes() const {return _themes;}
    virtual void setEditable(bool value);
    virtual QString name() const;
    virtual void setFonts(const QList<QFont> & new_fonts);

    //Получить карту json кода игры
    virtual QVariantMap getJsonData();
    //Настроить игру из карты JSON
    virtual  bool setFromJsonData(const QVariantMap & map);
signals:
    void signalQuestionClicked(QQuestionWidget*,Qt::MouseButton);
private slots:
    void questionClicked(QQuestionWidget * widget,Qt::MouseButton button);
private:
    void fillThemes();
    QList<QSpacerItem *> spacer_items;
    QList<QTheme *> _themes;
    QGridLayout * layoutMain;
};

#endif // QTURNBASEDQUIZ_H
