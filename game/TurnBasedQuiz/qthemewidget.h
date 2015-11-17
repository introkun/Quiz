#ifndef QTHEMEWIDGET_H
#define QTHEMEWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QToolButton>
class QThemeWidget : public QFrame
{
    Q_OBJECT
public:
    explicit QThemeWidget(QWidget *parent = 0);
    ~QThemeWidget();
    void setEditable(bool value);
    bool isEditable() const {return edit_mode;}


    QString themeName() const {return name;}

    void setThemeColor(const QColor & value);
    QColor themeColor() const {return color;}
    QImage picture() const {return image;}
    void setPicture(const QImage & picture) {image = picture;}
public slots:
    void setThemeName(const QString & value);
signals:
    void signalColorChanged(const QColor &);
    void widgetChanged();
private slots:
    void colorChanged();
    void pictureSet();
    void pictureClear();

private:
    QString name;
    QColor color;
    bool edit_mode;
    QLabel * nameLabel;
    QPushButton * colorButton;
    QToolButton * setImageButton;
    QToolButton * clearImageButton;
    QLineEdit * nameEdit;
    QVBoxLayout * layout;
    QHBoxLayout * layout_buttons;
    QImage image;
};

#endif // QTHEMEWIDGET_H
