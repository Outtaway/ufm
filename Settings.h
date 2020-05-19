#pragma once

#include <map>

#include <QString>
#include <QSettings>
#include <QVariant>

class Settings : public QSettings
{
    Q_OBJECT
public:

    enum class OPTIONS
    {
        DELETE_APPROVAL = 0,
        MAX_RECENT,
        SHOW_SIZE,
        SHOW_TYPE,
        SHOW_LAST_MODIFIED
    };

    static Settings& getInstance();

    Settings(const Settings& other) = delete;

    QVariant getOption(const OPTIONS option);

    bool optionExist(const OPTIONS options);

    void setOption(const OPTIONS option, const QVariant value);

signals:
    void optionChanged(const OPTIONS option, const QVariant value);

private:

    Settings();

    // C++17 feature
    static inline const QString settings_file_name_ = "application.ini";
    static inline const QSettings::Format settings_format_ = QSettings::Format::IniFormat;

    static inline QSettings settings_ = { settings_file_name_, settings_format_ };

    static inline const std::map<OPTIONS, QString> string_options_ =
    {
        { OPTIONS::DELETE_APPROVAL, "DELETE_APPROVAL" },
        { OPTIONS::MAX_RECENT, "RECENT_COUNT" },
        { OPTIONS::SHOW_SIZE, "SHOW_SIZE" },
        { OPTIONS::SHOW_TYPE, "SHOW_TYPE" },
        { OPTIONS::SHOW_LAST_MODIFIED, "SHOW_LAST_MODIFIED" },
    };
};
