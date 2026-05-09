#ifndef FLUXION_EDITOR_COMMANDS_COMMAND_HPP
#define FLUXION_EDITOR_COMMANDS_COMMAND_HPP

#include <string>

namespace fluxion::editor::commands
{
    // Base interface for every undo/redo entry.
    //
    // Concrete commands capture only the data they need to apply
    // and reverse a single user-visible action. Where they reference
    // scene objects they SHOULD use stable identifiers (Uuid for
    // actors) rather than raw pointers, so the command stack
    // survives a sequence like "delete actor X, undo, redo, undo"
    // without dangling pointers — each redo/undo re-resolves X by
    // id.
    //
    // Lifetime contract:
    //   * The CommandStack owns the unique_ptr.
    //   * redo() and undo() must be safely callable in any
    //     interleaved order (redo, undo, redo, undo, ...).
    //   * If the captured target no longer exists when the command
    //     runs (e.g., a parent layer was removed by some non-undoable
    //     path), the command MUST no-op cleanly rather than crash.
    //     The stack treats no-ops as successful, so the user can
    //     keep stepping through history.
    //
    // Extending the system:
    //   * Add a new command by subclassing Command, capturing the
    //     before/after state in the constructor, and implementing
    //     redo/undo. No edits to CommandStack or other commands are
    //     required.
    //   * Optional: override tryMerge to fold rapid-fire identical
    //     command targets (e.g., per-frame transform updates from
    //     a gizmo drag) into one undo entry.
    class Command
    {
    public:
        virtual ~Command() = default;
        Command(const Command&) = delete;
        Command& operator=(const Command&) = delete;
        Command(Command&&) = delete;
        Command& operator=(Command&&) = delete;

        // Apply forward. Called on first execute() and on every
        // redo() from the redo stack.
        virtual void redo() = 0;

        // Reverse. Called on every undo().
        virtual void undo() = 0;

        // Human-readable label, used by the Edit menu's "Undo X" /
        // "Redo X" entries. Kept short — typically the verb plus
        // the target type, e.g., "Add Actor", "Move Actor".
        [[nodiscard]] virtual std::string label() const = 0;

        // Try to fold `other` into `this`. Returns true iff this
        // command absorbed `other` — in which case CommandStack
        // discards `other` instead of pushing it. Used for drag
        // aggregation: every gizmo-frame can push a fresh
        // SetPositionCommand and the stack collapses them into a
        // single undo entry that captures the start and final
        // positions.
        //
        // The default implementation refuses every merge, which is
        // the right choice for commands whose effect depends on
        // ordering (delete, add, etc.).
        [[nodiscard]] virtual bool tryMerge(const Command& other)
        {
            (void)other;
            return false;
        }

    protected:
        Command() = default;
    };
}

#endif // FLUXION_EDITOR_COMMANDS_COMMAND_HPP
