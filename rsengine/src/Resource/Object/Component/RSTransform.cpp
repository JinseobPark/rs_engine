#include "pch.h"
#include "RSTransform.h"

namespace RS_Component
{
	RSTransform::RSTransform()
	{
		m_position = glm::vec3(0.0f);
		m_rotation = glm::vec3(0.0f);
		m_scale = glm::vec3(1.0f);
    m_transform = glm::mat4(1.0f);
		b_dirty = true;
	}

	RSTransform::RSTransform(const glm::vec3& position_, const glm::vec3& rotation_, const glm::vec3& scale_)
	{
		m_position = position_;
		m_rotation = rotation_;
		m_scale = scale_;
		UpdateTransformMatrix();
		b_dirty = true;
	}

	RSTransform::RSTransform(const RSTransform& other_)
	{
		m_position = other_.m_position;
		m_rotation = other_.m_rotation;
		m_scale = other_.m_scale;
		UpdateTransformMatrix();
		b_dirty = true;
	}

	void RSTransform::SetPosition(const glm::vec3& position_)
	{
		m_position = position_;
		b_dirty = true;
	}

	void RSTransform::SetPosition(const float x, const float y, const float z)
	{
		m_position = glm::vec3(x, y, z);
		b_dirty = true;
	}

	void RSTransform::SetRotation(const glm::vec3& rotation_)
	{
		m_rotation = glm::vec3(glm::mod(rotation_.x, 360.0f), glm::mod(rotation_.y, 360.0f), glm::mod(rotation_.z, 360.0f));
		b_dirty = true;
	}

	void RSTransform::SetRotation(const float x, const float y, const float z)
	{
		m_rotation = glm::vec3(glm::mod(x, 360.0f), glm::mod(y, 360.0f), glm::mod(z, 360.0f));
		b_dirty = true;
	}

	void RSTransform::SetScale(const glm::vec3& scale_)
	{
    m_scale = scale_;
    b_dirty = true;
	}

	void RSTransform::SetScale(const float scale_)
	{
    m_scale = glm::vec3(scale_);
    b_dirty = true;
	}

	void RSTransform::SetScale(const float x, const float y, const float z)
	{
    m_scale = glm::vec3(x, y, z);
    b_dirty = true;
	}

	void RSTransform::AddPosition(const glm::vec3& position_)
	{
    m_position += position_;
    b_dirty = true;
	}

	void RSTransform::AddPosition(const float x, const float y, const float z)
	{
    m_position += glm::vec3(x, y, z);
    b_dirty = true;
	}

	void RSTransform::AddRotation(const glm::quat& rotation_)
	{
		const auto temp = glm::quat(glm::radians(m_rotation));
		const glm::quat result = rotation_ * temp;
		m_rotation = glm::degrees(glm::eulerAngles(result));
		m_rotation.x = glm::mod(m_rotation.x, 360.0f);
		m_rotation.y = glm::mod(m_rotation.y, 360.0f);
		m_rotation.z = glm::mod(m_rotation.z, 360.0f);
		b_dirty = true;
	}

	void RSTransform::AddRotation(const glm::vec3& rotation_)
	{
		// convert rotation_ to quaternion
		const auto temp = glm::quat(glm::radians(m_rotation));
		const glm::quat result = glm::quat(glm::radians(rotation_)) * temp;
		m_rotation = glm::degrees(glm::eulerAngles(result));
		m_rotation.x = glm::mod(m_rotation.x, 360.0f);
		m_rotation.y = glm::mod(m_rotation.y, 360.0f);
		m_rotation.z = glm::mod(m_rotation.z, 360.0f);
		b_dirty = true;
	}

	void RSTransform::AddScale(const glm::vec3& scale_)
	{
    m_scale += scale_;
    b_dirty = true;
	}

	void RSTransform::AddScale(const float x, const float y, const float z)
	{
    m_scale += glm::vec3(x, y, z);
    b_dirty = true;
	}

	glm::vec3 RSTransform::GetPosition() const
	{
    return m_position;
	}

	glm::vec3 RSTransform::GetRotation() const
	{
    return m_rotation;
	}

	glm::vec3 RSTransform::GetScale() const
	{
    return m_scale;
	}

	glm::mat4 RSTransform::GetTransformMatrix()
	{
    if (b_dirty)
    {
      UpdateTransformMatrix();
      b_dirty = false;
    }

		return m_transform;
	}

	glm::mat4 RSTransform::GetTransformMatrixWithoutScale()
	{
    auto transform = glm::mat4(1.0f);
    const auto rotation_quat = glm::quat(glm::radians(m_rotation));

    transform = glm::translate(transform, m_position);
    transform *= glm::mat4_cast(rotation_quat);
    return transform;

	}

	void RSTransform::UpdateTransformMatrix()
	{
		m_transform = glm::mat4(1.0f);
		const auto rotation_quat = glm::quat(glm::radians(m_rotation));

		m_transform = glm::translate(m_transform, m_position);
		m_transform *= glm::mat4_cast(rotation_quat);
		m_transform = glm::scale(m_transform, m_scale);
	}

  void RSTransform::SetOrigin()
  {
    m_position = glm::vec3(0.0f);
    m_rotation = glm::vec3(0.0f);
    m_scale = glm::vec3(1.0f);
    UpdateTransformMatrix();
  }
}
