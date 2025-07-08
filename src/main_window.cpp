#include <QtWidgets>

#include "main_window.hpp"

LabeledListView::LabeledListView(QWidget* parent)
        : QWidget(parent)
{
    label = new QLabel(this);
    list_view = new QListView(this);
    list_view->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Expanding));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(list_view);
}

void LabeledListView::setLabel(const char* text)
{
    label->setText(text);
}

MainWindow::MainWindow()
{
    // Add widgets here.
    QWidget* main = new QWidget(this);

    QWidget* mid_column = new QWidget(main);
    LabeledListView* panel_left = new LabeledListView(main);
    LabeledListView* panel_right = new LabeledListView(main);

    panel_left->setLabel("Game");
    panel_right->setLabel("Save store");

    main->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding));
    panel_left->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,
                                          QSizePolicy::Ignored));
    panel_right->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,
                                           QSizePolicy::Ignored));

    QHBoxLayout *layout = new QHBoxLayout(main);
    layout->addWidget(panel_left);
    layout->addWidget(mid_column);
    layout->addWidget(panel_right);

    setWindowTitle("Save Manager");
    setMinimumSize(640, 480);
    resize(1024, 768);
    setCentralWidget(main);
}
