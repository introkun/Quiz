#ifndef QTURNBASEDQUIZ_H
#define QTURNBASEDQUIZ_H

#include "../qgame.h"
#include "qtheme.h"
#define THEMES_COUNT 10
class QTurnBasedQuiz : public QGame
{
    Q_OBJECT
public:
    explicit QTurnBasedQuiz(QWidget *parent = 0);
    virtual ~QTurnBasedQuiz();
    void setThemes(const QList<QTheme *> & themes);
    QList<QTheme *> themes() const {return _themes;}
    virtual void setEditable(bool value);
    virtual QString name() const;
signals:
    void signalQuestionClicked(QQuestionWidget*,Qt::MouseButton);
public slots:

private:
    void fillThemes();
    QList<QSpacerItem *> spacer_items;
    QList<QTheme *> _themes;
    QGridLayout * layoutMain;
};

#endif // QTURNBASEDQUIZ_H
