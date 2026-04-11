#pragma once

#include <Engine/EngineAPI.h>

class DEFORM_API Scene
{
public:
   // Creates a new scene by initializing the necessary data structures and resources. This function is responsible for setting up the scene's objects, components, and any other relevant data needed for the scene to function properly. The implementation of this function will depend on the specific requirements of the scene and the engine's architecture.
   void CreateNewScene();

   // Indicates whether the scene is loaded and ready for use.
   bool Loaded = false;

   // the current number of objects in the scene. This variable can be used to track the number of objects present in the scene, which may be useful for performance monitoring, debugging, or other purposes related to scene management.
   float CurrentOBJS = {};
};
