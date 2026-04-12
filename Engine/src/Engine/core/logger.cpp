#include "Engine.h"

#include <cstdio>
#include <string>
#include <vector>
#include <winuser.h>

std::vector<std::string> deform::Logger::s_Logs;

// Log a message to the console and store it in the log history. This allows for both real-time logging and later retrieval of past log messages.
void deform::Logger::Log(const std::string& msg)
{
    s_Logs.push_back(msg);
	printf("[LOG] %s\n", msg.c_str());
}

// Log a fatal error message, store it in the log history, and terminate the application. This is used for critical errors that prevent the application from continuing to run.
void deform::Logger::FatalError(const char* message)
{
    MessageBoxA(nullptr, message, "Fatal Error", MB_ICONERROR | MB_OK);
    printf("[LOG] %s\n", message);
}

// Retrieve the history of log messages. This allows other parts of the application, such as a console panel in an editor, to display past log messages to the user.
const std::vector<std::string>& deform::Logger::GetLogs()
{
    return s_Logs;
}
