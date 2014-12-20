#include "l3ds.h"
#include "Mesh.h"
#include <cmath>
using std::tie;
glm::vec3 ToGlm(LVector4 t){
    glm::vec3 res;
    res = glm::vec3(t.x / t.w,  t.z / t.w, t.y / t.w);
    return res;
}

glm::vec3 ToGlm(LVector3 t) {
    glm::vec3 res;
    res = glm::vec3(t.x, t.z, t.y);
    return res;
}

glm::vec2 ToGlm(LVector2 t) {
    glm::vec2 res;
    res = glm::vec2(t.x, t.y);
    return res;

}





bool Sphere::IntersectRay(const SRay & ray)
{
    glm::vec3 vel = center - ray.m_start;
    //vel = glm::normalize(vel);
    float cosal = glm::dot(glm::normalize(ray.m_dir), glm::normalize(vel));

    if (cosal < 0) {
        return false;
    }


    float al = std::acos(cosal);
    float x = glm::length(vel) * std::sin(al);


    if (glm::length(vel) <= r) {
        return false;
    }


    return (x < r);
}



int Sphere::IntersectRay(const SRay &ray, glm::vec3& out_point)
{
  /**/
  //TODO: неизвестный баг: возможна ситуация, в которой будет невозможно прочитать память
  // по указателю this???, решение - возвращать 0 по умолчанию
  glm::vec3 rad_vect = center - ray.m_start;
  float rad_len = glm::length(rad_vect);
  float cos = glm::dot(rad_vect, ray.m_dir) / (rad_len * glm::length(ray.m_dir));
  if (rad_len > r && cos < 0)
    return 0;
  float ab = rad_len * cos;
  float ob2 = rad_len * rad_len - ab * ab;
  float bc = glm::sqrt(r * r - ob2);
  if (rad_len > r + 0.1){ // отделение внешних точек от внутренних и граничных
    if (ob2 <= r * r){
      out_point = ray.m_start + glm::normalize(ray.m_dir) * (ab - bc);
      return 1;
    } else
      return 0;
  }
  if (glm::abs(ab + bc) >= 0.1){
    out_point = ray.m_start + glm::normalize(ray.m_dir) * (ab + bc);
    return 1;
  }
  return 0;
}

void Mesh::Fixtex()
{
    //return;
    glm::vec2 maxtex = glm::vec2(0,0);
    glm::vec2 mintex = glm::vec2(BIG_NUM, BIG_NUM);
    for (unsigned i = 0; i < texcoord.size(); i++) {
        if (VLess(maxtex, texcoord[i])) {
            maxtex = texcoord[i];
        }
        if (VLess(texcoord[i], mintex)) {
            mintex = texcoord[i];
        }
    }
    float p_len = glm::length(maxtex - mintex);
    if (std::abs(p_len) < M_EPS) {
        return;
    }
    for (unsigned i = 0; i < texcoord.size(); i++) {
        texcoord[i] = (maxtex - texcoord[i]) / p_len;
    }
}

void Mesh::getMinMax(glm::vec3 &vmin, glm::vec3 &vmax) {
    vmin = glm::vec3(BIG_NUM, BIG_NUM, BIG_NUM);
    vmax = glm::vec3(0, 0, 0);
    for (unsigned i = 0; i < indices.size(); i++) {
        int ind[3]; glm::vec3 gb;
        std::tie(ind[0], ind[1], ind[2], gb) = indices[i];
        for (int k = 0; k < 3; k++) {
            if (VLess(vertices[ind[k]], vmin)) {
                vmin = vertices[ind[k]];
            }
            if (VLess(vmax, vertices[ind[k]])) {
                vmax = vertices[ind[k]];
            }
        }
    }
}

void Mesh::ReScale(float val) {
    if (isPlane()) return;
    for (unsigned i = 0; i < vertices.size(); i++) {
        vertices[i] = val * vertices[i];
    }
    m_sphr.r *= val;
}

void Mesh::ReTrans(glm::vec3 x) {
    if (isPlane()) return;
    for (unsigned i = 0; i < vertices.size(); i++) {
        vertices[i] = x + vertices[i];
    }
    m_sphr.center += x;
}



