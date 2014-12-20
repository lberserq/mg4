#include "Types.h"

#include <cstdio>
bool Triangle::RayIntersection(const SRay &ray, glm::vec3 &point, glm::vec3 &out_norm)
{
    glm::vec3 edge1 = b - a;
    glm::vec3 edge2 = c - a;
    glm::vec3 Tv = ray.m_start - a;
    glm::vec3 D = ray.m_dir;
    glm::vec3 P = glm::cross(D, edge2);
    glm::vec3 Q = glm::cross(Tv, edge1);
    float det = glm::dot(P, edge1);

    if (std::abs(det) < M_EPS){
        return false;
    }

    float u = glm::dot(P, Tv) / det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    float v = glm::dot(Q, D) / det;
    if (v < 0.0f || v + u > 1.0f) {
        return false;
    }

    float t = glm::dot(Q, edge2) / det;
    if (t > M_EPS) {
        point = (1 - u - v) * b + u * c + v * a;
        out_norm = (1 - u - v) * nb + u * nc + v * na;
        return true;
    }
    return false;
}


int Triangle::IntersectRay(const SRay &ray, glm::vec3& out_point, glm::vec3 &out_norm)
{
    glm::vec3 e1 = b - a;
    glm::vec3 e2 = c - a;
    glm::vec3 m_norm = glm::normalize(glm::cross(e1, e2));
    float m_d = -glm::dot(m_norm, a);
    glm::vec3 m_cent = (a + b + c) / 3.0f;
    // http://ray-tracing.ru/articles213.html
    // если точка уже в плоскости, не искать
    if (glm::abs(glm::dot(m_norm, ray.m_start) + m_d) < 0.01)
        return 0;
    glm::vec3 PQ, T;
    float det, u, v;
    float t;
    PQ = glm::cross(ray.m_dir, e2);
    det = glm::dot(PQ, e1);
    if (det > -M_EPS && det < M_EPS) return 0; // нет пересечения, луч параллелен
    // расстояние от a до направления луча
    T = ray.m_start - a;
    // расчет u и проверка границы
    u = glm::dot(T, PQ) / det;
    if (u < 0.f || u > 1.f) return 0; // пересечение за границами треугольника
    PQ = glm::cross(T, e1);
    // расчет и проверка v параметра
    v = glm::dot(ray.m_dir, PQ) / det;
    if (v < 0.f || u + v > 1.f) return 0; // пересечение за границами треугольника
    t = glm::dot(e2, PQ) / det;
    if (t > M_EPS){ // есть пересечение
        out_point = ray.m_start + ray.m_dir * t;
        out_norm = (1 - u - v) * nb + u * nc + v * na;
        //out_norm = glm::normalize(glm::cross(b - a, c - a));
        //fprintf(stderr, "Intersected\n");
        return 1;
    }
    return 0;
}


bool  VLess(const glm::vec3 &a, const glm::vec3 &b) {
    return (glm::length(a) < glm::length(b));
}

bool  VLess(const glm::vec2 &a, const glm::vec2 &b) {
    return (glm::length(a) < glm::length(b));
}

glm::vec3  VMin(const glm::vec3 &a, const glm::vec3 &b) {
    return (VLess(a, b)) ? a : b;
}

glm::vec2  VMin(const glm::vec2 &a, const glm::vec2 &b) {
    return (VLess(a, b)) ? a : b;
}

