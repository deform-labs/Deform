#include "../Editor/Editor.h"
#include "../config.h"

using namespace Editor;

void UI::MainBar::UpdateMainBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::Button("New Scene"))
            {
                Logger::Log("New Scene Created!");
            }

            if (ImGui::Button("Open..."))
            {
                Logger::Log("New Scene Opened!");
            }

            if (ImGui::Button("Save"))
            {
                Logger::Log("Scene Saved!");
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            static bool showHierarchy = true;
            static bool showInspector = true;
            static bool showScene = true;
            static bool showConsole = true;
            ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &showInspector);
            ImGui::MenuItem("Scene", nullptr, &showScene);
            ImGui::MenuItem("Console", nullptr, &showConsole);
            ImGui::EndMenu();
        }

        

        if (ImGui::BeginMenu("Render"))
        {

            if (ImGui::Button("Settings"))
            {
                ImGui::OpenPopup("Render Settings");
            }

            if (ImGui::BeginPopupModal("Render Settings", NULL))
            {
                ImGui::Text("Change ClearColor");
                ImGui::Separator();
				
				ImGui::ColorEdit4("Clear Color", config.clearColor);

                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }

            ImGui::EndMenu();

        }

        ImGui::EndMainMenuBar();
    }
}

void OpenRenderSettings()
{
	ImGui::OpenPopup("Render Settings");
}
