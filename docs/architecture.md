# Architecture

## Decision

The intended architecture is a custom virtualized tree-grid hosted inside
Explorer, while delegating real file identity and file operations to Windows
Shell APIs.

The custom view should own presentation. Windows Shell should own filesystem and
namespace meaning.

## Current Implementation

The repository currently contains the first Explorer-hosted UI scaffold:

- Windhawk metadata;
- `explorer.exe` target;
- x64 architecture target;
- bottom-right Explorer-owned popup checkbox named `Expandable folders`;
- blank overlay child window over the file-list pane when enabled;
- local Windhawk value storage for the checkbox state;
- init/uninit lifecycle logging.

No real Explorer file-tree logic exists yet.

## Source Organization

The repository is organized as modular C++ source plus a generated Windhawk
single-file artifact.

Current modules:

- `windhawk_metadata.h` - Windhawk metadata block.
- `common.h` - common includes, constants, and shared structs.
- `state.*` - process-wide state.
- `window_utils.*` - DPI scaling and Explorer frame detection helpers.
- `empty_view.*` - blank overlay window class and paint procedure.
- `explorer_window.*` - popup checkbox, overlay, layout, and synchronization logic.
- `manager.*` - background manager loop.
- `entrypoint.cpp` - `Wh_ModInit` and `Wh_ModUninit`.
- `mod.wh.cpp` - unity entry that includes all modules for bundling.

`tools/bundle-windhawk.ps1` expands local includes into
`explorer-expandable-folders.wh.cpp`.

## Rejected Approach

Do not try to patch Explorer's native file list by injecting fake child rows.

Explorer's native folder view expects direct children of the current Shell
folder. Expanded grandchildren would not naturally belong to Explorer's native
selection model, context menu model, drag/drop model, command routing, or status
bar behavior.

## Intended Components

Future implementation should be split into focused modules:

- Explorer hook layer;
- view host manager;
- custom tree-grid HWND;
- row and tree model;
- visible-row projection;
- Shell item adapter;
- enumeration service;
- metadata and column provider;
- selection model;
- operation adapter;
- context menu adapter;
- drag/drop adapter;
- refresh and watcher layer;
- state store;
- fallback and safety manager.

## Core Invariants

1. UI rows are not files. Rows map to Shell-backed item identities.
2. Visible rows are a flattened projection of a tree.
3. Expansion loads immediate children only.
4. Sorting is per sibling group, never global across parent and child rows.
5. Selection is visual first, but operations use normalized item identities.
6. If a selected parent contains selected children, container operations should
   use the parent and skip duplicate descendants.
7. Destructive operations should be delegated to Shell APIs.
8. Async results must be cancelable or stale-safe.
9. Cloud placeholders must not hydrate accidentally.
10. Unsupported Shell locations must fall back to native Explorer behavior.

## Shell Delegation

Use Shell identity and operation APIs instead of raw paths wherever possible:

- `IShellItem` and PIDLs for identity;
- `IFileOperation` for copy, move, rename, and delete;
- `IContextMenu` for context menus;
- `IDataObject` and Shell drop targets for drag/drop;
- Windows Property System for columns and metadata.

Paths can be used as an optimization for ordinary filesystem folders, but not
as the source of truth.

## Safety Requirements

Because the mod runs inside `explorer.exe`, failures are user-visible.

Required safety features for future work:

- global kill switch through the existing Explorer-hosted checkbox;
- strict fallback to native Explorer view;
- small hooks and guarded probes;
- no blocking work on Explorer's UI thread;
- generation tokens for async enumeration;
- cleanup on unload;
- rate-limited logging;
- no unbounded caches or worker leaks.
