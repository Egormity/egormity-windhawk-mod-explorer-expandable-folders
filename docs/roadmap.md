# Roadmap

## Phase 0: Injectable Scaffold

Status: done.

- Create Windhawk source file.
- Target `explorer.exe`.
- Add x64 architecture metadata.
- Add initial Windhawk settings checkbox.
- Log init, settings changes, and uninit.
- Verify syntax and temporary DLL build.

## Phase 1: Explorer-Hosted Control

Status: initial version done.

- Detect visible `CabinetWClass` Explorer windows.
- Add an `Expandable folders` checkbox to each Explorer window.
- Store checkbox state in Windhawk local storage.
- Hide the native file-list view while the scaffold is enabled.
- Restore the native file-list view when unchecked.
- Remove controls cleanly on unload.

Remaining hardening:

- Replace fixed pane offsets with reliable native view detection.
- Handle tab/layout changes more precisely.
- Improve light/dark theme matching.

## Phase 2: Explorer Host Probe Hardening

Goal: make the host layer reliable before real tree logic.

- Detect supported Explorer windows and tabs.
- Detect tab/window lifetime.
- Find the file-list/content area from real child windows or Shell interfaces.
- Keep behavior behind the Explorer-hosted checkbox.
- Fall back silently when unsupported.

## Phase 3: Standalone Model

Goal: implement the logic outside Explorer internals.

- `TreeNode` model.
- `VisibleRow` model.
- Flattened visible-row projection.
- Expand/collapse operations.
- Per-sibling sorting.
- Identity-based selection.
- Parent/child selection normalization.
- Unit tests for model behavior once a test harness exists.

## Phase 4: Virtual Tree-Grid

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

## Phase 5: Shell Bridge

Goal: operate on real Shell identities.

- Represent rows with `IShellItem` or PIDLs.
- Enumerate normal local filesystem folders.
- Open items through Shell.
- Show real Shell context menus.
- Delegate copy, move, rename, and delete to Shell APIs.
- Begin drag/drop through Shell data objects.

## Phase 6: Async Loading And Refresh

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
