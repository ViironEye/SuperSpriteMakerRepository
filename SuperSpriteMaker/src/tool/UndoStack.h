#pragma once
#include <memory>
#include "Command.h"

class UndoStack {
public:
	void push(std::unique_ptr<Command> cmd)
	{
		m_redo.clear();
		m_undo.push_back(std::move(cmd));
	}

	void undo()
	{
		if (m_undo.empty()) return;

		auto cmd = std::move(m_undo.back());
		m_undo.pop_back();
		cmd->undo();
		m_redo.push_back(std::move(cmd));
	}

	void redo()
	{
		if (m_redo.empty()) return;

		auto cmd = std::move(m_redo.back());
		m_redo.pop_back();
		cmd->redo();
		m_undo.push_back(std::move(cmd));
	}

private:
	std::vector<std::unique_ptr<Command>> m_undo;
	std::vector<std::unique_ptr<Command>> m_redo;
};