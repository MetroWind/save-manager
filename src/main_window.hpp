#pragma once

#include <QtWidgets>

class LabeledListView : public QWidget
{
    Q_OBJECT
public:
    LabeledListView(QWidget* parent);

    void setLabel(const char* text);
    const QListView& listView() const { return *list_view; }
    QListView& listView() { return *list_view; }

private:
    QListView* list_view;
    QLabel* label;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
};
