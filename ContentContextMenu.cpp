#include "ContentContextMenu.h"
#include "Content.h"

#include <QDebug>
#include <QAction>

ContentContextMenu::ContentContextMenu(QWidget* parent, std::shared_ptr<Content> content) :
    QMenu(parent),
    content_(content)
{
    QAction* delete_action = new QAction("Delete");
    QAction* rename_action = new QAction("Rename");

    QMenu::addAction(delete_action);
    QMenu::addAction(rename_action);

    QObject::connect(delete_action, &QAction::triggered, this, &ContentContextMenu::deleteSelected);
    QObject::connect(rename_action, &QAction::triggered, this, &ContentContextMenu::renameSelected);
}

void ContentContextMenu::deleteSelected()
{
    content_->deleteSelected();
}

void ContentContextMenu::renameSelected()
{
    content_->renameSelected();
}
