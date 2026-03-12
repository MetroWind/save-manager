# Game Selection Feature

This change adds a "Games" menu to the application, allowing users to switch between different games defined in the [games.yaml](file:///d:/programs/save-manager/games.yaml) configuration file.

## User Review Required

> [!NOTE]
> The application will still default to the first game in [games.yaml](file:///d:/programs/save-manager/games.yaml) on startup. The "Games" menu will allow switching after the app has launched.

## Proposed Changes

### Models

#### [MODIFY] [models.hpp](file:///d:/programs/save-manager/src/models.hpp)
- Add [setGame(std::unique_ptr<GameInterface> game)](file:///d:/programs/save-manager/src/main_window.cpp#169-173) to [ActiveSaveModel](file:///d:/programs/save-manager/src/models.cpp#10-15).
- Add `setGameShortName(std::string_view short_name)` to [StoredSaveModel](file:///d:/programs/save-manager/src/models.cpp#47-52).

#### [MODIFY] [models.cpp](file:///d:/programs/save-manager/src/models.cpp)
- Implement [setGame](file:///d:/programs/save-manager/src/main_window.cpp#169-173) and `setGameShortName` using `beginResetModel()` and `endResetModel()` to notify views of the change.

### UI

#### [MODIFY] [main_window.hpp](file:///d:/programs/save-manager/src/main_window.hpp)
- Add `QMenu& gamesMenu()` to provide access to the games menu.
- Add `void setGameLabel(const char* name)` to update the label above the active saves list.

#### [MODIFY] [main_window.cpp](file:///d:/programs/save-manager/src/main_window.cpp)
- Create a `QMenuBar` and a "Games" `QMenu`.
- Implement `setGameLabel` to update `panel_left` label.

### Application Logic

#### [MODIFY] [app.hpp](file:///d:/programs/save-manager/src/app.hpp)
- Add `std::vector<GameDefinition> game_definitions` to store loaded games.
- Add `void onGameSelected()` slot.

#### [MODIFY] [app.cpp](file:///d:/programs/save-manager/src/app.cpp)
- In [run()](file:///d:/programs/save-manager/src/app.cpp#16-70), load all game definitions from YAML.
- Populate the "Games" menu with actions for each game.
- Implement `onGameSelected()` to switch the game in models and UI.

---

## Verification Plan

### Manual Verification
1.  **Prerequisites**: Ensure [games.yaml](file:///d:/programs/save-manager/games.yaml) has at least two games (e.g., Warriors Orochi 3 and Elden Ring).
2.  **Launch**: Run the application.
3.  **Default State**:
    - Verify the window title is "Warriors Orochi 3 Ultimate Save Manager".
    - Verify the left panel label is "Warriors Orochi 3 Ultimate".
4.  **Switch Game**:
    - Click the "Games" menu.
    - Select "Elden Ring".
    - Verify the window title updates to "Elden Ring Save Manager".
    - Verify the left panel label updates to "Elden Ring".
    - Verify the lists refresh to show Elden Ring related saves.
5.  **Switch Back**:
    - Select "Warriors Orochi 3 Ultimate" again and verify it switches back correctly.
