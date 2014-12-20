#include "Tracer.h"
#include "EasyBMP/EasyBMP.h"
#include "math.h"
using namespace glm;

#define d2r(x) \
    (x / 180.0f * M_PI)

SRay CTracer::MakeRay(glm::uvec2 pixelPos)
{
    SRay ray;
    ray.m_start = m_camera.m_pos;
    //glm::vec3
    double al = m_camera.m_viewAngle[0];
    double lw = m_camera.m_resolution[0] / 2.0f * 1.0f / std::tan(d2r(al) / 2.0f);
    int xpos = pixelPos[0] - m_camera.m_resolution[0] / 2;
    int ypos = pixelPos[1] - m_camera.m_resolution[1] / 2;
    glm::vec3 tr = m_camera.m_forward * (float) lw  - m_camera.m_right * (float) xpos + m_camera.m_up * (float) ypos;
    tr = glm::normalize(tr);
    ray.m_dir = tr;
    //std::cout << tr.x << ' ' << tr.y << ' ' << tr.z << '\n';
    return ray;
}

glm::vec3 CTracer::TraceRay(SRay ray)
{
    vec3 color = m_pScene->IntersectRay(ray);
    return color;
}

void CTracer::RenderImage(int xRes, int yRes)
{
    m_camera.m_resolution = uvec2(xRes, yRes);
    m_camera.m_pixels.resize(xRes * yRes);
    m_pScene->setupScene();


    int i, j;
    int cnt = yRes / 80;
    cnt = 80 / cnt;
#pragma omp parallel
    {
#pragma omp for firstprivate(j) lastprivate(i)
        for(i = 0; i < yRes; i++) {
            for(j = 0; j < xRes; j++)
            {
                SRay ray = MakeRay(uvec2(j, i));
                // fprintf(stderr, "Tracing Ray for Pixel %d %d\n", j, i);
                //fprintf(stderr, "Tracing Ray %d of %d\n", i * xRes + j, xRes * yRes);
                m_camera.m_pixels[i * xRes + j] = TraceRay(ray);
            }
            if (i % (yRes / cnt)) {
                fprintf(stderr, "-");
            }
        }
#pragma omp barrier
    }
}

void CTracer::SaveImageToFile(std::string fileName)
{

    int width = m_camera.m_resolution.x;
    int height = m_camera.m_resolution.y;

    BMP file;
    file.SetSize(width, height);


    int textureDisplacement = 0;
    //#pragma omp parallel for
    for (int i = height - 1; i >= 0; i--) {
    //for (int i = 0; i < height - 1; i++) {
        for (int j = 0; j < width; j++) {
            vec3 color = m_camera.m_pixels[textureDisplacement + j];
            RGBApixel pxl;
            pxl.Red = static_cast<unsigned>(color[0] * 255.0);
            pxl.Green = static_cast<unsigned>(color[1] * 255.0);
            pxl.Blue = static_cast<unsigned>(color[2] * 255.0);
            file.SetPixel(j, i, pxl);

        }
        textureDisplacement += width;
    }
    file.WriteToFile(fileName.c_str());
}
