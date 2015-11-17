#ifndef QQUESTIONSTABLEWIDGET_H
#define QQUESTIONSTABLEWIDGET_H

#include <QTableWidget>

class QQuestionsTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit QQuestionsTableWidget(QWidget *parent = 0);
    ~QQuestionsTableWidget();

signals:

public slots:
};

#endif // QQUESTIONSTABLEWIDGET_H
