#pragma once
#include <vector>
#include "Types.h"
#include "model.h"
#include "texture.h"



class CScene
{
  // Set of meshes
    glm::vec3 Light_pos;
    std::vector<Model *> m_models;
    std::vector<Texture> m_textures;
    void genBox();
public:
    glm::vec3 get_LigthPos() const {
        return Light_pos;
    }

    void setupScene();
    glm::vec3 IntersectRay(const SRay &ray);
    glm::vec3 TraceRay(const SRay &ray);

    bool LightTrace(const glm::vec3 &x);
};
