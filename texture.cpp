#include "texture.h"

Texture::Texture(const char *fname)
{
    file.ReadFromFile(fname);
    m_w = file.TellWidth();
    m_h = file.TellHeight();
}

glm::vec3 Texture::getPixel(float i, float j)
{
    int cur_x = std::roundf(i * m_h);
    int cur_y = std::roundf(j * m_w);
    if (cur_x >= m_h) {
        cur_x = m_h - 1;
    }

    if (cur_y >= m_w) {
        cur_y = m_w - 1;
    }
    if (cur_x < 0) {
        cur_x = 0;
    }

    if (cur_y  < 0) {
        cur_y = 0;
    }


    glm::vec3 res;
    if (cur_x < m_h - 1 && cur_y < m_w - 1) {
        RGBApixel pxl[4];
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                pxl[i * 2 + j] = file.GetPixel(cur_y + j, cur_x + i);
            }
        }
        unsigned r = 0, g = 0, b = 0;
        for (int i = 0; i < 4; i++) {
            r += pxl[i].Red;
            g += pxl[i].Green;
            b += pxl[i].Blue;
        }
        float big_const = 255.0f * 4.0f;
        res = glm::vec3(r / big_const , g / big_const, b / big_const);
    } else {
        RGBApixel pxl = file.GetPixel(cur_y, cur_x);
        res = glm::vec3(pxl.Red / 255.0f, pxl.Green / 255.0f, pxl.Blue / 255.0f);
    }
    return res;
}
