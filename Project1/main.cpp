#include <iostream>
#include <vector>
#include <random>

#include "LiteMath.h"
#include "Geometry.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION

using namespace HydraLiteMath;


std::vector< std::shared_ptr<GeoObject>> scene()
{
    std::vector<std::shared_ptr<GeoObject>> myScene;
    auto plane1 = std::make_shared<Plane>(float3(+0.0f, -1.0f, +0.0f), float3(0.0f, 1.0f, 0.0f), new IdealMirror(float3(0.3f, 0.3f, 0.3f)));
    myScene.push_back(plane1);


    auto sphere = std::make_shared<Sphere>(float3(-1.5f, +2.5f, +5.0f), float(1.5f), new Diffuse((float3(float(30) / float(255), float(144) / float(255), float(255) / float(255)))));
    auto parallel = std::make_shared<Parallel>(float3(-10.0f, +0.0f, -2.0f), float3(-5.0f, +7.0f, +5.0f), new Diffuse(float3(0.0f, 0.0f, float(128)/ float(255))));
    auto tr = std::make_shared<Triangle>(float3(+10.0f, +0.0f, +10.0f), float3(10.0f, +10.0f, 0.0f), float3(+10.0f, +0.0f, -10.0f), new Diffuse(float3(255 / float(255), 255 / float(255), 0 / float(255))));
    auto square = std::make_shared<Square>(float3(2.5f, +2.5f, +6.0f), float3(2.5f, +6.0f, 6.0f), float3(+6.0f, +6.0f, 6.0f));
    //auto square = std::make_shared<Square>(float3(+1.0f, +0.0f, +2.0f), float(6.0f), new IdealMirror(float3(255 / float(255), 191 / float(255), 255 / float(255))));
    myScene.push_back(square);
    myScene.push_back(tr);
    myScene.push_back(sphere);
    myScene.push_back(parallel);

    return myScene;
}

std::vector< std::shared_ptr<Point_light>> light()
{
    std::vector<std::shared_ptr<Point_light>> mylight;
   auto light1 = std::make_shared<Point_light>(float3(-1.0f, 12.0f, 4.0f), float3(+5.0f, +6.0f, 2.0f));
    auto light2 = std::make_shared<Point_light>(float3(5.0f, 12.0f, 5.0f), float3(5.0f, 6.0f, 2.0f));
    mylight.push_back(light1);
    mylight.push_back(light2);

    return mylight;
}

void RenderScene(uint32_t w, uint32_t h, uint32_t num_samples, const Camera& cam, const std::string& filename)
{
    auto  background_color = float3(0.3f, 0.1f, 0.5f);
    auto  film = std::make_unique<Film>(w, h, num_samples);
    auto  tracer = std::make_unique<SimpleRT>(16, background_color);
    float invWidth = 1.0f / float(w);
    float invHeight = 1.0f / float(h);

    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            float3 pixel_color = float3(0.0f, 0.0f, 0.0f);

            for (int s = 0; s < num_samples; ++s)
            {
                Ray ray = cam.genRay(w, h, x, h - y); //генерируем луч из камеры через текущий пиксель
                pixel_color += tracer->WhittedTraceRay(ray, scene(), light(), 0); //трассируем луч и получаем цвет
            }
            pixel_color /= film->num_samples;      // усредняем полученные цвета
            pixel_color *= cam.getExposureTime();  // умножаем на время экспонирования сенсора - выдержка виртуальной камеры
            film->SetPixelColor(x, y, pixel_color); //записываем цвет на виртуальный сенсор
        }
    }
    film->SaveImagePPM(filename); //сохраняем картинку
}

void create_scene()
{
    float3        eye(0.0f, 2.0f, 20.0f);
    float3        lookat(0.0f, 2.0f, 0.0f);
    float3        up(0.0f, 1.0f, 0.0f);
    float         field_of_view = 90.0f;
    unsigned int  w = 640;
    unsigned int  h = 480;
    Camera        cam(eye, lookat, up, field_of_view, float(w) / float(h));



    RenderScene(w, h, 1, cam, "basic_scene");
}



int main()
{
  create_scene();

  return 0;
}
