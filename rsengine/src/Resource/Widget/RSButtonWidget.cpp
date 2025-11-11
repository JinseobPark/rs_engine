#include "pch.h"
#include "RSButtonWidget.h"
#include "Resource/Manager/RSResourceManager.h"

namespace RS_Widget
{
	RSButtonWidget::~RSButtonWidget()
	{
		//release callback
		m_callback = nullptr;

	}
	void RSButtonWidget::Initialize()
	{
		RSWidgetBase::Initialize();
		if (m_default_texture_id == 0)
			m_default_texture_id = RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("default")->GetTexture();
		if (m_func_texture_id == 0)
			m_func_texture_id = RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy")->GetTexture();
	}

	void RSButtonWidget::Update(const float dt)
	{
		RSWidgetBase::Update(dt);
		is_hovered = (RSResourceManager::GetInstance()->GetGraphicsData()->m_pixel_id == widget_id);

		m_color = is_hovered ? glm::vec4(1.0f, 1.0f, 1.0f, m_alpha) : glm::vec4(0.8f, 0.8f, 0.8f, m_alpha);
		m_texture_id = is_hovered ? m_func_texture_id : m_default_texture_id;
	}

	void RSButtonWidget::Draw()
	{

	}

	void RSButtonWidget::Destroy()
	{
		
	}

	void RSButtonWidget::Clicked() const
	{
		if (m_callback)
			m_callback();
	}

	void RSButtonWidget::Released()
	{

	}

}