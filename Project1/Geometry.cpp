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

    float3 k = ray.origin - center; // 

    // расчет коэффициентов для уравнения

    float a = dot(ray.direction, ray.direction);
    // берем квадрат вектора скорости
    float b = dot(2 * k, ray.direction);
    // 2 * (точка испускания луча - центр сферы) * вектор скорости
    float c = dot(k, k) - radius* radius;
    // разность точки испускания и центра - квадрат радиуса

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return false;

   
     auto root1 = (-b - sqrt(discriminant)) / 2 * a; // первый корень
     auto root2 = (-b + sqrt(discriminant)) / 2 * a; // второй корень 

    if (root1 > t_min && root1 < t_max) //проверка границ и присвоение модифицированных значений
    {
        surf.t = root1;
        surf.hit = true;
        surf.hitPoint = ray.origin + surf.t * ray.direction;
        // расчет точки пересечения по уравнению вектора (луча)
        surf.normal = normalize(surf.hitPoint - center);
        // мы пересчитываем нормаль для новой поверхности 
        surf.m_ptr = m_ptr;
        return true;
    } 
    else if (root2 > t_min && root2 < t_max) //проверка границ и присвоение модифицированных значений 
    {
        surf.t = root2;
        surf.hit = true;
        surf.hitPoint = ray.origin + surf.t * ray.direction;
        // расчет точки пересечения по уравнению вектора (луча)
        surf.normal = normalize(surf.hitPoint - center);
        // мы пересчитываем нормаль для новой поверхности 
        surf.m_ptr = m_ptr;
        return true;
    }

    return false;
}


bool Triangle::Intersect(const Ray& ray, float tmin, float tmax, SurfHit& surf) const
{
    //float3 v0v1 = b - a;
    //float3 v0v2 = c - a;
    ////float kEpsilon = 1e-8;


    //float3 pvec = cross(ray.direction,v0v2);
    //float det = dot(v0v1,pvec);

    ////// if the determinant is negative the triangle is backfacing
    ////// if the determinant is close to 0, the ray misses the triangle
    //if (det < tmin) return false;

    ////// ray and triangle are parallel if det is close to 0
    //if (fabs(det) < tmin) return false;

    //float invDet = 1 / det;

    //float3 tvec = ray.origin - a;
    //auto u = dot(tvec,pvec) * invDet;
    //if (u < 0 || u > 1) return false;

    //float3 qvec = cross(tvec,v0v1);
    //float v = dot(ray.direction, qvec) * invDet;
    //if (v < 0 || u + v > 1) return false;

    //float t = dot(v0v2,qvec) * invDet;

    //surf.t = t;

    float3 E1 = b - a; // вектор одной стороны
    float3 E2 = c - a; // вектор второй стороны
    float3 T = ray.origin - a; // a - точка, которую мы перенесли в новый центр координат (барицентрические координаты)
    float3 D = ray.direction; // вектор скорости
    float3 P1 = cross(D, E2);
    float3 Q = cross(T, E1);
    float det = dot(E1, P1); // время
    
    if (det < tmin && det > tmax) {
        return false;
    }
    
    float invDet = 1 / det;
    // барицентрические координаты u и v должны удовлетворять условиям (оба больше 0, но сумма меньше 1)
    // расчитываем три параметра из матрицы
    float u = dot(T, P1) * invDet; // находим из формулы 
    float v = dot(ray.direction, Q) * invDet;
    surf.t = dot(E2, Q) * invDet;
    if ((u < 0) || (v < 0 || u + v > 1)) {
        return false;
    }

    if (surf.t > tmin && surf.t < tmax) { //проверка границ и присвоение модифицированных значений 
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

bool Square::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const
{
    float3 d = float3(a.x + c.x - b.x, a.y + c.y - b.y, a.z + c.z - b.z); // четвертая точка квадрата


    // задается через два треугольника, в которые непосредственно и передаем нужный нам цвет

    if (Triangle(a, b, c, new IdealMirror(float3(0.0f, 1.0f, 127 / float(255)))).Intersect(ray, t_min, t_max, surf))
        return true;

    if (Triangle(a, d, c, new IdealMirror(float3(0.0f, 1.0f, 127 / float(255)))).Intersect(ray, t_min, t_max, surf))
        return true;

       return false;
}

//bool Square::Intersect(const Ray& ray, float tmin, float tmax, SurfHit& surf) const
//{
//    if (!Plane::Intersect(ray, tmin, tmax, surf))
//        return false;
//
//    float u = (surf.hitPoint.x - point.x) / l;
//    float v = (surf.hitPoint.y - point.y) / l;
//    return !(u < 0 || v < 0 || u > 1 || v > 1);
//}

// Выровнен по осям координат поэтому строим по 2 точкам
bool Parallel::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const
{

    // Противоположные грани рассматриваемого прямоугольного параллелепипеда лежат в плоскостях, 
    // паралелльных координатным плоскостям
    // находим для каждой координаты t (координаты вершин параллелепипеда - координаты источника луча / координаты вектора времени)

    float x1 = (a.x - ray.origin.x) / ray.direction.x;
    float x2 = (b.x - ray.origin.x) / ray.direction.x;
    float y1 = (a.y - ray.origin.y) / ray.direction.y;
    float y2 = (b.y - ray.origin.y) / ray.direction.y;
    float z1 = (a.z - ray.origin.z) / ray.direction.z;
    float z2 = (b.z - ray.origin.z) / ray.direction.z;

    //if ((x1 > y2) || (y1 > x2))
    //    return false;
    //if (y1 > x1)
    //    x1 = y1;
    //if (y2 < x2)
    //    x2 = y2;

    //if ((x1 > z2) || (z1 > x2))
    //    return false;
    //if (z1 > x1)
    //    x1 = z1;
    //if (z2 < x2)
    //    x2 = z2;

    // Если оба параметра отрицательны - значит, луч не пересекает эту пару плоскостей, т.е. не пересекает и параллелепипед.
    // берем максимальное значение по осям для ближнего расстояния(tmin) и минимальное для дальнего(tmax).

     float tMin = max(max(min(x1, x2), min(y1, y2)), min(z1, z2));
     float tMax = min(min(max(x1, x2), max(y1, y2)), max(z1, z2));


     surf.t = tMin;

    // Пересечение с кубоидом существует, если tmin <= tmax и tmax > 0

    if (((tMin < tMax) && (tMax >= 0)) && (surf.t > t_min && surf.t < t_max)){ //проверка границ и присвоение модифицированных значений 
        surf.hit = true;
        surf.hitPoint = ray.origin + surf.t * ray.direction;
        surf.normal = normalize(surf.hitPoint);
        surf.m_ptr = m_ptr;
        return true;
    }
    return false;
}
