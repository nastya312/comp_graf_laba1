#include "Geometry.h"

bool Plane::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const
{
    surf.t = dot((point - ray.origin), normal) / dot(ray.direction, normal);
    // точка нахождения луча в момент времени - точка испускания луча / направление движения луча на нормаль = отрезок времени

    if (surf.t > t_min && surf.t < t_max)
    {
        surf.hit = true; // есть пересечение
        surf.hitPoint = ray.origin + surf.t * ray.direction;   // точка пересечения
        //высчитываем по формуле луча

        surf.normal = normal; // вектор нормали к поверхности в точке пересечения
        surf.m_ptr = m_ptr; //материал объекта, с которым нашли пересечение
        return true;
    }
    return false;
}

//Для пересечения со сферой подставляем луч в уравнение сферы и решаем получившееся квадратное уравнение
//Решения этого уравнения - точки пересечения луча со сферой
bool Sphere::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const
{

    float3 k = ray.origin - center;

    float a = dot(ray.direction, ray.direction);
    // берем квадрат вектора скорости
    float b = dot(2 * k, ray.direction);
    // 2 * (точка испускания луча - центр сферы) * вектор скорости
    float c = dot(k, k) - r_sq;
    // разность точки испускания и центра - квадрат радиуса

    float d = b * b - 4 * a * c;

    if (d < 0) return false;

    surf.t = (-b - sqrt(d)) / 2 * a;

    if (surf.t > t_min && surf.t < t_max)
    {
        surf.hit = true;
        surf.hitPoint = ray.origin + surf.t * ray.direction;
        surf.normal = normalize(surf.hitPoint - center);
        surf.m_ptr = m_ptr;
        return true;
    }

    surf.t = (-b + sqrt(d)) / 2 * a;
    if (surf.t > t_min && surf.t < t_max)
    {
        surf.hit = true;
        surf.hitPoint = ray.origin + surf.t * ray.direction;
        surf.normal = normalize(surf.hitPoint - center);
        // мы пересчитываем нормаль для новой поверхности 
        surf.m_ptr = m_ptr;
        return true;
    }

    return false;
}

bool Triangle::Intersect(const Ray& ray, float tmin, float tmax, SurfHit& surf) const
{
    float3 E1 = b - a; // вектор одной стороны
    float3 E2 = c - a; // вектор второй стороны
    float3 T = ray.origin - a; // a - точка, которую мы перенесли в новый центр координат (барицентрические координаты)
    float3 D = ray.direction; // вектор скорости
    float3 P = cross(D, E2);
    float3 Q = cross(T, E1);
    float det = dot(E1, P); // время

    if (det < tmin && det > tmax) {
        return false;
    }

    float invDet = 1 / det;

    // барицентрические координаты u и v должны удовлетворять условиям (оба больше 0, но сумма меньше 1)
    float u = dot(T, P) * invDet; // находим из формулы 

    if (u < 0 || u > 1) {
        return false;
    }

    float v = dot(ray.direction, Q) * invDet;

    if (v < 0 || u + v > 1) {
        return false;
    }

    surf.t = dot(E2, Q) * invDet;
    if (surf.t > tmin && surf.t < tmax) {
        surf.hit = true;
        surf.hitPoint = float3(surf.t, u, v);
        // находим точку пересечения
        surf.normal = cross(b - a, c - a);
        // находим нормаль через перемножение векторов плоскости
        surf.m_ptr = m_ptr;
        return true;
    }
    return false;
}

// Выровнен по осям координат поэтому строим по 2 точкам
bool Parallel::Intersect(const Ray& ray, float tmin, float tmax, SurfHit& surf) const
{

    // Противоположные грани рассматриваемого прямоугольного параллелепипеда лежат в плоскостях, 
    // паралелльных координатным плоскостям
    // находим для каждой координаты t (координаты вершин параллелепипеда - координаты источника луча / координаты вектора времени)

    float x1 = (t_min.x - ray.origin.x) / ray.direction.x;
    float x2 = (t_max.x - ray.origin.x) / ray.direction.x;
    float y1 = (t_min.y - ray.origin.y) / ray.direction.y;
    float y2 = (t_max.y - ray.origin.y) / ray.direction.y;
    float z1 = (t_min.z - ray.origin.z) / ray.direction.z;
    float z2 = (t_max.z - ray.origin.z) / ray.direction.z;


    // Если оба параметра отрицательны - значит, луч не пересекает эту пару плоскостей, т.е. не пересекает и параллелепипед.
    // берем максимальное значение по осям для ближнего расстояния(tmin) и минимальное для дальнего(tmax).

    float tMin = max(max(min(x1, x2), min(y1, y2)), min(z1, z2)); // находим 
    float tMax = min(min(max(x1, x2), max(y1, y2)), max(z1, z2));


    surf.t = tMin;

    // Пересечение с кубоидом существует, если tmin <= tmax и tmax > 0

    if (tMin < tMax && tMax > 0 && surf.t > tmin && surf.t < tmax) {
        surf.hit = true;
        surf.hitPoint = ray.origin + surf.t * ray.direction;
        surf.normal = normalize(surf.hitPoint);
        surf.m_ptr = m_ptr;
        return true;
    }
    return false;
}

bool Square::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const
{
    float3 d = float3(a.x + c.x - b.x, a.y + c.y - b.y, a.z + a.z - b.z); // четвертая точка квадрата


    // задается через два треугольника, в которые непосредственно и передаем нужный нам цвет

    if (Triangle(a, b, c, new IdealMirror((float3(199 / float(255), 21 / float(255), 133 / float(255))))).Intersect(ray, t_min, t_max, surf))
        return true;

    if (Triangle(a, d, c, new IdealMirror((float3(199 / float(255), 21 / float(255), 133 / float(255))))).Intersect(ray, t_min, t_max, surf))
        return true;


    return false;
}