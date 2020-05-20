#pragma once

#include <map>
#include <memory>

#include <QString>
#include <QStandardPaths>
#include <QIcon>
#include <QSqlDatabase>

#include "SectionBase.h"

class QuickAccessSection : public SectionBase
{
public:

    QuickAccessSection(QTreeWidgetItem* section, QString name);

    ~QuickAccessSection();

    void addItem(QString name, QString path) override;

    void deleteItem(QString name) override;

    void updateUi() override;

    QString getPathByName(QString name) override;

    void setup() override;

private:

    bool importQuickAccessFromDatabase_();

    bool exportQuickAccessToDatabase_();

    bool openDatabase_();

    bool isStandartLocation_(QString name);

    enum QUICK_ACCESS_TABLE_COLUMNS
    {
        NAME = 0,
        PATH
    };

    const char* const QUICK_ACCESS_DB_NAME = "quick_access.db";
    const char* const QUICK_ACCESS_TABLE_NAME = "quick_access";
    std::unique_ptr<QSqlDatabase> db_;

    std::map<QString, QStringList> standart_locations_;
    const char* const DESKTOP = "Desktop";
    const char* const DOCUMENTS = "Documents";
    const char* const DOWNLOADS = "Downloads";
    const char* const MUSIC = "Music";
    const char* const PICTURES = "Pictures";
    const char* const MOVIES = "Movies";

    std::map<QString, QString>          quick_items_;
    std::map<QString, QIcon>            locations_png_;

};
