#ifndef ASETTINGSHELPER_H
#define ASETTINGSHELPER_H

#include <QtCore/QVariant>

class ASettingsHelper {
    public:
        //! Get settings value.
        static QVariant value(const QString &key
            , const QString &group = QString());

        //! Get settings value.
        static QVariant value(const QString &key, const QVariant &def_value
            , const QString &group = QString());

        //! Set settings value.
        static void setValue(const QString &key, const QVariant &value
            , const QString &group = QString());

        //! Clear settings.
        static void clear();

};

#endif
