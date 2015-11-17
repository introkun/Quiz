#ifndef QREGISTRATIONDIALOG_H
#define QREGISTRATIONDIALOG_H
#include "typedefs.h"
#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
class QRegistrationDialog : public QDialog
{
    Q_OBJECT
public:
    typedef struct {
        unsigned int mac;
        QString name;
    } REG_DEVICE_T;
    explicit QRegistrationDialog(QWidget *parent = 0,const QList<REG_DEVICE_T> & regged = QList<REG_DEVICE_T>());
    virtual ~QRegistrationDialog();
    QList<REG_DEVICE_T> getRegDevices();
public slots:
    void rcLicked(unsigned int mac);
private slots:
    void delDeviceFromTable();
private:
    void addDeviceToTable(REG_DEVICE_T device);
    QVBoxLayout * layout;
    QTableWidget * table;
    QLabel * label_last;
    QHBoxLayout * layout_buttons;
    QPushButton * button_ok;
    QPushButton * button_cancel;
    QList<QPushButton *> buttons_del;
    QList<REG_DEVICE_T> devices;
};

#endif // QREGISTRATIONDIALOG_H
