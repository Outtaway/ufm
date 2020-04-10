#include "SectionBase.h"

#include <QTreeWidgetItem>

SectionBase::SectionBase(QTreeWidgetItem* section, QString section_name) :
    section_(section),
    section_name_(section_name)
{ }

SectionBase::~SectionBase()
{ }

QTreeWidgetItem* SectionBase::handle()
{
    return section_;
}

void SectionBase::setup()
{
    section_->setText(0, section_name_);
}

void SectionBase::clearSection_()
{
    auto children = section_->takeChildren();

    for (auto& child : children)
    {
        delete child;
    }
}
