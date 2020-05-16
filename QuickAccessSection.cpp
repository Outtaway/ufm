#include "QuickAccessSection.h"

#include <QTreeWidget>

QuickAccessSection::QuickAccessSection(QTreeWidgetItem* section, QString name) : SectionBase(section, name)
{
    setup();
}

void QuickAccessSection::setup()
{
    std::map<QString, QStringList> standart_dirs;

    standart_dirs.emplace(DESKTOP, QStandardPaths::standardLocations(QStandardPaths::DesktopLocation));
    standart_dirs.emplace(DOCUMENTS, QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
    standart_dirs.emplace(DOWNLOADS, QStandardPaths::standardLocations(QStandardPaths::DownloadLocation));
    standart_dirs.emplace(MUSIC, QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
    standart_dirs.emplace(PICTURES, QStandardPaths::standardLocations(QStandardPaths::PicturesLocation));
    standart_dirs.emplace(MOVIES, QStandardPaths::standardLocations(QStandardPaths::MoviesLocation));

    locations_png_.emplace(DESKTOP, ":/resources/desktop.png");
    locations_png_.emplace(DOCUMENTS, ":/resources/documents.png");
    locations_png_.emplace(DOWNLOADS, ":/resources/downloads.png");
    locations_png_.emplace(MUSIC, ":/resources/music.png");
    locations_png_.emplace(PICTURES, ":/resources/pictures.png");
    locations_png_.emplace(MOVIES, ":/resources/movies.png");

    // fill internal data structure
    for (auto& standart_dir : standart_dirs)
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
    standart_locations_.emplace(std::move(name), std::move(path));
}

void QuickAccessSection::updateUi()
{
    SectionBase::clearSection_();

    for (const auto& standart_location : standart_locations_)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem(section_);
        child->setIcon(0, locations_png_[standart_location.first]);
        child->setText(0, standart_location.first);

        section_->addChild(child);
    }
}

QString QuickAccessSection::getPathByName(QString name)
{
    return standart_locations_[name];
}
