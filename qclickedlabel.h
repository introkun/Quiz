#ifndef QCLICKEDLABEL_H
#define QCLICKEDLABEL_H

#include <QLabel>

class QClickedLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QClickedLabel(const QString & text = QString(),QWidget *parent = 0);

signals:
    void clicked(Qt::MouseButton);
protected:
    void mouseReleaseEvent (QMouseEvent * event) ;
};
#endif
