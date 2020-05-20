#pragma once

#include "Settings.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>

#include <memory>

class PathChain;

class Content : public QObject
{
    Q_OBJECT
public:

	Content(QTreeView* ui_tree_view, QString initial_directory = "");

private:

	void setup();

	void setupFilesystem();

public:

    enum COLUMNS
    {
        NAME = 0,
        SIZE,
        TYPE,
        LAST_MODIFIED
    };

	void tryExecute(const QModelIndex& index);

	void setCurrentDirectory(const QModelIndex& index);

	void setCurrentDirectoryString(const QString& dir);

	QModelIndex getCurrentDirectory();

    QModelIndex getIndexAtPoint(const QPoint& point);

	QString getCurrentDirectoryName();

	QString getCurrentDirectoryFullPath();

	QModelIndex getSelectedItem();

    bool currentIsRoot();

	void clearSelection();

	bool isDirectory(const QModelIndex& index);

	bool goOneDirectoryBack();

	bool goOneDirectoryForward();

	PathChain composeCurrentDirPathChain();

    QString getSelectedName();

    QString getSelectedPath();

public slots:
    void deleteSelected();
    void renameSelected();
    void newFile();
    void newDirectory();

private slots:

    void settingsChanged(const Settings::OPTIONS option, const QVariant value);

private:

    void deleteByIndex(QModelIndex to_delete);

	QString								initial_directory_;
	QTreeView*							content_tree_view_;
	std::unique_ptr<QFileSystemModel>	file_system_model_;
};
