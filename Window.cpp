#include "Window.h"
#include "ui_Window.h"

#include <chrono>
#include <iostream>

#include <QSettings>
#include <QDebug>
#include <QMenuBar>
#include <QStyleFactory>

using namespace std::literals;

Window::Window(QWidget* parent) :
    QWidget(parent)
{
    ui = new Ui::Window;
    ui->setupUi(this);

    content_ = std::make_shared<Content>(ui->dir_content);

    content_context_menu_ = new ContentContextMenu(ui->dir_content, content_);

    path_line_ = std::make_unique<PathLine>(ui->path_line);

    quick_panel_ = std::make_unique<QuickPanel>(ui->quick_panel);

    settings_button_ = new SettingsButton(this);

    menu_bar_ = new MenuBar(this);

    layout_= new QHBoxLayout(this);
    layout_->addWidget(menu_bar_);
    layout_->addWidget(settings_button_);
    static_cast<QVBoxLayout*>(this->layout())->insertLayout(0, layout_);

    QObject::connect(QApplication::instance(), &QApplication::aboutToQuit, this, &Window::onExit);

    QObject::connect(ui->dir_content->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &Window::dir_selection_changed);

    QObject::connect(ui->dir_content, &QTreeView::customContextMenuRequested, this, &Window::on_dir_content_custom_menu);

    QObject::connect(ui->quick_panel, &QTreeWidget::currentItemChanged,
        this, &Window::quick_panel_selection_changed);
}

Window::~Window()
{ }

void Window::dir_selection_changed(const QItemSelection& selected, const QItemSelection&)
{
    if (!selected.indexes().empty())
    {
        if (content_->isDirectory(selected.indexes().last()))
        {
            ui->pin_button->setEnabled(true);
            ui->forward->setEnabled(true);
        }
        else
        {
            ui->pin_button->setEnabled(false);
            ui->forward->setEnabled(false);
        }
        ui->rename_button->setEnabled(true);
        ui->delete_button->setEnabled(true);
        ui->move_button->setEnabled(true);
        ui->copy_button->setEnabled(true);
    }
    else
    {
        ui->rename_button->setEnabled(false);
        ui->delete_button->setEnabled(false);
        ui->pin_button->setEnabled(false);
        ui->move_button->setEnabled(false);
        ui->copy_button->setEnabled(false);
        ui->forward->setEnabled(false);
    }
}

void Window::quick_panel_selection_changed(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (quick_panel_->isSection(current->parent(), QuickPanel::QUICK_ACCESS_SECTION))
    {
        ui->unpin_button->setEnabled(true);
    }
    else
    {
        ui->unpin_button->setEnabled(false);
    }
}

void Window::on_dir_content_doubleClicked(const QModelIndex& index)
{
    if (content_->isDirectory(index))
    {
        content_->setCurrentDirectory(index);

        path_line_->pushBack(content_->getCurrentDirectory(), content_->getCurrentDirectoryName());

        quick_panel_->addItemToSection(QuickPanel::RECENT_SECTION, content_->getCurrentDirectoryName(), content_->getCurrentDirectoryFullPath());
    }
    else
        content_->tryExecute(index);

    content_->clearSelection();
}

void Window::on_dir_content_custom_menu(const QPoint& point)
{
    QModelIndex index = content_->getIndexAtPoint(point);
    if (index.isValid())
        content_context_menu_->exec(QCursor::pos());
}

void Window::on_back_clicked()
{
    // TODO: fix bad behavior when clicked back on root folder("")
    if (content_->goOneDirectoryBack())
        path_line_->popBack();

    content_->clearSelection();
}

void Window::on_forward_clicked()
{
    if (content_->goOneDirectoryForward())
        path_line_->pushBack(content_->getCurrentDirectory(), content_->getCurrentDirectoryName());

    content_->clearSelection();
}

void Window::on_path_line_itemClicked(QListWidgetItem*)
{
    auto selected_directory_pos = path_line_->selectedDirectoryPos();

    QModelIndex new_directory = path_line_->directoryIndexAt(selected_directory_pos);
    content_->setCurrentDirectory(new_directory);

    path_line_->erase(selected_directory_pos + 1u, path_line_->size() - (selected_directory_pos + 1u));

    path_line_->clearSelection();
    content_->clearSelection();
}

void Window::on_quick_panel_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (!item->parent())
    {
        return;
    }

    QString path;

    QString section = item->parent()->text(0);

    if (quick_panel_->sectionExists(section))
    {
        path = quick_panel_->getPathByName(section, item->text(column));
        if (section == QuickPanel::RECENT_SECTION)
        {
            quick_panel_->addItemToSection(QuickPanel::RECENT_SECTION, item->text(column), path);
        }
    }
    else
        return;

    content_->setCurrentDirectoryString(path);

    path_line_->setPathChain(content_->composeCurrentDirPathChain());
}

void Window::onExit()
{
    // recent section is destroyed so recents are exported to database
    quick_panel_.reset();
}

void Window::on_pin_button_clicked()
{
    QString name = content_->getSelectedName();
    QString path = content_->getSelectedPath();
    quick_panel_->addItemToSection(QuickPanel::QUICK_ACCESS_SECTION, name, path);

    content_->clearSelection();
}

void Window::on_unpin_button_clicked()
{
    QString selected_item_name = quick_panel_->getSelected();

    quick_panel_->deleteItemFromSection(QuickPanel::QUICK_ACCESS_SECTION, selected_item_name);
    quick_panel_->clearSelection();
}
