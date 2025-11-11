#include "pch.h"
#include "RSDirectionView.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Define/RSGizmoMeshData.h"

namespace RS_Widget
{
	using namespace RS_Camera;

	RSDirectionView::RSDirectionView()
	{
		m_x_axis_widget = new RSMeshWidget();
		m_y_axis_widget = new RSMeshWidget();
		m_z_axis_widget = new RSMeshWidget();
		m_center_widget = new RSMeshWidget();
	}

	RSDirectionView::~RSDirectionView()
	= default;

	void RSDirectionView::Initialize()
	{
		const RSResourceManager* resource_manager = RSResourceManager::GetInstance();
		p_camera = resource_manager->GetCamera();

    // Set Gizmo Mesh data
		m_x_axis_widget->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_AXIS_NAME));
		m_y_axis_widget->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_AXIS_NAME));
		m_z_axis_widget->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_AXIS_NAME));
		m_center_widget->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_CENTER_NAME));

    // Set Direction View Gizmo Axis Type rotation for n-axis
		m_y_axis_widget->SetFixedRotation(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    m_z_axis_widget->SetFixedRotation(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    // Set Gizmo Color
		m_x_axis_widget->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_y_axis_widget->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		m_z_axis_widget->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		m_center_widget->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Set Gizmo Size
		m_x_axis_widget->SetSize3D(60.0f);
		m_y_axis_widget->SetSize3D(60.0f);
		m_z_axis_widget->SetSize3D(60.0f);
		m_center_widget->SetSize3D(60.0f);

		// set position Right Button
		m_x_axis_widget->SetPosition(glm::vec3(-0.9f, -0.8f, 0.0f));
		m_y_axis_widget->SetPosition(glm::vec3(-0.9f, -0.8f, 0.0f));
		m_z_axis_widget->SetPosition(glm::vec3(-0.9f, -0.8f, 0.0f));
		m_center_widget->SetPosition(glm::vec3(-0.9f, -0.8f, 0.0f));


		resource_manager->GetWidgetManager()->AddWidgetID(m_x_axis_widget, 50);
		resource_manager->GetWidgetManager()->AddWidgetID(m_y_axis_widget, 51);
		resource_manager->GetWidgetManager()->AddWidgetID(m_z_axis_widget, 52);
		resource_manager->GetWidgetManager()->AddWidgetID(m_center_widget, 53);
	}

	void RSDirectionView::PostInitialize()
	{
	}

	void RSDirectionView::Update(float ) const
	{
		// Update Direction View by camera rotation
		if (!p_camera)
			return;

		const auto rotation_matrix = glm::mat4(glm::mat3(p_camera->GetView()));


		// set gizmo's rotation by camera view
		glm::vec3 camera_rotation = glm::eulerAngles(glm::quat_cast(rotation_matrix));
		// convert to degree
		camera_rotation = glm::degrees(camera_rotation);
		m_x_axis_widget->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
		m_y_axis_widget->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
		m_z_axis_widget->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);

	}

	void RSDirectionView::Draw()
	{

	}

	void RSDirectionView::Shutdown() const
	{
		delete m_x_axis_widget;
		delete m_y_axis_widget;
		delete m_z_axis_widget;
		delete m_center_widget;
	}

	void RSDirectionView::SetDirectionViewVisible(const bool is_visible_)
	{
		m_is_visible = is_visible_;
		if (m_is_visible)
		{
			m_x_axis_widget->SetVisibility(RSVisibility::NONE_HIT_VISIBLE);
			m_y_axis_widget->SetVisibility(RSVisibility::NONE_HIT_VISIBLE);
			m_z_axis_widget->SetVisibility(RSVisibility::NONE_HIT_VISIBLE);
			m_center_widget->SetVisibility(RSVisibility::NONE_HIT_VISIBLE);
		}
		else
		{
			m_x_axis_widget->SetVisibility(RSVisibility::NONE_HIT_INVISIBLE);
			m_y_axis_widget->SetVisibility(RSVisibility::NONE_HIT_INVISIBLE);
			m_z_axis_widget->SetVisibility(RSVisibility::NONE_HIT_INVISIBLE);
			m_center_widget->SetVisibility(RSVisibility::NONE_HIT_INVISIBLE);
		}
	}

}