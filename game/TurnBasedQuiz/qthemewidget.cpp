#include "qthemewidget.h"
#include <QColorDialog>
#include <QMessageBox>
#include <QFileDialog>
QThemeWidget::QThemeWidget(QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::Box);
    //setStyleSheet("border-width: 2px; border-color: yellow;");
    layout = new QVBoxLayout(this);
    layout_buttons = new QHBoxLayout();
    nameLabel = new QLabel();
    colorButton = new QPushButton(tr("Выбрать цвет..."));
    setImageButton = new QToolButton();
    clearImageButton = new QToolButton();
    nameEdit = new QLineEdit();

    setImageButton -> setIcon(QIcon(":/images/setimage.png"));
    setImageButton -> setToolTip(tr("Установить картинку темы"));
    clearImageButton -> setToolTip(tr("Удалить картинку темы"));
    clearImageButton -> setIcon(QIcon(":/images/clearimage.png"));
    colorButton -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    nameEdit -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    nameLabel -> setAlignment(Qt::AlignCenter);
    nameLabel -> setWordWrap(true);
    layout -> addWidget(nameLabel);
    layout -> addWidget(nameEdit);
    layout -> addLayout(layout_buttons);
    layout_buttons -> addWidget(colorButton);
    layout_buttons -> addWidget(setImageButton);
    layout_buttons -> addWidget(clearImageButton);
    layout -> setSpacing(1);
    layout -> setMargin(1);
    setThemeColor(palette().background().color());
    setLayout(layout);
    connect(colorButton,SIGNAL(clicked()),this,SLOT(colorChanged()));
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(setThemeName(QString)));
    connect(setImageButton,SIGNAL(clicked()),this,SLOT(pictureSet()));
    connect(clearImageButton,SIGNAL(clicked()),this,SLOT(pictureClear()));
    setEditable(true);
}


QThemeWidget::~QThemeWidget()
{
    delete nameLabel;
    delete nameEdit;
    delete colorButton;
    delete clearImageButton;
    delete setImageButton;
    delete layout_buttons;
    delete layout;
}


void QThemeWidget::setEditable(bool value)
{
    edit_mode = value;
    colorButton -> setVisible(value);
    setImageButton -> setVisible(value);
    clearImageButton -> setVisible(value);
    nameEdit -> setVisible(value);
    nameLabel -> setText(value ? tr("Название темы") : ("<h1>" + name + "</h1>"));

}


void QThemeWidget::colorChanged()
{
    QColor color = QColorDialog::getColor();
    if (!color.isValid())
        return;
    setThemeColor(color);
    emit widgetChanged();
}


void QThemeWidget::setThemeColor(const QColor & value)
{
    color = value;
    emit signalColorChanged(color);
    QPalette Pal(palette());

    // set black background
    Pal.setColor(QPalette::Background, color);
    setAutoFillBackground(true);
    setPalette(Pal);
}


void QThemeWidget::setThemeName(const QString & value)
{
    name = value;
    if (isEditable())
    {
        disconnect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(setThemeName(QString)));
        nameEdit -> setText(name);
        emit widgetChanged();
        connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(setThemeName(QString)));
    }
    else
        nameLabel -> setText("<h1>" + name + "</h1>");

}

void QThemeWidget::pictureSet()
{
    QString pic_file = QFileDialog::getOpenFileName(this,tr("Загрузка файла картинки"), "","*.jpg;*.png;*.jpeg");
    if (pic_file.isEmpty())
        return;
    image.load(pic_file);
    QMessageBox::information(parentWidget(),tr("Информация"),tr("Картинка темы успешно установлена"));
    emit widgetChanged();
}

void QThemeWidget::pictureClear()
{
    image = QImage();
    QMessageBox::information(parentWidget(),tr("Информация"),tr("Картинка темы успешно удалена"));
    emit widgetChanged();
}
