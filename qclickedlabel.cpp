#include "qclickedlabel.h"
#include <QMoveEvent>
QClickedLabel::QClickedLabel(const QString & text,QWidget *parent) :
    QLabel(text,parent)
{

}

void QClickedLabel::mouseReleaseEvent(QMouseEvent * event)
{
        emit clicked(event -> button());
}
