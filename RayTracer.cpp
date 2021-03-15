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
	float3 color = float3(1.0f, 1.0f, 1.0f); // цвет, который получим
	float3 chang_color = float3(1.0f, 1.0f, 1.0f); // цвет, который меняется в процессе цикла
	
	Ray chang_ray = ray; // луч, который меняется в процессе цикла
	
	// While (depth < MAX_DEPTH) 
	while (depth < max_ray_depth) {

		color = color * chang_color; // в начале каждого меняем цвет в зависимости от произошедших изменений

		float tnear = std::numeric_limits<float>::max();
		int   geoIndex = -1;

		SurfHit surf;

		// поиск объектов, с которыми пересекается луч и соотвественно точек пересечения
		for (int i = 0; i < geo.size(); ++i) {
			SurfHit temp;
			
			if (geo.at(i)->Intersect(chang_ray, 0.001, tnear, temp)) {
				if (temp.t < tnear) {
					// перебераются точки, выбирается нужная
					tnear = temp.t;
					geoIndex = i;
					surf = temp;
				}
			}
		}
		
		//если пересечения нет, вычисляется цвет фона
		// так как 
		if (geoIndex == -1) {
			float3 unit_direction = normalize(chang_ray.direction);
			float t = 0.5f * (unit_direction.y + 1.0f);
			chang_color = (1.0f - t) * float3(1.0f, 1.0f, 1.0f) + t * bg_color;
			// c = c + background_color

			
			break;
		}

		if (dot(chang_ray.direction, surf.normal) > 0) {
			surf.normal = -surf.normal;
		}
		
		Ray scattered; // луч, необходимый для работы с материалами (ray_out, выходящий луч)

		// определяем материал, после чего для каждого источника света генерируется теневой луч
		// в сторону источника
		if (typeid(*surf.m_ptr).name() == typeid(Defuse).name()) {
			chang_color = float3(0.0f, 0.0f, 0.0f); //Color c = (0, 0, 0)
			
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
					chang_color = chang_color + attenuation * light_source[i]->color; // изменяется цвет в соответсвии с результатом
				}
			}
			break;
		}

		// изменяется цвет, генрируется луч и находится пересечение с зеркалом
		else if (surf.m_ptr->Scatter(chang_ray, surf, chang_color, scattered)) {
			chang_ray = scattered; // ray = new_ray
			depth++; // depth += 1
		} 
		else {
			chang_color = float3(0.0f, 0.0f, 0.0f); // присваивает черный цвет, если не соответсвует ни единому условию 
			depth++;
		}
	}
	return color * chang_color;

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