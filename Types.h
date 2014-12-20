#pragma once

#include "glm/glm.hpp"
#include "vector"

struct SRay
{
  glm::vec3 m_start;
  glm::vec3 m_dir;
  int level_id;
  SRay() {
      level_id = 0;
  }
};

struct SCamera
{
  glm::vec3 m_pos;          // Camera position and orientation
  glm::vec3 m_forward;      // Orthonormal basis
  glm::vec3 m_right;
  glm::vec3 m_up;

  glm::vec2 m_viewAngle;    // View angles, rad
  glm::uvec2 m_resolution;  // Image resolution: w, h

  std::vector<glm::vec3> m_pixels;  // Pixel array

  SCamera():
      m_pos(glm::vec3(0, 0, 20)),
      m_forward(glm::vec3(0, 0, -1)),
      m_right(glm::vec3(1, 0, 0)),
      m_up(glm::vec3(0, 1, 0)),
        m_viewAngle(glm::vec2(60, 60))
  {}
};

struct SMesh
{
  std::vector<glm::vec3> m_vertices;  // vertex positions
  std::vector<glm::uvec3> m_triangles;  // vetrex indices
};
const float BIG_NUM = 100500.0f;

class Mplane;
class Triangle
{
    friend class Mplane;
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::vec3 na;
    glm::vec3 nb;
    glm::vec3 nc;

public:
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
             glm::vec3 nv0 = glm::vec3(0, 0, 0),
             glm::vec3 nv1 = glm::vec3(0, 0, 0),
             glm::vec3 nv2 = glm::vec3(0, 0, 0))
        :a(v0),
          b(v1),
          c(v2),
          na(nv0),
          nb(nv1),
          nc(nv2)
    {

    }

    glm::vec3 getNormal() {
        return glm::cross(b - a,  c - a);
    }
    bool RayIntersection(const SRay &ray, glm::vec3 &point, glm::vec3 &out_norm);
    int IntersectRay(const SRay &ray, glm::vec3& out_point, glm::vec3 &out_norm);

};

bool  VLess(const glm::vec3 &a, const glm::vec3 &b);

bool  VLess(const glm::vec2 &a, const glm::vec2 &b);

glm::vec3  VMin(const glm::vec3 &a, const glm::vec3 &b);
glm::vec2  VMin(const glm::vec2 &a, const glm::vec2 &b);

const double M_EPS = 1e-5;
const int MAX_DEPTH = 3;