bool Mesh::IntersectRay(const SRay &ray, glm::vec3 &out_point, glm::vec3 &out_norm, glm::vec2 &out_tex)
{
    glm::vec3 tmpx;
    if (!m_sphr.IntersectRay(ray)) {
        return false;
    }

    //fprintf(stderr, "MESH SPHERE INTERSECTED\n");
    /*out_point = m_sphr.center + m_sphr.r * glm::normalize(ray.m_dir);
    out_norm = glm::cross(out_point, m_sphr.center);
    out_tex = glm::vec2(0.5, 0.5);
    return true;*/
    glm::vec3 res = glm::vec3(BIG_NUM, BIG_NUM, BIG_NUM);
    int save_id = -1;
    glm::vec3 norm_vec;
    for (unsigned i = 0; i < indices.size(); i++) {
        gTriag  tr = indices[i];
        int id0, id1, id2; glm::vec3 normal;
        tie(id0, id1, id2, normal) = tr;
        Triangle cur_tr(vertices[id0], vertices[id1], vertices[id2],
                        vert_normals[id0], vert_normals[id1], vert_normals[id2]);
        glm::vec3 tmp, tmp2;
        if (cur_tr.IntersectRay(ray, tmp, tmp2)) {
            if (glm::length(tmp - ray.m_start) < glm::length(res - ray.m_start)) {
                res = tmp;
                save_id = i;
                norm_vec = tmp2;
            }
        }
    }
    if (save_id == -1) {
        return false;
    }
    out_point = res;
    out_tex = texcoord[save_id];
    out_norm = norm_vec;
    //fprintf(stderr, "Mesh intersected\n");
    return true;
}





std::vector<Mesh *> getMeshes(std::string &fname)
{
    L3DS *importer = new L3DS(fname.c_str());
    int cnt = importer->GetMeshCount();
    importer->SetOptimizationLevel(oFull);
    std::vector<Mesh *> resv;
    for (int i = 0; i < cnt; i++) {
        LMesh t_mesh = importer->GetMesh(i);
        int tr_cnt = t_mesh.GetTriangleCount();
        fprintf(stderr, "Model polygons %d\n", tr_cnt);

        Mesh *current_mesh = new  Mesh();
        Sphere sphr;
        glm::vec3 vert_min = glm::vec3(BIG_NUM, BIG_NUM, BIG_NUM), vert_max = glm::vec3(0, 0, 0);
        for (int j = 0; j < tr_cnt; j++) {
            gTriag tmp;
            int vec3[3];
            LTriangle2 tmptr = t_mesh.GetTriangle2(j);
            for (int k = 0; k < 3; k++) {
                if (VLess(ToGlm(tmptr.vertices[k]), vert_min)) {
                    vert_min = ToGlm(tmptr.vertices[k]);
                }
                if (VLess(vert_max, ToGlm(tmptr.vertices[k]))) {
                    vert_max = ToGlm(tmptr.vertices[k]);
                }
                current_mesh->vertices.push_back(ToGlm(tmptr.vertices[k]));
                current_mesh->vert_normals.push_back(ToGlm(tmptr.vertexNormals[k]));
                current_mesh->texcoord.push_back(ToGlm(tmptr.textureCoords[k]));
                //current_mesh->indices.push_back(current_mesh->vertices.size() - 1);
                vec3[k] = current_mesh->vertices.size() - 1;
            }
            glm::vec3 first = current_mesh->vertices[vec3[0]] - current_mesh->vertices[vec3[1]];
            glm::vec3 second = current_mesh->vertices[vec3[2]] - current_mesh->vertices[vec3[1]];
            glm::vec3 normal = glm::cross(first, second);
            normal = ToGlm(tmptr.faceNormal);
            tmp = std::make_tuple(vec3[0], vec3[1], vec3[2], normal);
            current_mesh->indices.push_back(tmp);
        }
        sphr.r = glm::length(vert_max);
        sphr.center = glm::vec3(0, 0, 0);
//        sphr.center = glm::vec3(0, 0, 0);
//        sphr.r = 1.0f;
        current_mesh->m_sphr = sphr;
        current_mesh->Fixtex();
        resv.push_back(current_mesh);
        fprintf(stderr, "VMIN == %f %f %f VMAX == %f %f %f\n", vert_min[0], vert_min[1], vert_min[2],
                vert_max[0], vert_max[1], vert_max[2]);
        fprintf(stderr, "center %f %f %f radius %f\n", sphr.center[0], sphr.center[1], sphr.center[2], sphr.r);
    }
    return resv;
}
/*
 *
 *tr1 -- tr2
 *|\---|
 *| \--|
 *|  \-|
 *-----|
 *
 *b c c
 *a a b
 */

bool Mplane::IntersectRay(const SRay &ray, glm::vec3 &out_point, glm::vec3 &out_norm, glm::vec2 &out_tex)
{
    if (!(m_triag1.IntersectRay(ray, out_point, out_norm) || m_triag2.IntersectRay(ray, out_point, out_norm))) {
        return false;
    }

//    glm::vec3 la = out_point - m_triag1.a;
//    glm::vec3 lb = out_point - m_triag1.b;

    glm::vec3 lv = out_point - m_triag1.a;
    glm::vec3 tmpa = m_triag1.b - m_triag1.a;
    glm::vec3 tmpb = m_triag2.b - m_triag1.a;


    float f1 = glm::dot(m_triag1.b - m_triag1.a, out_point - m_triag1.a);
    float f2 = glm::dot(m_triag2.b - m_triag1.a, out_point - m_triag1.a);
    f1 /= glm::length(tmpa);
    f2 /= glm::length(tmpb);

    out_tex[0] = f1 / m_h;
    out_tex[1] = f2 / m_w;
    out_norm = m_triag1.getNormal();
    return true;
}
