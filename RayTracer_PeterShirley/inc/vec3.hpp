#ifndef __YS_VEC3_HPP__
#define __YS_VEC3_HPP__

#include <cmath>
#include <cstdlib>
#include <iostream>


struct vec3
{
	vec3() = default;
	constexpr vec3(float _e0, float _e1, float _e2)
		:e{ _e0, _e1, _e2 }
	{}

	static constexpr vec3 one() { return vec3{ 1.f, 1.f, 1.f }; }

	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }
	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }

	inline const vec3& operator+() const { return *this; }
	inline vec3 operator-() const { return vec3(-x(), -y(), -z()); }
	inline float operator[](int _i) const { return e[_i]; }
	inline float& operator[](int _i) { return e[_i]; }

	inline vec3& operator+=(const vec3& _rhs);
	inline vec3& operator-=(const vec3& _rhs);
	inline vec3& operator*=(const vec3& _rhs);
	inline vec3& operator/=(const vec3& _rhs);
	inline vec3& operator*=(const float _rhs);
	inline vec3& operator/=(const float _rhs);

	inline float length() const { return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]); }
	inline float squared_length() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }
	inline void make_unit_vector();

	float e[3];
};


inline std::istream&
operator >> (std::istream& _is, vec3& _v)
{
	_is >> _v.e[0] >> _v.e[1] >> _v.e[2];
	return _is;
}


inline std::ostream&
operator << (std::ostream& _os, const vec3& _v)
{
	_os << _v.e[0] << " " << _v.e[1] << " " << _v.e[2];
	return _os;
}


inline void
vec3::make_unit_vector()
{
	float k = 1.0f / length();
	*this *= k;
}


inline vec3&
vec3::operator += (const vec3& _rhs)
{
	e[0] += _rhs.e[0]; e[1] += _rhs.e[1]; e[2] += _rhs.e[2];
	return *this;
}


inline vec3&
vec3::operator -= (const vec3& _rhs)
{
	e[0] -= _rhs.e[0]; e[1] -= _rhs.e[1]; e[2] -= _rhs.e[2];
	return *this;
}


inline vec3&
vec3::operator *= (const vec3& _rhs)
{
	e[0] *= _rhs.e[0]; e[1] *= _rhs.e[1]; e[2] *= _rhs.e[2];
	return *this;
}


inline vec3&
vec3::operator /= (const vec3& _rhs)
{
	e[0] /= _rhs.e[0]; e[1] /= _rhs.e[1]; e[2] /= _rhs.e[2];
	return *this;
}


inline vec3&
vec3::operator *= (const float _rhs)
{
	e[0] *= _rhs; e[1] *= _rhs; e[2] *= _rhs;
	return *this;
}


inline vec3&
vec3::operator /= (const float _rhs)
{
	e[0] /= _rhs; e[1] /= _rhs; e[2] /= _rhs;
	return *this;
}


inline vec3
operator + (const vec3& _lhs, const vec3& _rhs)
{
	return vec3(_lhs.e[0] + _rhs.e[0], _lhs.e[1] + _rhs.e[1], _lhs.e[2] + _rhs.e[2]);
}


inline vec3
operator - (const vec3& _lhs, const vec3& _rhs)
{
	return vec3(_lhs.e[0] - _rhs.e[0], _lhs.e[1] - _rhs.e[1], _lhs.e[2] - _rhs.e[2]);
}


inline vec3
operator * (const vec3& _lhs, const vec3& _rhs)
{
	return vec3(_lhs.e[0] * _rhs.e[0], _lhs.e[1] * _rhs.e[1], _lhs.e[2] * _rhs.e[2]);
}


inline vec3
operator / (const vec3& _lhs, const vec3& _rhs)
{
	return vec3(_lhs.e[0] / _rhs.e[0], _lhs.e[1] / _rhs.e[1], _lhs.e[2] / _rhs.e[2]);
}


inline vec3
operator * (float _s, const vec3& _v)
{
	return vec3(_v.e[0] * _s, _v.e[1] * _s, _v.e[2] * _s);
}


inline vec3
operator * (const vec3& _v, float _s)
{
	return _s * _v;
}


inline vec3
operator / (const vec3& _v, float _s)
{
	return vec3(_v.e[0] / _s, _v.e[1] / _s, _v.e[2] / _s);
}


inline vec3
unit_vector(const vec3& _v)
{
	return _v / _v.length();
}


inline float
dot(const vec3& _lhs, const vec3& _rhs)
{
	return _lhs.e[0]*_rhs.e[0] + _lhs.e[1]*_rhs.e[1] + _lhs.e[2]*_rhs.e[2];
}


inline vec3
cross(const vec3& _lhs, const vec3& _rhs)
{
	return vec3{_lhs.e[1]*_rhs.e[2] - _lhs.e[2]*_rhs.e[1],
				_lhs.e[2]*_rhs.e[0] - _lhs.e[0]*_rhs.e[2],
				//-(_lhs.e[0]*_rhs.e[2] - _lhs.e[2]*_rhs.e[0]),
				_lhs.e[0]*_rhs.e[1] - _lhs.e[1]*_rhs.e[0]};
}


inline vec3
reflect(const vec3& _vector, const vec3& _normal)
{
	return _vector - 2.f * dot(_vector, _normal) * _normal;
}


bool 
refract(const vec3& _vector, const vec3& _normal, float _niOverNt, vec3& _refracted)
{
	vec3 uv{ unit_vector(_vector) };
	float dt = dot(uv, _normal);
	float discriminant = 1.f - _niOverNt*_niOverNt*(1.f - dt*dt);
	if (discriminant > 0.f)
	{
		_refracted = _niOverNt*(uv - _normal*dt) - _normal*sqrt(discriminant);
		return true;
	}
	else
		return false;
}


#endif // __YS_VEC3_HPP__
