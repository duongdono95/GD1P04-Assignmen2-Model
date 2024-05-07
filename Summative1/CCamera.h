#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
class CCamera
{
public:
	CCamera(const glm::vec3& position, float aspectRatio, float fov = 45.0f)
		: m_position(position), m_aspectRatio(aspectRatio), m_fov(fov),
		m_lookDir(glm::vec3(0.0f, 0.0f, -1.0f)), // Looking towards -Z by default
		m_upDir(glm::vec3(0.0f, 1.0f, 0.0f)) { // Up direction is +Y
		UpdateMatrices();
	};
	void Update(float deltaTime);
	glm::mat4 GetViewMat() const { return m_viewMat; };
	glm::mat4 GetProjMat() const { return m_projMat; };

	void SetPosition(const glm::vec3& position);

private:
	void UpdateMatrices();
	glm::mat4 m_viewMat;
	glm::mat4 m_projMat;

	float m_fov = 100;
	float m_aspectRatio;
	glm::vec3 m_position;
	glm::vec3 m_lookDir;
	glm::vec3 m_upDir;
};

void CCamera::Update(float deltaTime) {
	UpdateMatrices();
}

void CCamera::UpdateMatrices() {
	m_viewMat = glm::lookAt(m_position, glm::vec3(0.0f, 0.0f, 0.0f), m_upDir);
	float orthoHalfHeight = m_fov * 0.05f;
	float orthoHalfWidth = orthoHalfHeight * m_aspectRatio;
	//m_projMat = glm::ortho(-orthoHalfWidth, orthoHalfWidth, -orthoHalfHeight, orthoHalfHeight, 0.1f, 100.0f);
	m_projMat = glm::perspective(glm::radians(m_fov), m_aspectRatio, 0.001f, 300.0f);
}

void CCamera::SetPosition(const glm::vec3& position) {
	m_position = position;
	UpdateMatrices();
}