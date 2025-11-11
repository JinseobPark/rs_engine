#include "pch.h"
#include "RSWidgetManager.h"
#include "RSResourceManager.h"
#include "Systems/Input/RSInputClass.h"

namespace RS_Widget
{
	using RS_Input::mRSINPUT;

	RSWidgetManager::RSWidgetManager()
	{
		last_widget_id = 100;
	}
	RSWidgetManager::~RSWidgetManager() = default;

	void RSWidgetManager::Initialize()
	{
		// Editor Initialize
		m_gizmo = new RSGizmo();
		m_gizmo->Initialize();

		// Direction view should be initialized after gizmo initialized
		if (DIRECTION_VIEW_USE)
		{
			m_direction_view = new RSDirectionView();
			m_direction_view->Initialize();
		}

		//if (LEGEND_VIEW_USE)
		//{
		//	// Legend widget
		//	m_legend_widget = new RSLegendWidget();
		//	m_legend_widget->Initialize();
		//	// The Legend widget is clickable.When dragging the legend widget, the legend widget is moved.
		//	AddButtonWidgetID(m_legend_widget, 60);
		//}

		InitializeQuad();
		for (const auto& widget : m_all_widgets)
		{
			widget->Initialize();
		}
	}

	void RSWidgetManager::PostInitialize()
	{
		SortWidgets();
	}

	void RSWidgetManager::Update(const float dt)
	{
		m_gizmo->Update(dt);
		if (DIRECTION_VIEW_USE)
			m_direction_view->Update(dt);

		for (const auto& widget : m_all_widgets)
		{
			widget->Update(dt);
		}
		SortWidgets();
	}
	void RSWidgetManager::SortWidgets()
	{
		// sort widgets by z_order
		m_all_widgets.sort([](RSWidgetBase* a, RSWidgetBase* b) { return a->GetZOrder() < b->GetZOrder(); });
	}

	void RSWidgetManager::Draw()
	{

	}

	void RSWidgetManager::VisibleDraw() const
	{
		for (const auto& widget : m_all_widgets)
		{
			if (widget->GetVisibility() == RSVisibility::HIT_VISIBLE || 
				  widget->GetVisibility() == RSVisibility::NONE_HIT_VISIBLE)
			{
				widget->DrawVisible();
			}
		}

	}

	void RSWidgetManager::HittableDraw() const
	{
		for (const auto& widget : m_all_widgets)
		{
			// when RSVisibility is HIT_VISIBLE or HIT_INVISIBLE
            if (widget->GetVisibility() == RSVisibility::HIT_VISIBLE ||
							  widget->GetVisibility() == RSVisibility::HIT_INVISIBLE)
			{
				widget->DrawHittable();
			}
		}
	}

	void RSWidgetManager::Clicked(const unsigned int widget_id)
	{
		assert(m_gizmo);
		if (widget_id >= GIZMO_START_ID && widget_id <= GIZMO_END_ID)
		{
			m_gizmo->SetStartMousePos(mRSINPUT->GetMousePosition().x, mRSINPUT->GetMousePosition().y);
			m_gizmo->SetActivatedGizmoID(widget_id);
		}

		// TODO : add bool type 'any button clicked'
		for (const auto& btn_widget : m_button_widgets)
		{
			if (btn_widget->GetWidgetID() == widget_id)
			{
				btn_widget->Clicked();
				m_clicked_button_id = widget_id;
				return;
			}
		}

		for (const auto& text_btn_widget : m_text_btn_widgets)
		{
			if (text_btn_widget->GetWidgetID() == widget_id)
			{
				text_btn_widget->Clicked();
				m_clicked_button_id = widget_id;
				return;
			}
		}
	}

	void RSWidgetManager::Released()
	{
		m_gizmo->SetActivatedGizmoID(0);
		for (const auto& btn_widget : m_button_widgets)
		{
			if (btn_widget->GetWidgetID() == m_clicked_button_id)
			{
				btn_widget->Released();
			}
		}

    for (const auto& text_btn_widget : m_text_btn_widgets)
    {
      if (text_btn_widget->GetWidgetID() == m_clicked_button_id)
      {
        text_btn_widget->Released();
      }
    }

		m_clicked_button_id = 0;
	}

	void RSWidgetManager::HittableDrawGizmo()
	{
		//assert(m_gizmo);
		//m_gizmo->DrawHittable();
	}

	void RSWidgetManager::VisibleDrawGizmo()
	{
		//assert(m_gizmo);
		//m_gizmo->DrawVisible();
	}

	void RSWidgetManager::Shutdown()
	{
		for (const auto& widget : m_all_widgets)
		{
			delete widget;
		}
		m_all_widgets.clear();
		m_button_widgets.clear();
		// deallocate quad buffer
		if (quad_vao)
			glDeleteVertexArrays(1, &quad_vao);
		quad_vao = 0;
		if (quad_vbo)
			glDeleteBuffers(1, &quad_vbo);
		quad_vbo = 0;


		delete m_gizmo;

		if (DIRECTION_VIEW_USE)
			delete m_direction_view;	

		last_widget_id = 100;
	}

