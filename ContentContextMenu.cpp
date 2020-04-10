#include "ContentContextMenu.h"
#include "Content.h"

#include <QDebug>
#include <QAction>

ContentContextMenu::ContentContextMenu(QWidget* parent, std::shared_ptr<Content> content) :
    QMenu(parent),
    content_(content)
{
    QAction* delete_action = new QAction("Delete");
    QMenu::addAction(delete_action);
    QObject::connect(delete_action, &QAction::triggered, this, &ContentContextMenu::deleteSelected);
}

void ContentContextMenu::deleteSelected()
{
    content_->deleteSelected();
}
