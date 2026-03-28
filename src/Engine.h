#pragma once

//IMPORTANT : ENGINE API HEADER
#include <EngineAPI.h>


#include <vector>
#include <string>
#include <Windows.h>

#include "Engine/core/render/d3d11/DXRender.h"
#include "Engine/core/render/opengl/GLRender.h"
#include "Engine/core/render/vulkan/VKRender.h"

#include "Engine/graphics/Window.h"



namespace deform
{
	class DEFORM_API Logger {
	public:
		static void Log(const std::string& msg);
		static void FatalError(const char* message);

		static const std::vector<std::string>& GetLogs();

	private:
		static std::vector<std::string> s_Logs;
	};

	class DEFORM_API Engine
	{
	public:
		Engine();
		~Engine();

	private:

	};
}