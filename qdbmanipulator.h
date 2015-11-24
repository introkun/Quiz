#ifndef QDBMANIPULATOR_H
#define QDBMANIPULATOR_H
#include <QtGui>
#include <QFile>
#include <QDir>
#include <QString>
#include <QTime>
#include <QObject>
#include "game/qgame.h"
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
    static QList<QGame *>  load(const QString & fileName,QString & error,QWidget * parent);
    /* Сохранение информации в файл
     * Входные данные:
     * fileName - имя файла(с путем к нему) для сохранения
     * theme_list - список данных для сохранения
     * Выходные данные
     * Возвращает true в случае успешного сохранения, иначе false
     * error - ошибка
     */
    static bool save(const QString & fileName,const QList<QGame *> & game_list,QString & error);

    static bool loadConfiguration(QList<QRegistrationDialog::REG_DEVICE_T> & devices);
    static bool saveConfiguration(const QList<QRegistrationDialog::REG_DEVICE_T> & reg_devices);

};

#endif // QDBMANIPULATOR_H
