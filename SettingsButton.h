#pragma once

#include <QPushButton>

class SettingsButton : public QPushButton
{
    Q_OBJECT

public:

    SettingsButton(QWidget* parent = nullptr);

private slots:

    void settingsInvoked();

};
