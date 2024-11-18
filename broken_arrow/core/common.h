#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

#ifdef _DEBUG
#define XOR_STR(STRING) STRING
#else
#define JM_XORSTR_DISABLE_AVX_INTRINSICS
#include "../utils/xor/xorstr.h"
#define XOR_STR(STRING) xorstr_(STRING)
#endif

#ifdef _DEBUG
#define PARANOID
#define PARANOID_PATTERN_UNIQUENESS
#endif

#pragma region common_compiler_specific
#ifdef _MSC_VER
#define COMPILER_MSC
#endif
#ifdef __clang__ // @note: clang-cl have defined both 'COMPILER_CLANG' and 'COMPILER_MSC'
#define COMPILER_CLANG
#endif

#ifdef COMPILER_MSC
// treat "discarding return value of function with 'nodiscard' attribute" warning as error
#pragma warning(error : 4834)
#endif
#pragma endregion

#pragma region common_log_specific
// define to enable logging output to console
#ifdef _DEBUG
#define LOG_CONSOLE
#endif

// define to enable logging output to file
#define LOG_FILE
#pragma endregion

#pragma region common_implementation_specific
#define _INTERNAL_CONCATENATE(LEFT, RIGHT) LEFT##RIGHT
#define _INTERNAL_STRINGIFY(NAME) #NAME
#define _INTERNAL_UNPARENTHESIZE(...) __VA_ARGS__

// convert plain text to string
#define STRINGIFY(NAME) _INTERNAL_STRINGIFY(NAME)
// concatenate plain text
#define CONCATENATE(LEFT, RIGHT) _INTERNAL_CONCATENATE(LEFT, RIGHT)
// unparenthesize variadic arguments
#define UNPARENTHESIZE(...) _INTERNAL_UNPARENTHESIZE(__VA_ARGS__)

// calculate elements count of fixed-size C array
#undef ARRAYSIZE
#define ARRAYSIZE(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#ifndef DEBUG_BREAK
#if defined(COMPILER_MSC)
#define DEBUG_BREAK() __debugbreak()
#elif defined(COMPILER_CLANG)
#define DEBUG_BREAK() __builtin_debugtrap()
#else
static_assert(false, "it is expected you to define DEBUG_BREAK() into something that will break in a debugger!");
#define DEBUG_BREAK()
#endif
#endif

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(EXPRESSION) static_cast<void>(!!(EXPRESSION) || (DEBUG_BREAK(), 0))
#else
// disable assertion for release builds
#define ASSERT(EXPRESSION) static_cast<void>(0)
#endif
#endif
#pragma endregion