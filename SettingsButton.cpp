#include "SettingsButton.h"

#include <QIcon>

SettingsButton::SettingsButton(QWidget* parent) :
    QPushButton(parent)
{
    QIcon settings_icon(":/resources/settings.png");

    this->setIcon(settings_icon);
    this->setIconSize(QSize(20, 20));
}
