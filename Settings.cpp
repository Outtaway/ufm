#include "Settings.h"

QVariant Settings::getOption(const Settings::OPTIONS option)
{
    settings_.sync();
    return settings_.value(string_options_.at(option));
}

void Settings::setOption(const Settings::OPTIONS option, const QVariant value)
{
    settings_.sync();
    settings_.setValue(string_options_.at(option), value);
}

bool Settings::optionExist(const Settings::OPTIONS option)
{
    settings_.sync();
    return settings_.value(string_options_.at(option)).isValid();
}
