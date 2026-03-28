#include "../../Engine.h"

#ifdef DEFORM_API
#undef DEFORM_API
#endif // DEFORM_API
#define DEFORM_API

#define DEFORM_API
std::vector<std::string> deform::Logger::s_Logs;

void deform::Logger::Log(const std::string& msg)
{
    s_Logs.push_back(msg);
}

void deform::Logger::FatalError(const char* message)
{
    MessageBoxA(nullptr, message, "Fatal Error", MB_ICONERROR | MB_OK);
}

const std::vector<std::string>& deform::Logger::GetLogs()
{
    return s_Logs;
}