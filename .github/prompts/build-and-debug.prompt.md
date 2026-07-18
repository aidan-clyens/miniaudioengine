---
description: "Build the project with CMake and automatically debug any compiler or linker errors found in the output."
name: "build-and-debug"
agent: "agent"
tools: [execute/runInTerminal, execute/getTerminalOutput,
  read/readFile, read/terminalLastCommand, edit/editFiles,
  search/codebase, search/fileSearch, search/textSearch
]
---

Run the CMake build command from the workspace root:

```
"C:\Program Files\CMake\bin\cmake.EXE" --build C:/Projects/miniaudioengine/build --config Debug --target ALL_BUILD --
```

After the build completes, examine the full terminal output for errors:

- A successful build ends with `Build succeeded` or exit code 0.
- A failed build contains lines like `[build] Build finished with exit code 1`, MSVC `error C####`, or GCC/Clang `error:` diagnostics.

If no errors are found, report that the build succeeded and stop.

If errors are found, proceed with the following debugging workflow:

1. **Parse all errors and warnings** from the build output. Group them by file and list every affected symbol or line number.
2. **Read each affected source file** around the reported line(s) to understand the context before making any changes.
3. **Identify the root cause** — do not treat symptoms (e.g., cascading "unknown type" errors) as separate bugs; trace back to the first real error.
4. **Apply fixes** directly to the source files using the file editing tools. Follow the project's C++20 conventions (see [copilot-instructions.md](../copilot-instructions.md)):
   - Naming: PascalCase classes, `m_` data members, `p_` pointer members, `eX` enum types
   - Specifiers: `override`, `noexcept`, `explicit`, `const` as appropriate
   - No heap allocation or mutexes in real-time callback paths (`src/engine/`)
5. **Rebuild** after applying fixes by running `cmake --build build` again.
6. **Repeat** until the build output shows exit code 0 or until a blocker requires user input.

If a fix requires architectural changes beyond a local edit (e.g., changing a public API, adding a new dependency), stop and describe the problem and proposed solution for the user to approve before proceeding.
