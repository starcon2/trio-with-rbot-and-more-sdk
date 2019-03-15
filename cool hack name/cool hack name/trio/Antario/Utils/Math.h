#pragma once
#include "../SDK/Vector.h"
#include "../SDK/VMatrix.h"
#include "../Utils/SPoint.h"
#include <array>
#define PI 3.14159265358979323846f

class Math
{
public:
	static Vector VectorTransform(Vector vector, matrix3x4_t matrix);
	static void VectorTransform_2(const Vector & in1, const matrix3x4_t & in2, Vector & out);

	void VectorTransform(const float* in1, const matrix3x4_t& in2, float* out);

	float DotProduct(const float* a, const float* b);

	void SinCos(float radians, float* sine, float* cosine);

	void RotateTriangle(std::array<Vector2D, 3>& points, float rotation);

	void VectorAngles_2(const Vector & forward, Vector & angles);

	void AngleVectors(const Vector& angles, Vector* forward = nullptr, Vector* right = nullptr, Vector* up = nullptr);

	float DistanceTo(Vector vecDst, Vector vecSrc);

	Vector NormalizeAngle(Vector angle);

	Vector CalcAngle(Vector src, Vector dst);

	void CalcAngle(Vector src, Vector dst, Vector & angles);

	void CalcAngle234(Vector src, Vector dst, Vector& angles);

	void CalcAngle2(Vector src, Vector dst, Vector& angles);

	float GRD_TO_BOG(float GRD);

	void VectorAngles234(Vector forward, Vector& angles);

	void SinCos2634(float a, float* s, float* c);

	float NormalizeAngle(float flAng);

	void AngleVectors234(const Vector& angles, Vector* forward);

	void VectorAngles(const Vector& forward, Vector& up, Vector& angles);




	float RandomFloat(float min, float max);

	Vector ClampAngles(Vector& angles);

	void NormalizeAngles(Vector& angles);

	void VectorAngles(const Vector& forward, Vector& angles);

	static constexpr float Math::rad_2_deg(float radian)
	{
		return radian * (180.f / PI);
	}

	static constexpr float Math::deg_2_rad(float degree)
	{
		return degree * (PI / 180.f);
	}
};

extern Math g_Math;
