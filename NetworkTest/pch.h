#pragma once

//-----------------------------------------------------------------------
// PCH (pre complied header)
// 
// Erklärung: Prä-compilierte Header Datei für Biliotheks-Includes
//            Vermeidet mühselige, mehrfache Inkludierung von Headern
//            und beschleunigt die Kompilierzeit immens
// Autor    : Vinzenz Funk
// Zeit     : seit Oktober 2021
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Windows
#define WIN32_LEAN_AND_MEAN
//#define _WIN32_WINNT _WIN32_WINNT_WIN10	// nicht zwingend nötig
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// Standard libraries
// from: https://github.com/hsutter/cppfront/blob/main/include/cpp2util.h
//#define USE_CPP_MODULES // Comment in to activate imports for c++20 Modules instead of legacy stdlib headers
#if defined(USE_CPP_MODULES) && defined(__cpp_modules)

#ifndef _MSC_VER
	//  This is the ideal -- note that we just voted "import std;"
	//  into draft C++23 in late July 2022, so implementers haven't
	//  had time to catch up yet. As of this writing (September 2022)
	//  no compiler will take this path yet, but they're on the way...
import std;
#else // MSVC
	//  Note: When C++23 "import std;" is available, we will switch to that here
	//  In the meantime, this is what works on MSVC which is the only compiler
	//  I've been able to get access to that implements modules enough to demo
	//  (but we'll have more full-C++20 compilers soon!)
import std.core;
import std.regex;
import std.filesystem;
import std.memory;
import std.threading;

//  Suppress spurious MSVC modules warning
#pragma warning(disable:5050)
#endif

//  Otherwise, "fake it till you make it"... include (nearly) all the
//  standard headers, with a feature test #ifdef for each header that
//  isn't yet supported by all of { VS 2022, g++-10, clang++-12 }
//  ... this should approximate "import std;" on those compilers
#else
#include <version>
#include <concepts>
#ifdef __cpp_lib_coroutine
#include <coroutine>
#endif
#include <any>
#include <bitset>
#include <chrono>
#include <compare>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <initializer_list>
#include <optional>
#ifdef __cpp_lib_source_location
#include <source_location>
#endif
#include <tuple>
#include <type_traits>
#include <typeindex>
#ifndef CPP2_NO_RTTI
#include <typeinfo>
#endif
#include <utility>
#include <variant>
#include <memory>
#ifdef __cpp_lib_memory_resource
#include <memory_resource>
#endif
#include <new>
#include <scoped_allocator>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <cstdint>
#include <limits>
#include <cassert>
#include <cerrno>
#ifndef CPP2_NO_EXCEPTIONS
#include <exception>
#endif
#include <stdexcept>
#include <system_error>
#include <cctype>
#include <charconv>
#include <cstring>
#if __has_include(<cuchar>)
#include <cuchar>
#endif
#include <cwchar>
#include <cwctype>
#ifdef __cpp_lib_format
#include <format>
#endif
#include <string>
#include <string_view>
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <bit>
#include <cfenv>
#include <cmath>
#include <complex>
#include <numbers>
#include <numeric>
#include <random>
#include <ratio>
#include <valarray>
#include <clocale>
#include <codecvt>
#include <locale>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <ostream>
#ifdef __cpp_lib_spanstream
#include <spanstream>
#endif
#include <sstream>
#include <streambuf>
#ifdef __cpp_lib_syncstream
#include <syncstream>
#endif
#include <filesystem>
#include <regex>
#include <atomic>
#ifdef __cpp_lib_barrier
#include <barrier>
#endif
#include <condition_variable>
#include <future>
#ifdef __cpp_lib_latch
#include <latch>
#endif
#include <mutex>
#ifdef __cpp_lib_semaphore
#include <semaphore>
#endif
#include <shared_mutex>
#ifdef __cpp_lib_jthread
#include <stop_token>
#endif
#include <thread>

// libstdc++ currently has a dependency on linking TBB if <execution> is
// included, and TBB seems to be not automatically installed and linkable
// on some GCC installations, so let's not pull in that little-used header
// in our -pure-cpp2 "import std;" simulation mode... if you need this,
// use mixed mode (not -pure-cpp2) and #include all the headers you need
// including this one
// 
// #include <execution>
#endif

//////////////////////////////////////////////////////////////////////////
// std extensions
//#include "cpp23.hpp" // c++ 23 language features on lower c++ versions

//////////////////////////////////////////////////////////////////////////
// Vektoria


