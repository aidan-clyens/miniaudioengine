---
description: "Scaffold a new miniaudioengine component: header, implementation, unit test fixture, and CMakeLists entries. Parameterized by layer (0–4) and component name."
argument-hint: "<ComponentName> <layer 0-4> [BaseInterface]"
agent: "agent"
---

Scaffold a new C++ component for the miniaudioengine project.

## Inputs

Determine from the user's message:
- **ComponentName** — PascalCase class name (e.g. `FooProcessor`)
- **Layer** — integer 0–4 (ask if ambiguous)
- **BaseInterface** — optional; infer from layer if not given (see table below)

## Layer Reference

| Layer | Name | `src/` path | `tests/unit/` path | Default namespace | Typical base interface | CMake static lib target(s) |
|-------|------|-------------|-------------------|-------------------|------------------------|---------------------------|
| 0 | Framework | `src/framework/` | `tests/unit/framework/` | `miniaudioengine::framework` | `IController` / `IDataPlane` / none | `framework` |
| 1 | Data (real-time) | `src/data/<domain>/` | `tests/unit/data/` | `miniaudioengine::framework` | `IDataPlane` | `data-audio` or `data-midi` |
| 2 | Processing | `src/processing/<domain>/` | `tests/unit/processing/` | `miniaudioengine::audio` | `IAudioProcessor` | `processor-audio` |
| 3 | Control | `src/control/<domain>/` | `tests/unit/control/` | `miniaudioengine::audio` or `miniaudioengine::midi` | `IController` | `control-audio` or `control-midi` |
| 4 | Public API | `src/public/<domain>/` | `tests/unit/control/` | `miniaudioengine` | `IManager` | `trackmanager` / `devicemanager` / `filemanager` |

For layers 1–4, ask whether the domain is `audio` or `midi` if it cannot be inferred from the component name.

## Files to Generate

### 1. Header — `src/<layer-path>/include/<snake_name>.h`

```cpp
#ifndef __<SNAKE_UPPER>_H__
#define __<SNAKE_UPPER>_H__

#include <memory>
// Add layer-appropriate includes (see below)

namespace <namespace>
{

/** @class <ComponentName>
 *  @brief TODO: describe the component.
 */
class <ComponentName> : public <BaseInterface>
{
public:
    explicit <ComponentName>();
    ~<ComponentName>() override = default;

    <ComponentName>(const <ComponentName>&) = delete;
    <ComponentName>& operator=(const <ComponentName>&) = delete;

    // TODO: declare public interface methods

private:
    // TODO: declare private members (m_ prefix for data, p_ prefix for shared_ptrs)
};

using <ComponentName>Ptr = std::shared_ptr<<ComponentName>>;

} // namespace <namespace>

#endif // __<SNAKE_UPPER>_H__
```

**Layer-appropriate includes to add:**
- Framework (shared): `"logger.h"`, `"device.h"`, `"file.h"` as needed
- Layer 1: `"dataplane.h"`, `"logger.h"` — add `noexcept` to any callback methods; no mutex, no allocation
- Layer 2: `"audioprocessor.h"`, `"logger.h"`
- Layer 3: `"controller.h"`, `"logger.h"` — mutex allowed
- Layer 4: `"manager.h"`, `"logger.h"`, `"trackmanager.h"` as needed

### 2. Implementation — `src/<layer-path>/src/<snake_name>.cpp`

```cpp
#include "<snake_name>.h"

using namespace <namespace>;

// TODO: implement methods
```

For Layer 1 (data plane), annotate any callback with `noexcept` and add a comment:
```cpp
// REAL-TIME SAFE: no mutex, no allocation, no blocking I/O
```

### 3. Unit Test — `tests/unit/<test-subdir>/test_<snake_name>_unit.cpp`

Follow the `ActionN_Description` naming convention for sequential scenarios:

```cpp
#include <gtest/gtest.h>
#include <iostream>

#include "<snake_name>.h"
#include "logger.h"

using namespace <namespace>;

class <ComponentName>Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        p_component = std::make_shared<<ComponentName>>();
    }

    void TearDown() override
    {
        p_component.reset();
    }

    <ComponentName>Ptr& component() { return p_component; }

private:
    <ComponentName>Ptr p_component;
};

/** @brief <ComponentName>Test - Init */
TEST_F(<ComponentName>Test, Action1_Init)
{
    EXPECT_NE(component(), nullptr) << "<ComponentName> should not be null after creation";
    LOG_INFO("Created <ComponentName>: ", component()->to_string());
}
```

**Singleton teardown** — if the component is a singleton (Layer 4 managers), replace the fixture body with:
```cpp
void SetUp() override   { <ComponentName>::instance().clear(); } // or equivalent reset
void TearDown() override { <ComponentName>::instance().clear(); }
<ComponentName>& m_component = <ComponentName>::instance();
```

### 4. CMakeLists.txt additions

**Do not create a new CMakeLists.txt.** Add to the existing one for the layer:

In `tests/unit/<test-subdir>/CMakeLists.txt`, add the new test file to the existing `add_library(miniaudioengine-unit-test-<layer> STATIC ...)` block:
```cmake
add_library(miniaudioengine-unit-test-<layer> STATIC
  ...existing files...
  test_<snake_name>_unit.cpp   # ADD THIS LINE
)
```

In `src/<layer-path>/CMakeLists.txt`, add the source file to the existing `add_library(<target> STATIC ...)` and expose the header:
```cmake
add_library(<target> STATIC
  ...existing sources...
  src/<snake_name>.cpp     # ADD THIS LINE
)
target_include_directories(<target> PUBLIC include/)
```

## Naming Rules (enforce throughout)

- Class: PascalCase (`AudioDataPlane`)
- Methods: snake_case (`process_audio()`)
- Data members: `m_` prefix (`m_running`)
- Shared-ptr members: `p_` prefix (`p_statistics`)
- Include guard: `#ifndef __<SNAKE_UPPER>_H__` style
- Boolean getters: `is_` / `has_` prefix
- Use `override` on all derived virtual methods
- Use `explicit` on single-parameter constructors

## Real-Time Safety (Layer 1 only)

Any method that runs inside a callback **must** follow these rules — flag violations as `// VIOLATION`:
1. No `std::mutex`, `lock_guard`, or blocking primitive
2. No `new`, `malloc`, `push_back`, or dynamic allocation
3. No file reads, no `sleep` calls
4. Cross-thread communication via `LockfreeRingBuffer<T, Size>` only
5. Total callback work must be under 1 ms

## Output

Emit each file in a fenced code block labelled with its path, then print the exact lines to add to each CMakeLists.txt as a diff or clearly labelled snippet. Do not generate a summary document.
