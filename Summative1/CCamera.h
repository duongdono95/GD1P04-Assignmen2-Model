#ifndef CAMERA_H
#define CAMERA_Hs
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class CCamera {
public:
    enum Mode {
        MANUAL,
        AUTO
    };
    CCamera(const glm::vec3& position, float aspectRatio, float fov = 45.0f);
    void Update(float deltaTime);
    void SetMode(Mode mode);
    Mode GetMode() const {
        return m_mode;
    };
    void AdjustOrbit(float angleDelta, float radiusDelta);
    void SetPosition(const glm::vec3& position);
    void SetLookDirection(const glm::vec3& lookDir, const glm::vec3& upDir);
    glm::mat4 GetViewMat() const { return m_viewMat; };
    glm::mat4 GetProjMat() const { return m_projMat; };
    glm::vec3 GetForwardVector() const {
        return glm::normalize(glm::vec3(m_viewMat[0][2], m_viewMat[1][2], m_viewMat[2][2]));  // Assuming view matrix is correctly updated
    }
    glm::vec3 GetRightVector() const {
        return glm::normalize(glm::vec3(m_viewMat[0][0], m_viewMat[1][0], m_viewMat[2][0]));  // Assuming view matrix is correctly updated
    }

private:
    void UpdateMatrices();
    void UpdatePosition();
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_upDir;
    glm::mat4 m_viewMat;
    glm::mat4 m_projMat;
    float m_orbitRadius;
    float m_orbitAngle;
    float m_fov;
    float m_aspectRatio;
    Mode m_mode;
    float m_yOffset;
    glm::vec3 m_lookDir;
};

CCamera::CCamera(const glm::vec3& position, float aspectRatio, float fov)
    : m_position(position), m_target(0.0f), m_upDir(0.0f, 1.0f, 0.0f),
    m_fov(fov), m_aspectRatio(aspectRatio), m_orbitRadius(70.0f), m_orbitAngle(0.0f),
    m_mode(AUTO){
    UpdateMatrices();
}

void CCamera::Update(float deltaTime) {
    if (m_mode == AUTO) {
        m_orbitAngle += deltaTime * 0.1f;  // Adjust speed
    }
    UpdatePosition();
    UpdateMatrices();
}

void CCamera::SetMode(Mode mode) {
    m_mode = mode;
}

void CCamera::AdjustOrbit(float angleDelta, float radiusDelta) {
    m_orbitAngle += angleDelta;
    m_orbitRadius = glm::max(10.0f, m_orbitRadius + radiusDelta);
}

void CCamera::UpdatePosition() {
    m_position.x = m_orbitRadius * cos(m_orbitAngle);
    m_position.z = m_orbitRadius * sin(m_orbitAngle);
    m_position.y = 30.0f;
    //m_position.y = m_orbitRadius * sin(m_orbitAngle / 2);
}

void CCamera::UpdateMatrices() {
    m_viewMat = glm::lookAt(m_position, m_target, m_upDir);
    m_projMat = glm::perspective(glm::radians(m_fov), m_aspectRatio, 0.001f, 1000.0f);
}

void CCamera::SetPosition(const glm::vec3& position) {
    m_position = position;
    UpdateMatrices();
}

void CCamera::SetLookDirection(const glm::vec3& lookDir, const glm::vec3& upDir) {
    m_lookDir = lookDir;
    m_upDir = upDir;
    UpdateMatrices();
}


#endif // CAMERA_H