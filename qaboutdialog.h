#ifndef QABOUTDIALOG_H
#define QABOUTDIALOG_H
#include <QtWidgets>
#include <QDialog>

class QAboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QAboutDialog(QWidget *parent = 0,const QString & version = QString(tr("Неизвестно")));
    virtual ~QAboutDialog();

private slots:
private:
    QVBoxLayout *verticalLayout_main;
    QHBoxLayout *horizontalLayout_button;
    QHBoxLayout *horizontalLayout_main;
    QLabel * label_app_ico;
    QLabel * label_main;
    QLabel * label_license;
    QLabel * label_ext_components;
    QLabel * label_composer;
    QPushButton * button_ok;
    QSpacerItem * spaceitem_1;
    QSpacerItem * spaceitem_2;

};

#endif // QABOUTDIALOG_H
