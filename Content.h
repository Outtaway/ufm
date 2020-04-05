#pragma once

#include <QFileSystemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>

#include <memory>

class PathChain;

class Content
{
public:
	Content(QTreeView* ui_tree_view, QString initial_directory = "");

	void setup();

	void tryExecute(const QModelIndex& index);

	void setCurrentDirectory(const QModelIndex& index);

	void setCurrentDirectoryString(const QString& dir);

	QModelIndex getCurrentDirectory();

	QString getCurrentDirectoryName();

	QString getCurrentDirectoryFullPath();

	QModelIndex getSelectedItem();

	void clearSelection();

	bool isDirectory(const QModelIndex& index);

	bool goOneDirectoryBack();

	bool goOneDirectoryForward();

	PathChain composeCurrentDirPathChain();

private:

	void setupFilesystem();

	QString								initial_directory_;
	QTreeView*							content_tree_view_;
	std::unique_ptr<QFileSystemModel>	file_system_model_;
};