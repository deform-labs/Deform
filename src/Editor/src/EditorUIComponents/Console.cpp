#include "../../Editor.h"

void Editor::UI::Console::UpdateConsole()
{
    ImGui::Begin("Console", nullptr);

    const auto& logs = Logger::GetLogs();

    for (const auto& log : logs)
    {
        ImGui::Text("%s", log.c_str());
    }

    ImGui::End();
}