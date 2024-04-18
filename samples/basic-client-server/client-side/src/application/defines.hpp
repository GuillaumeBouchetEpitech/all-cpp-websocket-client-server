
#pragma once

#if defined __EMSCRIPTEN__

#define D_WEB_BUILD
#include <emscripten.h>

#else

#define D_NATIVE_BUILD

#endif
