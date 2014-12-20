#ifndef TEXTURE_H
#define TEXTURE_H
#include "EasyBMP/EasyBMP.h"
#include <glm/glm.hpp>
class Texture
{
    BMP file;
    int m_w, m_h;
public:
    Texture(const char * fname);
    glm::vec3 getPixel(float i, float j);
};

#endif // TEXTURE_H
