#include "Content.h"
#include "Content.h"
#include "PathChain.h"

#include <QDebug>
#include <QMessageBox>

Content::Content(QTreeView* ui_tree_view, QString initial_directory) :
    initial_directory_(initial_directory),
	content_tree_view_(ui_tree_view)
{
    setup();
}

void Content::setup()
{
    setupFilesystem();

    // set up initial directory
    content_tree_view_->setRootIndex(file_system_model_->index(initial_directory_));

    // user cannot expand directory to see its content
    // user should double click on directory to see its content
    content_tree_view_->setItemsExpandable(false);

    if (!Settings::getInstance().optionExist(Settings::OPTIONS::SHOW_SIZE))
    {
        Settings::getInstance().setOption(Settings::OPTIONS::SHOW_SIZE, false);
    }
    content_tree_view_->setColumnHidden(COLUMNS::SIZE, !Settings::getInstance().getOption(Settings::OPTIONS::SHOW_SIZE).toBool());

    if (!Settings::getInstance().optionExist(Settings::OPTIONS::SHOW_TYPE))
    {
        Settings::getInstance().setOption(Settings::OPTIONS::SHOW_TYPE, false);
    }
    content_tree_view_->setColumnHidden(COLUMNS::TYPE, !Settings::getInstance().getOption(Settings::OPTIONS::SHOW_TYPE).toBool());

    if (!Settings::getInstance().optionExist(Settings::OPTIONS::SHOW_LAST_MODIFIED))
    {
        Settings::getInstance().setOption(Settings::OPTIONS::SHOW_LAST_MODIFIED, false);
    }
    content_tree_view_->setColumnHidden(COLUMNS::LAST_MODIFIED, !Settings::getInstance().getOption(Settings::OPTIONS::SHOW_LAST_MODIFIED).toBool());

    content_tree_view_->header()->setStyleSheet(QString::fromUtf8("border: 1px solid #FFFFFF;"
                                                                  "border-bottom: 1px solid #D3D3D3"));

    content_tree_view_->setContextMenuPolicy(Qt::CustomContextMenu);

    // exclude double click edit trigger
    content_tree_view_->setEditTriggers(content_tree_view_->editTriggers() & ~QTreeView::EditTrigger::DoubleClicked);

    if (!Settings::getInstance().optionExist(Settings::OPTIONS::DELETE_APPROVAL))
    {
        Settings::getInstance().setOption(Settings::OPTIONS::DELETE_APPROVAL, true);
    }

    QObject::connect(&Settings::getInstance(), &Settings::optionChanged, this, &Content::settingsChanged);
    qDebug() << content_tree_view_->selectionMode();
}

void Content::setupFilesystem()
{
    file_system_model_ = std::make_unique<QFileSystemModel>();
    file_system_model_->setRootPath(initial_directory_);
    file_system_model_->setReadOnly(false);

    content_tree_view_->setModel(file_system_model_.get());
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

QModelIndex Content::getIndexAtPoint(const QPoint& point)
{
    return content_tree_view_->indexAt(point);
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

void Content::deleteSelected()
{
    QModelIndex to_delete = getSelectedItem();
    if (to_delete.isValid())
    {
        bool approval_needed = Settings::getInstance().getOption(Settings::OPTIONS::DELETE_APPROVAL).toBool();

        if (approval_needed)
        {
            QMessageBox approve_box;
            approve_box.setWindowTitle("Delete selected?");
            approve_box.setText("Are you sure you want to delete: " + file_system_model_->fileName(to_delete));
            approve_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            approve_box.setDefaultButton(QMessageBox::Cancel);
            if (approve_box.exec() == QMessageBox::Ok)
            {
                deleteByIndex(to_delete);
            }
        }
        else
        {
            deleteByIndex(to_delete);
        }
    }
}

void Content::renameSelected()
{
    QModelIndex to_rename = getSelectedItem();
    if (to_rename.isValid())
    {
        content_tree_view_->edit(to_rename);
    }
}

QString Content::getSelectedName()
{
    QModelIndex selected = getSelectedItem();
    if (!selected.isValid())
        return {};
    return file_system_model_->fileName(getSelectedItem());
}

QString Content::getSelectedPath()
{
    QModelIndex selected = getSelectedItem();
    if (!selected.isValid())
        return {};
    return file_system_model_->filePath(getSelectedItem());
}

void Content::settingsChanged(const Settings::OPTIONS option, const QVariant value)
{
    switch(option)
    {
    case Settings::OPTIONS::SHOW_SIZE:
        content_tree_view_->setColumnHidden(COLUMNS::SIZE, !value.toBool());
        break;
    case Settings::OPTIONS::SHOW_TYPE:
        content_tree_view_->setColumnHidden(COLUMNS::TYPE, !value.toBool());
        break;
    case Settings::OPTIONS::SHOW_LAST_MODIFIED:
        content_tree_view_->setColumnHidden(COLUMNS::LAST_MODIFIED, !value.toBool());
        break;
    default:
        break;
    }
}

void Content::deleteByIndex(QModelIndex to_delete)
{
    bool result = file_system_model_->remove(to_delete);
    qDebug() << "Deleting" << file_system_model_->fileName(to_delete) << "..." << (result ? "Success" : "Fail");
}
