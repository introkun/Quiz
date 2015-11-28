#ifndef QGAMESETTINGSWIDGET_H
#define QGAMESETTINGSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLayout>
#include "qgame.h"
class QGameSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QGameSettingsWidget(QWidget *parent = 0,QGame * game = 0);
    virtual ~QGameSettingsWidget();
signals:

public slots:
    void fontChangeClicked();
private:
    QGame * game;
    QList<QPushButton *> font_buttons;
    QVBoxLayout * layout_main;
};

#endif // QGAMESETTINGSWIDGET_H
