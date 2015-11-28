#include "qgamesettingsdialog.h"

QGameSettingsDialog::QGameSettingsDialog(QWidget *parent, const QList<QGame *> & games) : QDialog(parent)
{
    layout_main = new QVBoxLayout(this);
    tab_widget = new QTabWidget();
    layout_buttons = new QHBoxLayout();
    button_close = new QPushButton(tr("Закрыть"));
    spacer_buttons = new QSpacerItem(50,1,QSizePolicy::Expanding,QSizePolicy::Minimum);
    foreach (QGame * game, games)
    {
        widgets.append(new QGameSettingsWidget(this,game));
        tab_widget -> addTab(widgets.last(),game -> name());
    }
    layout_main -> addWidget(tab_widget);
    layout_main -> addLayout(layout_buttons);
    layout_buttons -> addSpacerItem(spacer_buttons);
    layout_buttons -> addWidget(button_close);
    connect(button_close,SIGNAL(clicked()),this,SLOT(accept()));
}


QGameSettingsDialog::~QGameSettingsDialog()
{
    layout_buttons -> removeItem(spacer_buttons);
    delete spacer_buttons;
    delete button_close;
    foreach (QGameSettingsWidget * widget,widgets)
        delete widget;
    delete tab_widget;
    delete layout_buttons;
    delete layout_main;
}


void QGameSettingsDialog::showEvent(QShowEvent * event)
{
    adjustSize();
    QDialog::showEvent(event);
}
