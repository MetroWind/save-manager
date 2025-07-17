#include <optional>
#include <utility>

#include <QtWidgets>

#include "main_window.hpp"
#include "game.hpp"
#include "utils.hpp"

LabeledListView::LabeledListView(QWidget* parent)
        : QWidget(parent)
{
    label = new QLabel(this);
    list_view = new QListView(this);
    list_view->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Expanding));
    list_view->setUniformItemSizes(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(list_view);
}

void LabeledListView::setLabel(const char* text)
{
    label->setText(text);
}

CenteredToolbar::CenteredToolbar(QWidget* parent)
        : QToolBar(parent)
{
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding));
    addWidget(spacer);
}

void CenteredToolbar::doneAddingActions()
{
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding));
    addWidget(spacer);
}

StoredSaveDelegate::StoredSaveDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* StoredSaveDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setFrame(false);
    return editor;
}

void StoredSaveDelegate::setEditorData(
    QWidget* editor, const QModelIndex &index) const
{
    QString value = index.data(Qt::EditRole).toString();
    QLineEdit *spinBox = static_cast<QLineEdit*>(editor);
    spinBox->setText(value);
}

void StoredSaveDelegate::setModelData(
    QWidget *editor, QAbstractItemModel* model, const QModelIndex &index) const
{
    QLineEdit* spinBox = static_cast<QLineEdit*>(editor);
    QString value = spinBox->text();
    model->setData(index, value, Qt::EditRole);
}

void StoredSaveDelegate::updateEditorGeometry(
    QWidget* editor, const QStyleOptionViewItem& option,
    const QModelIndex&) const
{
    editor->setGeometry(option.rect);
}

MainWindow::MainWindow(QAbstractItemModel& active_save_model,
                       QAbstractItemModel& stored_save_model)
{
    // Add widgets here.
    QWidget* main = new QWidget(this);

    // Center toolbar
    mid_column = new CenteredToolbar(main);
    mid_column->setOrientation(Qt::Vertical);
    mid_column->setMovable(false);

    QAction* act_store = new QAction(
        QIcon::fromTheme(QIcon::ThemeIcon::GoNext), "Backup save", mid_column);
    act_store->setObjectName(ACTION_STORE);
    QAction* act_restore = new QAction(
        QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious), "Restore save",
        mid_column);
    act_restore->setObjectName(ACTION_RESTORE);

    mid_column->addAction(act_store);
    mid_column->addAction(act_restore);

    mid_column->doneAddingActions();

    // The panels
    panel_left = new LabeledListView(main);
    panel_right = new LabeledListView(main);

    panel_left->setLabel("Game");
    panel_right->setLabel("Save store");

    main->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding));
    panel_left->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,
                                          QSizePolicy::Ignored));
    panel_right->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,
                                           QSizePolicy::Ignored));

    panel_left->listView().setModel(&active_save_model);
    panel_right->listView().setModel(&stored_save_model);

    StoredSaveDelegate* delegate =
        new StoredSaveDelegate(&panel_right->listView());
    panel_right->listView().setItemDelegate(delegate);

    // Right toolbar
    right_bar = new CenteredToolbar(main);
    right_bar->setOrientation(Qt::Vertical);
    QAction* delete_stored = new QAction(
        QIcon::fromTheme(QIcon::ThemeIcon::ListRemove), "Delete backup",
        right_bar);
    delete_stored->setObjectName(ACTION_DELETE_STORED);
    right_bar->addAction(delete_stored);

    right_bar->doneAddingActions();

    QHBoxLayout *layout = new QHBoxLayout(main);
    layout->addWidget(panel_left);
    layout->addWidget(mid_column);
    layout->addWidget(panel_right);
    layout->addWidget(right_bar);

    setMinimumSize(640, 480);
    resize(1024, 768);
    setCentralWidget(main);
}

std::optional<int> MainWindow::selectedActiveSave() const
{
    auto* selection = panel_left->listView().selectionModel();
    if(!selection->hasSelection())
    {
        return std::nullopt;
    }
    return selection->selectedIndexes()[0].row();
}

std::optional<int> MainWindow::selectedStoredSave() const
{
    auto* selection = panel_right->listView().selectionModel();
    if(!selection->hasSelection())
    {
        return std::nullopt;
    }
    return selection->selectedIndexes()[0].row();
}

void MainWindow::setGameName(const char* name)
{
    setWindowTitle(QString(name) + " Save Manager");
}
