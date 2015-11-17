#ifndef QGAME_H
#define QGAME_H

#include <QWidget>
#include <QLayout>
class QGame : public QWidget
{
    Q_OBJECT
public:
    typedef enum {
        GAME_TURNBASEDQUIZ,
        GAME_PARALLELQUIZ,
        GAME_CAPTAINQUIZ
    } GAME_TYPE;

    static QGame * createGame(GAME_TYPE gameType,QWidget * parent = 0);
    virtual ~QGame();
    virtual void setEditable(bool value) {edit_mode = value;}
    bool isEditable() const {return edit_mode;}
    virtual QString name() const = 0;
    void setImage(const QImage & picture);
    QImage image() const {return picture;}
    virtual void changeSize(){}
signals:

public slots:

protected:
    explicit QGame(QWidget *parent = 0);
    GAME_TYPE _gameType;
    bool edit_mode;
    QImage picture;
};

#endif // QGAME_H
