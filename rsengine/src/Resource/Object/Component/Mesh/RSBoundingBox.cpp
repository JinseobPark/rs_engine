#include "pch.h"
#include "RSBoundingBox.h"
#include "Util/RSUtilFunctions.h"

namespace RS_Mesh
{
	RSBoundingBox::RSBoundingBox()
	{
		m_triangles.resize(12);
	}

	void RSBoundingBox::SetMinMax(const glm::vec3& min_, const glm::vec3& max_)
	{
		glm::vec3 temp_min = min_;
		glm::vec3 temp_max = max_;

		// if min is bigger than max, swap
		if (temp_min.x > temp_max.x)
			SwapData(&temp_min.x, &temp_max.x);

		if (temp_min.y > temp_max.y)
			SwapData(&temp_min.y, &temp_max.y);

		if (temp_min.z > temp_max.z)
			SwapData(&temp_min.z, &temp_max.z);

		// if min xyz is same with max xyz, minimum distance is 0.1f
		//if (temp_min.x == temp_max.x)
		{
			temp_min.x -= 0.01f;
			temp_max.x += 0.01f;
		}

		//if (temp_min.y == temp_max.y)
		{
			temp_min.y -= 0.01f;
			temp_max.y += 0.01f;
		}

		//if (temp_min.z == temp_max.z)
		{
			temp_min.z -= 0.01f;
			temp_max.z += 0.01f;
		}

		this->min = temp_min;
		this->max = temp_max;

		// Front
		m_triangles[0].v0 = glm::vec3(min.x, min.y, min.z);
		m_triangles[0].v1 = glm::vec3(max.x, min.y, min.z);
		m_triangles[0].v2 = glm::vec3(max.x, max.y, min.z);

		m_triangles[1].v0 = glm::vec3(min.x, min.y, min.z);
		m_triangles[1].v1 = glm::vec3(max.x, max.y, min.z);
		m_triangles[1].v2 = glm::vec3(min.x, max.y, min.z);

		// Back
		m_triangles[2].v0 = glm::vec3(min.x, min.y, max.z);
		m_triangles[2].v1 = glm::vec3(max.x, min.y, max.z);
		m_triangles[2].v2 = glm::vec3(max.x, max.y, max.z);

		m_triangles[3].v0 = glm::vec3(min.x, min.y, max.z);
		m_triangles[3].v1 = glm::vec3(max.x, max.y, max.z);
		m_triangles[3].v2 = glm::vec3(min.x, max.y, max.z);

		// Left
		m_triangles[4].v0 = glm::vec3(min.x, min.y, min.z);
		m_triangles[4].v1 = glm::vec3(min.x, min.y, max.z);
		m_triangles[4].v2 = glm::vec3(min.x, max.y, max.z);

		m_triangles[5].v0 = glm::vec3(min.x, min.y, min.z);
		m_triangles[5].v1 = glm::vec3(min.x, max.y, max.z);
		m_triangles[5].v2 = glm::vec3(min.x, max.y, min.z);

		// Right
		m_triangles[6].v0 = glm::vec3(max.x, min.y, min.z);
		m_triangles[6].v1 = glm::vec3(max.x, min.y, max.z);
		m_triangles[6].v2 = glm::vec3(max.x, max.y, max.z);

		m_triangles[7].v0 = glm::vec3(max.x, min.y, min.z);
		m_triangles[7].v1 = glm::vec3(max.x, max.y, max.z);
		m_triangles[7].v2 = glm::vec3(max.x, max.y, min.z);

		// Top
		m_triangles[8].v0 = glm::vec3(min.x, max.y, min.z);
		m_triangles[8].v1 = glm::vec3(max.x, max.y, min.z);
		m_triangles[8].v2 = glm::vec3(max.x, max.y, max.z);

		m_triangles[9].v0 = glm::vec3(min.x, max.y, min.z);
		m_triangles[9].v1 = glm::vec3(max.x, max.y, max.z);
		m_triangles[9].v2 = glm::vec3(min.x, max.y, max.z);

		// Bottom
		m_triangles[10].v0 = glm::vec3(min.x, min.y, min.z);
		m_triangles[10].v1 = glm::vec3(max.x, min.y, min.z);
		m_triangles[10].v2 = glm::vec3(max.x, min.y, max.z);

		m_triangles[11].v0 = glm::vec3(min.x, min.y, min.z);
		m_triangles[11].v1 = glm::vec3(max.x, min.y, max.z);
		m_triangles[11].v2 = glm::vec3(min.x, min.y, max.z);


	}

	bool RSBoundingBox::IsCheckCollisionRayCast(const glm::vec3& ray_origin, const glm::vec3& ray_direction) const
	{
		// Ray Casting
		for (const auto& [v0, v1, v2] : m_triangles)
		{
			if (RS_Util::GetDistanceRayTriangle(ray_origin, ray_direction, v0, v1, v2) > 0.0f)
				return true;
		}

		return false;
	}

	void RSBoundingBox::DrawBoundingBox() const
	{
		// Draw Cube Line by Quad line with cube min, max
		unsigned int cube_vao = 0, cube_vbo = 0;

		// front, back, left, right, top, bottom
		const float vertices[] = 
		{
			min.x, min.y, min.z,
			max.x, min.y, min.z,
			max.x, max.y, min.z,
			min.x, max.y, min.z,

			min.x, min.y, max.z,
			max.x, min.y, max.z,
			max.x, max.y, max.z,
			min.x, max.y, max.z,

			min.x, min.y, min.z,
			min.x, min.y, max.z,
			min.x, max.y, max.z,
			min.x, max.y, min.z,

			max.x, min.y, min.z,
			max.x, min.y, max.z,
			max.x, max.y, max.z,
			max.x, max.y, min.z,

			min.x, max.y, min.z,
			max.x, max.y, min.z,
			max.x, max.y, max.z,
			min.x, max.y, max.z,

			min.x, min.y, min.z,
			max.x, min.y, min.z,
			max.x, min.y, max.z,
			min.x, min.y, max.z
		};

		glGenVertexArrays(1, &cube_vao);
		glGenBuffers(1, &cube_vbo);
		glBindVertexArray(cube_vao);
		glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDrawArrays(GL_LINE_LOOP, 4, 4);
		glDrawArrays(GL_LINE_LOOP, 8, 4);
		glDrawArrays(GL_LINE_LOOP, 12, 4);
		glDrawArrays(GL_LINE_LOOP, 16, 4);
		glDrawArrays(GL_LINE_LOOP, 20, 4);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &cube_vao);
		cube_vao = 0;
		glDeleteBuffers(1, &cube_vbo);
		cube_vbo = 0;
	}

	void RSBoundingBox::SwapData(float* a, float* b)
	{
		const float temp = *a;
		*a = *b;
		*b = temp;
	}

}
