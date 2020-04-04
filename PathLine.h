#pragma once

#include "PathChain.h"

#include <QListWidget>
#include <QModelIndex>
#include <QString>


class PathLine
{
public:

	PathLine(QListWidget* path_line_list);

	void setPathChain(PathChain new_path_chain);

	void pushBack(QModelIndex dir_index, QString dir_name);

	void popBack();

	int selectedDirectoryPos();

	void clearSelection();

	QModelIndex directoryIndexAt(size_t pos);

	QString directoryNameAt(size_t pos);

	size_t size();

	void erase(size_t offset, size_t count);

private:

	void updateUi();

	const QString	path_line_prefix = " 🠚 ";

	QListWidget*	path_line_list_;
	PathChain		path_chain_;

};