#pragma once

#include <memory>

#include <QMenu>

class Content;

class ContentContextMenu : public QMenu
{
    Q_OBJECT

public:

    ContentContextMenu(QWidget* parent = nullptr, std::shared_ptr<Content> content = nullptr);

private slots:

    void deleteSelected();

private:

    std::shared_ptr<Content> content_;
};

