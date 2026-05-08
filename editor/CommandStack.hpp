#ifndef FLUXION_EDITOR_COMMANDSTACK_HPP
#define FLUXION_EDITOR_COMMANDSTACK_HPP

#include <cstddef>
#include <deque>
#include <memory>
#include <string>
#include "commands/Command.hpp"

namespace fluxion::editor
{
    // Two-stack undo/redo with bounded history and optional merge
    // support for drag-style commands.
    //
    // Memory: the undo stack is capped at maxSize (default 256). When
    // the cap is exceeded, the OLDEST entries are dropped — the user
    // keeps the most recent history. Tunable via the constructor for
    // editor sessions that touch huge scenes.
    //
    // Merge: push() asks the most recent undo entry whether it can
    // absorb the incoming command. If yes, the new command is dropped
    // and the existing entry's captured state expands. This is how a
    // gizmo drag (which pushes one command per frame) collapses into
    // a single undo step.
    class CommandStack
    {
    public:
        explicit CommandStack(std::size_t maxSize = 256) noexcept:
            maxSize_{maxSize} {}

        // Calls cmd->redo() then pushes. Use for commands whose
        // mutation should be applied right now.
        void execute(std::unique_ptr<commands::Command> cmd);

        // Pushes WITHOUT calling redo(). Use when the mutation has
        // already been applied (e.g., a gizmo updated the actor live
        // and only at drag-release captures the start/end pair into
        // a command). Same redo-stack-clear semantics as execute().
        void push(std::unique_ptr<commands::Command> cmd);

        [[nodiscard]] bool canUndo() const noexcept { return !undoStack_.empty(); }
        [[nodiscard]] bool canRedo() const noexcept { return !redoStack_.empty(); }

        void undo();
        void redo();

        // Drop all history. Editors call this on scene load / new
        // scene because the captured before-states no longer match
        // the current scene.
        void clear() noexcept;

        // Edit-menu hints — empty when nothing is on the
        // corresponding stack.
        [[nodiscard]] std::string undoLabel() const;
        [[nodiscard]] std::string redoLabel() const;

    private:
        void trimToMax();

        std::deque<std::unique_ptr<commands::Command>> undoStack_;
        std::deque<std::unique_ptr<commands::Command>> redoStack_;
        std::size_t maxSize_;
    };
}

#endif // FLUXION_EDITOR_COMMANDSTACK_HPP
