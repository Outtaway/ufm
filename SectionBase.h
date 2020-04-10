#pragma once

#include <QString>

class QTreeWidgetItem;

class SectionBase
{
public:

    SectionBase(QTreeWidgetItem* section, QString tab_name);

    virtual ~SectionBase();

    QTreeWidgetItem* handle();

    virtual void addItem(QString name, QString path) = 0;

    virtual void setup();

    virtual void updateUi() = 0;

    virtual QString getPathByName(QString name) = 0;

protected:

    void clearSection_();

    QTreeWidgetItem* section_;
    QString          section_name_;
};
