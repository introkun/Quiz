#ifndef QTHEME_H
#define QTHEME_H
#include "qquestionwidget.h"
#include "qthemewidget.h"
#include <QObject>
#include <QGridLayout>
#include <QVariantMap>
#define THEME_SIZE 10
class QTheme : public QObject
{
    Q_OBJECT
public:
    explicit QTheme(QWidget * parent = NULL,int size = THEME_SIZE);
    ~QTheme();
    void fillLayout(QGridLayout * layout, int row);
    QVariantMap themeMap() const;
    void setThemeMap(const QVariantMap & map);
    void setEditable(bool value);

    bool isChild(QWidget * w) const;
    QImage image() const {return themeWidget -> picture();}
    QString name() const {return themeWidget -> themeName();}
signals:
    void signalQuestionClicked(QQuestionWidget *,Qt::MouseButton);
private:
    QThemeWidget * themeWidget;
    QList<QQuestionWidget *> questionWidgets;
};

#endif // QTHEME_H
