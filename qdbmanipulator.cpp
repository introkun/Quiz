#include <QVariantMap>
#include "qdbmanipulator.h"
#include <QJsonDocument>
#include "qobjecthelper.h"

QDbManipulator::QDbManipulator(QObject *parent) :
    QObject(parent)
{
}


/* Загрузка информации из файла
 * Входные данные:
 * fileName - имя файла(с путем к нему) для загрузки
 * Выходные данные
 * Возвращает списки данных для загрузки
 * error - ошибка
 */
QList<QGame *>  QDbManipulator::load(const QString & fileName,QString & error,QWidget * parent)
{
    bool ok;
    QList<QGame *> games_return_list;
    QFile jf(fileName);
    if (!jf.open(QIODevice::ReadOnly))
    {
        error = tr("Ошибка открытия файла ") + fileName;
        return games_return_list;
    }
    QByteArray bytes = jf.readAll();
    QJsonParseError json_error;
    QJsonDocument json_doc = QJsonDocument::fromJson(bytes,&json_error);
    QJsonObject json_obj;
    if (json_error.error != QJsonParseError::NoError)
    {
        error = json_error.errorString();
        return games_return_list;
    }
    json_obj = json_doc.object();
    QVariantMap file_map = json_obj.toVariantMap();

    QVariantList games_list = file_map["games"].toList();
    foreach(QVariant game,games_list)
    {
        QVariantMap game_map = game.toMap();
        int game_type = game_map["type"].toInt(&ok);
        if (!ok)
            continue;
        //пробуем создать игру заданного типа
        QGame * createGame = QGame::createGame((QGame::GAME_TYPE)game_type,parent);
        if (createGame)
        {
            games_return_list.append(createGame);
            createGame -> setFromJsonData(game_map);//внутренняя настройка игры с карты
        }
    }

    return games_return_list;
}


/* Сохранение информации в файл
 * Входные данные:
 * fileName - имя файла(с путем к нему) для сохранения
 * game_list - список игр для сохранения
 * Выходные данные
 * Возвращает true в случае успешного сохранения, иначе false
 * error - ошибка
 */
bool QDbManipulator::save(const QString & fileName,const QList<QGame *> & game_list, QString & error)
{
    QFile jf(fileName);
    if (!jf.open(QIODevice::WriteOnly))
    {
        error = tr("Ошибка открытия файла ") + fileName;
        return false;
    }
    QVariantMap file_map;//карта файл
    QVariantList tmp_game_list;
    foreach(QGame * game, game_list)
        tmp_game_list.append(game -> getJsonData());
    file_map["games"] = tmp_game_list;
    QJsonObject json_obj;
    QJsonDocument json_doc;
    json_obj = QJsonObject::fromVariantMap(file_map);
    json_doc.setObject(json_obj);
    QByteArray json_data = json_doc.toJson();
    jf.write(json_data);
    jf.close();
    return true;
}


bool QDbManipulator::loadConfiguration(QList<QRegistrationDialog::REG_DEVICE_T> & devices)
{
    QFile jf("registration.conf");
    if (!jf.open(QIODevice::ReadOnly))
        return false;

    QByteArray bytes = jf.readAll();
    QJsonParseError json_error;
    QJsonDocument json_doc = QJsonDocument::fromJson(bytes,&json_error);
    QJsonObject json_obj;
    if (json_error.error != QJsonParseError::NoError)
        return false;

    json_obj = json_doc.object();
    QVariantMap file_map = json_obj.toVariantMap();

    QVariantList reg_list = file_map["registration"].toList();

    //выделяем карты конфигураций
    foreach(QVariant value, reg_list)
    {
        QVariantMap reg_map = value.toMap();
        QRegistrationDialog::REG_DEVICE_T reg_device;
        reg_device.mac = (unsigned int)reg_map["id"].toInt();
        reg_device.name = reg_map["name"].toString();
        devices.append(reg_device);
    }
    return true;
}

bool QDbManipulator::saveConfiguration(const QList<QRegistrationDialog::REG_DEVICE_T> & reg_devices)
{
    QFile jf("registration.conf");
    if (!jf.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QVariantMap file_map;//карта файл
    QVariantList reg_list;
    foreach (QRegistrationDialog::REG_DEVICE_T reg_device, reg_devices)
    {
        QVariantMap reg_map;
        reg_map["id"] = reg_device.mac;
        reg_map["name"] = reg_device.name;
        reg_list.append(reg_map);
    }

    file_map["registration"] = reg_list;
    QJsonObject json_obj;
    QJsonDocument json_doc;
    json_obj = QJsonObject::fromVariantMap(file_map);
    json_doc.setObject(json_obj);
    QByteArray json_data = json_doc.toJson();
    jf.write(json_data);
    jf.close();
    return true;
}

