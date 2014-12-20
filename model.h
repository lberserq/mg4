#ifndef MODEL_H
#define MODEL_H
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Types.h"
#include "Mesh.h"
#include "texture.h"
#include <vector>

enum M_TYPE
{
    REFLECT_ALL = 0,
    FRENEL_MODEL = 1,
    DIFFUSE_MODEL = 2
};

class CScene;
class Model
{
    std::vector<Mesh *> m_meshes;
    std::vector<Texture *> m_textures;
    M_TYPE m_type;
    float coeff_diff, coeff_spec;
    CScene &m_scene;
    glm::vec3 Tr;
    float oxz_rot;
    float oy_rot;
    glm::mat4 T;
    glm::mat4 Tinv;
    glm::mat4 Tdir;
    glm::mat4 Tdirinv;
    void gen_full() {
        glm::mat4 res;
        res = glm::translate(res, Tr);
        res = glm::rotate(res, oxz_rot, glm::vec3(0, 1, 0));
        res = glm::rotate(res, oy_rot, glm::vec3(0, 0, 1));
        //return res;
        T = res;
        Tinv = glm::inverse(T);
    }

    void gen_noTr() {
        glm::mat4 res;
        res = glm::rotate(res, oxz_rot, glm::vec3(0, 1, 0));
        res = glm::rotate(res, oy_rot, glm::vec3(0, 0, 1));
        // return res;
        Tdir = res;
        Tdirinv = glm::inverse(Tdir);
    }

    void fix_vert();
    SRay convert_ray(const SRay &ray);

    glm::vec3 Phong(glm::vec3 pos, glm::vec3 normal, glm::vec3 lpos, glm::vec3 epos, glm::vec3 ps_color);
public:
    Model(CScene &m_parent, std::vector<Mesh *> &meshes, std::vector<Texture *> &texture
          , M_TYPE type):
        m_meshes(meshes),
        m_textures(texture),
        m_type(type),
        m_scene(m_parent){
        coeff_diff = 0.5, coeff_spec = 0.5;
        switch (type) {
        case REFLECT_ALL:
            coeff_diff = 0.0f;
            coeff_spec = 1.0f;
            break;
        case FRENEL_MODEL:
            coeff_diff = 0.7;
            coeff_spec = 0.3;
            break;
        case DIFFUSE_MODEL:
            coeff_diff = 1.0f;
            coeff_spec = 0.0f;
        }
       fix_vert();
    }
    bool isPlane() {
        return (m_meshes.size() == 1 &&  m_meshes[0]->isPlane());
    }

    glm::vec3 getColor(const SRay &m_ray, glm::vec3 inter_point,  glm::vec3 normal, glm::vec2 texcoord);

    bool IntersectRay(const SRay &m_ray, glm::vec3 &out_point, glm::vec3 &out_norm, glm::vec2 &texcoord,
                      glm::vec3 &coutput,
                      bool intersection_only = false);
    void set_coeff(float dif, float spec) {
        coeff_diff = dif;
        coeff_spec = spec;
    }

    void set_place(glm::vec3 placement, float angoxz, float angy) {
        Tr =  placement;
        oxz_rot = angoxz;
        oy_rot = angy;
        gen_noTr();
        gen_full();

    }
};

#endif // MODEL_H
