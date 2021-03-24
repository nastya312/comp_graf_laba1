#ifndef RT_SAMPLE_NEW_GEOMETRY_H
#define RT_SAMPLE_NEW_GEOMETRY_H

#include "LiteMath.h"
#include "RayTracer.h"
#include "Material.h"

using namespace HydraLiteMath;

//Базовый виртуальный класс для геометрического объекта, с которым может пересекаться луч
class GeoObject
{
public:
	virtual bool Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const = 0;
	std::shared_ptr<Material> m_ptr;
	explicit GeoObject(Material* a_m) : m_ptr(a_m) {};
	virtual ~GeoObject() = default;
};

//Бесконечная плоскость
class Plane : public GeoObject
{
public:
	Plane(const float3 a_point, const float3 a_normal, Material* a_m) : GeoObject(a_m), point(a_point), normal(a_normal) {}
	~Plane() = default;

	bool Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const override;

private:
	float3 point;
	float3 normal;
};

class Sphere : public GeoObject
{
public:

	Sphere(const float3& a_center, const float& a_r, Material* a_m) : GeoObject(a_m), center(a_center), radius(a_r) {}
	~Sphere() = default;

	bool Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const override;

private:
	float3 center;
	float radius;

};

class Triangle : public GeoObject
{
public:

	Triangle(const float3 a, const float3 b, const float3 c, Material* a_m) : GeoObject(a_m), a(a), b(b), c(c) {}
	~Triangle() = default;

	bool Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const override;

private:
	float3 a, b, c;

};

class Parallel : public GeoObject {
public:
	Parallel(const float3& a, const float3& b, Material* a_m) :
		GeoObject(a_m), a(a), b(b) {}
	~Parallel() = default;

	bool Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const override;

private:
	float3 a, b;
	// прямоугольный параллелепипед задан вершинами
};

class Square : public GeoObject {
public:
	Square(const float3& a, const float3& b, const float3& c) : GeoObject(new Diffuse(float3(1.0f, 1.0f, 1.0f))), a(a), b(b), c(c) {}
	~Square() = default;

	bool Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const override;

private:
	float3 a, b, c;
	
};

#endif //RT_SAMPLE_NEW_GEOMETRY_H
