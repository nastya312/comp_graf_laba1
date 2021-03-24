#ifndef RT_SAMPLE_NEW_RAY_H
#define RT_SAMPLE_NEW_RAY_H

#include "LiteMath.h"

using namespace HydraLiteMath;

class GeoObject;
class LightSource;

class Ray
{
public:

  float3 origin;
  float3 direction;

  Ray() : origin(0.0f, 0.0f, 0.0f), direction(0.0, 0.0, 1.0) {};
  Ray(const float3& a_o, const float3& a_d) : origin(a_o), direction(a_d) {};
  Ray(const Ray& ray) : origin(ray.origin), direction(ray.direction) {};

  Ray& operator= (const Ray& rhs) { if (this == &rhs) return (*this); origin = rhs.origin; direction = rhs.direction; return (*this); };

  ~Ray(){};
};

class Point_light {
public:
	explicit Point_light(const float3 direction, const float3 color) : direction(direction), color(color) {};
	virtual ~Point_light() = default;

	float3 direction;
	float3 color;
};


class SimpleRT
{
public:
  SimpleRT() = default;
  SimpleRT(const int &a_max_ray_depth, const float3 &a_bg_color) : max_ray_depth(a_max_ray_depth), bg_color(a_bg_color){};
  ~SimpleRT() = default;

  float3 TraceRay(const Ray &ray, const std::vector<std::shared_ptr<GeoObject>> &geo,const int &depth);
  float3 WhittedTraceRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geo, const std::vector<std::shared_ptr<Point_light>>& light_source, int depth);
  bool ShadowRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geo);


private:
  int max_ray_depth; // максимальная глубина трассировки
  float3 bg_color;   // цвет фона
};

#endif //RT_SAMPLE_NEW_RAY_H
