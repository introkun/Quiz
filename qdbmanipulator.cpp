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
QList<QTheme *>  QDbManipulator::load(const QString & fileName,QImage & picture,QString & error,QWidget * theme_parent)
{
    QList<QTheme *> themes_return_list;
    QFile jf(fileName);
    if (!jf.open(QIODevice::ReadOnly))
    {
        error = tr("Ошибка открытия файла ") + fileName;
        return themes_return_list;
    }
    QByteArray bytes = jf.readAll();
    QJsonParseError json_error;
    QJsonDocument json_doc = QJsonDocument::fromJson(bytes,&json_error);
    QJsonObject json_obj;
    if (json_error.error != QJsonParseError::NoError)
    {
        error = json_error.errorString();
        return themes_return_list;
    }
    json_obj = json_doc.object();
    QVariantMap file_map = json_obj.toVariantMap();

    QVariantList themes_list = file_map["themes"].toList();

    //выделяем карты конфигураций
    foreach(QVariant value, themes_list)
    {
        QTheme * theme = new QTheme(theme_parent);
        theme -> setThemeMap(value.toMap());
        themes_return_list.append(theme);
    }

    if (file_map["picture"].isValid())
    {
        picture = QImage::fromData(QByteArray::fromBase64(file_map["picture"].toByteArray()),"PNG");
    }
    return themes_return_list;
}


/* Сохранение информации в файл
 * Входные данные:
 * fileName - имя файла(с путем к нему) для сохранения
 * theme_list - список данных для сохранения
 * Выходные данные
 * Возвращает true в случае успешного сохранения, иначе false
 * error - ошибка
 */
bool QDbManipulator::save(const QString & fileName,const QList<QTheme *> & theme_list,const QImage & picture, QString & error)
{
    QFile jf(fileName);
    if (!jf.open(QIODevice::WriteOnly))
    {
        error = tr("Ошибка открытия файла ") + fileName;
        return false;
    }

    QVariantMap file_map;//карта файл
    QVariantList th_list;
    foreach (QTheme * theme, theme_list)
        th_list.append(theme -> themeMap());
    file_map["themes"] = th_list;

    if (!picture.isNull())
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        picture.save(&buffer, "PNG");
        file_map["picture"] = ba.toBase64();
    }

    QJsonObject json_obj;
    QJsonDocument json_doc;
    json_obj = QJsonObject::fromVariantMap(file_map);
    json_doc.setObject(json_obj);
    QByteArray json_data = json_doc.toJson();
    jf.write(json_data);
    jf.close();
    return true;
}


bool QDbManipulator::loadConfiguration(QList<QRegistrationDialog::REG_DEVICE_T> & devices, QList<QFont> & fonts)
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

    QVariantList font_list = file_map["fonts"].toList();

    //выделяем шрифты
    foreach(QVariant value, font_list)
    {
        QFont font;
        if (font.fromString(value.toString()))
            fonts.append(font);
    }
    return true;
}

bool QDbManipulator::saveConfiguration(const QList<QRegistrationDialog::REG_DEVICE_T> & reg_devices, const QList<QFont> & fonts)
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

    QVariantList font_list;
    foreach (QFont font, fonts)
    {
        font_list.append(font.toString());
    }

    file_map["registration"] = reg_list;
    file_map["fonts"] = font_list;
    QJsonObject json_obj;
    QJsonDocument json_doc;
    json_obj = QJsonObject::fromVariantMap(file_map);
    json_doc.setObject(json_obj);
    QByteArray json_data = json_doc.toJson();
    jf.write(json_data);
    jf.close();
    return true;
}

