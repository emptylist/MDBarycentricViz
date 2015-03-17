#include "barycentrictrajectory.h"
#include "beziercurve.h"

BarycentricTrajectory::BarycentricTrajectory()
    : m_v1(1.0f, 1.0f, 1.0f)
    , m_v2(1.0f, -1.0f, -1.0f)
    , m_v3(-1.0f, 1.0f, -1.0f)
    , m_v4(-1.0f, -1.0f, 1.0f)
    , m_color1(1.0f, 0.0f, 0.0f)
    , m_color2(0.0f, 0.0f, 1.0f)
    , m_color3(1.0f, 2.0f, 0.5f)
    , m_vertices(NULL)
    , m_colors(NULL)
    , m_nVertices(0)
    , m_nColors(0)
    , m_lineWidth(2.0)
    , f_projected(false)
    , f_verticesCached(false)
{}

BarycentricTrajectory::~BarycentricTrajectory() {
    for (auto it = frames.begin(); it != frames.end(); it++) {
        delete *it;
    }
    if (m_vertices != NULL) { free(m_vertices); }
    if (m_colors != NULL) { free(m_colors); }
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

const DataPoint& BarycentricTrajectory::operator[] (size_t idx) const {
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

void BarycentricTrajectory::recalculateVertices() {
    if (m_nVertices != frames.size()) {
        if (m_vertices != NULL) { free(m_vertices); }
        m_vertices = (decltype(m_vertices))malloc(3 * sizeof *m_vertices * frames.size());
        f_colorsCached = false;
        m_nVertices = frames.size();
    }
    if (!f_projected) { project(); }
    for (size_t idx = 0; idx < m_nVertices; idx++) {
        m_vertices[3 * idx] = (GLfloat)(frames[idx]->projectedX());
        m_vertices[(3 * idx) + 1] = (GLfloat)frames[idx]->projectedY();
        m_vertices[(3 * idx) + 2] = (GLfloat)frames[idx]->projectedZ();
    }
    f_verticesCached = true;
}

const GLfloat * BarycentricTrajectory::vertices() {
    if (!f_verticesCached) { recalculateVertices(); }
    return m_vertices;
}

void BarycentricTrajectory::recalculateColors() {

    if (m_nColors != frames.size()) {
        if (m_colors != NULL) { free(m_colors); }
        m_colors = (decltype(m_colors))malloc(3 * sizeof *m_colors * frames.size());
        m_nColors = frames.size();
    }

    BezierCurve curve = calcCurve(m_color1, m_color2, m_color3, m_nColors);
    memcpy(m_colors, curve.vertices, 3 * m_nColors * sizeof(float));
}

const GLfloat * BarycentricTrajectory::colors() {
    if (!f_colorsCached) { recalculateColors(); }
    return m_colors;
}

size_t BarycentricTrajectory::nVertices() const {
    return m_nVertices;
}
