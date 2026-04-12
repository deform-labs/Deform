#pragma once

//IMPORTANT : ENGINE API HEADER
#include <Engine/EngineAPI.h>

#include <vector> // Include the vector library for storing logs in the Logger class
#include <string> // Include the string library for handling log messages in the Logger class

#include "Engine/core/render/d3d11/DXRender.h"
#include "Engine/core/render/opengl/GLRender.h"
//#include "Engine/core/render/vulkan/VKRender.h"  // VKRender requires Vulkan SDK

#include "Engine/graphics/Window.h"



namespace deform
{
	class Logger {
	public:
	    // Log a message to the console and store it in the log history.
		// This allows for both real-time logging and later retrieval of past log messages.
		static DEFORM_API void Log(const std::string& msg);

		// Log a fatal error message, store it in the log history, and terminate the application.
		// This is used for critical errors that prevent the application from continuing to run.
		static DEFORM_API void FatalError(const char* message);

		// Retrieve the history of log messages.
		// This allows other parts of the application, such as a console panel in an editor, to display past log messages to the user.
		static DEFORM_API const std::vector<std::string>& GetLogs();

	private:
	    // A static vector that stores the history of log messages. This allows for retrieval of past log messages and is shared across all instances of the Logger class.
		static std::vector<std::string> s_Logs;
	};
}
