#include "CommandStack.hpp"

namespace fluxion::editor
{
    void CommandStack::execute(std::unique_ptr<commands::Command> cmd)
    {
        if (!cmd) return;
        cmd->redo();
        push(std::move(cmd));
    }

    void CommandStack::push(std::unique_ptr<commands::Command> cmd)
    {
        if (!cmd) return;

        // Merge attempt against the most recent entry. When the merge
        // is accepted, `cmd` is dropped on this scope's exit; the
        // stack's tail entry has expanded to cover the new state.
        if (!undoStack_.empty() && undoStack_.back()->tryMerge(*cmd))
            return;

        undoStack_.push_back(std::move(cmd));
        // Any new mutation rewrites the future: clear redo history.
        redoStack_.clear();
        trimToMax();
    }

    void CommandStack::undo()
    {
        if (undoStack_.empty()) return;
        auto cmd = std::move(undoStack_.back());
        undoStack_.pop_back();
        cmd->undo();
        redoStack_.push_back(std::move(cmd));
    }

    void CommandStack::redo()
    {
        if (redoStack_.empty()) return;
        auto cmd = std::move(redoStack_.back());
        redoStack_.pop_back();
        cmd->redo();
        undoStack_.push_back(std::move(cmd));
    }

    void CommandStack::clear() noexcept
    {
        undoStack_.clear();
        redoStack_.clear();
    }

    std::string CommandStack::undoLabel() const
    {
        return canUndo() ? undoStack_.back()->label() : std::string{};
    }

    std::string CommandStack::redoLabel() const
    {
        return canRedo() ? redoStack_.back()->label() : std::string{};
    }

    void CommandStack::trimToMax()
    {
        // Drop the OLDEST entries first — the user benefits more from
        // recent history than ancient. Both stacks combined are
        // bounded by maxSize on the undo side; the redo stack is
        // always a subset of (cleared on push) so it's implicitly
        // bounded too.
        while (undoStack_.size() > maxSize_)
            undoStack_.pop_front();
    }
}
