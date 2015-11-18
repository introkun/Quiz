#ifndef QQUESTIONSTABLEWIDGET_H
#define QQUESTIONSTABLEWIDGET_H

#include <QTableWidget>
#include <QMenu>
class QQuestionsTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit QQuestionsTableWidget(QWidget *parent = 0);
    ~QQuestionsTableWidget();
private slots:
    void menuPopUp();
private:
    void initMenu();
    void deleteMenu();

    QMenu * menu;
};

#endif // QQUESTIONSTABLEWIDGET_H
