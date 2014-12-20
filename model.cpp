#include "model.h"
#include "Scene.h"
#include "Types.h"



//bool Mesh::IntersectRay(const SRay &ray, glm::vec3 out_point, glm::vec2 &out_tex)


glm::vec3 Model::Phong(glm::vec3 pos, glm::vec3 normal, glm::vec3 lpos, glm::vec3 epos, glm::vec3 ps_color)
{

    glm::vec3 n = glm::normalize(normal);
    glm::vec3 v = glm::normalize(epos - pos);
    glm::vec3 l = glm::normalize(lpos - pos);

    float rv  = 2 * glm::dot(n, v) * glm::dot(n, l) - glm::dot(l, v);
    rv = std::max(rv, 0.0f);


    glm::vec3 diffc = ps_color;
    glm::vec3 specc = 0.8f * glm::vec3(1, 1, 1);
    float id, is, sp_power;
    switch (m_type) {
    case REFLECT_ALL:
        id = 0.0f;
        is = 1.0f;
        sp_power = 7.0f;
        break;
    case DIFFUSE_MODEL:
        id = 1.0f;
        is = 0.0f;
        sp_power = 0.0f;
        break;
    case FRENEL_MODEL:
        id = 0.6;
        is = 0.4f;
        sp_power = 7.0f;
        break;
    }

    //glm::vec3 ia = 0.003 * glm::vec3(0.01, 0.01, 0.01);
    glm::vec3 ia = glm::vec3(0, 0 ,0.0f);
    float tmp = glm::dot(n, l);

    glm::vec3 res = ia + id * diffc * std::max(glm::dot(n, l), 0.0f)
            + is * specc * std::max(std::pow(rv, sp_power), 0.0f);
    return res;
}

inline glm::vec3 fromVec4(glm::vec4 x) {
    return glm::vec3(x[0] / x[3], x[1] / x[3], x[2] / x[3]);
}

inline glm::vec3 ApplyMat4(const glm::mat4 &T, const glm::vec3 &x) {
    glm::vec4 p = glm::vec4(x[0], x[1], x[2], 1);
    return fromVec4(T * p);
}

SRay Model::convert_ray(const SRay &m_ray)
{
    SRay my_ray;
    my_ray.level_id = m_ray.level_id;
    my_ray.m_dir = ApplyMat4(Tdirinv, m_ray.m_dir);
    my_ray.m_start = ApplyMat4(Tinv , m_ray.m_start);
    return my_ray;
}


glm::vec3 shade_coeff = glm::vec3(0.3, 0.3, 0.3);

glm::vec3 Model::getColor(const SRay &m_ray, glm::vec3 out_point, glm::vec3 normal, glm::vec2 mintex)
{
    SRay my_ray = convert_ray(m_ray);

    glm::vec3 light_pos = m_scene.get_LigthPos();
    light_pos = ApplyMat4(T, light_pos);

    glm::vec3 tmnormal = ApplyMat4(Tdir, normal);
    if (glm::dot(tmnormal, light_pos - out_point) < 0) {
        normal = -normal;
    }

    glm::vec3  loc_pos = ApplyMat4(Tinv, out_point);

    glm::vec3 ps_color = m_textures[0]->getPixel(mintex[0], mintex[1]);
    glm::vec3 mycolor = Phong(loc_pos, normal, light_pos, my_ray.m_start, ps_color);


    /*float cos_psi = glm::dot(glm::normalize(light_pos - dmin), glm::normalize(normal));
    mycolor = ps_color * cos_psi;
    if (cos_psi < 0) {
        mycolor = shade_coeff;
    }*/
    //glm::vec3 mycolor = ps_color;


    float xa = 1.0f;
    float ya = coeff_spec;
    if ( m_scene.LightTrace(out_point)) {
        //fprintf(stderr, "Can see light\n");
        xa = coeff_diff;

    } else {
        mycolor = shade_coeff;
    }

    glm::vec3 specc = glm::vec3(0, 0, 0);
    if (m_type != DIFFUSE_MODEL && m_ray.level_id < MAX_DEPTH) {
        SRay ray2;
        ray2.level_id = m_ray.level_id + 1;
        ray2.m_start = out_point;
        glm::vec3 tnorm = ApplyMat4(Tdir, normal);
        ray2.m_dir = glm::reflect(glm::normalize(out_point - m_ray.m_start),
                                  tnorm);

        specc = m_scene.TraceRay(ray2);
    }
    glm::vec3 res = xa * mycolor + ya * specc;
    return res;
}

