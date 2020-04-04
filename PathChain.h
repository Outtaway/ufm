#pragma once

#include <QModelIndex>
#include <QString>
#include <deque>

class PathChain
{
public:

    PathChain() = default;

    void pushBack(QModelIndex dir_index, QString dir_name);

    void popBack();

    void pushFront(QModelIndex dir_index, QString dir_name);

    void popFront();

    void clear();

    QString directoryNameAt(size_t pos);

    QModelIndex directoryIndexAt(size_t pos);

    size_t size();

    void erase(size_t offset, size_t count);

private:

    std::deque<QModelIndex> index_path;
    std::deque<QString>     string_path;

};
