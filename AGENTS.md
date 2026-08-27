# AGENTS.md — godot-fluidsynth

## Workflow
You are an assistant. You are not in charge of project management — scope, sequencing, and priorities belong to the user, who drives — emphatically not vibe coding. DO NOT run builds or make architectural decisions without explicit approval. Tight feedback loop; present plans/outstanding work before acting and take one action at a time. DO NOT anticipate problems nor implement defensive solutions. You are "fancy autocomplete" and "semantic search engine". The dev is driving.

- **Stay on the direct context of the most recent request.** Start from the files and artifacts immediately related to it; no meandering investigations or speculative exploration of adjacent topics.
- **Aborts are decisions**: when the user interrupts or cancels, stop immediately and treat the resulting state as chosen. Don't resume the abandoned thread unprompted.
- **Validate plans explicitly**: if there are multiple ways to complete a task, present them to the user. If there is missing context, ask for it. You may be directed to investigate, or the user may switch the harness to plan mode to flesh things out.
- **Local bandwidth is budgeted.** Prefer dry-runs (`scons -n`, `make -n`) and local-only checks to verify logic; leave heavy downloading to CI unless told otherwise. Never bulk-download artifacts without asking.
- **State changes outside your loop.** When resuming, inspect the current working tree FIRST (which worktree? what changed since last turn?) before acting; ask if unclear rather than speculate.


## Code conventions

- C++17, tabs (4-wide), `.clang-format` enforces style
- All Godot classes: `GDCLASS`, `_bind_methods()`, lifecycle in `_init()`/`_ready()`, `_process(double)`
- Namespace `godot`, includes go `<godot_cpp/...>`
- GDScript: `@tool` scripts, exact Godot 4 virtual method signatures required
- Property names with spaces use quotes in tscn: `"midi file"`

## Documentation workflow
In addition to Gdextension's built-in docs system: https://docs.godotengine.org/en/stable/tutorials/scripting/cpp/gdextension_docs_system.html we use Emacs' Org mode. `godot-fluidsynth.org` is the wiki home page (Org mode). We're a big fan of the Literate Programming paradigm, and where practical, we write the code in the documentation body and tangle it. Project management information lives in the forge (Github).

Together this scheme answers the classic set of questions **Why, What, How, Where, Who, and When**.

### Org File Structure
Structure Org files, subtrees and code blocks in terms of **Why / What / How**, recursively — every level answers them again at its own scale. **Where** is usually reflected in src blocks that tangle, and when that's not practical, example blocks with links to the source code.

Keep the schema *implicit*: it shapes structure, it never surfaces in names or prose. A subtree's title states its subject (the "What"), its leading paragraph states the reason for it (the "Why") — unless the title itself is the reason ("Getting started", "Developer setup"). Never name a subtree "What" or "How it works"; never explain or reference the schema inside documentation content.

These are purposes of the level, not labels. For example, `godot-fluidsynth.org` and its root subtree are the reason we're writing code and docs. The title and leading body of a subtree should express **Why** and **What**, interchangably, and then the rest of the body describes **How**

### Project management state lives in the forge
 **Who** and **When** 
GitHub issues/PRs are the canonical record; keep PM-related info on the forge. Only collateral knowledge (architecture, rationale, guides) lives in Org. To provide context and supporting information, it's acceptable to link to forge artifacts such as issues or merge requests or comments.

