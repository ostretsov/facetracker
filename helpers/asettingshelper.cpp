#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>

#include "asettingshelper.h"

// ========================================================================== //
// Get settings value.
// ========================================================================== //
QVariant ASettingsHelper::value(const QString &key, const QString &group) {
    QSettings settings(qApp->property("conf").toString(), QSettings::IniFormat);

    if(!group.isEmpty()) settings.beginGroup(group);

    QVariant value = settings.value(key);

    if(!group.isEmpty()) settings.endGroup();

    return value;
}


// ========================================================================== //
// Get settings value.
// ========================================================================== //
QVariant ASettingsHelper::value(const QString &key, const QVariant &def_value
    , const QString &group) {

    QSettings settings(qApp->property("conf").toString(), QSettings::IniFormat);

    if(!group.isEmpty()) settings.beginGroup(group);

    QVariant value = settings.value(key, def_value);

    if(!group.isEmpty()) settings.endGroup();

    return value;
}


// ========================================================================== //
// Set settings value.
// ========================================================================== //
void ASettingsHelper::setValue(const QString &key, const QVariant &value
    , const QString &group) {

    QSettings settings(qApp->property("conf").toString(), QSettings::IniFormat);

    if(!group.isEmpty()) settings.beginGroup(group);

    settings.setValue(key, value);

    if(!group.isEmpty()) settings.endGroup();
}


// ========================================================================== //
// Clear settings.
// ========================================================================== //
void ASettingsHelper::clear() {
    QSettings(qApp->property("conf").toString(), QSettings::IniFormat).clear();
}
