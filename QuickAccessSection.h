#pragma once

#include <map>

#include <QString>
#include <QStandardPaths>

#include "SectionBase.h"

class QuickAccessSection : public SectionBase
{
public:

    QuickAccessSection(QTreeWidgetItem* section, QString name);

    void setup() override;

    void addItem(QString name, QString path) override;

    void updateUi() override;

    QString getPathByName(QString name) override;

private:

    const char* const DESKTOP = "Desktop";
    const char* const DOCUMENTS = "Documents";
    const char* const DOWNLOADS = "Downloads";
    const char* const MUSIC = "Music";
    const char* const PICTURES = "Pictures";
    const char* const MOVIES = "Movies";

    std::map<QString, QString>          standart_locations_;

};