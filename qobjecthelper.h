
#ifndef QOBJECTHELPER_H
#define QOBJECTHELPER_H

#include <QtCore/QLatin1String>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtCore/QVariant>
  class QObjectHelper {
    public:
      QObjectHelper();
      ~QObjectHelper();

    /**
    * This method converts a QObject instance into a QVariantMap.
    *
    * @param object The QObject instance to be converted.
    * @param ignoredProperties Properties that won't be converted.
    */
    static QVariantMap qobject2qvariant( const QObject* object,
                                  const QStringList& ignoredProperties = QStringList(QString(QLatin1String("objectName"))));

    /**
    * This method converts a QVariantMap instance into a QObject
    *
    * @param variant Attributes to assign to the object.
    * @param object The QObject instance to update.
    */
    static void qvariant2qobject(const QVariantMap& variant, QObject* object);

    private:
      Q_DISABLE_COPY(QObjectHelper)
      class QObjectHelperPrivate;
      QObjectHelperPrivate* const d;
  };


#endif // QOBJECTHELPER_H
