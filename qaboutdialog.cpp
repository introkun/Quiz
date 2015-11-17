#include "qaboutdialog.h"

QAboutDialog::QAboutDialog(QWidget *parent,const QString & version) :
    QDialog(parent)
{
    horizontalLayout_main = new QHBoxLayout(this);
    verticalLayout_main = new QVBoxLayout();
    horizontalLayout_button = new QHBoxLayout();

    label_app_ico = new QLabel(this);
    label_main = new QLabel(this);
    label_license = new QLabel(this);
    label_ext_components = new QLabel(this);
    label_composer = new QLabel(this);
    button_ok = new QPushButton(this);
    spaceitem_1 = new QSpacerItem(0,0,QSizePolicy::Expanding);
    spaceitem_2 = new QSpacerItem(0,0,QSizePolicy::Expanding);

    this -> setWindowTitle(tr("О программе"));
    this -> setLayout(horizontalLayout_main);
    setModal(true);

    horizontalLayout_main -> addWidget(label_app_ico);
    horizontalLayout_main -> addLayout(verticalLayout_main);

    verticalLayout_main -> addWidget(label_main);
    verticalLayout_main -> addWidget(label_license);
    verticalLayout_main -> addWidget(label_ext_components);
    verticalLayout_main -> addWidget(label_composer);
    verticalLayout_main -> addLayout(horizontalLayout_button);

    horizontalLayout_button -> addSpacerItem(spaceitem_1);
    horizontalLayout_button -> addWidget(button_ok);
    horizontalLayout_button -> addSpacerItem(spaceitem_2);

    label_license -> setWordWrap(true);
    label_ext_components -> setWordWrap(true);
    label_composer -> setWordWrap(true);

    button_ok -> setText(tr("Закрыть"));

    label_main -> setText("<b>" + tr("Версия программы") + "</b>: " + version + "<br><b>" + tr("Назначение программы") + "</b>:" + tr("Организация викторин для соревнования команд"));
    label_license -> setText("<b>" + tr("Лицензия программы BSD") + " :</b><br>" + tr("Разрешается повторное распространение и использование как в виде исходного кода, так и в двоичной форме, с изменениями или без, при соблюдении следующих условий: <br>1. При повторном распространении исходного кода должно оставаться указанное выше уведомление об авторском праве, этот список условий и последующий отказ от гарантий. <br>2. При повторном распространении двоичного кода должна сохраняться указанная выше информация об авторском праве, этот список условий и последующий отказ от гарантий в документации и/или в других материалах, поставляемых при распространении.<br>ЭТА ПРОГРАММА ПРЕДОСТАВЛЕНА ВЛАДЕЛЬЦАМИ АВТОРСКИХ ПРАВ И/ИЛИ ДРУГИМИ СТОРОНАМИ «КАК ОНА ЕСТЬ» БЕЗ КАКОГО-ЛИБО ВИДА ГАРАНТИЙ, ВЫРАЖЕННЫХ ЯВНО ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ, НО НЕ ОГРАНИЧИВАЯСЬ ИМИ, ПОДРАЗУМЕВАЕМЫЕ ГАРАНТИИ КОММЕРЧЕСКОЙ ЦЕННОСТИ И ПРИГОДНОСТИ ДЛЯ КОНКРЕТНОЙ ЦЕЛИ. НИ В КОЕМ СЛУЧАЕ НИ ОДИН ВЛАДЕЛЕЦ АВТОРСКИХ ПРАВ И НИ ОДНО ДРУГОЕ ЛИЦО, КОТОРОЕ МОЖЕТ ИЗМЕНЯТЬ И/ИЛИ ПОВТОРНО РАСПРОСТРАНЯТЬ ПРОГРАММУ, КАК БЫЛО СКАЗАНО ВЫШЕ, НЕ НЕСЁТ ОТВЕТСТВЕННОСТИ, ВКЛЮЧАЯ ЛЮБЫЕ ОБЩИЕ, СЛУЧАЙНЫЕ, СПЕЦИАЛЬНЫЕ ИЛИ ПОСЛЕДОВАВШИЕ УБЫТКИ, ВСЛЕДСТВИЕ ИСПОЛЬЗОВАНИЯ ИЛИ НЕВОЗМОЖНОСТИ ИСПОЛЬЗОВАНИЯ ПРОГРАММЫ (ВКЛЮЧАЯ, НО НЕ ОГРАНИЧИВАЯСЬ ПОТЕРЕЙ ДАННЫХ, ИЛИ ДАННЫМИ, СТАВШИМИ НЕПРАВИЛЬНЫМИ, ИЛИ ПОТЕРЯМИ ПРИНЕСЕННЫМИ ИЗ-ЗА ВАС ИЛИ ТРЕТЬИХ ЛИЦ, ИЛИ ОТКАЗОМ ПРОГРАММЫ РАБОТАТЬ СОВМЕСТНО С ДРУГИМИ ПРОГРАММАМИ), ДАЖЕ ЕСЛИ ТАКОЙ ВЛАДЕЛЕЦ ИЛИ ДРУГОЕ ЛИЦО БЫЛИ ИЗВЕЩЕНЫ О ВОЗМОЖНОСТИ ТАКИХ УБЫТКОВ."));
    label_ext_components -> setText("<b>" + tr("Используемые внешние компоненты") + ":</b>" + tr("<br>1.Инструментарий разработки и библиотеки Qt5:лицензия LGPL 2.1, компания-разработчик <a href=""http://digia.com"">DIGIA</a><br>2.Пульты Intro 2 и USB-stick 8615 для функционирования викторины в режиме первенства ответа команд."));
    label_composer -> setText(tr("<b>Разработчик:</b><br>Дроздов Евгений, Минск 2015, ed.soiam@gmail.com"));
    connect(button_ok,SIGNAL(clicked()),this,SLOT(accept()));
}


QAboutDialog::~QAboutDialog()
{
    disconnect(button_ok,SIGNAL(clicked()),this,SLOT(accept()));
    delete label_app_ico;
    delete label_main;
    delete label_license;
    delete label_ext_components;
    delete label_composer;
    delete button_ok;
    horizontalLayout_button -> removeItem(spaceitem_1);
    horizontalLayout_button -> removeItem(spaceitem_2);
    delete spaceitem_1;
    delete spaceitem_2;
    delete horizontalLayout_button;
    delete verticalLayout_main;
    delete horizontalLayout_main;
}
