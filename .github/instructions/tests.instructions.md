---
description: "Use when writing, modifying, or reviewing unit tests, test fixtures, mocks, or test CMakeLists. Covers gtest conventions, mock construction, singleton teardown, and WHOLEARCHIVE linking."
applyTo: "tests/**"
---

# Test Layer Guidelines

## Test File & Suite Naming

- File names: `test_<component>_unit.cpp` (e.g. `test_trackmanager_unit.cpp`)
- Fixture class name matches component: `<ComponentName>Test` (e.g. `TrackManagerTest`, `AudioDataPlaneTest`)
- Test method names use `TEST_F(FixtureName, ActionN_Description)` for ordered scenarios or `TEST_F(FixtureName, DescriptiveName)` for independent cases
- Stateless framework tests may use bare `TEST(SuiteName, TestName)` without a fixture class body

## Fixture Pattern

```cpp
class AudioDataPlaneTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        p_audio_dataplane = std::make_shared<AudioDataPlane>();
    }

    void TearDown() override
    {
        p_audio_dataplane->stop();
        p_audio_dataplane.reset();
    }

    AudioDataPlanePtr& audio_dataplane() { return p_audio_dataplane; }

private:
    AudioDataPlanePtr p_audio_dataplane;
};
```

## Singleton Reset — `clear_tracks()`

Any test that exercises `TrackManager` **must** call `clear_tracks()` in both `SetUp` and `TearDown` to isolate test state. Hold the singleton as a member reference:

```cpp
class TrackManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_track_manager.clear_tracks();
        m_track_manager.set_audio_output_device(
            DeviceManager::instance().get_default_audio_output_device());
    }

    void TearDown() override { m_track_manager.clear_tracks(); }

private:
    TrackManager& m_track_manager = TrackManager::instance();
};
```

## Mock Construction Patterns

Mocks live in `tests/mocks/include/` under `namespace miniaudioengine::test`. All share `#ifndef __<NAME>_MOCK_H__` include guards.

| Mock | Ptr alias | Construction |
|------|-----------|--------------|
| `MockController` | `MockControllerPtr` | `std::make_shared<MockController>()` |
| `MockDataPlane` | `MockDataPlanePtr` | `std::make_shared<MockDataPlane>()` |
| `MockAudioOutputDevice` | `MockAudioOutputDevicePtr` | `std::make_shared<MockAudioOutputDevice>()` |
| `MockAudioController` | — | `MockAudioController mock_ctrl;` (stack or member) |

- Always use `std::make_shared` for pointer-based mocks; never `new`
- Mocks inherit the real interface (`IController`, `IDataPlane`, `IAudioDevice`) — downcast only when testing base interface behaviour
- `test-mocks` is an **INTERFACE** (header-only) CMake library; add it to `target_link_libraries` as a `PRIVATE` dep of the test static lib

## Adding a New Test File

1. Place the `.cpp` in the matching subdirectory (`framework/`, `control/`, or `data/`)
2. Add it to the corresponding `add_library(miniaudioengine-unit-test-<layer> STATIC ...)` in that subdirectory's `CMakeLists.txt`
3. **Do not** add it directly to the `miniaudioengine-unit-tests` executable — it is linked transitively

## WHOLEARCHIVE — Why It Exists

Test `.cpp` files are compiled into intermediate static libraries (`miniaudioengine-unit-test-framework`, `-control`, `-data`) and then linked into the `miniaudioengine-unit-tests` executable. Because nothing in `main()` calls test symbols directly, the linker would dead-strip them without `WHOLEARCHIVE`:

- **MSVC**: `/WHOLEARCHIVE:<lib>` in `target_link_options`
- **GCC/Clang**: `-Wl,--whole-archive <lib> -Wl,--no-whole-archive`

This is already configured in `tests/unit/CMakeLists.txt`. **Do not remove or bypass these flags** — doing so silently drops test cases without error.

## Namespace Imports

Use explicit `using namespace` declarations at file scope (not inside functions):

```cpp
using namespace miniaudioengine;           // public API types
using namespace miniaudioengine::audio;    // AudioDataPlane, etc.
using namespace miniaudioengine::core;     // LockfreeRingBuffer, DoubleBuffer, etc.
using namespace miniaudioengine::test;     // mocks
```

## Includes Order

```cpp
#include <gtest/gtest.h>      // always first
#include <iostream>           // std headers
#include "component.h"        // component under test
#include "mock_foo.h"         // mocks (from tests/mocks/include/)
#include "logger.h"           // optional — for debug output in tests
```