bool Model::IntersectRay(const SRay &m_ray, glm::vec3 &out_point, glm::vec3 &out_norm, glm::vec2 &texcoord,
                         glm::vec3 &coutput, bool intersection_only)
{
    glm::vec3 dmin = glm::vec3(BIG_NUM, BIG_NUM, BIG_NUM);
    glm::vec2 mintex;
    glm::vec3 normal;
    /*SRay my_ray;
    my_ray.level_id = m_ray.level_id;
    my_ray.m_dir = ApplyMat4(Tdirinv, m_ray.m_dir);
    my_ray.m_start = ApplyMat4(Tinv , m_ray.m_start);
    */
    SRay my_ray = convert_ray(m_ray);

    int id = -1;
    for (unsigned i = 0; i < m_meshes.size(); i++) {
        glm::vec3 res;
        glm::vec2 tex;
        glm::vec3 norm_vec;
        //
        if (m_meshes[i]->IntersectRay(my_ray, res, norm_vec, tex)) {
            if (VLess(res - m_ray.m_start, dmin - m_ray.m_start)) {
                dmin = res;
                mintex = tex;
                normal = norm_vec;
                id = i;
            }
        }
    }
    if (id == -1) {
        return false;
    }
    if (intersection_only) {
        return true;
    }
    out_point = ApplyMat4(T, dmin);
    out_norm = ApplyMat4(Tdir, normal);
    texcoord = mintex;



//    glm::vec3 light_pos = m_scene.get_LigthPos();
//    glm::vec3 tmnormal = ApplyMat4(Tdir, normal);
//    if (glm::dot(tmnormal, light_pos - out_point) < 0) {
//        normal = -normal;
//    }


//    light_pos = ApplyMat4(T, light_pos);

//    glm::vec3 ps_color = m_textures[id]->getPixel(mintex[0], mintex[1]);
//    glm::vec3 mycolor = Phong(dmin, normal, light_pos, my_ray.m_start, ps_color);


//    /*float cos_psi = glm::dot(glm::normalize(light_pos - dmin), glm::normalize(normal));
//    mycolor = ps_color * cos_psi;
//    if (cos_psi < 0) {
//        mycolor = shade_coeff;
//    }*/
//    //glm::vec3 mycolor = ps_color;


//    float xa = 1.0f;
//    float ya = coeff_spec;
//    if ( m_scene.LightTrace(out_point)) {
//        //fprintf(stderr, "Can see light\n");
//        xa = coeff_diff;

//    }

//    glm::vec3 specc = glm::vec3(0, 0, 0);
//    if (m_type != DIFFUSE_MODEL && m_ray.level_id < MAX_DEPTH) {
//        SRay ray2;
//        ray2.level_id = m_ray.level_id + 1;
//        ray2.m_start = out_point;
//        glm::vec3 tnorm = ApplyMat4(Tdir, normal);
//        ray2.m_dir = glm::reflect(glm::normalize(out_point - m_ray.m_start),
//                                  tnorm);

//        specc = m_scene.TraceRay(ray2);
//    }
//    coutput = xa * mycolor + ya * specc;
    return true;
}


void Model::fix_vert()
{
    return;
    if (m_meshes.size() == 1 && m_meshes[0]->isPlane()) {
        return;
    }

    glm::vec3 vmin = glm::vec3(BIG_NUM, BIG_NUM, BIG_NUM),
            vmax = glm::vec3(0, 0, 0);
    for (unsigned i = 0; i < m_meshes.size(); i++) {
        glm::vec3 lvmin, lvmax;
        m_meshes[i]->getMinMax(lvmin, lvmax);
        if (VLess(lvmin, vmin)) {
            vmin = lvmin;
        }
        if (VLess(vmax, lvmax)) {
            vmax = lvmax;
        }
    }

    glm::vec3 tmpv = 0.5f * (vmax - vmin);
    for (unsigned i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->ReTrans(tmpv);
    }

    for (unsigned i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->ReScale( 1.0f / glm::length(vmax - vmin));
       // m_meshes[i]->ReScale(0.9f);
    }


}







