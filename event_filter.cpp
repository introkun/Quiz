#include "event_filter.h"
#include <QEvent>
Event_Filter::Event_Filter(QObject *parent) :
    QObject(parent)
{
}

bool Event_Filter::eventFilter(QObject *pObject, QEvent *pEvent)
{
   if (!pObject)
       return false;
   if(pEvent -> type() == QEvent::Resize)
   {
       emit form_resize();
       return true;
   }
   return false;
}
