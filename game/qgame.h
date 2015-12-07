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

    typedef struct _game_font{
        QString name;
        QFont font;
    } GAME_FONT;
    static QGame * createGame(GAME_TYPE gameType,QWidget * parent = 0);
    virtual ~QGame();
    GAME_TYPE gameType() const{return _gameType;}
    virtual void setEditable(bool value) {edit_mode = value;}
    bool isEditable() const {return edit_mode;}
    virtual QString name() const = 0;
    void setImage(const QImage & picture);
    QImage image() const {return picture;}
    virtual void changeSize(){}

    QList<GAME_FONT> fonts() const { return _fonts;}
    virtual void setFonts(const QList<GAME_FONT> & new_fonts);
    virtual void setRCList(const QList<QRegistrationDialog::REG_DEVICE_T> & rc_list){_rc_list = rc_list;}
    QList<QRegistrationDialog::REG_DEVICE_T> rcList() const {return _rc_list;}

    //Получить карту json кода игры
    virtual QVariantMap getJsonData() const = 0;
    //Настроить игру из карты JSON
    virtual  bool setFromJsonData(const QVariantMap & map)= 0;


signals:
    void signalRCClicked(uint,unsigned short);
    void signalNeedSave();//сигнал необходимости сохранения в файл игры
public slots:

protected:
    explicit QGame(QWidget *parent = 0);

    QVariantList getJSonFonts() const;
    void setFromJSonFonts(const QVariantList & font_list);

    GAME_TYPE _gameType;
    bool edit_mode;
    QImage picture;
    QList<GAME_FONT> _fonts;
    QList<QRegistrationDialog::REG_DEVICE_T> _rc_list;
};

#endif // QGAME_H