	RSWidgetBase* RSWidgetManager::GetWidget(const unsigned int widget_id_) const
	{
		for (const auto& widget : m_all_widgets)
			if (widget->GetWidgetID() == widget_id_)
				return widget;
		return nullptr;
	}

	RSWidgetBase* RSWidgetManager::GetWidget(const std::string& widget_name_) const
	{
		for (auto& widget : m_all_widgets)
			if (widget->GetWidgetName() == widget_name_)
				return widget;
		return nullptr;
	}

	unsigned int RSWidgetManager::GetWidgetId(const std::string& widget_name_) const
	{
		for (auto& widget : m_all_widgets)
			if (widget->GetWidgetName() == widget_name_)
				return widget->GetWidgetID();
		return 0;
	}

	void RSWidgetManager::AddWidget(RSWidgetBase* widget)
	{
		widget->SetWidgetID(last_widget_id);
		last_widget_id++;
		m_all_widgets.push_back(widget);
	}

	void RSWidgetManager::AddWidgetID(RSWidgetBase* widget_, const unsigned int widget_id_)
	{
		widget_->SetWidgetID(widget_id_);
		m_all_widgets.push_back(widget_);
	}

	RSButtonWidget* RSWidgetManager::AddButtonWidget(const glm::vec2    position_, const glm::vec2 size_,
                                                   const std::string& name_, const GLuint        texture_id_)
	{
		const auto button_widget = new RSButtonWidget();
		button_widget->SetPosition(position_.x, position_.y);
		button_widget->SetSize(size_.x, size_.y);
		button_widget->SetTexture(texture_id_);
		button_widget->SetWidgetID(last_widget_id);
		button_widget->SetWidgetName(name_);
		button_widget->Initialize();
		last_widget_id++;
		m_all_widgets.push_back(button_widget);
		m_button_widgets.push_back(button_widget);

		return button_widget;
	}

	RSButtonWidget* RSWidgetManager::AddButtonWidgetID(const glm::vec2    position_, const glm::vec2 size_,
                                                     const std::string& name_, const unsigned int  widget_id_,
                                                     const GLuint       texture_id_)
	{
		for (auto iter = m_all_widgets.begin(); iter != m_all_widgets.end(); ++iter)
			if ((*iter)->GetWidgetID() == widget_id_) // if widget_id is already exist, return
			{
				RS_WARN("Widget ID %d is already exist", widget_id_);
				return static_cast<RSButtonWidget*>(*iter);
			}

		const auto button_widget = new RSButtonWidget();
		button_widget->SetPosition(position_.x, position_.y);
		button_widget->SetSize(size_.x, size_.y);
		button_widget->SetTexture(texture_id_);
		button_widget->SetWidgetID(widget_id_);
		button_widget->SetWidgetName(name_);
		button_widget->Initialize();
		m_all_widgets.push_back(button_widget);
		m_button_widgets.push_back(button_widget);
		return button_widget;
	}

	void RSWidgetManager::AddButtonWidget(RSButtonWidget* btn_widget_)
	{
		btn_widget_->SetWidgetID(last_widget_id);
		last_widget_id++;
		m_all_widgets.push_back(btn_widget_);
		m_button_widgets.push_back(btn_widget_);
	}

	RSButtonWidget* RSWidgetManager::AddButtonWidgetID(RSButtonWidget* btn_widget_, const unsigned int widget_id_)
	{
		btn_widget_->SetWidgetID(widget_id_);
		m_all_widgets.push_back(btn_widget_);
		m_button_widgets.push_back(btn_widget_);
		return btn_widget_;
	}

	RSTextWidget* RSWidgetManager::AddTextWidget(const glm::vec2 position_, const float size_, const std::wstring& text_,
                                               const RSTextStaticDynamic type_)
	{
		const auto text_widget = new RSTextWidget(type_);
		text_widget->SetPosition(position_.x, position_.y);
		text_widget->SetTextSize(size_);
		text_widget->SetText(text_);
		text_widget->SetWidgetName("text_widget");
		text_widget->SetWidgetID(last_widget_id);
		text_widget->Initialize();
		last_widget_id++;
		m_all_widgets.push_back(text_widget);
		m_text_widgets.push_back(text_widget);

		return text_widget;
	}

	RSTextWidget* RSWidgetManager::AddTextWidgetID(const glm::vec2     position_, const float    size_,
                                                 const std::wstring& text_, const unsigned int widget_id_,
                                                 RSTextStaticDynamic type_)
	{
		for (auto iter = m_all_widgets.begin(); iter != m_all_widgets.end(); ++iter)
			if ((*iter)->GetWidgetID() == widget_id_) // if widget_id is already exist, return
			{
				RS_WARN("Widget ID %d is already exist", widget_id_);
				return static_cast<RSTextWidget*>(*iter);
			}

		const auto text_widget = new RSTextWidget(type_);
		text_widget->SetPosition(position_.x, position_.y);
		text_widget->SetTextSize(size_);
		text_widget->SetText(text_);
		text_widget->SetWidgetName("text_widget");
		text_widget->SetWidgetID(widget_id_);
		text_widget->Initialize();
		m_all_widgets.push_back(text_widget);
		m_text_widgets.push_back(text_widget);

		return text_widget;
	}

