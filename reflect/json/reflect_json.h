#pragma once

#if defined(_MSC_VER)
#define CPLUSPLUS _MSVC_LANG
#else
#define CPLUSPLUS __cplusplus
#endif

static_assert(CPLUSPLUS >= 201402L, "__cplusplus >= 201402L");

#if CPLUSPLUS >= 202002L
#include "internal/reflect_json_cpp20.hpp"
#else
#include "internal/reflect_json_cpp17.hpp"
#endif

