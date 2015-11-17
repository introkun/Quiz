#include "qtheme.h"
#include "qobjecthelper.h"
#include <QBuffer>
QTheme::QTheme(QWidget * parent, int size) : QObject(parent)
{
    themeWidget = new QThemeWidget();
    for (int i = 0; i < size; i++)
    {
        questionWidgets.append(new QQuestionWidget(parent));
        connect(themeWidget,SIGNAL(signalColorChanged(QColor)),questionWidgets.last(),SLOT(setColor(QColor)));
        connect(questionWidgets.last() ,SIGNAL(signalQuestionClicked(QQuestionWidget*,Qt::MouseButton)),this,SIGNAL(signalQuestionClicked(QQuestionWidget*,Qt::MouseButton)));
        questionWidgets.last() -> setRating(i + 1);
    }
}

QTheme::~QTheme()
{
    delete themeWidget;
    while (questionWidgets.count())
    {
        delete questionWidgets.first();
        questionWidgets.removeFirst();
    }
}

void QTheme::fillLayout(QGridLayout * layout, int row)
{

    layout -> addWidget(themeWidget,row,0);

    int i = 1;
    foreach (QQuestionWidget * widget,questionWidgets)
        layout -> addWidget(widget,row,i++);
}


QVariantMap QTheme::themeMap() const
{
    QVariantMap quest_map,return_map;
    QVariantList quest_list;
    foreach (QQuestionWidget * question, questionWidgets)
    {
        quest_map["question"] = question -> question();//QObjectHelper::qobject2qvariant(question);
        quest_map["answer"] = question -> answer();
        quest_map["rating"] = question -> rating();
        quest_list.append(quest_map);
    }
    return_map["questions"] = quest_list;
    return_map["name"] = themeWidget -> themeName();
    return_map["color"] = themeWidget -> themeColor().rgb();
    if (!themeWidget -> picture().isNull())
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        themeWidget -> picture().save(&buffer, "PNG");
        return_map["picture"] = ba.toBase64();
    }
    return return_map;
}


void QTheme::setThemeMap(const QVariantMap & map)
{
    QVariantList questions = map["questions"].toList();
    while (questionWidgets.count())
    {
        delete questionWidgets.first();
        questionWidgets.removeFirst();
    }

    foreach (QVariant question, questions)
    {
        QVariantMap quest_map = question.toMap();
        questionWidgets.append(new QQuestionWidget(dynamic_cast<QWidget *>(parent())));
        if (quest_map["question"].isValid())
            questionWidgets.last() -> setQuestion(quest_map["question"].toString());
        if (quest_map["answer"].isValid())
            questionWidgets.last() -> setAnswer(quest_map["answer"].toString());
        if (quest_map["rating"].isValid())
            questionWidgets.last() -> setRating(quest_map["rating"].toInt());
        connect(themeWidget,SIGNAL(signalColorChanged(QColor)),questionWidgets.last(),SLOT(setColor(QColor)));
        connect(questionWidgets.last() ,SIGNAL(signalQuestionClicked(QQuestionWidget*,Qt::MouseButton)),this,SIGNAL(signalQuestionClicked(QQuestionWidget*,Qt::MouseButton)));
    }

    if (map["color"].isValid())
        themeWidget -> setThemeColor(QColor((QRgb)map["color"].toInt()));
    if (map["name"].isValid())
        themeWidget -> setThemeName(map["name"].toString());
    if (map["picture"].isValid())
    {
        themeWidget -> setPicture(QImage::fromData(QByteArray::fromBase64(map["picture"].toByteArray()),"PNG"));
    }
}


void QTheme::setEditable(bool value)
{
    foreach (QQuestionWidget * widget,questionWidgets)
        widget -> setEditable(value);
    themeWidget -> setEditable(value);
}

bool QTheme::isChild(QWidget * w) const
{
    foreach (QWidget * child, questionWidgets)
        if (child == w)
            return true;
    return false;
}
