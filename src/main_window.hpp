#pragma once

#include <optional>

#include <QtCore>
#include <QtWidgets>

#include "game.hpp"

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

// A toolbar that puts the tool buttons in the center
class CenteredToolbar : public QToolBar
{
    Q_OBJECT
public:
    CenteredToolbar(QWidget* parent=nullptr);
    void doneAddingActions();
};

// The delegate to edit the title of a stored save
class StoredSaveDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    StoredSaveDelegate(QObject* parent=nullptr);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index)
        const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void updateEditorGeometry(
        QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static constexpr std::string_view ACTION_STORE = "store";
    static constexpr std::string_view ACTION_RESTORE = "restore";
    static constexpr std::string_view ACTION_DELETE_STORED = "delete-stored";

    MainWindow(QAbstractItemModel& active_save_model,
               QAbstractItemModel& stored_save_model);

    // Returns the index of the selected active save.
    std::optional<int> selectedActiveSave() const;
    std::optional<int> selectedStoredSave() const;
    const LabeledListView& panelLeft() const { return *panel_left; }
    const LabeledListView& panelRight() const { return *panel_right; }
    const CenteredToolbar& midToolbar() const { return *mid_column; }
    const CenteredToolbar& rightToolbar() const { return *right_bar; }
    QMenu& gamesMenu() { return *menu_games; }
    void setGameName(const char* name);
    void setGameLabel(const char* name);

private:
    LabeledListView* panel_left;
    LabeledListView* panel_right;
    CenteredToolbar* mid_column;
    CenteredToolbar* right_bar;
    QMenu* menu_games;
};
