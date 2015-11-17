#include "qquestiondialog.h"

#define ATTENTION_TEXT tr("Внимание, вопрос!")
#define ANSWER_TIME_SEC 45
QQuestionDialog::QQuestionDialog(QWidget *parent,const QString & question, const QString & answer,const QImage & image, const QList<QFont> & fonts, const QList<QRegistrationDialog::REG_DEVICE_T> & reg_rc, const QString & caption) : QDialog(parent)
{
    this -> reg_rc = reg_rc;
    this -> question = question;
    this -> answer = answer;
    this -> caption = caption;
    fast_answer = false;
    stage = STAGE_PREPARE;
    ticks = 0;
    layout_work = new QHBoxLayout(this);
    layout_quest = new QVBoxLayout();
    layout_queue = new QVBoxLayout();
    label_caption = new QLabel(ATTENTION_TEXT);
    label_question = new QLabel();
    label_picture = new QLabel();
    label_answer = new QLabel();
    label_time = new QLabel();
    label_queue_caption = new QLabel("<b>" + tr("Очередь команд:\n") + "</b>");
    label_queue = new QLabel();
    button_control = new QPushButton(tr("Спрятать"));
    spacer_queue = new QSpacerItem(1,10,QSizePolicy::Minimum,QSizePolicy::Expanding);
    spacer_question = new QSpacerItem(1,10,QSizePolicy::Minimum,QSizePolicy::Expanding);
    frame_question = new QFrame();
    layout_work -> addWidget(frame_question);
    frame_question -> setLayout(layout_quest);
    layout_work -> addLayout(layout_queue);

    layout_queue -> addWidget(label_queue_caption);
    layout_queue -> addWidget(label_queue);
    layout_queue -> addSpacerItem(spacer_queue);
    layout_queue -> addWidget(button_control);
    layout_quest -> addWidget(label_caption);
    layout_quest -> addWidget(label_picture);
    layout_quest -> addWidget(label_question);
    layout_quest -> addWidget(label_time);
    layout_quest -> addWidget(label_answer);
    layout_quest -> addSpacerItem(spacer_question);

    timer = new QTimer();
    frame_question -> setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setWindowFlags(Qt::Window);
    showFullScreen();
    label_caption -> setAlignment(Qt::AlignHCenter);
    label_question -> setAlignment(Qt::AlignHCenter);
    label_answer -> setAlignment(Qt::AlignHCenter);
    label_time -> setAlignment(Qt::AlignHCenter);
    label_picture -> setAlignment(Qt::AlignHCenter);
    label_queue_caption -> setAlignment(Qt::AlignHCenter);
    if (image.isNull())
        label_picture -> hide();
    else
        label_picture -> setPixmap(QPixmap::fromImage(image).scaled(QSize(width()/2,height()/3),Qt::KeepAspectRatio));
    label_queue -> setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    connect(button_control,SIGNAL(clicked()),this,SLOT(reject()));
    timer -> start(100);
    /*setStyleSheet("QDialog {color: qlineargradient(spread:pad, x1:0 y1:0, x2:1 y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                           "background: qlineargradient( x1:0 y1:0, x2:1 y2:0, stop:0 cyan, stop:1 blue);}");*/

    //label_question -> setStyleSheet("QLabel {border-style: solid; border-width: 3px;border-radius: 7px;}");
    //label_answer -> setStyleSheet("QLabel {border-style: solid; border-width: 3px;border-radius: 7px;}");
    //label_time -> setStyleSheet("QLabel {border-style: solid; border-width: 3px;border-radius: 7px;}");
    label_question -> setWordWrap(true);
    label_answer -> setWordWrap(true);
    label_question -> hide();
    label_answer -> hide();
    label_time -> hide();

    QPalette pal = frame_question -> palette();
    QPixmap pix = QPixmap::fromImage(QImage(":/images/papirus.jpg")).scaled(frame_question -> size());
    pal.setBrush(QPalette::Window,QBrush(pix));
    frame_question -> setPalette(pal);
    frame_question -> setAutoFillBackground(true);


    //button_control->setFlat(true);background-color: transparent ;
    button_control -> setFixedHeight(button_control -> height() * 3);
    //button_control -> setStyleSheet("QPushButton {  border-style: solid; border-width: 3px;border-radius: 7px;}");
    //button_control -> setWindowOpacity(0);

    if (fonts.size() >= 6)
    {
        label_caption -> setFont(fonts.at(0));
        label_question -> setFont(fonts.at(1));
        label_time -> setFont(fonts.at(2));
        label_answer -> setFont(fonts.at(3));
        label_queue -> setFont(fonts.at(4));
        label_queue_caption -> setFont(fonts.at(4));
        button_control -> setFont(fonts.at(5));
    }
    main_sound = new QSound(":/sounds/time.wav");
    main_sound -> setLoops(-1);

    button_control -> setStyleSheet("QPushButton{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 white, stop: 1 grey); border-style: solid; border-color: black;border-width: 5px; border-radius: 10px;}");
    green_clicks = 0;
}

