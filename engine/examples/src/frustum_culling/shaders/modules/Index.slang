#ifndef __SLANG__
#pragma once
#define EXPORT
#define TYPE_ALIAS using
#include <cstdint>
#else
module Index;
#define EXPORT public
#define TYPE_ALIAS typealias
#endif

EXPORT TYPE_ALIAS Index = uint32_t;
