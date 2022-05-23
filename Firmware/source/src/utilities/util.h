#ifndef UVENT_UTIL_H
#define UVENT_UTIL_H

#include <Arduino.h>
#include <malloc.h>
#include "logging.h"

#ifdef __cplusplus

// Undef the Arduino implementation
#undef min

template<typename Tpa, typename Tpb>
constexpr auto min(const Tpa& a, const Tpb& b) -> decltype(a < b ? a : b)
{
    return b < a ? b : a;
}

#undef max
template<typename Tpa, typename Tpb>
inline auto max(const Tpa& a, const Tpb& b) -> decltype(b > a ? b : a)
{
    return b > a ? b : a;
}
#endif// __cplusplus

#define EPSILON 0.0000001

/**
 * @param ptr The pointer to the field/var keeping the time
 * @param n The amount of millis required to elapse
 * @return True if n millis have passed, else false.
 */
bool has_time_elapsed(uint32_t* ptr, uint32_t n);

// Returns the current time in seconds
inline float now_s() { return millis() * 1e-3; }

bool is_whole(double x, double epsilon = EPSILON);

#endif//UVENT_UTIL_H
