#pragma once
#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

// 平方square
template<typename T>
constexpr auto sq(const T& x) noexcept
{
	return x * x;
}

// 将角度限制在[-180, 180)
template<typename T>
T wrap_angle(T theta) noexcept
{
	const T modded = fmod(theta, (T)2.0 * (T)PI_D);
	return (modded > (T)PI_D) ? (modded - (T)2.0 * (T)PI_D) : modded;
}

// ???
template<typename T>
constexpr T interpolate(const T& src, const T& dst, float alpha) noexcept
{
	return src + (dst - src) * alpha;
}

// 角度转弧度
template<typename T>
constexpr T to_rad(T deg) noexcept
{
	return deg * PI / (T)180.0;
}

// 高斯
template<typename T>
constexpr T gauss(T x, T sigma) noexcept
{
	const auto ss = sq(sigma);
	return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * ss)) * exp(-sq(x) / ((T)2.0 * ss));
}

