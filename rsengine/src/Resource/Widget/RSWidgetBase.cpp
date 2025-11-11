#include "pch.h"
#include "RSWidgetBase.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Util/RSUtilFunctions.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/constants.hpp"

namespace RS_Widget
{
    using RS_Shader::RSShaderNames;
    RSWidgetBase::RSWidgetBase() = default;

    void RSWidgetBase::Initialize()
    {
		if (m_texture_id == 0)
            SetTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("default")->GetTexture());
     }

    void RSWidgetBase::PostInitialize()
    {
    }

    void RSWidgetBase::Update(float dt)
    {

    }

    void RSWidgetBase::Draw()
    {


	}
    void RSWidgetBase::DrawHittable()
    {
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "model", GetModelMatrix());
        RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "color_id", widget_id);
        RSResourceManager::GetInstance()->GetBufferManager()->DrawQuad();
    }

    void RSWidgetBase::DrawVisible()
    {
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "model", GetModelMatrix());
        RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "color", m_color);
		    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "flipY", b_flip_y);

        RSResourceManager::GetInstance()->GetBufferManager()->DrawQuad();
	}

    void RSWidgetBase::Destroy()
    {

	}



    void RSWidgetBase::TransposeSize()
    {
	    const glm::vec3 size = GetSize();
		SetSize(size.y, size.x, size.z);
    }

    glm::mat4 RSWidgetBase::GetModelMatrix() const
    {
        /*
        * 			glm::mat4 transform = glm::mat4(1.0f);
			glm::quat rotation_quat = glm::quat(glm::radians(m_rotation));

			transform = glm::translate(transform, m_position);
			transform *= glm::mat4_cast(rotation_quat);
			//transform = glm::rotate(transform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			//transform = glm::rotate(transform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			//transform = glm::rotate(transform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::scale(transform, m_scale);
			return transform;
        */
        //glm::mat4 model_matrix = glm::mat4(1.0f);
        //model_matrix = glm::translate(model_matrix, ConvertToScreen(glm::vec3(m_position.x, m_position.y, 0.0f)));
        ////model_matrix = glm::translate(model_matrix, ConvertToScreen(m_position)::vec3(m_position.x, m_position.y, 0.0f));
        //model_matrix = glm::scale(model_matrix, glm::vec3(m_size.x, m_size.y, 1.0f));
        //return model_matrix;

        auto model_matrix = glm::mat4(1.0f);
        const auto rotation_quat = glm::quat(glm::radians(m_transform.GetRotation()));
        model_matrix = glm::translate(model_matrix, RS_Util::ConvertToScreen(m_transform.GetPosition()));
        model_matrix *= glm::mat4_cast(rotation_quat);
        model_matrix = glm::scale(model_matrix, m_transform.GetScale());
        return model_matrix;
    }

}