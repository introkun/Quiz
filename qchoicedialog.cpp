#include "qchoicedialog.h"

QChoiceDialog::QChoiceDialog(QWidget *parent,CHOICE_T choice, const QStringList & choices, const QString & caption, const QString & text) :
    QDialog(parent)
{
    choice_mode = choice;
    setWindowFlags(Qt::SplashScreen);
    setStyleSheet("QDialog {border: 2px solid black;border-radius: 5px;}");
    layout_main = new QVBoxLayout(this);
    layout_scroll = new QVBoxLayout();
    layout_buttons = new QHBoxLayout();
    label_caption = new QLabel();
    label_instruction = new QLabel();
    scroll_area = new QScrollArea();
    accept_button = new QCommandLinkButton(tr("Выбор"));
    reject_button = new QCommandLinkButton(tr("Отмена"));
    layout_main -> addWidget(label_caption);
    layout_main -> addWidget(label_instruction);
    layout_main -> addWidget(scroll_area);
    layout_main -> addLayout(layout_buttons);

    connect(reject_button,SIGNAL(clicked()),this,SLOT(reject()));
    connect(accept_button,SIGNAL(clicked()),this,SLOT(acceptButtonClicked()));

    label_caption -> setAlignment(Qt::AlignHCenter);
    label_caption -> setStyleSheet("QLabel{font-weight: bold;}");
    label_instruction -> setWordWrap(true);
    scroll_area -> setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area -> setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroll_area -> setWidgetResizable(false);
    setCaption(caption);
    setText(text);
    QWidget * scroll_content = new QWidget();
    scroll_content -> setLayout(layout_scroll);
    scroll_area -> setWidget(scroll_content);
    scroll_area -> setWidgetResizable(true);
    setChoiceList(choices,true);
    accept_button -> setIcon(QIcon(QPixmap::fromImage(QImage(":/images/yes.png"))));
    reject_button -> setIcon(QIcon(QPixmap::fromImage(QImage(":/images/fail.png"))));
    layout_buttons -> addWidget(accept_button);
    layout_buttons -> addWidget(reject_button);
    reject_button -> setDefault(true);
    //accept_button -> setStyleSheet ("text-align: center;");
    //reject_button -> setStyleSheet ("text-align: center;");

    if (choice == CHOICE_SINGLE)
        accept_button -> hide();
    else
        accept_button -> setEnabled(false);

}


QChoiceDialog::~QChoiceDialog()
{
    delete reject_button;
    delete accept_button;
    foreach (QCommandLinkButton * button, choice_buttons)
        delete button;
    delete layout_scroll;
    delete scroll_area;
    delete label_instruction;
    delete label_caption;
    delete layout_buttons;
    delete layout_main;

}


//Установка значка icon выбору index
void QChoiceDialog::setChoiceIcon(int index,const QIcon & icon)
{
    if (choice_buttons.count() <= index || index < 0)
        return;
    choice_buttons.at(index) -> setIcon(icon);
}


//Запрос значка icon выбора index
QIcon QChoiceDialog::choiceIcon(int index) const
{
    if (choice_buttons.count() <= index || index <= 0)
        return QIcon();
    return choice_buttons.at(index) -> icon();
}


//Установить список для выбора(прошлые выбранные пункты сбрасываются)
void QChoiceDialog::setChoiceList(const QStringList & choices,bool init_icons)
{
    QCommandLinkButton * btn;
    foreach (QCommandLinkButton * button, choice_buttons)
        delete button;
    foreach(QString s,choices)
    {
        btn = new QCommandLinkButton(s);
        if (init_icons)
            btn -> setIcon(QIcon(QPixmap::fromImage(QImage(":/images/selected.png"))));
        btn -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
        if (choice_mode == CHOICE_SINGLE)
        {
            connect(btn,SIGNAL(clicked()),this,SLOT(choiceButtonClicked()));
        }
        else
        {
            btn -> setCheckable(true);
            btn -> setChecked(false);
            connect(btn,SIGNAL(clicked(bool)),this,SLOT(choiceButtonClicked(bool)));
        }
        choice_buttons.append(btn);
        layout_scroll -> addWidget(btn,0,Qt::AlignTop);
    }
    layout_scroll -> addStretch(0);
}


//Запрос списка для выбора
QStringList QChoiceDialog::choiceList() const
{
    QStringList chl;
    foreach (QCommandLinkButton * btn, choice_buttons)
        chl.append(btn -> text());
    return chl;
}


//Сделать выбранными пункты
void QChoiceDialog::setChoiceResult(const QList<int> & selected_list)
{

    for (int i = 0; i < choice_buttons.count(); i++)
    {
        disconnect(choice_buttons.at(i),SIGNAL(clicked(bool)),this,SLOT(choiceButtonClicked(bool)));
        choice_buttons.at(i) -> setChecked(selected_list.count(i) ? true :false);
        connect(choice_buttons.at(i),SIGNAL(clicked(bool)),this,SLOT(choiceButtonClicked(bool)));
    }
}


//Запрос текущих выбранных пунктов
QList<int> QChoiceDialog::choiceResult() const
{
    return results;
}


//Установить заголовок диалогового окна
void QChoiceDialog::setCaption(const QString & s)
{
    label_caption -> setText(s);
    label_caption -> setVisible(!s.isEmpty());
}


//Запрос заголовка диалогового окна
QString QChoiceDialog::caption() const
{
    return label_caption -> text();
}


//Установить текст диалогового окна
void QChoiceDialog::setText(const QString & s)
{
    label_instruction -> setText(s);
    label_instruction -> setVisible(!s.isEmpty());

}


//Запрос текста диалогового окна
QString QChoiceDialog::text() const
{
    return label_instruction -> text();
}


//нажатие кнопка Выбор в multiply режиме
void QChoiceDialog::acceptButtonClicked()
{
    results.clear();
    for (int i = 0; i < choice_buttons.count(); i++)
        if (choice_buttons.at(i) -> isChecked())
        {
            results.append(i);
        }
    accept();

}

//нажатие кнопка в multiply режиме
void QChoiceDialog::choiceButtonClicked(bool value)
{
    int i;
    for (i = 0; i < choice_buttons.count(); i++)
        if (choice_buttons.at(i) -> isChecked())
        {
            break;
        }
    accept_button -> setEnabled(i != choice_buttons.count());
}


//нажатие кнопка в single режиме
void QChoiceDialog::choiceButtonClicked()
{
    results.clear();
    for (int i = 0; i < choice_buttons.count(); i++)
        if (sender() == choice_buttons.at(i))
        {
            results.append(i);
            break;
        }
    accept();
}


void QChoiceDialog::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    scroll_area -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    //scroll_area -> setFixedHeight(reject_button -> size().height() * 5);

    adjustSize();
    scroll_area -> widget() -> adjustSize();
}
