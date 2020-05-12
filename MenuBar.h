#pragma once

#include <QMenuBar>
#include <QAction>

class MenuBar : public QMenuBar
{
    Q_OBJECT

public:

    MenuBar(QWidget* parent = nullptr);

private slots:

    void openAboutWindow();

};
