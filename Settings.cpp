#include "Settings.h"

#include <QDebug>
Settings& Settings::getInstance()
{
    static Settings instance;

    return instance;
}

Settings::Settings()
{ }

QVariant Settings::getOption(const Settings::OPTIONS option)
{
    settings_.sync();
    return settings_.value(string_options_.at(option));
}

void Settings::setOption(const Settings::OPTIONS option, const QVariant value)
{
    settings_.sync();
    settings_.setValue(string_options_.at(option), value);
    emit optionChanged(option, value);
}

bool Settings::optionExist(const Settings::OPTIONS option)
{
    settings_.sync();
    return settings_.value(string_options_.at(option)).isValid();
}
