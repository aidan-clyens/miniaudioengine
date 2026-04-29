---
description: "Step-by-step guide for safely refactoring any component in the miniaudioengine project. Covers planning, implementation, validation, and cleanup phases. Parameterized by refactor type and affected component(s)."
argument-hint: "<ComponentName or path> <refactor-type: rename|extract|pimpl|split|move|inline>"
agent: "agent"
---

Guide a safe, complete refactor of a miniaudioengine component. Follow every phase in order — do not skip steps.

## Inputs

Determine from the user's message:
- **Target** — class name, file, or directory being refactored
- **RefactorType** — one of: `rename`, `extract`, `pimpl`, `split`, `move`, `inline`
- **Layer** — infer from the path (see layer table below); ask if ambiguous
- **Scope** — does this touch the public API (`include/miniaudioengine/`) or only internals?

## Layer Reference

| Layer | Path | Namespace | Real-time? |
|-------|------|-----------|------------|
| 0 | `src/framework/` | `miniaudioengine::framework` | No |
| 1 | `src/data/` | `miniaudioengine::framework` | **Yes** |
| 2 | `src/processing/` | `miniaudioengine::audio` | No |
| 3 | `src/control/` | `miniaudioengine::audio` / `::midi` | No |
| 4 | `src/public/` + `include/` | `miniaudioengine` | No |

---

## Phase 1 — Understand the Impact

### 1.1 Read the target file(s)

Read every header and `.cpp` involved. Do not rely on memory alone.

### 1.2 Map all usages

Search for every reference to the symbol being changed:

```
grep -r "<SymbolName>" src/ include/ tests/ examples/
```

List each usage by file and line. Group by:
- **Internal implementations** (same component)
- **Dependent components** (cross-file, same layer)
- **Cross-layer consumers** (higher layers that include this header)
- **Unit tests and mocks**
- **Examples**

### 1.3 Check layer constraints

Confirm that after the refactor, all `#include` directives still satisfy the **upward-only dependency rule** (lower layers must never include higher-layer headers). Flag any potential violation before proceeding.

### 1.4 Check real-time safety (Layer 1 only)

If the target is in `src/data/` or is called from an RtAudio/RtMidi callback:
- No `std::mutex`, `std::lock_guard`, or any blocking primitive
- No heap allocation (`new`, `malloc`, `std::vector::push_back`, `std::make_shared`)
- No blocking I/O
- Cross-thread messaging must still use `LockfreeRingBuffer`

---

## Phase 2 — Plan the Changes

Write out a concrete change list before touching any file. For each file describe:
- What changes (rename, move, add, delete)
- Why
- Any order dependencies (e.g. "header before cpp before test")

For a **public API change** (anything under `include/miniaudioengine/`):
- Determine whether backward-compatibility stubs are needed
- If yes, plan a deprecated wrapper header (see Phase 4 → Cleanup)

For a **`pimpl` refactor** specifically:
- The public header becomes the handle; all RtAudio/RtMidi/libsndfile types move into a `*Impl` struct inside the `.cpp`
- The handle header must include only `<memory>` and project-level types
- Factory functions live in `src/public/.../include/<snake>_factory.h`

---

## Phase 3 — Implement

Apply changes in this order to keep the build green at each step:

### Step A — Header changes first

1. Update or create the new header(s) under the correct path.
2. Add/update include guards: `#ifndef __<SNAKE_UPPER>_H__` / `#define` / `#endif`.
3. Add the `using <Name>Ptr = std::shared_ptr<Name>;` alias to every new class header.
4. Annotate public declarations with Doxygen (`@brief`, `@param`, `@return`).

### Step B — Implementation changes

1. Update `.cpp` files to match the new header.
2. Apply naming conventions:
   - Data members: `m_` prefix
   - Pointer members: `p_` prefix
   - Methods: `snake_case`
3. Add `override` to every derived virtual method.
4. Add `noexcept` to real-time callbacks and methods that must not throw.
5. Add `explicit` to any new single-parameter constructor.

### Step C — Update all consumers

Work through the usage list from Phase 1.2 from **lowest layer to highest**:
1. Internal implementations
2. Dependent components
3. Cross-layer consumers
4. CMakeLists — add/rename source files and targets as needed

### Step D — Update unit tests and mocks

1. Mocks live in `tests/mocks/include/` under `miniaudioengine::test`.
2. Mirror interface names with `Mock` prefix (e.g. `MockAudioController`).
3. Update mock method signatures to match the new interface.
4. Update test fixtures: rename types, update `SetUp()` / `TearDown()`.
5. Add `clear_tracks()` / singleton reset calls in `TearDown()` wherever a singleton is exercised.

### Step E — Update examples

Update files under `examples/` to use the new API.

---

## Phase 4 — Validate

### 4.1 Build

```
cmake --build build --config Debug
```

Fix every compiler error and warning before continuing. Do not suppress warnings.

### 4.2 Run unit tests

```
.\build\tests\unit\Debug\miniaudioengine-unit-tests.exe
```

All tests that passed before the refactor must still pass. Investigate any new failure — do not accept regressions.

To run only the affected suite:
```
.\build\tests\unit\Debug\miniaudioengine-unit-tests.exe --gtest_filter="<ClassName>.*"
```

### 4.3 Check architecture compliance

Verify no `#include` in a lower layer now points to a higher-layer header. Use the `review-architecture` prompt if a full scan is needed.

---

## Phase 5 — Cleanup

### 5.1 Deprecated stubs (public API changes only)

If old public headers existed under `include/miniaudioengine/`, leave a forwarding stub:

```cpp
// <old_name>.h — deprecated, use <new_name>.h
#pragma once
#warning "<old_name>.h is deprecated; include <new_name>.h instead"
#include "<new_name>.h"
```

### 5.2 Remove dead code

Delete any file, type alias, or forward declaration made redundant by the refactor. Do not leave commented-out code.

### 5.3 Update repo memory

Append a short summary of the completed refactor to `/memories/repo/` so future sessions have accurate context:
- What was renamed/moved/wrapped
- New public types and header paths
- Any pre-existing test failures unrelated to this refactor

---

## Refactor-Type Quick Reference

| Type | Core action | Watch out for |
|------|-------------|---------------|
| `rename` | Rename class + file + usages | Include guards, CMakeLists target names |
| `extract` | Pull a responsibility into a new class | Circular includes, shared state |
| `pimpl` | Hide impl behind pointer in `.cpp` | Header-only types, factory pattern needed |
| `split` | Divide one class into two | Thread ownership, who holds the shared_ptr |
| `move` | Relocate to a different layer | Layer constraint violations |
| `inline` | Collapse a small class into its caller | Real-time safety if caller is Layer 1 |

## Output

Save the completed refactor plan and implementation steps in a new file under `docs/` named `refactor-<target>-<type>.md` for future reference.