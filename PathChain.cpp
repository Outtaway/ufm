#include "PathChain.h"

void PathChain::pushBack(QModelIndex dir_index, QString dir_name)
{
    index_path.push_back(dir_index);
    string_path.push_back(dir_name);
}

void PathChain::popBack()
{
    index_path.pop_back();
    string_path.pop_back();
}

void PathChain::pushFront(QModelIndex dir_index, QString dir_name)
{
    index_path.push_front(dir_index);
    string_path.push_front(dir_name);
}

void PathChain::popFront()
{
    index_path.pop_front();
    string_path.pop_front();
}

void PathChain::clear()
{
    index_path.clear();
    string_path.clear();
}

QString PathChain::directoryNameAt(size_t pos)
{
    return string_path.at(pos);
}

QModelIndex PathChain::directoryIndexAt(size_t pos)
{
    return index_path.at(pos);
}

size_t PathChain::size()
{
    return index_path.size();
}

void PathChain::erase(size_t offset, size_t count)
{
    index_path.erase(index_path.begin() + offset, index_path.begin() + offset + count);
    string_path.erase(string_path.begin() + offset, string_path.begin() + offset + count);
}
