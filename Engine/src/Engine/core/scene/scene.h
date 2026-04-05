#pragma once

#include <Engine/EngineAPI.h>

class DEFORM_API Scene
{
public:
   void CreateNewScene();
   bool Loaded = false;

   float CurrentOBJS = {};
};
