#pragma once

// Absolute difference between a and b
inline double abs_diff(double a, double b)
{
    return abs(a - b);
}

// Determine if a and b are equal, accepting up to epsilon as the difference.
inline bool equals(double a, double b)
{
    constexpr double epsilon = 0.000001;
    return abs_diff(a, b) < epsilon;
}
