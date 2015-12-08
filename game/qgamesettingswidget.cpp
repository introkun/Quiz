#include "qgamesettingswidget.h"
#include <QFontDialog>
#include "../qchoicedialog.h"
#define PHOTO_STR tr("Фотография")
QGameSettingsWidget::QGameSettingsWidget(QWidget *parent, QGame * game) : QWidget(parent)
{
    this -> game = game;
    QList<QGame::GAME_FONT> fonts;
    layout_main = new QVBoxLayout(this);
    if (game)
        fonts = game -> fonts();
    foreach (QGame::GAME_FONT font, fonts)
    {
        font_buttons.append(new QPushButton(font.name));
        font_buttons.last() -> setFont(font.font);
        layout_main -> addWidget(font_buttons.last());
        connect(font_buttons.last(),SIGNAL(clicked()),this,SLOT(fontChangeClicked()));
    }
    buttonPicture = new QToolButton();
    buttonPicture -> setText(PHOTO_STR);
    buttonPicture -> setFixedSize(200,200);
    buttonPicture -> setIconSize(QSize(buttonPicture -> size().width() - 6,buttonPicture -> size().height() - 6));
    buttonPicture -> setStyleSheet("QToolButton {border: 2px solid black;border-radius: 5px;}");
    buttonPicture -> setCursor(Qt::PointingHandCursor);
    layout_main -> addWidget(buttonPicture,0,Qt::AlignHCenter);
    connect(buttonPicture,SIGNAL(clicked()),this,SLOT(pictureClicked()));
    if (!game -> image().isNull())
    {
        QIcon ico(QPixmap::fromImage(game -> image()));
        buttonPicture -> setIcon(ico);
        buttonPicture -> setText(QString());
    }
}

QGameSettingsWidget::~QGameSettingsWidget()
{
    foreach (QPushButton * font_button, font_buttons)
        delete font_button;
    delete buttonPicture;
    delete layout_main;
}


void QGameSettingsWidget::fontChangeClicked()
{
    bool ok;

    QPushButton * sender_widget = dynamic_cast<QPushButton *>(sender());
    if (!sender_widget)
        return;
    QFont game_font = QFontDialog::getFont(&ok,sender_widget -> font(),this,tr("Выбор шрифта"));
    if (ok)
    {
        emit signalNeedSave();
        QList<QGame::GAME_FONT> fonts;
        QGame::GAME_FONT change_font;
        sender_widget -> setFont(game_font);
        foreach (QPushButton * font_button, font_buttons)
        {
            change_font.name = font_button -> text();
            change_font.font = font_button -> font();
            fonts.append(change_font);
        }
        game -> setFonts(fonts);
    }
}


void QGameSettingsWidget::pictureClicked()
{
    QImage photo;
    QChoiceDialog * cd;
    QStringList sl;
    if (buttonPicture -> text().isEmpty())
    {
        sl.append(tr("Изменить фотографию"));
        sl.append(tr("Удалить фотографию"));
    }
    else
        sl.append(tr("Установить фотографию"));
    cd = new QChoiceDialog(this,QChoiceDialog::CHOICE_SINGLE,sl,tr("Основная фотография игры"),tr("Выберите одну из возможных операций над фотографией игры"));
    cd -> exec();
    QList<int> res = cd -> choiceResult();
    delete cd;
    if (!res.count())
        return;

    if (buttonPicture -> text().isEmpty())
    {
        if (res.at(0) == 0)
        {
            QString pic_file = QFileDialog::getOpenFileName(this,tr("Открытие фотографии"), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0),"*.jpg;*.jpeg;*.png");
            if (pic_file.isEmpty())
                return;
            photo = QImage(pic_file);
            QIcon ico(QPixmap::fromImage(photo));
            buttonPicture -> setIcon(ico);
            buttonPicture -> setText(QString());
        }
        else
        {
            photo = QImage();
            buttonPicture -> setIcon(QIcon());
            buttonPicture -> setText(PHOTO_STR);
        }
    }
    else
    {
        if (res.at(0) == 0)
        {
            QString pic_file = QFileDialog::getOpenFileName(this,tr("Открытие фотографии"), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0),"*.jpg;*.jpeg;*.png");
            if (pic_file.isEmpty())
                return;
            photo = QImage(pic_file);
            QIcon ico(QPixmap::fromImage(photo));
            buttonPicture -> setIcon(ico);
            buttonPicture -> setText(QString());
        }
    }
    game -> setImage(photo);
    emit signalNeedSave();
}