QQuestionDialog::~QQuestionDialog()
{
    disconnect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    timer -> stop();
    layout_queue -> removeItem(spacer_queue);
    layout_quest -> removeItem(spacer_question);
    delete spacer_queue;
    delete spacer_question;
    delete button_control;
    delete label_caption;
    delete label_question;
    delete label_answer;
    delete label_time;
    delete label_queue;
    delete label_queue_caption;
    delete label_picture;
    delete layout_quest;
    delete frame_question;

    delete layout_queue;
    delete layout_work;
    delete timer;
    delete main_sound;
}


void QQuestionDialog::answerReady()
{
    ticks = ANSWER_TIME_SEC * 10;
    fast_answer = true;
    timerTick();
}

void QQuestionDialog::showAnswer()
{
    label_answer -> show();
    label_answer -> setText("<font color = ""green"">" + tr("Правильный ответ") + "</font>: " + answer);
    disconnect(button_control,SIGNAL(clicked()),this,SLOT(showAnswer()));
    button_control -> setText(tr("Закрыть"));
    connect(button_control,SIGNAL(clicked()),this,SLOT(accept()));
}

void QQuestionDialog::timerTick()
{

    switch (stage)
    {
    case STAGE_PREPARE:
        if (ticks % 10 == 0)
        {
            QString color = "red";
            if (ticks % 20 == 0)
                color = "black";
            label_caption -> setText(QString("<font color=""%1"">").arg(color) + ATTENTION_TEXT + "</font>");
        }
        if (ticks >= 40)
        {
            ticks = -1;
            stage = STAGE_QUESTION;
            next_tick = 0;
            question_parts = 0;
            if (!caption.isEmpty())
                label_caption -> setText(caption);

        }
        break;

    case STAGE_QUESTION:

        label_question -> show();

        if (next_tick == ticks)
        {
            QStringList sl = question.split(" ");
            question_parts++;
            if (question_parts > (unsigned int)sl.count())
            {
                ticks = -1;
                stage = STAGE_TIMING;
                main_sound -> play();
                disconnect(button_control,SIGNAL(clicked()),this,SLOT(reject()));
                button_control -> setText(tr("Досрочный ответ"));
                connect(button_control,SIGNAL(clicked()),this,SLOT(answerReady()));
                break;
            }
            while (question_parts < (unsigned int)sl.count())
                sl.removeLast();
            label_question -> setText(sl.join(" "));
            next_tick = sl.join(" ").count();
        }

        break;

    case STAGE_TIMING:
        if (ticks % 10 == 0)
        {
            label_time -> show();
            label_time -> setText(tr("Время для размышления: %1 секунд").arg(QString::number(ANSWER_TIME_SEC - ticks / 10)));
            if (ANSWER_TIME_SEC == ticks / 10)
            {
                stage = STAGE_ANSWERING;
                if (!fast_answer)
                    QSound::play(":/sounds/answer.wav");
                disconnect(button_control,SIGNAL(clicked()),this,SLOT(answerReady()));

                button_control -> setText(tr("Показать правильный ответ"));
                label_time -> hide();
                connect(button_control,SIGNAL(clicked()),this,SLOT(showAnswer()));
            }
        }

        break;
    case STAGE_ANSWERING:
        if (!main_sound -> isFinished())
            main_sound -> stop();
        break;
    }
    ticks++;
}


void QQuestionDialog::rcClicked(unsigned int mac)
{

    for (int i = 0; i < rc_macs.count(); i++)
        if (rc_macs.at(i) == mac)
            return;
    rc_macs.append(mac);
    QString color;
    QString bonus;
    if (stage != STAGE_TIMING && stage != STAGE_ANSWERING)
    {
        color = "red";
        bonus = tr(" (-1 балл)");
        QSound::play(":/sounds/falsestart.wav");
    }
    else
    {
        green_clicks++;
        if(green_clicks > 3)//отобржаем 3 первых нажавших пульта
            return;
        color = "green";
        QSound::play(":/sounds/truestart.wav");
    }
    QString command_name;
    for (int i = 0; i < reg_rc.count(); i++)
    {
        if (reg_rc.at(i).mac == mac && !reg_rc.at(i).name.isEmpty())
        {
            command_name = reg_rc.at(i).name;
            break;
        }
    }
    label_queue -> setText(label_queue -> text() + QString("<br><font color = ""%1""><b>").arg(color) + (command_name.isEmpty() ? QString::number(mac) : command_name) + bonus + "</b></font>");
}

void QQuestionDialog::showEvent (QShowEvent * event)
{

    QDialog::showEvent(event);
}
