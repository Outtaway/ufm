#include "MenuBar.h"

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent)
{
    QAction* option_action = new QAction("Options", this);
    this->addAction(option_action);
}
