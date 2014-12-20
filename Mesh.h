#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <tuple>
#include <string>
#include "Types.h"
typedef std::tuple<int, int, int, glm::vec3> gTriag;

struct Sphere
{
    glm::vec3 center;
    float r;
    bool IntersectRay(const SRay &ray);
    int IntersectRay(const SRay &ray, glm::vec3 &out_point);
};


class Mesh
{
    Sphere m_sphr;
    std::vector<glm::vec3> vertices;
    std::vector<gTriag> indices;
    std::vector<glm::vec2> texcoord;
    std::vector<glm::vec3> vert_normals;
    void Fixtex();
public:
    virtual bool isPlane() {
        return false;
    }
    void getMinMax(glm::vec3 &vmin, glm::vec3 &vmax);

    //void FixVert();
    void ReScale(float val);
    void ReTrans(glm::vec3 x);
    friend std::vector<Mesh *> getMeshes(std::string &);
    virtual bool IntersectRay(const SRay &ray, glm::vec3 &out_point, glm::vec3 &out_norm, glm::vec2 &out_tex);
};

class Mplane: public Mesh
{
    Triangle m_triag1, m_triag2;
    float m_w, m_h;

public:
    Mplane(Triangle tr1, Triangle tr2, float w, float h):
        m_triag1(tr1),
        m_triag2(tr2),
        m_w(w),
        m_h(h)
    {}

    bool isPlane() override{
        return true;
    }
    bool IntersectRay(const SRay &ray, glm::vec3 &out_point, glm::vec3 &out_norm, glm::vec2 &out_tex) override;
};
std::vector<Mesh *> getMeshes(std::string &);




