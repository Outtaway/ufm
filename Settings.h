#pragma once

#include <map>

#include <QString>
#include <QSettings>
#include <QVariant>

class Settings
{
public:

    enum class OPTIONS
    {
        DELETE_APPROVAL = 0,
        MAX_RECENT
    };

    Settings() = delete;
    Settings(const Settings& other) = delete;

    static QVariant getOption(const OPTIONS option);

    static void setOption(const OPTIONS option, const QVariant value);

    static bool optionExist(const OPTIONS options);

private:

    // C++17 feature
    static inline const QString settings_file_name_ = "application.ini";
    static inline const QSettings::Format settings_format_ = QSettings::Format::IniFormat;

    static inline QSettings settings_ = { settings_file_name_, settings_format_ };

    static inline const std::map<OPTIONS, QString> string_options_ =
    {
        { OPTIONS::DELETE_APPROVAL, "DELETE_APPROVAL" },
        { OPTIONS::MAX_RECENT, "RECENT_COUNT" }
    };
};
