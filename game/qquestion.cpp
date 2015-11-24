#include "qquestion.h"

QQuestion::QQuestion(QObject *parent) : QObject(parent)
{
    _value = 0;
    _good_answer = -1;
}

QQuestion::~QQuestion()
{

}


QQuestion::QQuestion(const QQuestion & question)
{
    this -> _value = question._value;
    this -> _good_answer = question._good_answer;
    this -> _single_answer = question._single_answer;
    this -> _question = question._question;
    this -> _answers = question._answers;
}


QQuestion &QQuestion::operator =(const QQuestion & question)
{
    *this = QQuestion(question);
    return *this;
}
