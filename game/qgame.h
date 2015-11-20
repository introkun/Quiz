#ifndef QGAME_H
#define QGAME_H

#include <QWidget>
#include <QLayout>
#include <QBuffer>
#include "../qregistrationdialog.h"
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
    GAME_TYPE gameType() const{return _gameType;}
    virtual void setEditable(bool value) {edit_mode = value;}
    bool isEditable() const {return edit_mode;}
    virtual QString name() const = 0;
    void setImage(const QImage & picture);
    QImage image() const {return picture;}
    virtual void changeSize(){}

    QList<QFont> fonts() const { return _fonts;}
    virtual void setFonts(const QList<QFont> & new_fonts) {_fonts = new_fonts;}
    virtual void setRCList(const QList<QRegistrationDialog::REG_DEVICE_T> & rc_list){_rc_list = rc_list;}
    QList<QRegistrationDialog::REG_DEVICE_T> rcList() const {return _rc_list;}

    //Получить карту json кода игры
    virtual QVariantMap getJsonData() = 0;
    //Настроить игру из карты JSON
    virtual  bool setFromJsonData(const QVariantMap & map) = 0;

signals:
    void signalRCClicked(uint,unsigned short);
public slots:

protected:
    explicit QGame(QWidget *parent = 0);
    GAME_TYPE _gameType;
    bool edit_mode;
    QImage picture;
    QList<QFont> _fonts;
    QList<QRegistrationDialog::REG_DEVICE_T> _rc_list;
};

#endif // QGAME_H
