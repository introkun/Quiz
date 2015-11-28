#ifndef QGAMESETTINGSDIALOG_H
#define QGAMESETTINGSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLayout>
#include <QPushButton>
#include <QSpacerItem>
#include "qgame.h"
#include "qgamesettingswidget.h"
class QGameSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QGameSettingsDialog(QWidget *parent = 0,const QList<QGame *> & games = QList<QGame *>());
    virtual ~QGameSettingsDialog();
signals:
protected:
    virtual void showEvent(QShowEvent *);
private:
    QList<QGameSettingsWidget *> widgets;
    QVBoxLayout * layout_main;
    QTabWidget * tab_widget;
    QHBoxLayout * layout_buttons;
    QPushButton * button_close;
    QSpacerItem * spacer_buttons;
};

#endif // QGAMEFONTSDIALOG_H
