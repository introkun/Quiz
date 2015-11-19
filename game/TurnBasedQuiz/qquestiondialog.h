#ifndef QQUESTIONDIALOG_H
#define QQUESTIONDIALOG_H
#include "qregistrationdialog.h"
#include <QWidget>
#include <QDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTextEdit>
#include <QImage>
#include <QSound>
class QQuestionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QQuestionDialog(QWidget *parent = 0,const QString & question = QString(), const QString & answer = QString(),const QImage & image = QImage(),const QList<QFont> & fonts = QList<QFont>(), const QList<QRegistrationDialog::REG_DEVICE_T> & reg_rc = QList<QRegistrationDialog::REG_DEVICE_T>(),const QString & caption = QString());
    ~QQuestionDialog();
public slots:
    void rcClicked(unsigned int mac,unsigned short group);
protected:
    virtual void showEvent (QShowEvent * event);
private slots:
    void timerTick();
    void answerReady();
    void showAnswer();
private:
    typedef enum {
        STAGE_PREPARE,
        STAGE_QUESTION,
        STAGE_TIMING,
        STAGE_ANSWERING
    } STAGE_T;
    QHBoxLayout * layout_work;
    QVBoxLayout * layout_quest;
    QVBoxLayout * layout_queue;
    QLabel * label_caption;
    QLabel * label_question;
    QLabel * label_picture;
    QLabel * label_answer;
    QLabel * label_time;
    QLabel * label_queue_caption;
    QLabel * label_queue;
    QFrame * frame_question;
    QPushButton * button_control;
    QSpacerItem * spacer_queue;
    QSpacerItem * spacer_question;
    QTimer * timer;
    STAGE_T stage;
    unsigned int ticks;
    unsigned int next_tick;
    unsigned int question_parts;
    QString question;
    QString answer;
    QString caption;
    QList<unsigned int> rc_macs;
    QList<QRegistrationDialog::REG_DEVICE_T> reg_rc;
    QSound * main_sound;
    bool fast_answer;
    unsigned int green_clicks;
};

#endif // QQUESTIONDIALOG_H
