#include "MenuBar.h"

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent)
{
    QAction* about_action = new QAction(this);
    about_action->setText("About");
    this->addAction(about_action);
}
