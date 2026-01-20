---
agent: agent
description: Updates the custom Copilot instructions and agent documentation in .github directory to reflect current codebase architecture and completed refactors
---

# Update Copilot Instructions and Agents

## Purpose
This prompt updates all custom Copilot instructions and agent documentation files in the `.github` directory to accurately reflect the current state of the Minimal Audio Engine codebase. It ensures that AI coding assistants have up-to-date information about the architecture, completed refactors, and current component organization.

## When to Use
- After completing major architectural refactors (e.g., MIDI refactor, engine pattern migrations)
- When component structure changes (e.g., splitting engines into controller + data)
- After removing or adding new dependencies
- When layer boundaries are reorganized (control plane, data plane, processing plane)
- Periodically to ensure documentation stays synchronized with code

## What Gets Updated

### Files to Review and Update:
1. **`.github/copilot-instructions.md`** - Main instructions for AI coding agents
   - Architecture diagrams and layer descriptions
   - Component structure and file organization
   - Threading and concurrency patterns
   - Dependency list (vcpkg.json references)
   - Completed components vs. TODO items
   - Critical files reference section

2. **`.github/agents/Architecture Analyst.md`** - Architecture analysis agent
   - Design pattern descriptions (Engine, Singleton, Observer)
   - Threading model explanations
   - Key architecture files by layer
   - Analysis framework and review checklists

3. **`.github/agents/Code Review.agent.md`** - Code review agent
   - Component isolation requirements
   - Architecture compliance checks
   - Module boundary definitions

4. **`.github/agents/PlantUML.agent.md`** - Diagram generation agent
   - Example classes and components
   - Architecture references

5. **`.github/agents/Digital Audio Expert.agent.md`** - Audio programming expert
   - (Usually minimal changes needed - focuses on general audio concepts)

## Key Areas to Check

### Architecture Updates
- [ ] Layer diagrams show current structure (Framework → Data Plane → Processing Plane → Control Plane → CLI)
- [ ] Control Plane components listed accurately (AudioStreamController, MidiPortController, TrackManager, DeviceManager, FileManager)
- [ ] Data Plane components listed accurately (AudioDataPlane, MidiDataPlane, callback handlers)
- [ ] Legacy patterns marked as deprecated (IEngine<T> pattern being phased out)

### Component Organization
- [ ] File structure matches actual src/ directory layout
- [ ] Namespace organization is correct (miniaudioengine::control, Data, Core)
- [ ] Include paths reference actual file locations

### Threading & Concurrency
- [ ] Control plane described as synchronous singletons (no dedicated threads)
- [ ] Data plane described as callback-based (RtAudio/RtMidi threads)
- [ ] Processing plane marked as NOT YET IMPLEMENTED
- [ ] Legacy IEngine<T> pattern marked as deprecated

### Dependencies
- [ ] vcpkg.json dependencies match actual requirements
- [ ] Removed dependencies no longer listed (e.g., cli11, replxx if removed)
- [ ] External libraries accurately described (RtAudio, RtMidi, libsndfile, gtest)

### Refactor Status
- [ ] Completed refactors moved from "In Progress" to "Completed Components"
- [ ] TODO items reflect actual remaining work
- [ ] Critical files section references existing files only

### Examples & References
- [ ] Code examples use current class names (not deprecated ones)
- [ ] File paths in links are valid
- [ ] Test file references are accurate

## Process

1. **Scan codebase for current structure:**
   - List directories in src/ (framework, data, control, cli)
   - Check what files exist in each component
   - Identify singleton classes and their patterns

2. **Identify completed refactors:**
   - Search for deprecated classes (AudioEngine, MidiEngine)
   - Verify replacement components exist (AudioStreamController, MidiPortController)
   - Check if legacy patterns are still in use

3. **Update architecture documentation:**
   - Revise layer diagrams to show current structure
   - Update threading model descriptions
   - Correct file organization sections

4. **Update component status:**
   - Move completed refactors from TODO to Completed
   - Remove outdated warnings and notes
   - Update critical files references

5. **Verify consistency:**
   - Search for references to old class names
   - Check that all file paths are valid
   - Ensure dependency lists match vcpkg.json

## Common Updates

### After MIDI Refactor (Example)
When MidiEngine is replaced with MidiPortController + MidiDataPlane:

**Before:**
- Data Plane: MidiEngine (legacy IEngine pattern - needs refactor)
- TODO: Refactor MidiEngine to control plane synchronous pattern

**After:**
- Control Plane: MidiPortController (synchronous MIDI port management)
- Data Plane: MidiDataPlane + MidiCallbackHandler (lock-free callbacks)
- Completed: MIDI refactor complete ✅

### After Removing Dependencies (Example)
When cli11 and replxx are removed:

**Before:**
```markdown
- `cli11`: Command-line parsing
- `replxx`: Interactive REPL with history
```

**After:**
```markdown
(Dependencies removed, basic CLI implemented without external libs)
```

## Validation

After updates, verify:
- [ ] No references to deprecated classes (grep for AudioEngine, MidiEngine)
- [ ] All file paths in documentation exist in codebase
- [ ] Architecture diagrams match actual src/ structure
- [ ] Threading model descriptions are accurate
- [ ] Dependency list matches vcpkg.json
- [ ] Examples use current API patterns

## Tips

- Use `file_search` to find all .md files in .github/
- Use `grep_search` to find references to deprecated classes
- Use `list_dir` to verify directory structure
- Use `read_file` to check actual file contents
- Update multiple files in parallel when possible (multi_replace_string_in_file)
- Don't announce which tools you're using - just make the changes

## Example Prompt

"Update the custom Copilot instructions and agents in '.github' accordingly with updates from this codebase"

The agent will:
1. Scan .github/ directory for all documentation files
2. Check current codebase structure in src/
3. Identify architectural changes and completed refactors
4. Update all documentation files to reflect current state
5. Verify consistency across all files
