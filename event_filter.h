#ifndef EVENT_FILTER_H
#define EVENT_FILTER_H

#include <QObject>

class Event_Filter : public QObject
{
    Q_OBJECT
public:
    explicit Event_Filter(QObject *parent = 0);
    
signals:
    void form_resize();//сигнал необходимости пересчета параметров таблиц главного окна
protected:
    virtual bool eventFilter(QObject *, QEvent *);//фильтр
public slots:
    
};

#endif // EVENT_FILTER_H
