#pragma once

#if defined(_MSVC_LANG) && _MSVC_LANG > __cplusplus
#define VULKANITE_CPP_VERSION _MSVC_LANG
#else
#define VULKANITE_CPP_VERSION __cplusplus
#endif

#if VULKANITE_CPP_VERSION >= 202302L
#define VULKANITE_SUPPORTED true
#else
#define VULKANITE_SUPPORTED false
#endif

#if !VULKANITE_SUPPORTED

#error Vulkanite requires C++23 or later. Set your language standard to C++23 or newer

#endif