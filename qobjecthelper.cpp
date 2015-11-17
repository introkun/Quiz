#include "qobjecthelper.h"

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QObject>

class QObjectHelper::QObjectHelperPrivate {
};

QObjectHelper::QObjectHelper()
  : d (new QObjectHelperPrivate)
{
}

QObjectHelper::~QObjectHelper()
{
  delete d;
}

QVariantMap QObjectHelper::qobject2qvariant( const QObject* object,
                              const QStringList& ignoredProperties)
{
  QVariantMap result;
  const QMetaObject *metaobject = object->metaObject();
  int count = metaobject->propertyCount();
  for (int i=0; i<count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();

    if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
      continue;

    result[QLatin1String(name)] = object->property(name);
 }
  return result;
}

void QObjectHelper::qvariant2qobject(const QVariantMap& variant, QObject* object)
{
  const QMetaObject *metaobject = object->metaObject();

  QVariantMap::const_iterator iter;
  for (iter = variant.constBegin(); iter != variant.constEnd(); ++iter) {
      QVariant key = iter.key();
    int pIdx = metaobject->indexOfProperty(key.toString().toLatin1().data());

    if ( pIdx < 0 ) {
      continue;
    }

    QMetaProperty metaproperty = metaobject->property( pIdx );
    QVariant::Type type = metaproperty.type();
    QVariant v( iter.value() );
    if ( v.canConvert( type ) ) {
      v.convert( type );
      metaproperty.write( object, v );
    } else if (QString(QLatin1String("QVariant")).compare(QLatin1String(metaproperty.typeName())) == 0) {
     metaproperty.write( object, v );
    }
  }
}
