#include "qregistrationdialog.h"

QRegistrationDialog::QRegistrationDialog(QWidget *parent,const QList<REG_DEVICE_T> & regged) :
    QDialog(parent)
{
    layout = new QVBoxLayout(this);
    table = new QTableWidget(0,3);
    label_last = new QLabel();
    layout_buttons = new QHBoxLayout();
    button_ok = new QPushButton(tr("Да"));
    button_cancel = new QPushButton(tr("Нет"));
    foreach (REG_DEVICE_T reg_dev, regged)
        addDeviceToTable(reg_dev);

    layout -> addWidget(label_last);
    layout -> addWidget(table);
    layout -> addLayout(layout_buttons);
    layout_buttons -> addWidget(button_ok);
    layout_buttons -> addWidget(button_cancel);
    connect(button_cancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(button_ok,SIGNAL(clicked()),this,SLOT(accept()));

    QStringList sl;
    sl << tr("Адрес пульта") << tr("Имя команды") << tr("Действие");
    table -> setHorizontalHeaderLabels(sl);
}


QRegistrationDialog::~QRegistrationDialog()
{
    for (int i = 0; i < buttons_del.count(); i++)
        delete buttons_del.at(i);
    delete label_last;
    delete button_cancel;
    delete button_ok;
    delete table;
    delete layout_buttons;
    delete layout;
}


void QRegistrationDialog::addDeviceToTable(REG_DEVICE_T device)
{
    foreach (REG_DEVICE_T old_dev, devices)
        if (old_dev.mac == device.mac)
            return;//не дублируем

    table -> insertRow(table -> rowCount());
    table -> setItem(table -> rowCount() - 1,0,new QTableWidgetItem(QString::number(device.mac)));
    table -> item(table -> rowCount() - 1,0) -> setFlags(Qt::NoItemFlags);
    table -> setItem(table -> rowCount() - 1,1,new QTableWidgetItem(device.name));
    buttons_del.append(new QPushButton(tr("Удалить")));
    connect(buttons_del.last(),SIGNAL(clicked()),this,SLOT(delDeviceFromTable()));
    table -> setCellWidget(table -> rowCount() - 1,2,buttons_del.last());
    devices.append(device);
}


void QRegistrationDialog::delDeviceFromTable()
{
    for (int i = 0; i < table -> rowCount(); i++)
        if (sender() == table -> cellWidget(i,2))
        {
            table -> removeCellWidget(i,2);

            uint32 mac = table -> item(i,0) -> text().toInt();
            table -> removeRow(i);
            for (int j = 0; j < devices.count(); j++)
                if (devices.at(j).mac == mac)
                {
                    devices.removeAt(j);
                    break;
                }
            for (int j = 0; j < buttons_del.count(); j++)
                if (sender() == buttons_del.at(j))
                {
                    delete buttons_del.at(j);
                    buttons_del.removeAt(j);
                    break;
                }
            return;
        }
}


void QRegistrationDialog::rcLicked(unsigned int mac)
{
    REG_DEVICE_T device;
    device.mac = mac;
    label_last -> setText(tr("Последний пульт: %1").arg(QString::number(mac)));
    addDeviceToTable(device);
}


QList<QRegistrationDialog::REG_DEVICE_T> QRegistrationDialog::getRegDevices()
{
    REG_DEVICE_T new_device;
    for(int i = 0; i < table -> rowCount(); i++)
    {
        for(int j = 0; j < devices.count(); j++)
            if ((unsigned int)table -> item(i,0) -> text().toInt() == devices.at(j).mac)
            {
                new_device.mac = devices.at(j).mac;
                new_device.name = table -> item(i,1) -> text();
                devices.replace(j,new_device);
                break;
            }
    }
    return devices;
}
