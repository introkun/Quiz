#ifndef QQUESTION_H
#define QQUESTION_H

#include <QObject>
#include <QStringList>
class QQuestion : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString question READ question WRITE setQuestion)
    Q_PROPERTY(QStringList answers READ answers WRITE setAnswers)
    Q_PROPERTY(QString singleAnswer READ singleAnswer WRITE setSingleAnswer)
    Q_PROPERTY(int rating READ rating WRITE setRating)
    Q_PROPERTY(int goodAnswerIndex READ goodAnswerIndex WRITE setGoodAnswerIndex)
public:
    explicit QQuestion(QObject *parent = 0);
    QQuestion(const QQuestion & question);
    QQuestion &operator =(const QQuestion & question);
    ~QQuestion();

    QString question() const {return _question;}
    void setQuestion(const QString & question) {_question = question;}
    QString singleAnswer() const {return _single_answer;}
    void setSingleAnswer(const QString & single_answer) {_single_answer = single_answer;}
    QStringList answers() const {return _answers;}
    void setAnswers(const QStringList & answers) {_answers = answers;}
    int rating() const {return _value;}
    void setRating(int rating) {_value = rating;}
    int goodAnswerIndex() const {return _good_answer;}
    void setGoodAnswerIndex(int index) {_good_answer = index;}
private:
    QString _question;//вопрос
    QStringList _answers;//список ответов
    QString _single_answer;//одиночный ответ
    int _value;//сложность вопроса
    int _good_answer;//индекс правильного ответа
};

#endif // QQUESTION_H
