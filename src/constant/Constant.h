#pragma once

#include "lib/vecmath/vecmath.h"
#include <vector>

using namespace std;

namespace Constant
{
    static constexpr const float MEAN = 1.2;
    static constexpr const double UPPER_SPEED_LIMIT = 1.8;
    static constexpr const double LOWER_SPEED_LIMIT = 0.6;
    static constexpr const float PI = 3.14159265359F;

    // Speed - color
    // No disability, without overtaking behavior
    static constexpr const float V1 = 1.24;
    static const Color3f GREEN = {0.0, 128.0, 0.0};
    // The blind
    static constexpr const float V6 = 0.52;
    static const Color3f BLACK = {0.0, 0.0, 0.0};
    // Walking with crutches
    static constexpr const float V3 = 0.94;
    static const Color3f RED = {216.0, 32.0, 42.0};
    // No disability, with overtaking behavior
    static constexpr const float V2 = 2.28;
    static const Color3f PURPLE = {133.0, 22.0, 255.0};
    // Wheelchairs
    static constexpr const float V5 = 0.69;
    static const Color3f GRAY = {130.0, 130.0, 130.0};
    // Walking with sticks
    static constexpr const float V4 = 0.81;
    static const Color3f WOOD = {224.0, 183.0, 92.0};
}