# Research Notes

## Finder-Style Expandable Folders

macOS Finder's list-view expansion behaves like an outline/table view:

- the underlying data is hierarchical;
- the visible UI is a flattened list of currently visible rows;
- expanding inserts child rows directly after the parent;
- collapsing removes the visible descendant range;
- the UI does small row diffs instead of rebuilding a full nested tree.

The smoothness comes from:

- lazy loading only immediate children;
- row virtualization and reuse;
- async metadata, icon, and preview loading;
- small row insert/remove operations;
- caching;
- filesystem event updates instead of constant polling.

## Sorting

Sorting should be hierarchical:

- sort each folder's immediate children;
- preserve parent/child containment;
- rebuild or diff the flattened visible list after sorting.

Do not globally sort every visible row together. That would separate children
from parents and break the outline model.

## Selection

The UI selection is based on visible rows, but file operations must resolve back
to stable item identities.

Selection state should use item IDs, not row indexes only. Row indexes change
when folders expand, collapse, refresh, rename, or sort.

Before container operations, normalize selection:

- if a selected item has a selected ancestor, skip the descendant;
- copy, move, delete, compress, and drag/drop should usually normalize;
- properties, rename, and open may need operation-specific behavior.

## Windows Explorer Context

Explorer is a Shell namespace browser, not just a filesystem path viewer.
Ordinary files, virtual folders, libraries, devices, search results, ZIP
folders, cloud placeholders, and namespace extensions can all appear through
Shell abstractions.

That means the architecture should prefer Shell item identities and Shell
operations over path-only logic.

## Windhawk Context

Windhawk mods are C++ DLLs loaded into target processes. This project targets
`explorer.exe`.

Current source metadata:

```cpp
// @id              explorer-expandable-folders
// @name            Explorer Expandable Folders
// @version         0.3.1
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32
```

Current UI control:

```text
Explorer window -> bottom-right Expandable folders popup checkbox
```

Current persisted value:

```text
explorerViewEnabled
```

## Current Permission Finding

Direct install from a non-elevated shell failed for Windhawk's ProgramData and
HKLM storage:

```text
C:\ProgramData\Windhawk\ModsSource
C:\ProgramData\Windhawk\Engine\Mods\64
HKLM:\Software\Windhawk\Engine\Mods
```

The practical path is to compile and enable through the Windhawk UI unless an
elevated install flow is explicitly available.
