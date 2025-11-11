#include "pch.h"
#include "RSGizmo.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/Input/RSInputClass.h"
#include "Define/RSGizmoMeshData.h"
#include "Systems/Timer/RSDebugTimer.h"
#include "Systems/Timer/RSTimer.h"

#include "Util/RSUtilFunctions.h"


namespace RS_Widget
{
	using RS_Input::mRSINPUT;

	RSGizmo::RSGizmo()
	{
		m_translate_x = new RSGizmoWidget();
		m_translate_y = new RSGizmoWidget();
		m_translate_z = new RSGizmoWidget();
		m_translate_center = new RSGizmoWidget();
		m_translate_yz = new RSGizmoWidget();
		m_translate_xz = new RSGizmoWidget();
		m_translate_xy = new RSGizmoWidget();
		
		m_rotate_x = new RSGizmoWidget();
		m_rotate_y = new RSGizmoWidget();
		m_rotate_z = new RSGizmoWidget();

		m_scale_x = new RSGizmoWidget();
		m_scale_y = new RSGizmoWidget();
		m_scale_z = new RSGizmoWidget();
		m_scale_center = new RSGizmoWidget();


	}
	RSGizmo::~RSGizmo() = default;

	void RSGizmo::Initialize() const
	{
		const RSResourceManager* resource_manager = RSResourceManager::GetInstance();
		// Load RSGizmo Mesh
		resource_manager->GetMeshManager()->InitGizmoMeshes();

		// Set RSGizmo Mesh
		m_translate_x->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_AXIS_NAME));
		m_translate_y->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_AXIS_NAME));
		m_translate_z->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_AXIS_NAME));
		m_translate_center->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_CENTER_NAME));
		m_translate_yz->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_PLANE_NAME));
		m_translate_xz->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_PLANE_NAME));
		m_translate_xy->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_TRANSLATE_PLANE_NAME));

		m_rotate_x->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_ROTATE_AXIS_NAME));
		m_rotate_y->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_ROTATE_AXIS_NAME));
		m_rotate_z->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_ROTATE_AXIS_NAME));

		m_scale_x->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_SCALE_AXIS_NAME));
		m_scale_y->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_SCALE_AXIS_NAME));
		m_scale_z->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_SCALE_AXIS_NAME));
		m_scale_center->GetMesh()->SetMeshData(resource_manager->GetMeshManager()->GetMesh(GIZMO_SCALE_CENTER_NAME));

		// Set RSGizmo Axis Type
		m_translate_x->SetGizmoAxisType(GizmoAxisType::X_AXIS);
    m_translate_y->SetGizmoAxisType(GizmoAxisType::Y_AXIS);
    m_translate_z->SetGizmoAxisType(GizmoAxisType::Z_AXIS);
    m_translate_center->SetGizmoAxisType(GizmoAxisType::CENTER);
    m_translate_yz->SetGizmoAxisType(GizmoAxisType::X_AXIS_INV);
    m_translate_xz->SetGizmoAxisType(GizmoAxisType::Y_AXIS_INV);
    m_translate_xy->SetGizmoAxisType(GizmoAxisType::Z_AXIS_INV);

    m_rotate_x->SetGizmoAxisType(GizmoAxisType::X_AXIS);
    m_rotate_y->SetGizmoAxisType(GizmoAxisType::Y_AXIS);
    m_rotate_z->SetGizmoAxisType(GizmoAxisType::Z_AXIS);

    m_scale_x->SetGizmoAxisType(GizmoAxisType::X_AXIS);
    m_scale_y->SetGizmoAxisType(GizmoAxisType::Y_AXIS);
    m_scale_z->SetGizmoAxisType(GizmoAxisType::Z_AXIS);
    m_scale_center->SetGizmoAxisType(GizmoAxisType::CENTER);

		// Set RSGizmo Color. x = red, y = green, z = blue, center = white
		m_translate_x->SetDefaultColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_translate_y->SetDefaultColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		m_translate_z->SetDefaultColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		m_translate_center->SetDefaultColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_translate_yz->SetDefaultColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_translate_xz->SetDefaultColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		m_translate_xy->SetDefaultColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		m_rotate_x->SetDefaultColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_rotate_y->SetDefaultColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		m_rotate_z->SetDefaultColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		m_scale_x->SetDefaultColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_scale_y->SetDefaultColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		m_scale_z->SetDefaultColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		m_scale_center->SetDefaultColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		// Enroll Widget Data
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_translate_x, 10);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_translate_y, 11);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_translate_z, 12);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_translate_center, 13);

		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_translate_yz, 14);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_translate_xz, 15);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_translate_xy, 16);

		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_rotate_x, 20);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_rotate_y, 21);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_rotate_z, 22);

		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_scale_x, 30);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_scale_y, 31);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_scale_z, 32);
		resource_manager->GetWidgetManager()->AddButtonWidgetID(m_scale_center, 33);

	}

	void RSGizmo::Update(float )
	{
		// Update gizmo data.
		// get camera view, projection matrix
		// get target object's position, rotation, scale
		// set gizmo's position, rotation, scale

		// gizmo size is fixed
		// then, does gizmo draw by screen space? or world space?
		// if screen space, then gizmo size is fixed, but if world space, gizmo size is not fixed.
		// So, object should be converted to screen space with camera view, projection matrix.
		// and then, gizmo should be drawn by screen space.

		if(target_object == nullptr)
			return;

		if (m_activated_gizmo_id != 0)
		{
			// RSGizmo is activated
			switch (m_activated_gizmo_id)
			{
				case 10:	// translate x
				{
					// Set RSObject Position by RSGizmo
					SetObjectPositionByGizmo(true, false, false);
					break;
				}
				case 11:	// translate y
				{
					SetObjectPositionByGizmo(false, true, false);
					break;
				}
				case 12:	// translate z
				{
					SetObjectPositionByGizmo(false, false, true);
					break;
				}
				case 13:	// translate center
				{
					SetObjectPositionByGizmo(true, true, true);
					break;
				}
				case 14:	// translate yz
				{
					SetObjectPositionByGizmo(false, true, true);
					break;
				}
				case 15:	// translate xz
				{
					SetObjectPositionByGizmo(true, false, true);
					break;
				}
				case 16:	// translate xy
				{
					SetObjectPositionByGizmo(true, true, false);
					break;
				}
				case 20:	// rotate x
				{
					SetObjectRotationByGizmo(true, false, false);
					break;
				}
				case 21:	// rotate y
				{
					SetObjectRotationByGizmo(false, true, false);
					break;
				}
				case 22:	// rotate z
				{
					SetObjectRotationByGizmo(false, false, true);
					break;
				}
				case 30:	// scale x
				{
					SetObjectScaleByGizmo(true, false, false);
					break;
				}
				case 31:	// scale y
				{
					SetObjectScaleByGizmo(false, true, false);
					break;
				}
				case 32:	// scale z
				{
					SetObjectScaleByGizmo(false, false, true);
					break;
				}
				case 33:	// scale center
				{
					SetObjectScaleByGizmo(true, true, true);
					break;
				}
				default:
					break;
			}
		}
		else
		{
			// RSGizmo is not activated
			// Check RSGizmo is clicked
			// If RSGizmo is clicked, set gizmo id and set start mouse pos


		}

		// get camera view, projection matrix
		glm::mat4 view = RSResourceManager::GetInstance()->GetCamera()->GetView();
		auto view_rotate = glm::mat4(glm::mat3(view));
		glm::mat4 proj = RSResourceManager::GetInstance()->GetCamera()->GetProj();
		glm::mat4 model = target_object->GetTransformMatrix();

		// convert object position to screen space
		auto object_position = glm::vec3(model[3][0], model[3][1], model[3][2]);
		//auto model_rotation = glm::mat4(glm::mat3(model));
		glm::vec4 screen_position = proj * view * glm::vec4(object_position, 1.0f);
		screen_position /= screen_position.w;

		switch (m_state)
		{
			case RSGizmoState::NONE:
				break;
			case RSGizmoState::TRANSLATION:
			{
				// set gizmo's position
				m_translate_x->SetPosition(screen_position.x, screen_position.y);
				m_translate_y->SetPosition(screen_position.x, screen_position.y);
				m_translate_z->SetPosition(screen_position.x, screen_position.y);
				m_translate_center->SetPosition(screen_position.x, screen_position.y);
				m_translate_yz->SetPosition(screen_position.x, screen_position.y);
				m_translate_xz->SetPosition(screen_position.x, screen_position.y);
				m_translate_xy->SetPosition(screen_position.x, screen_position.y);

				// set gizmo's rotation by camera view
				glm::vec3 camera_rotation = glm::eulerAngles(glm::quat_cast(view_rotate));
				// convert to degree
				camera_rotation = glm::degrees(camera_rotation);
				m_translate_x->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_translate_y->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_translate_z->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_translate_center->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_translate_yz->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_translate_xz->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_translate_xy->SetRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				break;
			}
			case RSGizmoState::ROTATION:
			{
				// set gizmo's position
				m_rotate_x->SetPosition(screen_position.x, screen_position.y);
				m_rotate_y->SetPosition(screen_position.x, screen_position.y);
				m_rotate_z->SetPosition(screen_position.x, screen_position.y);

				// Additional, set gizmo's rotation by object's rotation
				glm::vec3 object_rotation = target_object->GetTransform()->GetRotation();
				m_rotate_x->SetRotation(object_rotation.x, object_rotation.y, object_rotation.z);
				m_rotate_y->SetRotation(object_rotation.x, object_rotation.y, object_rotation.z);
				m_rotate_z->SetRotation(object_rotation.x, object_rotation.y, object_rotation.z);

				// Additional set gizmo's rotation by camera view
				glm::vec3 camera_rotation = glm::eulerAngles(glm::quat_cast(view_rotate));
				camera_rotation = glm::degrees(camera_rotation);
				m_rotate_x->AddRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_rotate_y->AddRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_rotate_z->AddRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				break;
			}
			case RSGizmoState::SCALE:
			{
				// set gizmo's position
				m_scale_x->SetPosition(screen_position.x, screen_position.y);
				m_scale_y->SetPosition(screen_position.x, screen_position.y);
				m_scale_z->SetPosition(screen_position.x, screen_position.y);
				m_scale_center->SetPosition(screen_position.x, screen_position.y);

				// Set gizmo's rotation by object's rotation
				//glm::vec3 object_rotation = glm::eulerAngles(glm::quat_cast(model));
				glm::vec3 object_rotation = target_object->GetTransform()->GetRotation();
				m_scale_x->SetRotation(object_rotation.x, object_rotation.y, object_rotation.z);
				m_scale_y->SetRotation(object_rotation.x, object_rotation.y, object_rotation.z);
				m_scale_z->SetRotation(object_rotation.x, object_rotation.y, object_rotation.z);
				m_scale_center->SetRotation(object_rotation.x, object_rotation.y, object_rotation.z);

				// Additional set gizmo's rotation by camera view
				glm::vec3 camera_rotation = glm::eulerAngles(glm::quat_cast(view_rotate));
				camera_rotation = glm::degrees(camera_rotation);
				m_scale_x->AddRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_scale_y->AddRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_scale_z->AddRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);
				m_scale_center->AddRotation(camera_rotation.x, camera_rotation.y, camera_rotation.z);

				break;
			}
		}


	}

	void RSGizmo::Draw()
	{
	}

	/*void RSGizmo::DrawHittable()
	{
		if(target_object == nullptr)
			return;

		switch (m_state)
		{
			case RSGizmoState::NONE:
				break;
			case RSGizmoState::TRANSLATION:
			{
				m_translate_x->DrawHittable();
				m_translate_y->DrawHittable();
				m_translate_z->DrawHittable();
				m_translate_center->DrawHttable();
				m_translate_yz->DrawHittable();
				m_translate_xz->DrawHittable();
				m_translate_xy->DrawHittable();
				break;
			}
			case RSGizmoState::ROTATION:
			{
				m_rotate_x->DrawHittable();
				m_rotate_y->DrawHittable();
				m_rotate_z->DrawHittable();
				break;
			}
			case RSGizmoState::SCALE:
			{
				m_scale_x->DrawHittable();
				m_scale_y->DrawHittable();
				m_scale_z->DrawHittable();
				m_scale_center->DrawHittable();
				break;
			}
			default:
				break;
		}
	}

	void RSGizmo::DrawVisible()
	{
		if (target_object == nullptr)
			return;

		switch (m_state)
		{
		case RSGizmoState::NONE:
			break;
		case RSGizmoState::TRANSLATION:
		{
			m_translate_x->DrawVisible();
			m_translate_y->DrawVisible();
			m_translate_z->DrawVisible();
			m_translate_center->DrawVisible();
			m_translate_yz->DrawVisible();
			m_translate_xz->DrawVisible();
			m_translate_xy->DrawVisible();
			break;
		}
		case RSGizmoState::ROTATION:
		{
			m_rotate_x->DrawVisible();
			m_rotate_y->DrawVisible();
			m_rotate_z->DrawVisible();
			break;
		}
		case RSGizmoState::SCALE:
		{
			m_scale_x->DrawVisible();
			m_scale_y->DrawVisible();
			m_scale_z->DrawVisible();
			m_scale_center->DrawVisible();
			break;
		}
		default:
			break;
		}
	}*/

	void RSGizmo::Destroy()
	{
	}

	void RSGizmo::SetTargetObject(RS_Object::RSObject* object)
	{
		target_object = object;
		SetState(m_state);
	}

	void RSGizmo::SetState(const RSGizmoState state)
	{
		m_state = state;
		// all gizmo widget set is draw (false)
		SetStateResetGizmo();

		if (target_object == nullptr) return;
		
		// set gizmo widget is draw (true)
		switch (m_state)
		{
			case RSGizmoState::NONE:
				break;
			case RSGizmoState::TRANSLATION:
			{
				m_translate_x->SetIsDraw(true);
				m_translate_y->SetIsDraw(true);
				m_translate_z->SetIsDraw(true);
				m_translate_center->SetIsDraw(true);
				m_translate_yz->SetIsDraw(true);
				m_translate_xz->SetIsDraw(true);
				m_translate_xy->SetIsDraw(true);
				break;
			}
			case RSGizmoState::ROTATION:
			{
				m_rotate_x->SetIsDraw(true);
				m_rotate_y->SetIsDraw(true);
				m_rotate_z->SetIsDraw(true);
				break;
			}
			case RSGizmoState::SCALE:
			{
				m_scale_x->SetIsDraw(true);
				m_scale_y->SetIsDraw(true);
				m_scale_z->SetIsDraw(true);
				m_scale_center->SetIsDraw(true);
				break;
			}
		}

	}
	void RSGizmo::SetStartMousePos(const UINT mouse_x, const UINT mouse_y)
	{
		start_mouse_pos = glm::vec2(mouse_x, mouse_y);
		RS_ASSERT(target_object != nullptr, "Target Object is nullptr");
		start_object_model_matrix = *target_object->GetTransform(); // copy constructor
	}
	
	void RSGizmo::SetObjectPositionByGizmo(bool x_, bool y_, bool z_)
	{
		// Constants
		const glm::vec3 initial_position = start_object_model_matrix.GetPosition();

		// Get mouse positions
		const glm::vec2 start_position_mouse = start_mouse_pos;
		const glm::vec2 current_mouse_pos = mRSINPUT->GetMousePosition();


		glm::vec3 ray_origin, ray_dir;

		// Determine the type of movement based on selected axes
		int axis_count = (x_ ? 1 : 0) + (y_ ? 1 : 0) + (z_ ? 1 : 0);

		if (axis_count == 1)
		{
			// Single-Axis Movement
			glm::vec3 move_axis;
			if (x_) move_axis = glm::vec3(1.0f, 0.0f, 0.0f);
			else if (y_) move_axis = glm::vec3(0.0f, 1.0f, 0.0f);
			else if (z_) move_axis = glm::vec3(0.0f, 0.0f, 1.0f);

			// Function to calculate the parameter t along the axis for a given mouse position
			auto get_axis_parameter = [&](const glm::vec2& mouse_pos) -> float
				{
					GetRayScreen(ray_origin, ray_dir, mouse_pos);

					// Compute the closest point on the axis to the ray
					const glm::vec3 p1 = initial_position;                // A point on the axis
					const glm::vec3 p2 = initial_position + move_axis;    // Another point on the axis

					// Line-line the closest point calculation
					const glm::vec3 da = p2 - p1;        // Direction of the axis
					const glm::vec3 db = ray_dir;        // Direction of the ray
					const glm::vec3 dc = p1 - ray_origin;

					const float da_dot_da = glm::dot(da, da);
					const float da_dot_db = glm::dot(da, db);
					const float da_dot_dc = glm::dot(da, dc);
					const float db_dot_db = glm::dot(db, db);
					const float db_dot_dc = glm::dot(db, dc);

					float denom = da_dot_da * db_dot_db - da_dot_db * da_dot_db;

					// Check for parallel lines
					if (fabs(denom) < 1e-6f)
					{
						return 0.0f;
					}

					const float s = (da_dot_db * db_dot_dc - db_dot_db * da_dot_dc) / denom;
					// The point along the axis is p = p1 + s * da
					return s;
				};

			// Get the initial and current parameters along the axis
			float start_param = get_axis_parameter(start_position_mouse);
			float current_param = get_axis_parameter(current_mouse_pos);

			// Calculate the movement along the axis
			float delta_param = current_param - start_param;
			glm::vec3 movement = delta_param * move_axis;

			// Update the object's position
			target_object->GetTransform()->SetPosition(initial_position + movement);
		}
		else if (axis_count == 2)
		{
			// Plane Movement
			glm::vec3 plane_normal;
			if (!x_) plane_normal = glm::vec3(1.0f, 0.0f, 0.0f); // Movement in YZ plane
			else if (!y_) plane_normal = glm::vec3(0.0f, 1.0f, 0.0f); // Movement in XZ plane
			else if (!z_) plane_normal = glm::vec3(0.0f, 0.0f, 1.0f); // Movement in XY plane

			// Function to compute intersection point of mouse ray with plane
			auto get_plane_intersection = [&](const glm::vec2& mouse_pos) -> glm::vec3
				{
					GetRayScreen(ray_origin, ray_dir, mouse_pos);
					
					// Plane equation: (P - P0) • N = 0
					float denom = glm::dot(ray_dir, plane_normal);
					if (fabs(denom) > 1e-6f)
					{
						const float t = glm::dot(initial_position - ray_origin, plane_normal) / denom;
						const glm::vec3 intersection_point = ray_origin + t * ray_dir;
						return intersection_point;
					}
					else
					{
						// Ray is parallel to the plane
						return initial_position;
					}
				};

			// Get the initial and current intersection points
			glm::vec3 start_intersection = get_plane_intersection(start_position_mouse);
			glm::vec3 current_intersection = get_plane_intersection(current_mouse_pos);

			// Calculate the movement vector
			glm::vec3 movement = current_intersection - start_intersection;

			// Apply axis constraints (the axis not selected should not change)
			if (x_) movement.x = current_intersection.x - start_intersection.x;
			else movement.x = 0.0f;
			if (y_) movement.y = current_intersection.y - start_intersection.y;
			else movement.y = 0.0f;
			if (z_) movement.z = current_intersection.z - start_intersection.z;
			else movement.z = 0.0f;

			// Update the object's position
			target_object->GetTransform()->SetPosition(initial_position + movement);
	}
		else
		{
			// Free Movement or other cases
			// For simplicity, let's handle free movement in the camera's view plane
			glm::vec3 plane_normal = RSResourceManager::GetInstance()->GetCamera()->GetForwardDir();

			// Function to compute intersection point of mouse ray with plane
			auto get_plane_intersection = [&](const glm::vec2& mouse_pos) -> glm::vec3
				{
					GetRayScreen(ray_origin, ray_dir, mouse_pos);

					// Plane equation: (P - P0) • N = 0
					const float denom = glm::dot(ray_dir, plane_normal);
					if (fabs(denom) > 1e-6f)
					{
						const float t = glm::dot(initial_position - ray_origin, plane_normal) / denom;
						const glm::vec3 intersection_point = ray_origin + t * ray_dir;
						return intersection_point;
					}
					else
					{
						// Ray is parallel to the plane
						return initial_position;
					}
				};

			// Get the initial and current intersection points
			glm::vec3 start_intersection = get_plane_intersection(start_position_mouse);
			glm::vec3 current_intersection = get_plane_intersection(current_mouse_pos);

			// Calculate the movement vector
			glm::vec3 movement = current_intersection - start_intersection;

			// Update the object's position
			target_object->GetTransform()->SetPosition(initial_position + movement);
		}
	}
	void RSGizmo::SetObjectRotationByGizmo(bool x_, bool y_, bool z_)
	{
		// Constants
		const glm::vec3 initial_position = start_object_model_matrix.GetPosition();
		const glm::vec2 start_position_mouse = start_mouse_pos;
		const glm::vec2 current_mouse_pos = mRSINPUT->GetMousePosition();

		// Get object's initial rotation in quaternion form
		auto start_rotation_quat = glm::quat(glm::radians(start_object_model_matrix.GetRotation()));
		glm::mat4 start_rotation_mat = glm::mat4_cast(start_rotation_quat);

		// Lambda function to convert screen position to world position on a plane perpendicular to the object's axis
		auto screen_to_world_on_plane = [&](const glm::vec2 screen_pos) -> glm::vec3
			{
				// Ray
				glm::vec3 ray_dir;
				glm::vec3 ray_origin;

				GetRayScreen(ray_origin, ray_dir, screen_pos);

				// Define the plane using the object's local axis
				glm::vec3 plane_normal;
				if (x_)
					plane_normal = glm::vec3(start_rotation_mat * glm::vec4(1, 0, 0, 0)); // Local X axis
				else if (y_)
					plane_normal = glm::vec3(start_rotation_mat * glm::vec4(0, 1, 0, 0)); // Local Y axis
				else
					plane_normal = glm::vec3(start_rotation_mat * glm::vec4(0, 0, 1, 0)); // Local Z axis

				plane_normal = glm::normalize(plane_normal);

				// Compute intersection point of the ray with the plane
				float denom = glm::dot(plane_normal, ray_dir);
				if (fabs(denom) > 1e-6)
				{
					const float t = glm::dot(initial_position - ray_origin, plane_normal) / denom;
					const glm::vec3 intersection_point = ray_origin + t * ray_dir;
					return intersection_point;
				}

				// Ray is parallel to the plane
				return initial_position;
			};

		// Convert start and current mouse positions to world coordinates on the plane defined by the object's local axis
		glm::vec3 start_world = screen_to_world_on_plane(start_position_mouse);
		glm::vec3 current_world = screen_to_world_on_plane(current_mouse_pos);

		// Compute vectors from the object to the start and current positions
		glm::vec3 start_vec = start_world - initial_position;
		glm::vec3 current_vec = current_world - initial_position;

		// Determine rotation axis in world space (object's local axis transformed)
		glm::vec3 rotation_axis_local;
		if (x_)
			rotation_axis_local = glm::vec3(start_rotation_mat * glm::vec4(1, 0, 0, 0)); // Local X axis
		else if (y_)
			rotation_axis_local = glm::vec3(start_rotation_mat * glm::vec4(0, 1, 0, 0)); // Local Y axis
		else
			rotation_axis_local = glm::vec3(start_rotation_mat * glm::vec4(0, 0, 1, 0)); // Local Z axis

		rotation_axis_local = glm::normalize(rotation_axis_local);

		// Project vectors onto the plane perpendicular to the rotation axis
		start_vec -= glm::dot(start_vec, rotation_axis_local) * rotation_axis_local;
		current_vec -= glm::dot(current_vec, rotation_axis_local) * rotation_axis_local;

		// Normalize the vectors
		start_vec = glm::normalize(start_vec);
		current_vec = glm::normalize(current_vec);

		// Calculate the angle between the vectors
		float angle = acos(glm::clamp(glm::dot(start_vec, current_vec), -1.0f, 1.0f));

		// Determine the direction of rotation
		float sign = glm::dot(rotation_axis_local, glm::cross(start_vec, current_vec));
		if (sign < 0) angle = -angle;

		// Create the rotation quaternion
		glm::quat rotation_quat = glm::angleAxis(angle, rotation_axis_local);

		// Apply the rotation to the object's initial rotation
		glm::quat new_rotation_quat = rotation_quat * start_rotation_quat;

		// Convert to Euler angles and set the object's rotation
		glm::vec3 new_rotation_euler = glm::degrees(glm::eulerAngles(new_rotation_quat));
		target_object->GetTransform()->SetRotation(new_rotation_euler);

	}

	void RSGizmo::SetObjectScaleByGizmo(bool x_, bool y_, bool z_)
	{
		// Constants
		const glm::vec3 initial_position = start_object_model_matrix.GetPosition();

		// Get mouse positions
		const glm::vec2 start_position_mouse = start_mouse_pos;
		const glm::vec2 current_mouse_pos = mRSINPUT->GetMousePosition();

		// Get camera matrices
		const glm::mat4 view = RSResourceManager::GetInstance()->GetCamera()->GetView();

		// Get the object's initial rotation
		const glm::vec3 object_rotation = start_object_model_matrix.GetRotation();
		const auto rotation_quat = glm::quat(glm::radians(object_rotation));
		const glm::mat4 rot_mat = glm::mat4_cast(rotation_quat);

		if (x_ && y_ && z_)
		{
			glm::vec2 delta_screen = current_mouse_pos - start_position_mouse;
			delta_screen.y = -delta_screen.y; // Invert the vertical movement
			// Sum the horizontal and vertical movements
			float delta = delta_screen.x + delta_screen.y;

			// Define scaling sensitivity
			float sensitivity = 0.005f; // Adjust as needed for desired scaling speed

			// Compute scaling factor
			float scale_change = 1.0f + delta * sensitivity;

			// Ensure the scaling factor is positive and above a minimum value
			if (scale_change < 0.01f)
				scale_change = 0.01f;

			// Apply uniform scaling
			auto scale_factors = glm::vec3(scale_change);

			// Apply the scaling to the object's initial scale
			glm::vec3 new_scale = start_object_model_matrix.GetScale() * scale_factors;

			// Set the new scale to the object
			target_object->GetTransform()->SetScale(new_scale);
			return;
		}

		// Convert screen position to world position on a plane perpendicular to the camera's view direction
		auto screen_to_world_on_plane = [&](const glm::vec2 screen_pos) -> glm::vec3
			{
				// Ray
				glm::vec3 ray_dir;
				glm::vec3 ray_origin;
				GetRayScreen(ray_origin, ray_dir, screen_pos);

				// Plane normal (using camera's forward direction)
				const glm::vec3 plane_normal = glm::normalize(glm::vec3(view[0][2], view[1][2], view[2][2]));

				// Compute intersection point of the ray with the plane
				float denom = glm::dot(plane_normal, ray_dir);
				if (fabs(denom) > 1e-6)
				{
					const float t = glm::dot(initial_position - ray_origin, plane_normal) / denom;
					const glm::vec3 intersection_point = ray_origin + t * ray_dir;
					return intersection_point;
				}

				// Ray is parallel to the plane
				return initial_position;
			};

		// Convert start and current mouse positions to world coordinates
		glm::vec3 start_world = screen_to_world_on_plane(start_position_mouse);
		glm::vec3 current_world = screen_to_world_on_plane(current_mouse_pos);

		// Compute the delta movement in world space
		glm::vec3 delta_world = current_world - start_world;

		// Transform delta movement to object's local space
		auto delta_local = glm::vec3(glm::inverse(rot_mat) * glm::vec4(delta_world, 0.0f));

		// Determine the scale factors
		auto scale_factors = glm::vec3(1.0f);

		// For each axis, compute the scaling factor based on delta movement

		if (x_)
		{
			scale_factors.x += delta_local.x;
		}
		else if (y_)
		{
			scale_factors.y += delta_local.y;
		}
		else if (z_)
		{
			scale_factors.z += delta_local.z;
		}

		// Ensure the scale factors are positive and avoid scaling to zero or negative
		scale_factors = glm::max(scale_factors, glm::vec3(0.001f));

		// Apply the scaling to the object's initial scale
		glm::vec3 new_scale = start_object_model_matrix.GetScale() * scale_factors;

		// Set the new scale to the object
		target_object->GetTransform()->SetScale(new_scale);
	}

	
	void RSGizmo::SetStateResetGizmo() const
	{
		m_translate_x->SetIsDraw(false);
		m_translate_y->SetIsDraw(false);
		m_translate_z->SetIsDraw(false);
		m_translate_center->SetIsDraw(false);
		m_translate_yz->SetIsDraw(false);
		m_translate_xz->SetIsDraw(false);
		m_translate_xy->SetIsDraw(false);

		m_rotate_x->SetIsDraw(false);
		m_rotate_y->SetIsDraw(false);
		m_rotate_z->SetIsDraw(false);

		m_scale_x->SetIsDraw(false);
		m_scale_y->SetIsDraw(false);
		m_scale_z->SetIsDraw(false);
		m_scale_center->SetIsDraw(false);
	}

	void RSGizmo::CalculateRayOriginDirection(glm::vec3& ray_origin_out, glm::vec3& ray_dir_out,
		const glm::vec2& mouse_pos) const
	{
		const glm::mat4 view = RSResourceManager::GetInstance()->GetCamera()->GetView();
		const glm::mat4 proj = RSResourceManager::GetInstance()->GetCamera()->GetProj();

		const auto screen_pos_world = glm::vec3(
			mouse_pos.x / static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth()) * 2.0f - 1.0f,
			1.0f - mouse_pos.y / static_cast<float>(RS_Application::mRSAPPLICATION->GetHeight()) * 2.0f,
			-1.0f);
		glm::vec4 world_pos = glm::inverse(proj * view) * glm::vec4(screen_pos_world, 1.0f);
		world_pos /= world_pos.w;

		if (RSResourceManager::GetInstance()->GetCamera()->GetCameraIsPerspective())
		{
			ray_origin_out = RSResourceManager::GetInstance()->GetCamera()->GetPosition();
			ray_dir_out = glm::normalize(glm::vec3(world_pos) - ray_origin_out);
		}
		else // orthogonal
		{
			ray_origin_out = glm::vec3(world_pos);
			ray_dir_out = glm::normalize(glm::vec3(glm::inverse(view) * glm::vec4(0, 0, -1, 0)));
		}

	}
	void RSGizmo::GetRayScreen(glm::vec3& ray_origin_out, glm::vec3& ray_direction_out, const glm::vec2& screen_pos)
	{
		// Get camera matrices and properties
		const glm::mat4 view = RSResourceManager::GetInstance()->GetCamera()->GetView();
		const glm::mat4 proj = RSResourceManager::GetInstance()->GetCamera()->GetProj();
		const glm::vec3 camera_position = RSResourceManager::GetInstance()->GetCamera()->GetPosition();
		const glm::vec3 camera_forward = RSResourceManager::GetInstance()->GetCamera()->GetForwardDir();

		const bool is_perspective = RSResourceManager::GetInstance()->GetCamera()->GetCameraIsPerspective();

		float width = static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth());
		float height = static_cast<float>(RS_Application::mRSAPPLICATION->GetHeight());

		if (is_perspective)
		{
			// Perspective camera
			// Convert screen position to NDC
			glm::vec4 clip_coords;
			clip_coords.x = (2.0f * screen_pos.x) / width - 1.0f;
			clip_coords.y = 1.0f - (2.0f * screen_pos.y) / height;
			clip_coords.z = -1.0f; // Near plane
			clip_coords.w = 1.0f;

			// Unproject to eye space
			glm::vec4 eye_coords = glm::inverse(proj) * clip_coords;
			eye_coords = glm::vec4(eye_coords.x, eye_coords.y, -1.0f, 0.0f);

			// Convert to world space
			ray_direction_out = glm::normalize(glm::vec3(glm::inverse(view) * eye_coords));
			ray_origin_out = camera_position;
		}
		else
		{
			// Orthographic camera
			// Convert screen position to NDC
			glm::vec4 clip_coords;
			clip_coords.x = (2.0f * screen_pos.x) / width;
			clip_coords.y = 1.0f - (2.0f * screen_pos.y) / height;
			clip_coords.z = -1.0f; // Near plane
			clip_coords.w = 1.0f;

			// Unproject to world coordinates
			glm::vec4 world_coords = glm::inverse(proj * view) * clip_coords;
			world_coords /= world_coords.w;

			ray_direction_out = glm::normalize(camera_forward);
			ray_origin_out = glm::vec3(world_coords);
		}
	}
} // namespace SPH_MINT