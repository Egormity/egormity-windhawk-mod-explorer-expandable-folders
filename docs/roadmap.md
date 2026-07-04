# Roadmap

## Phase 0: Scaffold

Status: done.

- Create Windhawk source file.
- Target `explorer.exe`.
- Add x64 architecture metadata.
- Add visible `enabled` checkbox.
- Log init, settings changes, and uninit.
- Verify syntax and temporary DLL build.

## Phase 1: Explorer Host Probe

Goal: prove safe Explorer integration without changing file behavior.

- Detect supported Explorer windows.
- Detect tab/window lifetime.
- Find the file-list/content area.
- Create and resize a harmless child `HWND`.
- Hide/remove it cleanly on unload.
- Keep all behavior behind the `enabled` checkbox.
- Fall back silently when unsupported.

## Phase 2: Standalone Model

Goal: implement the logic outside Explorer internals.

- `TreeNode` model.
- `VisibleRow` model.
- Flattened visible-row projection.
- Expand/collapse operations.
- Per-sibling sorting.
- Identity-based selection.
- Parent/child selection normalization.
- Unit tests for model behavior once a test harness exists.

## Phase 3: Virtual Tree-Grid

Goal: render a Finder-like details view without creating thousands of controls.

- Owner-drawn rows.
- Columns.
- Disclosure arrows.
- Indentation.
- Scrolling.
- Hit testing.
- Keyboard navigation.
- Multi-select.
- Focus and hover states.

## Phase 4: Shell Bridge

Goal: operate on real Shell identities.

- Represent rows with `IShellItem` or PIDLs.
- Enumerate normal local filesystem folders.
- Open items through Shell.
- Show real Shell context menus.
- Delegate copy, move, rename, and delete to Shell APIs.
- Begin drag/drop through Shell data objects.

## Phase 5: Async Loading And Refresh

Goal: keep Explorer responsive.

- Worker-thread enumeration.
- Immediate-children-only expansion.
- Metadata and icon caching.
- Generation tokens to discard stale async results.
- Debounced refresh notifications.
- Selection and expansion preservation by identity.

## MVP Scope

Supported:

- normal local filesystem folders;
- Windows 11 Explorer;
- Details-style view;
- columns: Name, Date modified, Type, Size;
- expand/collapse folders;
- per-sibling sorting;
- basic multi-select;
- Shell-delegated open/context menu/file operations.

Fallback or disable:

- Home / Quick Access;
- Search results;
- Gallery;
- Recycle Bin;
- This PC root;
- Libraries;
- ZIP folders;
- MTP devices;
- network locations;
- special OneDrive/cloud behavior.

## Main Risks

- Explorer UI internals can change between Windows builds.
- Bad hooks can destabilize `explorer.exe`.
- Shell namespace locations are more complex than paths.
- Cloud placeholders can hydrate unexpectedly if queried incorrectly.
- Context menu and drag/drop behavior must preserve third-party Shell
  extensions.

## Next Recommended Task

Implement the Phase 1 host probe with no file operations:

1. Load only when `enabled` is true.
2. Detect Explorer windows.
3. Log supported/unsupported window classification.
4. Avoid modifying the native view until detection is reliable.
