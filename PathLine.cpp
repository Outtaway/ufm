#include "PathLine.h"

#include <QListWidget>

PathLine::PathLine(QListWidget* path_line_list) :
	path_line_list_(path_line_list)
{
    path_line_list_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    path_line_list_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void PathLine::setPathChain(PathChain new_path_chain)
{
	path_chain_ = new_path_chain;
	updateUi();
}

void PathLine::pushBack(QModelIndex dir_index, QString dir_name)
{
	path_chain_.pushBack(dir_index, dir_name);
	updateUi();
}

void PathLine::popBack()
{
	path_chain_.popBack();
	updateUi();
}

int PathLine::selectedDirectoryPos()
{
	return path_line_list_->currentRow();
}

void PathLine::clearSelection()
{
	path_line_list_->clearSelection();
}

QModelIndex PathLine::directoryIndexAt(size_t pos)
{
	return path_chain_.directoryIndexAt(pos);
}

QString PathLine::directoryNameAt(size_t pos)
{
	return path_chain_.directoryNameAt(pos);
}

size_t PathLine::size()
{
	return path_chain_.size();
}

void PathLine::erase(size_t offset, size_t count)
{
	path_chain_.erase(offset, count);
	updateUi();
}

void PathLine::updateUi()
{
	path_line_list_->clear();
	for (size_t i = 0; i < path_chain_.size(); ++i)
	{
		path_line_list_->addItem(path_line_prefix + path_chain_.directoryNameAt(i));
	}
}
