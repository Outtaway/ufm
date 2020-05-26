#include "Content.h"
#include "Content.h"
#include "PathChain.h"

#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

Content::Content(QTreeView* ui_tree_view, QString initial_directory) :
    initial_directory_(initial_directory),
	content_tree_view_(ui_tree_view)
{
    setup_();
}

void Content::setup_()
{
    setupFilesystem_();

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

void Content::setupFilesystem_()
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

bool Content::currentIsRoot()
{
    return file_system_model_->rootPath() == file_system_model_->filePath(getCurrentDirectory());
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
    if (currentIsRoot())
        return false;
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
    if (currentIsRoot())
        return;
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
                deleteByIndex_(to_delete);
            }
        }
        else
        {
            deleteByIndex_(to_delete);
        }
    }
    clearSelection();
}

void Content::renameSelected()
{
    if (currentIsRoot())
        return;
    QModelIndex to_rename = getSelectedItem();
    if (to_rename.isValid())
    {
        content_tree_view_->edit(to_rename);
    }
}

void Content::newFile()
{
    // TODO: fix file is not created when user is on drive
    if (currentIsRoot())
        return;
    QString new_file_name = QInputDialog::getText(nullptr, "UFM", "Enter name of new file: ");
    QString current_directory_path = file_system_model_->filePath(getCurrentDirectory());
    QString new_file_path = QDir::cleanPath(current_directory_path + "/" + new_file_name);
    qDebug() << "new file path: " << new_file_path;
    QFileInfo info(new_file_path);
    if (info.exists())
    {
        QMessageBox messageBox;
        messageBox.critical(0, "UFM", "File with such name exists!");
        return;
    }
    QFile file(new_file_path);
    file.open(QFile::WriteOnly);
    file.write("");
}

void Content::newDirectory()
{
    if (currentIsRoot())
        return;
    QString new_folder_base_name = "new_folder";
    QString new_folder_name;
    QString current_directory_path = file_system_model_->filePath(getCurrentDirectory());
    for (size_t i = 1;;++i)
    {
        QFileInfo info(QDir::cleanPath(current_directory_path + "/" + new_folder_base_name + QString::number(i)));
        if (!info.exists())
        {
            new_folder_name = new_folder_base_name + QString::number(i);
            break;
        }
    }
    qDebug() << "Creation of new directory acquired, new directory name: " << new_folder_name;
    QModelIndex new_directory = file_system_model_->mkdir(getCurrentDirectory(), new_folder_name);
    content_tree_view_->edit(new_directory);
}

void Content::moveSelectedTo()
{
    if (currentIsRoot())
        return;
    QFileDialog file_dialog;
    file_dialog.setFileMode(QFileDialog::Directory);
    file_dialog.setOption(QFileDialog::ShowDirsOnly);
    QString new_location = file_dialog.getExistingDirectory(nullptr, "Select folder to move to", file_system_model_->filePath(getCurrentDirectory()));
    if (new_location == "")
    {
        qDebug() << "New location was not selected";
        return;
    }
    QString old_path = file_system_model_->filePath(getSelectedItem());
    QString new_path = QDir::cleanPath(new_location + "/" + getSelectedName());
    qDebug() << "old path of moved item: " << old_path;
    qDebug() << "new path of moved item: " << new_path;
    QDir dir;
    if (!dir.rename(old_path, new_path))
    {
        qDebug() << "couldn't move item";
        return;
    }
    clearSelection();
}

void Content::copySelectedTo()
{
    // TODO: make available copy of directory
    // TODO: make available copy in the same directory
    if (currentIsRoot())
        return;
    QFileDialog file_dialog;
    file_dialog.setFileMode(QFileDialog::Directory);
    file_dialog.setOption(QFileDialog::ShowDirsOnly);
    QString new_location = file_dialog.getExistingDirectory(nullptr, "Select folder to copy to", file_system_model_->filePath(getCurrentDirectory()));
    if (new_location == "")
    {
        qDebug() << "New location was not selected";
        return;
    }
    QString old_path = file_system_model_->filePath(getSelectedItem());
    QString new_path = QDir::cleanPath(new_location + "/" + getSelectedName());
    qDebug() << "old path of copied item: " << old_path;
    qDebug() << "new path of copied item: " << new_path;
    if (!QFile::copy(old_path, new_path))
    {
        qDebug() << "couldn't copy item";
        return;
    }
    clearSelection();
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

void Content::deleteByIndex_(QModelIndex to_delete)
{
    bool result = file_system_model_->remove(to_delete);
    qDebug() << "Deleting" << file_system_model_->fileName(to_delete) << "..." << (result ? "Success" : "Fail");
}
