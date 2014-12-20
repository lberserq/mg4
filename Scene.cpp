#include "Scene.h"
#include "model.h"
#include "Mesh.h"
#include <vector>


class  MeshTexPool {
    std::vector <Mesh *> m_pool;
    std::vector <Texture *>  t_pool;
    int m_last_k;
    int m_last_t;
public:
    MeshTexPool() {
        m_last_k = 0;
        m_last_t = 0;
    }

    void MPoolLoadFile(const char *fname) {
        std::string tmp = fname;
        std::vector<Mesh *>  msh = getMeshes(tmp);
        m_pool.insert(m_pool.end(), msh.begin(), msh.end());
        m_last_k = msh.size();
    }

    void LoadTexFile(std::vector<const char *> &lst) {
        Texture *tmp;
        for (unsigned i = 0; i < lst.size(); i++) {
            tmp = new Texture(lst[i]);
            t_pool.push_back(tmp);
        }
        m_last_t = lst.size();
    }

    void LoadTexFile(const char *file) {
        Texture *tmp;
        tmp = new Texture(file);
        t_pool.push_back(tmp);
        m_last_t = 1;
    }


    int get_texsize() const {
        return t_pool.size();
    }

    void AddMesh(Mesh * t) {
        m_pool.push_back(t);
        m_last_k = 1;
    }


    Model * BuildModel(CScene &parent, std::vector<int> &meshid, std::vector<int> &texid, M_TYPE type) {
        std::vector<Mesh *> meshes;
        std::vector<Texture *> textures;
        if (texid.size() != meshid.size()) {
            throw "Invalid size";
        }
        for (unsigned i = 0; i < meshid.size(); i++) {
            if (meshid[i] >= m_pool.size() || meshid[i] < 0) {
                throw "invalid  mesh index";
            }
            if (texid[i] >= m_pool.size() || texid[i] < 0) {
                throw "invalid texture index";
            }
            meshes.push_back(m_pool[meshid[i]]);
            textures.push_back(t_pool[texid[i]]);
        }
        return new Model(parent, meshes, textures, type);
    }


    Model * getLast(CScene &parent, M_TYPE type) {
        std::vector<Mesh *> meshes;
        std::vector<Texture *> textures;
        if (t_pool.size() == 0) {
            throw "No textures";
        }
        if (m_last_t == 0) {
            m_last_t = 1;
        }
        for (int i = m_pool.size() - m_last_k; i < m_pool.size(); i++) {
            meshes.push_back(m_pool[i]);
            int idj = t_pool.size() - m_last_t;
            textures.push_back(t_pool[idj + i % m_last_t]);

        }
        //m_last_k = m_last_t = 0;
        return new Model(parent, meshes, textures, type);
    }



};

MeshTexPool pool;

Mesh * genPlane(glm::vec3 tr[4], float w, float h)
{
    Triangle a(tr[0], tr[1], tr[2]);
    Triangle b(tr[2], tr[3], tr[0]);

    Mesh *res = new Mplane(a, b, w, h);
    return res;
}

void CScene::genBox() {
    glm::vec3 tr[4];
    float al = 7.5;
    tr[0] = al * glm::vec3(-1, -1, -1);
    tr[1] = al * glm::vec3(-1, -1,  1);
    tr[2] = al * glm::vec3(1, -1,   1);
    tr[3] = al * glm::vec3(1, -1,  -1);
    Mesh * floor = genPlane(tr, al * 2, al * 2);

    tr[0] = al * glm::vec3(-1, 1, -1);
    tr[1] = al * glm::vec3(-1, 1,  1);
    tr[2] = al * glm::vec3(1, 1,   1);
    tr[3] = al * glm::vec3(1, 1,  -1);
    Mesh * upst = genPlane(tr, al * 2, al * 2);



    tr[0] = al * glm::vec3(-1, -1, -1);
    tr[1] = al * glm::vec3(-1,  1, -1);
    tr[2] = al * glm::vec3(-1, 1,   1);
    tr[3] = al * glm::vec3(-1, -1,  1);
    Mesh *left = genPlane(tr, 2, 2);



    tr[0] = al * glm::vec3(1, -1, -1);
    tr[1] = al * glm::vec3(1,  1, -1);
    tr[2] = al * glm::vec3(1, 1,   1);
    tr[3] = al * glm::vec3(1, -1,  1);
    Mesh *right = genPlane(tr, 2, 2);


    tr[0] = al * glm::vec3(-1, -1, -1);
    tr[1] = al * glm::vec3(-1, 1,  -1);
    tr[2] = al * glm::vec3(1, 1,   -1);
    tr[3] = al * glm::vec3(1, -1,  -1);
    Mesh *forw = genPlane(tr, al * 2, al * 2);


    pool.AddMesh(floor);
    pool.LoadTexFile("FLOOR.bmp");
    Model * floorm = pool.getLast(*this, DIFFUSE_MODEL);


    pool.AddMesh(left);
    pool.LoadTexFile("WALLS.bmp");
    Model * leftm = pool.getLast(*this, DIFFUSE_MODEL);

    pool.AddMesh(right);
    pool.LoadTexFile("WALLS.bmp");
    Model * rightm = pool.getLast(*this, DIFFUSE_MODEL);

    pool.AddMesh(upst);
    pool.LoadTexFile("UPST.bmp");
    Model * upstm = pool.getLast(*this, DIFFUSE_MODEL);


    pool.AddMesh(forw);
    pool.LoadTexFile("WALLS.bmp");
    Model * forwm = pool.getLast(*this, DIFFUSE_MODEL);



    m_models.push_back(upstm);
    m_models.push_back(floorm);
    m_models.push_back(leftm);
    m_models.push_back(rightm);
    m_models.push_back(forwm);

    Light_pos = (al - 0.5f) * glm::vec3(1, 1, 1);


}

