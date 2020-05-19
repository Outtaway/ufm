#include "QuickAccessSection.h"

#include <QTreeWidget>

QuickAccessSection::QuickAccessSection(QTreeWidgetItem* section, QString name) : SectionBase(section, name)
{
    setup();
}

void QuickAccessSection::setup()
{
    std::map<QString, QStringList> standart_locations;

    standart_locations.emplace(DESKTOP, QStandardPaths::standardLocations(QStandardPaths::DesktopLocation));
    standart_locations.emplace(DOCUMENTS, QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
    standart_locations.emplace(DOWNLOADS, QStandardPaths::standardLocations(QStandardPaths::DownloadLocation));
    standart_locations.emplace(MUSIC, QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
    standart_locations.emplace(PICTURES, QStandardPaths::standardLocations(QStandardPaths::PicturesLocation));
    standart_locations.emplace(MOVIES, QStandardPaths::standardLocations(QStandardPaths::MoviesLocation));

    locations_png_.emplace(DESKTOP, ":/resources/desktop.png");
    locations_png_.emplace(DOCUMENTS, ":/resources/documents.png");
    locations_png_.emplace(DOWNLOADS, ":/resources/downloads.png");
    locations_png_.emplace(MUSIC, ":/resources/music.png");
    locations_png_.emplace(PICTURES, ":/resources/pictures.png");
    locations_png_.emplace(MOVIES, ":/resources/movies.png");

    // fill internal data structure
    for (auto& standart_dir : standart_locations)
    {
        if (!standart_dir.second.empty())
        {
            addItem(standart_dir.first, standart_dir.second.last());
        }
    }

    SectionBase::setup();

    QIcon icon(":/resources/quick_access.png");
    section_->setIcon(0, icon);

    updateUi();
}

void QuickAccessSection::addItem(QString name, QString path)
{
    quick_items_.emplace(std::move(name), std::move(path));
}

void QuickAccessSection::updateUi()
{
    SectionBase::clearSection_();

    for (const auto& standart_location : quick_items_)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem(section_);

        if (locations_png_.find(standart_location.first) != locations_png_.end())
        {
            child->setIcon(0, locations_png_[standart_location.first]);
        }
        else
        {
            // if item is not standart location, then just set folder icon
            QIcon icon(":/resources/folder.png");
            child->setIcon(0, icon);
        }

        child->setText(0, standart_location.first);

        section_->addChild(child);
    }
}

QString QuickAccessSection::getPathByName(QString name)
{
    return quick_items_[name];
}
