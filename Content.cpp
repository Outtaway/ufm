#include "Content.h"
#include "PathChain.h"

Content::Content(QTreeView* ui_tree_view, QString initial_directory) :
    initial_directory_(initial_directory),
	content_tree_view_(ui_tree_view)
{ }

void Content::setup()
{
    setupFilesystem();

    // set up initial directory
    content_tree_view_->setRootIndex(file_system_model_->index(initial_directory_));

    // user cannot expand directory to see its content
    // user should double click on directory to see its content
    content_tree_view_->setItemsExpandable(false);

    // hide size, type, date modified,
    content_tree_view_->hideColumn(1);
    content_tree_view_->hideColumn(2);
    content_tree_view_->hideColumn(3);

    content_tree_view_->header()->setStyleSheet(QString::fromUtf8("border: 1px solid #FFFFFF;"
                                                                  "border-bottom: 1px solid #D3D3D3"));
}

void Content::tryExecute(const QModelIndex& index)
{
    QString file_path = file_system_model_->filePath(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(file_path));
}

void Content::setCurrentDirectory(const QModelIndex& index)
{
    content_tree_view_->setRootIndex(index);
}

void Content::setCurrentDirectoryString(const QString& dir)
{
    content_tree_view_->setRootIndex(file_system_model_->index(dir));
}

QModelIndex Content::getCurrentDirectory()
{
    return content_tree_view_->rootIndex();
}

QString Content::getCurrentDirectoryName()
{
    return file_system_model_->fileName(getCurrentDirectory());
}

QString Content::getCurrentDirectoryFullPath()
{
    return file_system_model_->filePath(getCurrentDirectory());
}

QModelIndex Content::getSelectedItem()
{
    return content_tree_view_->currentIndex();
}

void Content::clearSelection()
{
    content_tree_view_->setCurrentIndex(QModelIndex());
}

bool Content::isDirectory(const QModelIndex& index)
{
    return file_system_model_->isDir(index);
}

bool Content::goOneDirectoryBack()
{
    QModelIndex parent_directory = getCurrentDirectory().parent();

    setCurrentDirectory(parent_directory);
    return true;
}

bool Content::goOneDirectoryForward()
{
    QModelIndex selected_item = getSelectedItem();

    if (selected_item.isValid() && isDirectory(selected_item))
    {
        setCurrentDirectory(selected_item);
        return true;
    }

    return false;
}

PathChain Content::composeCurrentDirPathChain()
{
    QModelIndex directory = getCurrentDirectory();
    PathChain path_chain;

    while (directory.isValid())
    {
        path_chain.pushFront(directory, file_system_model_->fileName(directory));
        directory = directory.parent();
    }

    return path_chain;
}

void Content::setupFilesystem()
{
    file_system_model_ = std::make_unique<QFileSystemModel>();
    file_system_model_->setRootPath(initial_directory_);
    content_tree_view_->setModel(file_system_model_.get());
}
