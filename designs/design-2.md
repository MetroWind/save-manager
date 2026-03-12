# Design: Glob-Based Save Directory Discovery

## Goal
Support games whose save directory path varies (e.g., depends on a user-specific ID or version string) by allowing a glob pattern in `games.yaml`. The system should use the first directory matching the glob as the active save directory.

## Current state
- `games.yaml` defines `save-dir` as a static string.
- `DirFinderInterface`, `StaticDirFinder`, and `FirstDirFromGlob` are already defined but not yet integrated into the `GameInterface` hierarchy.
- `GameWithSingleSave` currently stores a `std::filesystem::path` directly.

## Proposed Design

### 1. Configuration Changes (`games.yaml`)
Add a new field `save-dir-type` to the game definition.
- `static` (default): Use `save-dir` as-is (with environment variable expansion).
- `first-from-glob`: Use `save-dir` as a glob pattern and pick the first match.

Example:
```yaml
elden-ring:
  type: single-save
  name: Elden Ring
  save-dir: "%USERPROFILE%/AppData/Roaming/EldenRing/*"
  save-dir-type: first-from-glob
  save-files:
    - "ER0000.sl2"
```

### 2. Data Structure Changes (`game.hpp`)
- Update `GameDefinition` to include `SaveDirType` enum and field.
- Refactor `GameWithSingleSave` to use dependency injection for directory discovery.

```cpp
class GameWithSingleSave : public GameInterface
{
public:
    GameWithSingleSave(std::string_view name, std::string_view short_name,
                       std::unique_ptr<DirFinderInterface> dir_finder,
                       std::span<const std::string> files);
    // ...
    std::filesystem::path getSaveDir() const override { return dir_finder->getDir(); }

private:
    // ...
    std::unique_ptr<DirFinderInterface> dir_finder;
};
```

### 3. Factory Logic (`GameInterface::createFromDefinition`)
The factory method will now handle the instantiation of the appropriate `DirFinderInterface` implementation based on the `GameDefinition`.

- If `save_dir_type` is `STATIC`, create `StaticDirFinder` with `def.save_dir`.
- If `save_dir_type` is `FIRST_FROM_GLOB`, create `FirstDirFromGlob` with `def.save_dir`.

### 4. Implementation Details
- `FirstDirFromGlob::getDir()` should ensure the pattern is expanded (via `expandPath`) before passing it to the `Glob` iterator.
- `GameWithSingleSave::saves()` will call `getSaveDir()` once per call (or once on construction if we want to cache it, but let's stick to dynamic for now as directories might be created/moved).

## Benefits
- Decouples `GameInterface` from the specifics of how a directory is located.
- Highly extensible: new discovery strategies can be added by implementing `DirFinderInterface`.
