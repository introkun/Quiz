#ifndef QGAMEFONTSDIALOG_H
#define QGAMEFONTSDIALOG_H

#include <QDialog>
#include "qgame.h"
class QGameFontsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QGameFontsDialog(QWidget *parent = 0,const QList<QGame *> & games = QList<QGame *>());
    virtual ~QGameFontsDialog();
signals:

public slots:
};

#endif // QGAMEFONTSDIALOG_H