	RSWidgetBase* RSWidgetManager::AddWidget(const glm::vec2 position_, const glm::vec2 size_, const std::string& name_,
                                           const GLuint    texture_id_)
	{
		const auto new_widget = new RSWidgetBase();
		new_widget->SetPosition(position_.x, position_.y);
		new_widget->SetSize(size_.x, size_.y);
		new_widget->SetTexture(texture_id_);
		new_widget->SetWidgetID(last_widget_id);
		new_widget->SetWidgetName(name_);
		new_widget->Initialize();
		last_widget_id++;
		m_all_widgets.push_back(new_widget);

		return new_widget;
	}

	RSTextButtonWidget* RSWidgetManager::AddTextButtonWidget(const glm::vec2     position_, const glm::vec2 size_,
                                                           const std::wstring& text_, const float         text_size,
                                                           const std::string&  name_)
	{
    const auto text_button_widget = new RSTextButtonWidget(text_);
    text_button_widget->SetPosition(position_.x, position_.y);
    text_button_widget->SetSize(size_.x, size_.y);
    text_button_widget->SetTextSize(text_size);
    text_button_widget->SetWidgetName(name_);
    text_button_widget->SetWidgetID(last_widget_id);
    text_button_widget->Initialize();
    last_widget_id++;
    m_all_widgets.push_back(text_button_widget);
		m_text_btn_widgets.push_back(text_button_widget);

		return text_button_widget;
	}

  void RSWidgetManager::AddTextButtonWidget(RSTextButtonWidget* text_btn_widget_)
  {
    text_btn_widget_->SetWidgetID(last_widget_id);
    last_widget_id++;
    m_all_widgets.push_back(text_btn_widget_);
    m_text_btn_widgets.push_back(text_btn_widget_);
  }

  void RSWidgetManager::RemoveWidget(RSWidgetBase* widget)
	{
		const unsigned int widget_id = GetWidgetId(widget->GetWidgetName());
		if (widget_id == 0)
		{
			RS_WARN("Widget %s is not found", widget->GetWidgetName().c_str());
			return;
		}

		RemoveFromBtnWidget(widget_id);
		RemoveFromTextWidget(widget_id);
		RemoveFromTextButtonWidget(widget_id);
		RemoveFromAllWidget(widget_id);

	}

	void RSWidgetManager::RemoveWidget(const unsigned int widget_id_)
	{
		RemoveFromBtnWidget(widget_id_);
		RemoveFromTextWidget(widget_id_);
    RemoveFromTextButtonWidget(widget_id_);
		RemoveFromAllWidget(widget_id_);
	}

	void RSWidgetManager::RemoveFromAllWidget(const unsigned int widget_id_)
	{
		std::string widget_name;
		for (auto iter = m_all_widgets.begin(); iter != m_all_widgets.end(); ++iter)
		{
			if ((*iter)->GetWidgetID() == widget_id_)
			{
				widget_name = (*iter)->GetWidgetName();
				delete* iter;
				m_all_widgets.erase(iter);
				RS_INFO("Widget %s is removed success", widget_name.c_str());
				return;
			}
		}
		RS_WARN("Widget %s is not found", widget_name.c_str());
	}

	void RSWidgetManager::RemoveFromBtnWidget(const unsigned int widget_id_)
	{
		for (auto iter = m_button_widgets.begin(); iter != m_button_widgets.end(); ++iter)
		{
			if ((*iter)->GetWidgetID() == widget_id_)
			{
				//delete* iter;
				m_button_widgets.erase(iter);
				return;
			}
		}
	}

	void RSWidgetManager::RemoveFromTextWidget(const unsigned int widget_id_)
	{
		for (auto iter = m_text_widgets.begin(); iter != m_text_widgets.end(); ++iter)
		{
			if ((*iter)->GetWidgetID() == widget_id_)
			{
				//delete* iter;
				(*iter)->Shutdown();
				m_text_widgets.erase(iter);
				return;
			}
		}
	}

	void RSWidgetManager::RemoveFromTextButtonWidget(const unsigned int widget_id_)
	{
    for (auto iter = m_text_btn_widgets.begin(); iter != m_text_btn_widgets.end(); ++iter)
    {
      if ((*iter)->GetWidgetID() == widget_id_)
      {
        //delete* iter;
        m_text_btn_widgets.erase(iter);
        return;
      }
    }
	}

	RSLegendWidget* RSWidgetManager::GetLegendWidget() const
	{
		// find the legend widget by name
    for (const auto& widget : m_all_widgets)
    {
      if (widget->GetWidgetName() == LEGEND_WIDGET_NAME)
        return dynamic_cast<RSLegendWidget*>(widget);
    }
		return nullptr;
	}

	void RSWidgetManager::ResizeWidgets() const
	{
		for (auto& widget : m_text_widgets)
		{
			widget->Resize();
		}
	}

	void RSWidgetManager::InitializeQuad()
	{
		if (quad_vao == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quad_vao);
			glGenBuffers(1, &quad_vbo);
			glBindVertexArray(quad_vao);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
	}


}