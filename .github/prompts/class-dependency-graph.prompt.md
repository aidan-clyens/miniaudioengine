---
description: "Generate a Mermaid class dependency graph showing inheritance, composition, and association relationships between C++ classes in the miniaudioengine project."
agent: "agent"
argument-hint: "Optional: scope to a layer or namespace (e.g. 'engine', 'services', 'core', 'processing')"
---

Generate a **Mermaid class dependency graph** for this project by scanning the C++ source and header files.

## Step 1 — Discover classes

Read headers under `src/` and `include/` to identify all classes and interfaces. For each type, record:

- **Class name** and its **namespace** (`miniaudioengine::core`, `miniaudioengine::audio`, `miniaudioengine::midi`, or root `miniaudioengine`)
- **Base classes** (inheritance — `class Foo : public IFoo`)
- **Composed members** — `shared_ptr<T>`, `unique_ptr<T>`, or direct object members of project types
- **Association / weak references** — `weak_ptr<T>` members or raw non-owning pointer members to project types
- **`Ptr` type aliases** — `using FooPtr = std::shared_ptr<Foo>`

Focus on project-defined types; ignore standard library and third-party types (RtAudio, RtMidi, libsndfile, etc.) unless they appear as a named base class.

Key directories to scan:
- `src/framework/include/` — core primitives and interfaces (`IController`, `IDataPlane`, `LockfreeRingBuffer`, `Logger`)
- `src/adapters/include/` — adapter wrappers (`AudioAdapter`, `FileAdapter`, `MidiAdapter`)
- `src/engine/include/` — engine types (`AudioController`, `AudioDataPlane`, `MidiController`, `MidiDataPlane`)
- `src/processing/include/` — processing types (`IAudioProcessor`, `Sample`, `SamplePlayer`)
- `src/services/include/` — service layer (`TrackService`, `DeviceService`, `FileService`)
- `include/miniaudioengine/` — public API types and deprecated stubs

## Step 2 — Classify nodes

Assign each class a layer for styling:

| Layer | Namespace / path | Style |
|-------|-----------------|-------|
| `core` | `miniaudioengine::core` / framework | `fill:#lightblue` |
| `adapters` | `src/adapters/` | `fill:#ffe0b2` |
| `engine` | `src/engine/` | `fill:#lightgreen` |
| `processing` | `miniaudioengine::audio` / processing | `fill:#lightyellow` |
| `services` | Layer 4 services | `fill:#lightsalmon` |
| `public` | `include/miniaudioengine/` public API | `fill:#lightgray` |
| `interface` | Any `I`-prefixed abstract base | `fill:#e3f2fd,stroke:#1565c0,stroke-dasharray:5` |

## Step 3 — Render the Mermaid diagram

Produce a fenced Mermaid block. Choose the diagram type based on scope:

- **`classDiagram`** — preferred for scoped views (single layer or namespace); shows methods and members
- **`graph TD`** — preferred for the full cross-layer overview; avoids visual overload

### `classDiagram` conventions (scoped view)
- Show the 3–5 most important public methods per class using `+method() returnType`
- Mark abstract/interface classes with `<<interface>>` or `<<abstract>>`
- Use `<|--` for inheritance, `*--` for composition (`shared_ptr` owner), `o--` for aggregation (`weak_ptr`), `-->` for association
- Annotate cardinality where meaningful (e.g., `"1" *-- "0..*"`)

### `graph TD` conventions (full graph)
- One node per class; label with short readable name
- Arrow direction: **from dependent → dependency** (`A --> B` means A depends on B)
- Group related nodes with `subgraph` blocks matching the layer categories above
- Apply `style` directives using the colours from Step 2
- Omit deprecated stub classes from `include/miniaudioengine/` unless the `${argument}` scope explicitly includes them

### Scope filter
If an `${argument}` scope was provided, limit the diagram to classes in that layer/namespace and their **direct** dependencies (one hop). Otherwise produce the full cross-layer graph.

## Step 4 — Add a legend

After the diagram block, include a brief Markdown table mapping each style colour to its layer name, e.g.:

| Colour | Layer |
|--------|-------|
| Light blue | Core / framework |
| Orange | Adapters |
| Light green | Engine |
| Light yellow | Processing |
| Light salmon | Services |
| Light gray | Public API |
| Dashed blue border | Interface / abstract base |
