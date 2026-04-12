#pragma once


// The DEFORM_API macro is defined to either export or import symbols from the Deform Engine DLL
// depending on whether DEFORM_ENGINE_EXPORTS is defined.
// This allows the engine to be used as a shared library in other projects.
#ifdef DEFORM_ENGINE_EXPORTS
	#define DEFORM_API __declspec(dllexport)
#else
	#define DEFORM_API __declspec(dllimport)
#endif
