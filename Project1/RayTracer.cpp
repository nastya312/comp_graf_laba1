#include <limits>
#include "RayTracer.h"
#include "Material.h"
#include "Geometry.h"
#include <iostream>


//Базовый алгоритм трассировки луча
float3 SimpleRT::TraceRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geo, const int& depth)
{
	float tnear = std::numeric_limits<float>::max();
	int   geoIndex = -1;

	SurfHit surf;

	for (int i = 0; i < geo.size(); ++i)
	{
		SurfHit temp;

		if (geo.at(i)->Intersect(ray, 0.001, tnear, temp))
		{
			if (temp.t < tnear)
			{
				tnear = temp.t;
				geoIndex = i;
				surf = temp;
			}
		}
	}

	if (geoIndex == -1)
	{
		float3 unit_direction = normalize(ray.direction);
		float t = 0.5f * (unit_direction.y + 1.0f);

		return (1.0f - t) * float3(1.0f, 1.0f, 1.0f) + t * bg_color;
	}

	float3 surfColor(0.0f, 0.0f, 0.0f);
	if (dot(ray.direction, surf.normal) > 0)
	{
		surf.normal = -surf.normal;
	}

	Ray scattered;
	if (depth < max_ray_depth && surf.m_ptr->Scatter(ray, surf, surfColor, scattered))
	{
		return surfColor * TraceRay(scattered, geo, depth + 1);
	}
	else
	{
		return float3(0.0f, 0.0f, 0.0f);
	}
}


float3 SimpleRT::WhittedTraceRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geo, const std::vector<std::shared_ptr<Point_light>>& light_source, int depth)
{

	float tnear = std::numeric_limits<float>::max();
	int   geoIndex = -1;

	//float3 chang_color(1.0f, 1.0f, 1.0f); // цвет, который меняется в процессе цикла

	float3 сolor(0.0f, 0.0f, 0.0f);

	SurfHit surf;

	for (int i = 0; i < geo.size(); ++i)
	{
		SurfHit temp;

		if (geo.at(i)->Intersect(ray, 0.001, tnear, temp))
		{
			if (temp.t < tnear)
			{
				tnear = temp.t;
				geoIndex = i;
				surf = temp;
			}
		}
	}

	if (geoIndex == -1)
	{
		float3 unit_direction = normalize(ray.direction);
		float t = 0.5f * (unit_direction.y + 1.0f);

		return (1.0f - t) * float3(1.0f, 1.0f, 1.0f) + t * bg_color;
	}
	;
	if (dot(ray.direction, surf.normal) > 0)
	{
		surf.normal = -surf.normal;
	}

	Ray chang_ray = ray; // луч, который меняется в процессе цикла

	Ray scattered;
	if (typeid(*surf.m_ptr).name() == typeid(Diffuse).name()) {
		
		float3 attenuation;

		// для всех источников цвета
		for (int i = 0; i < light_source.size(); i++) {
			Ray rayIn;
			rayIn.origin = light_source[i]->direction; // формирует исходную точку для луча, который высчитывается в диффузии 
			rayIn.direction = normalize(light_source[i]->direction - surf.hitPoint); // из точки испускания вычитается точка пересечения (расчет направления в сторону источника света)
			Ray shadow(surf.hitPoint, rayIn.direction); // испускается луч

			// если в направлении луча нет объектов, мешающих перемещению до источника
			if (!ShadowRay(shadow, geo))
			{
				surf.m_ptr->Scatter(rayIn, surf, attenuation, scattered);
				сolor = сolor + attenuation * light_source[i]->color; // изменяется цвет в соответсвии с результатом
			}
		}
		return сolor;
	}
	else if (typeid(*surf.m_ptr).name() == typeid(IdealMirror).name())
	{
		surf.m_ptr->Scatter(chang_ray, surf, сolor, scattered);
		chang_ray = scattered;
		if (depth < max_ray_depth)
			return сolor * WhittedTraceRay(chang_ray, geo, light_source, depth + 1);
		else return сolor;
	}
	else 
		return float3(0.0f, 0.0f, 0.0f); // присваивает черный цвет, если не соответсвует ни единому условию 
}


// теневой луч возвращает информацию, есть ли какие-то объекты на пути луча от найденной точки пересечения
// до источника света или нет
bool SimpleRT::ShadowRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geo) {
	Ray timeRay = ray;
	float tnear = std::numeric_limits<float>::max();
	int   geoIndex = -1;
	SurfHit surf;
	for (int i = 0; i < geo.size(); ++i) // пробегается по количеству объектов
	{
		SurfHit temp;
		if (geo.at(i)->Intersect(timeRay, 0.001, tnear, temp))
		{
			if (temp.t < tnear)
			{
				tnear = temp.t;
				geoIndex = i;
				surf = temp;
			}
			return true;
		}
	}
	return false;
}