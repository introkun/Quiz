#ifndef QQUESTIONWIDGET_H
#define QQUESTIONWIDGET_H

#include <QFrame>
#include "qclickedlabel.h"
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
class QQuestionWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY( QString question READ question WRITE setQuestion )
    Q_PROPERTY( QString answer READ answer WRITE setAnswer )
    Q_PROPERTY( unsigned int rating READ rating WRITE setRating )
public:

    explicit QQuestionWidget(QWidget *parent = 0);
    ~QQuestionWidget();
    void setEditable(bool value);
    bool isEditable() const {return edit_mode;}

    QString question() const {return quest;}

    QString answer() const {return ans;}

    unsigned int rating() const {return rate;}

public slots:
    void setColor(const QColor & value);
    void setRating(unsigned int value);
    void setQuestion(const QString & value);
    void setAnswer(const QString & value);
private slots:
    void slotQuestionClicked(Qt::MouseButton button);
signals:
    void signalQuestionClicked(QQuestionWidget *,Qt::MouseButton);
    void signalNeedSave();
private:
    bool edit_mode;
    QString quest;
    QString ans;
    unsigned int rate;

    QVBoxLayout * layout;
    QHBoxLayout * layout_question;
    QHBoxLayout * layout_answer;
    QLineEdit * questLineEdit;
    QLineEdit * ansLineEdit;
    QLabel * questLabel;
    QLabel * ansLabel;
    QClickedLabel * ratingLabel;
};

#endif // QQUESTIONWIDGET_H
