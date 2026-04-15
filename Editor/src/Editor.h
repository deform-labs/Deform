#pragma once
#pragma warning(disable: 4005) // Disable macro redefinition warning for ImGui headers

#include "imgui.h"

#include <Engine.h>

#include <vector>
#include <string>


// The Editor namespace contains the UIState struct, which holds the state of the editor's UI, and the UI namespace, which contains classes for each of the editor's panels (Hierarchy, Inspector, MainBar, Console) with static methods to update their contents.
namespace Editor {
    // UIState struct holds the state of the editor's UI, including which panels are visible and the clear color for the scene.
    struct UIState
    {
        bool showHierarchy = true;
        bool showInspector = true;
        bool showScene = true;
        bool showConsole = true;
        float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    };

    // The UI namespace contains classes for each of the editor's panels (Hierarchy, Inspector, MainBar, Console) with static methods to update their contents.
    namespace UI {
        class Hierarchy { public: static void UpdateHierarchy(); }; // The Hierarchy panel displays a list of all the objects in the scene and allows the user to select and manipulate them.
        class Inspector { public: static void UpdateInspector(); }; // The Inspector panel displays the properties of the currently selected object and allows the user to edit them.
        class MainBar { public: static void UpdateMainBar(UIState& state); }; // The MainBar panel contains the main menu and toolbar for the editor, allowing the user to access various functions and settings.
        class Console { public: static void UpdateConsole(); }; // The Console panel displays the output of the editor's logging system and allows the user to interact with it.
    }
}
