#include "barycentrictrajectory.h"

BarycentricTrajectory::BarycentricTrajectory() :
    m_v1(1.0f, 1.0f, 1.0f),
    m_v2(1.0f, -1.0f, -1.0f),
    m_v3(-1.0f, 1.0f, 1.0f),
    m_v4(-1.0f, -1.0f, 1.0f),
    f_projected(false),
    f_verticesCached(false)
{}

BarycentricTrajectory::~BarycentricTrajectory() {
    for (auto it = frames.begin(); it != frames.end(); it++) {
        delete[] *it;
    }
}

void BarycentricTrajectory::project() {
    if (!f_projected) {
        for (auto it = frames.begin(); it != frames.end(); it++) {
            (*it)->project(v1(), v2(), v3(), v4());
        }
    }
    f_projected = true;
}

void BarycentricTrajectory::newFrame(float d1, float d2, float d3, float d4) {
    frames.push_back(new DataPoint(d1, d2, d3, d4));
    f_verticesCached = false;
}

const DataPoint& BarycentricTrajectory::operator[] (size_t idx) {
    return *(frames[idx]);
}

void BarycentricTrajectory::v1(const QVector3D &v) {
    m_v1 = v;
    f_projected = false;
}

void BarycentricTrajectory::v2(const QVector3D &v) {
    m_v2 = v;
    f_projected = false;
}

void BarycentricTrajectory::v3(const QVector3D &v) {
    m_v3 = v;
    f_projected = false;
}

void BarycentricTrajectory::v4(const QVector3D &v) {
    m_v4 = v;
    f_projected = false;
}

const GLfloat * BarycentricTrajectory::vertices() {
    if (!f_verticesCached) {
        if (m_nVertices != frames.size()) {
            // Oh noes!  C-style code!  Pointer arithmetic! MALLOC! The Horror!
            free(m_vertices);
            m_vertices = (decltype(m_vertices)) malloc(3 * sizeof(*m_vertices) * frames.size());
        }
        if (!f_projected) { project(); }
        for (size_t idx = 0; idx < m_nVertices; idx++) {
            m_vertices[3 * idx] = (GLfloat)(frames[idx]->projectedX());
            m_vertices[(3 * idx) + 1] = (GLfloat)frames[idx]->projectedY();
            m_vertices[(3 * idx) + 2] = (GLfloat)frames[idx]->projectedZ();

        }
        f_verticesCached = true;
    }

    return m_vertices;
}

size_t BarycentricTrajectory::nVertices() {
    return m_nVertices;
}
