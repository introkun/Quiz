#include "qgamesettingswidget.h"
#include <QFontDialog>
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
}

QGameSettingsWidget::~QGameSettingsWidget()
{
    foreach (QPushButton * font_button, font_buttons)
        delete font_button;
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

/*void MainWindow::setPicture()
{
    QString pic_file = QFileDialog::getOpenFileName(this,tr("Загрузка файла картинки"), "","*.jpg;*.png;*.jpeg");
    if (pic_file.isEmpty())
        return;
    QImage picture;
    picture.load(pic_file);
    games.at(0) -> setImage(picture);
}


void MainWindow::clearPicture()
{
    games.at(0) -> setImage(QImage());
}*/
