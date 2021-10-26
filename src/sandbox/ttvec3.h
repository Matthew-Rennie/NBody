#pragma once
#include "ttmath/ttmath/ttmath.h"
#include "ttmath/ttmath/ttmathuint_x86_64.h"
struct ttvec3
{
	static const int exponent = 1;
	static const int mantissa = 4;

	using BigFloat = ttmath::Big<exponent, mantissa>;

	BigFloat x = 0.0;
	BigFloat y = 0.0;
	BigFloat z = 0.0;

	ttvec3 operator+(const ttvec3& other) const
	{
		ttvec3 return_value;
		return_value.x = this->x + other.x;
		return_value.y = this->y + other.y;
		return_value.z = this->z + other.z;
		return return_value;
	}

	friend ttvec3 operator-(const ttvec3& other)
	{
		ttvec3 return_value;
		return_value.x = -other.x;
		return_value.y = -other.y;
		return_value.z = -other.z;
		return return_value;
	}

	ttvec3 operator-(const ttvec3& other) const
	{
		ttvec3 return_value;
		return_value.x = this->x - other.x;
		return_value.y = this->y - other.y;
		return_value.z = this->z - other.z;
		return return_value;
	}

	ttvec3& operator+=(const ttvec3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	ttvec3& operator-=(const ttvec3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}

	ttvec3 operator*(const BigFloat& d) const
	{
		ttvec3 return_value;
		return_value.x = this->x * d;
		return_value.y = this->y * d;
		return_value.z = this->z * d;
		return return_value;
	}

	ttvec3 operator/(const BigFloat& d) const
	{
		ttvec3 return_value;
		return_value.x = this->x / d;
		return_value.y = this->y / d;
		return_value.z = this->z / d;
		return return_value;
	}

	ttvec3& operator*=(const BigFloat& d)
	{
		this->x *= d;
		this->y *= d;
		this->z *= d;
		return *this;
	}

	ttvec3& operator/=(const BigFloat& d)
	{
		this->x /= d;
		this->y /= d;
		this->z /= d;
		return *this;
	}

	BigFloat Length()
	{
		BigFloat l = (x * x + y * y + z * z);
		l.Sqrt();
		return l;
	}
};