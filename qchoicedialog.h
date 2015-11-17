#ifndef QCHOICEDIALOG_H
#define QCHOICEDIALOG_H

#include <QDialog>
#include <QtWidgets>
class QChoiceDialog : public QDialog
{
    Q_OBJECT
public:
    typedef enum {
        CHOICE_SINGLE,      //одиночный выбор
        CHOICE_MULTIPLY     //множественный выбор
    } CHOICE_T;
    //Конструктор объекта с родительским виджетом parent, типом выбора choice, и списком choices
    explicit QChoiceDialog(QWidget *parent = 0,CHOICE_T choice = CHOICE_SINGLE, const QStringList & choices = QStringList(), const QString & caption = QString(), const QString & text = QString());
    virtual ~QChoiceDialog();
    //Установка значка icon выбору index
    void setChoiceIcon(int index,const QIcon & icon);
    //Запрос значка icon выбора index
    QIcon choiceIcon(int index) const;

    //Установить список для выбора(прошлые выбранные пункты сбрасываются)
    void setChoiceList(const QStringList & choices,bool init_icons = false);
    //Запрос списка для выбора
    QStringList choiceList() const;

    //Сделать выбранными пункты
    void setChoiceResult(const QList<int> & selected_list);
    //Запрос текущих выбранных пунктов
    QList<int> choiceResult() const;
    //Установить заголовок диалогового окна
    void setCaption(const QString & s);
    //Запрос заголовка диалогового окна
    QString caption() const;
    //Установить текст диалогового окна
    void setText(const QString & s);
    //Запрос текста диалогового окна
    QString text() const;
signals:
protected:
    virtual void showEvent(QShowEvent * event);
private slots:
    //нажатие кнопка Выбор в multiply режиме
    void acceptButtonClicked();
    //нажатие кнопка в multiply режиме
    void choiceButtonClicked(bool value);
    //нажатие кнопка в single режиме
    void choiceButtonClicked();
private:
    QList <QCommandLinkButton *> choice_buttons;
    QCommandLinkButton * accept_button;
    QCommandLinkButton * reject_button;
    QScrollArea * scroll_area;
    QVBoxLayout * layout_main;
    QVBoxLayout * layout_scroll;
    QLabel * label_caption;
    QLabel * label_instruction;
    QHBoxLayout * layout_buttons;
    QVBoxLayout * layout_temp;
    QList<int> results;
    CHOICE_T choice_mode;
};

#endif // QCHOICEDIALOG_H
