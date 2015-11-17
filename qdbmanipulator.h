#ifndef QDBMANIPULATOR_H
#define QDBMANIPULATOR_H
#include <QtGui>
#include <QFile>
#include <QDir>
#include <QString>
#include <QTime>
#include <QObject>
#include "game/TurnBasedQuiz/qtheme.h"
#include "qregistrationdialog.h"
class QDbManipulator : public QObject
{
    Q_OBJECT
public:

    explicit QDbManipulator(QObject *parent = 0);
    /* Загрузка информации из файла
     * Входные данные:
     * fileName - имя файла(с путем к нему) для загрузки
     * Выходные данные
     * Возвращает списки данных для загрузки
     * error - ошибка
     */
    static QList<QTheme *>  load(const QString & fileName,QImage & picture,QString & error,QWidget * theme_parent);
    /* Сохранение информации в файл
     * Входные данные:
     * fileName - имя файла(с путем к нему) для сохранения
     * theme_list - список данных для сохранения
     * Выходные данные
     * Возвращает true в случае успешного сохранения, иначе false
     * error - ошибка
     */
    static bool save(const QString & fileName,const QList<QTheme *> & theme_list,const QImage & picture, QString & error);

    static bool loadConfiguration(QList<QRegistrationDialog::REG_DEVICE_T> & devices, QList<QFont> & fonts);
    static bool saveConfiguration(const QList<QRegistrationDialog::REG_DEVICE_T> & reg_devices,const QList<QFont> & fonts);

};

#endif // QDBMANIPULATOR_H
