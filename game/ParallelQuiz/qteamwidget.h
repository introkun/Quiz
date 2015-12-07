#ifndef QTEAMWIDGET_H
#define QTEAMWIDGET_H

#include <QFrame>
#include <QLayout>
#include <QLabel>
class QTeamWidget : public QFrame
{
    Q_OBJECT
public:
    explicit QTeamWidget(QWidget *parent = 0, uint id = 0,const QString & team_name = QString());
    virtual ~QTeamWidget();
    void clearResults();
    void increaseTrue();
    void increaseFalse();
    void prepareForQuestion();

    void setFonts(const QFont & team_font, const QFont & results_font);
    bool isUnknownAnswer() const {return unknown;}
    uint id() const {return _id;}
private:
    void updateInfo();
    QVBoxLayout * layout;
    QHBoxLayout * layout_main;
    QVBoxLayout * layout_info;
    QLabel * label_name;//Название команды
    QLabel * label_quest_status;//метка отображения ответа на текущий вопрос
    QLabel * label_quest_true;//метка отображения количества правильных ответов
    QLabel * label_quest_false;//метка отображения количества неправильных ответов
    QLabel * label_quest_unknown;//метка отображения количества неотвеченных
    QSpacerItem * spacer;
    uint _id;//id пульта команды
    int quest_true;
    int quest_false;
    int quest_unknown;
    bool unknown;
};

#endif // QTEAMWIDGET_H
