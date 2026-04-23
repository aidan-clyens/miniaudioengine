---
description: "Review design views in a Software Design Description (SDD) for internal consistency, completeness against design concerns, and cross-view coherence — purely from a design philosophy perspective, without considering the current implementation."
name: "Review Design Document"
argument-hint: "Design document to review (e.g., #file:DESIGN_DOC.md)"
agent: "agent"
---

Review each Design View in the attached design document strictly from a design philosophy perspective. Do **not** compare against the current implementation — evaluate only the document itself.

For each view, assess:

1. **Completeness** — Are all stated Design Concerns (DC-XX) for this view actually addressed by the diagram or content?
2. **Internal consistency** — Is the content within this view self-consistent and unambiguous?
3. **Cross-view coherence** — Does this view agree with other views in the document? Flag any contradictions (e.g., a component appears with a different name, role, or structure in another view).
4. **Design soundness** — Does the design depicted follow sound software engineering principles (e.g., separation of concerns, appropriate ownership, type safety, single responsibility)?

Structure the output as a section per view, with a clear status (**No issues** / **Issues found**) and bullet-point findings under each issue. End with a summary table listing all views and their status.
