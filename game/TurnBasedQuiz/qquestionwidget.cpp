#include "qquestionwidget.h"

QQuestionWidget::QQuestionWidget(QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::StyledPanel);
    layout = new QVBoxLayout(this);
    layout_question = new QHBoxLayout();
    layout_answer  = new QHBoxLayout();
    questLineEdit = new QLineEdit();
    ansLineEdit = new QLineEdit();
    questLabel = new QLabel(tr("Вопрос"));
    ansLabel = new QLabel(tr("Ответ"));
    ratingLabel = new QClickedLabel();
    questLineEdit -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    ansLineEdit -> setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    questLabel -> setAlignment(Qt::AlignHCenter);
    ansLabel -> setAlignment(Qt::AlignHCenter);
    ratingLabel -> setAlignment(Qt::AlignCenter);
    layout -> setSpacing(1);
    layout -> setMargin(1);
    layout_answer -> setSpacing(1);
    layout_question -> setSpacing(1);
    layout -> addLayout(layout_question);
    layout -> addLayout(layout_answer);
    layout_question -> addWidget(questLabel);
    layout_question -> addWidget(questLineEdit);
    layout_answer -> addWidget(ansLabel);
    layout_answer -> addWidget(ansLineEdit);
    layout -> addWidget(ratingLabel);
    layout -> setSpacing(1);
    setEditable(true);
    setRating(1);
    connect(questLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setQuestion(QString)));
    connect(ansLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setAnswer(QString)));
    connect(ratingLabel,SIGNAL(clicked(Qt::MouseButton)),this,SLOT(slotQuestionClicked(Qt::MouseButton)));
    ratingLabel -> setCursor(QCursor(Qt::PointingHandCursor));
}


QQuestionWidget::~QQuestionWidget()
{
    delete questLabel;
    delete questLineEdit;
    delete ansLabel;
    delete ansLineEdit;
    delete ratingLabel;
    delete layout_question;
    delete layout_answer;
    delete layout;
}


void QQuestionWidget::setEditable(bool value)
{
    edit_mode = value;
    questLabel -> setVisible(edit_mode);
    questLineEdit -> setVisible(edit_mode);
    ansLabel -> setVisible(edit_mode);
    ansLineEdit -> setVisible(edit_mode);
    ratingLabel -> setVisible(!edit_mode);
    //отображаем виджет только в режиме редактирования либо когда заполнен и вопрос и ответ
    setVisible(value || (!question().isEmpty() && !answer().isEmpty()));
}


void QQuestionWidget::setColor(const QColor & value)
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, value);
    setAutoFillBackground(true);
    setPalette(Pal);
}


void QQuestionWidget::setRating(unsigned int value)
{
    rate = value;
    ratingLabel -> setText("<h1>" + QString::number(rate) + "</h1>");
}

void QQuestionWidget::setQuestion(const QString & value)
{
    disconnect(questLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setQuestion(QString)));
    quest = value;
    questLineEdit -> setText(value);
    connect(questLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setQuestion(QString)));
    if (edit_mode)
        emit signalNeedSave();
}


void QQuestionWidget::setAnswer(const QString & value)
{
    disconnect(ansLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setAnswer(QString)));
    ans = value;
    ansLineEdit -> setText(value);
    connect(ansLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setAnswer(QString)));
    if (edit_mode)
        emit signalNeedSave();
}


void QQuestionWidget::slotQuestionClicked(Qt::MouseButton button)
{
    emit signalQuestionClicked(this,button);
}