void CScene::setupScene()
{
    Model *tmp;
    //pool.MPoolLoadFile("/home/lberserq/models/rabbit.3DS");
    //pool.MPoolLoadFile("models/highpolygonal/Chess.3DS");
    //    pool.MPoolLoadFile("models/Crystal2.3DS");
    //    //pool.MPoolLoadFile("/home/lberserq/models/cube.3DS");
    //    std::vector<const char *> tex = {"models/textures/LR.bmp"};
    //    pool.LoadTexFile(tex);
    //    tmp = pool.getLast(*this, REFLECT_ALL);
    //    tmp->set_place(glm::vec3(0, 0, 0), 45, 45);
    //    m_models.push_back(tmp);
    pool.LoadTexFile("models/textures/LR.bmp");
    pool.MPoolLoadFile("/home/lberserq/models/monkey.3DS");

    tmp = pool.getLast(*this, DIFFUSE_MODEL);
    tmp->set_place(glm::vec3(0, -5, 0), 0, 0);

    m_models.push_back(tmp);


    pool.MPoolLoadFile("/home/lberserq/models/rabbit.3DS");
    tmp = pool.getLast(*this, FRENEL_MODEL);
    tmp->set_place(glm::vec3(-2, -2, -3), 0, 0);

    m_models.push_back(tmp);

    for (int i = -4; i < 1; i+=4)
        for (int j = -4; j < 1; j+=4)
            for (int k = -4; k < 1; k+=4) {
                int x = (i >= 0) * 4 + (j >= 0) * 2 + (k >= 0);
                M_TYPE type = static_cast<M_TYPE>(x % 3);
                fprintf(stderr, "Type %d %d\n", x, type);
                tmp = pool.getLast(*this, type);
                //tmp->set_place(glm::vec3(-2 + i, -2 + i, -3 + i), 0, 0);
                glm::vec3 pos= glm::vec3(-5 + i, j, k);
                if (k >= 0) {
                    pos[0] *= 0.3f;
                    pos[1] *= 0.3f;
                    tmp->rescale(0.3);
                }
                //glm::vec3 pos = glm::vec3(-2 + (i + j + 1) * 2, -5 + j, -2 + j);
                tmp->set_place(pos, 45 + (x - 2) * 15, 45);
                tmp->set_coeff(std::abs(x) * 0.01f * i, 1 - std::abs(x) * 0.01f);
                m_models.push_back(tmp);
            }

    genBox();



}


glm::vec3 triangle_test(const SRay &ray) {
    glm::vec3 a = glm::vec3(0, 0, 0);
    glm::vec3 b = glm::vec3(0, 1, 0);
    glm::vec3 c = glm::vec3(1, 1, 0);
    Triangle p(a, b, c, c, a, b);

    SRay tmpray;
    tmpray.m_start = glm::vec3(0, 0, 10);
    tmpray.m_dir = glm::vec3(0, 0, -1);
    tmpray = ray;
    //glm::vec3 a1(0, 0, 0);
    //if (ray.m_dir.x > 0) return glm::vec3(1, 1, 0);
    glm::vec3 temp_point;
    //p.RayIntersection(tmpray, temp_point);
    glm::vec3 tmpnorm;
    if (p.RayIntersection(tmpray, temp_point, tmpnorm))  {
        //fprintf(stderr, "SUCCESS\n");
        return glm::vec3(1.0f, 1, 0);
    }

    return glm::vec3(0, 1, 0);
}

glm::vec3 SphereTest(const SRay &ray) {
    Sphere t;
    t.center = glm::vec3(0, 0, 0);
    t.r = 0.5f;
    glm::vec3 a;
    if (t.IntersectRay(ray)) {
        return glm::vec3(1, 1, 0);
    } else {
        return glm::vec3(1, 0, 0);
    }
}

glm::vec3 CScene::IntersectRay(const SRay &ray)
{
    // TODO: make for each model in scene

    //return triangle_test(ray);
    //return SphereTest(ray);

    return TraceRay(ray);
}

glm::vec3 CScene::TraceRay(const SRay &ray)
{
    glm::vec3 color = glm::vec3(0, 0, 0);
    glm::vec3 point = glm::vec3(BIG_NUM, BIG_NUM, BIG_NUM);
    glm::vec3 mnorm;
    glm::vec2 mtex;
    glm::vec3 c;
    Model *tmp = NULL;
    for (unsigned  i = 0; i < m_models.size(); i++) {
        glm::vec3 m_p, m_n, m_c;
        glm::vec2 m_t;
        if (m_models[i]->IntersectRay(ray, m_p, m_n, m_t, m_c)) {
            if (VLess(m_p - ray.m_start, point - ray.m_start)) {
                point = m_p;
                mnorm = m_n;
                mtex = m_t;
                c = m_c;
                tmp = m_models[i];
            }
        }
    }
    if (tmp != NULL) {
        color = tmp->getColor(ray, point, mnorm, mtex);
    }
    //color = c;
    return color;
}


bool  CScene::LightTrace(const glm::vec3 &x)
{
    int cnt = 0;
    SRay ray;
    ray.m_start = x;
    ray.m_dir = glm::normalize(x - Light_pos);
    glm::vec3 c;
    for (unsigned i = 0; i < m_models.size() && cnt < 2; i++) {
        glm::vec3 m_p, m_n; glm::vec2 m_t;
        if (!m_models[i]->isPlane() && m_models[i]->IntersectRay(ray, m_p, m_n, m_t, c, true)) {
            cnt++;
        }
    }
    return (cnt < 2);
}

