#include "Material.h"

SurfHit& SurfHit::operator= (const SurfHit& rhs)
{
    if (this == &rhs)
        return (*this);

    hit = rhs.hit;
    hitPoint = rhs.hitPoint;
    normal = rhs.normal;
    m_ptr = rhs.m_ptr;
    t = rhs.t;

    return (*this);
}

//////////////////

float3 reflect(const float3& v, const float3& normal)
{
    // Если поверхность обладает отражающими свойствами, то
    // строится вторичный луч отражения. Направление луча
    // определяется по закону отражения(геометрическая оптика) :
    // i – 2 · n ·(n · i)
    return v - 2 * dot(v, normal) * normal;
}


bool IdealMirror::Scatter(const Ray& ray_in, const SurfHit& surf, float3& attenuation, Ray& ray_out)
{
    float3 reflection_dir = reflect(normalize(ray_in.direction), surf.normal);

    ray_out = Ray(surf.hitPoint + surf.normal * 1e-4, reflection_dir);
    // сместить исходную точку, чтобы избежать перекрытия самим объектом

    attenuation = color;

    return (dot(ray_out.direction, surf.normal) > 0);
    // проверяется косинус больше нуля
}

bool Defuse::Scatter(const Ray& ray_in, const SurfHit& surf, float3& attenuation, Ray& ray_out)
{
    float kd = dot(normalize(surf.normal), ray_in.direction);
    // высчитывается косинус через единичные вектора 
    // Можно просто посмотреть на угол между нормальным вектором в этой точке и вектором, описывающим направление света. 
    // Чем меньше угол, тем лучше освещена поверхность. Чтобы считать было ещё удобнее, можно просто взять скалярное 
    // произвдение между вектором нормали и вектором освещения. Напоминаю, что скалярное произвдение между двумя векторами 
    // a и b равно произведению норм векторов на косинус угла между векторами: a*b = |a| |b| cos(alpha(a,b)). 
    // Если взять векторы единичной длины, то простейшее скалярное произведение даст нам интенсивность освещения поверхности.


    attenuation = color * max(kd, 0.0f); // умножаем на косинус - нашу интенсивность света
    return false;
}