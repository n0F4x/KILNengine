#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <array>
#include "Plane.slang"
template <typename T, unsigned N>
using Array = std::array<T, N>;
#else
module Frustum;
import Plane;
#define EXPORT public
#define PUBLIC_MEMBER public
#endif

EXPORT struct Frustum {
    PUBLIC_MEMBER Array<Plane, 6> planes;
};
