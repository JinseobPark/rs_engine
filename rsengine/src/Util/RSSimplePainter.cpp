#include "pch.h"
#include "RSSimplePainter.h"
#include "glm/gtx/intersect.hpp"
#include <cmath>
#include <numbers>

namespace RS_Util
{
    void SimplePainter::DrawSimpleQuad()
    {
	    constexpr float cube_length = 0.125f;

        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glVertex3f(-cube_length, cube_length, 0);
        glVertex3f(-cube_length, -cube_length, 0);
        glVertex3f(cube_length, -cube_length, 0);
        glVertex3f(cube_length, cube_length, 0);
        glEnd();  // End of drawing color-cube
    }

    void SimplePainter::DrawSimpleQuadLine()
    {
        constexpr float cube_length = 0.125f;
        glBegin(GL_LINE_LOOP);
        glVertex3f(-cube_length, cube_length, 0);

        glVertex3f(-cube_length, -cube_length, 0);

        glVertex3f(cube_length, -cube_length, 0);

        glVertex3f(cube_length, cube_length, 0);

        glVertex3f(-cube_length, cube_length, 0);
        glEnd();

    }

    void SimplePainter::DrawSimpleCube()
    {
	    constexpr float cube_length = 0.125f;

        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        // Up face (y = 1.0f)
        glVertex3f(cube_length, cube_length, -cube_length);
        glVertex3f(-cube_length, cube_length, -cube_length);
        glVertex3f(-cube_length, cube_length, cube_length);
        glVertex3f(cube_length, cube_length, cube_length);

        // Bottom face (y = -1.0f)
        glVertex3f(cube_length, -cube_length, cube_length);
        glVertex3f(-cube_length, -cube_length, cube_length);
        glVertex3f(-cube_length, -cube_length, -cube_length);
        glVertex3f(cube_length, -cube_length, -cube_length);

        // Front face  (z = 1.0f)
        glVertex3f(cube_length, cube_length, cube_length);
        glVertex3f(-cube_length, cube_length, cube_length);
        glVertex3f(-cube_length, -cube_length, cube_length);
        glVertex3f(cube_length, -cube_length, cube_length);

        // Back face (z = -1.0f)
        glVertex3f(cube_length, -cube_length, -cube_length);
        glVertex3f(-cube_length, -cube_length, -cube_length);
        glVertex3f(-cube_length, cube_length, -cube_length);
        glVertex3f(cube_length, cube_length, -cube_length);

        // Left face (x = -1.0f)
        glVertex3f(-cube_length, cube_length, cube_length);
        glVertex3f(-cube_length, cube_length, -cube_length);
        glVertex3f(-cube_length, -cube_length, -cube_length);
        glVertex3f(-cube_length, -cube_length, cube_length);

        // Right face (x = 1.0f)
        glVertex3f(cube_length, cube_length, -cube_length);
        glVertex3f(cube_length, cube_length, cube_length);
        glVertex3f(cube_length, -cube_length, cube_length);
        glVertex3f(cube_length, -cube_length, -cube_length);
        glEnd();  // End of drawing color-cube
    }

    void SimplePainter::DrawSimpleCubeLine()
    {
        glColor3f(1, 1, 1);
        constexpr float cube_length = 0.125f;
        glBegin(GL_LINE_LOOP);
        glVertex3f(cube_length, cube_length, -cube_length);
        glVertex3f(-cube_length, cube_length, -cube_length);
        glVertex3f(-cube_length, cube_length, cube_length);
        glVertex3f(cube_length, cube_length, cube_length);
        glVertex3f(cube_length, cube_length, -cube_length);
        glEnd();  // End of drawing color-cube

        glBegin(GL_LINE_LOOP);
        glVertex3f(cube_length, -cube_length, cube_length);
        glVertex3f(-cube_length, -cube_length, cube_length);
        glVertex3f(-cube_length, -cube_length, -cube_length);
        glVertex3f(cube_length, -cube_length, -cube_length);
        glVertex3f(cube_length, -cube_length, cube_length);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(cube_length, cube_length, cube_length);
        glVertex3f(cube_length, -cube_length, cube_length);
        glVertex3f(-cube_length, cube_length, -cube_length);
        glVertex3f(-cube_length, -cube_length, -cube_length);
        glVertex3f(-cube_length, cube_length, cube_length);
        glVertex3f(-cube_length, -cube_length, cube_length);
        glVertex3f(cube_length, cube_length, -cube_length);
        glVertex3f(cube_length, -cube_length, -cube_length);
        glEnd();
    }

    void SimplePainter::DrawLine(const glm::vec3 vertex0, const glm::vec3 vertex1, const glm::vec3 color_)
    {
        glBegin(GL_LINES);
        glColor3f(color_.x, color_.y, color_.z);
        glVertex3f(vertex0.x, vertex0.y, vertex0.z);
        glVertex3f(vertex1.x, vertex1.y, vertex1.z);
        glEnd();
    }

    void SimplePainter::DrawLines(const std::vector<glm::vec3>& polygon_, const glm::vec3 color_)
    {
        if (!polygon_.empty())
        {
            glBegin(GL_LINE_LOOP);
            glColor3f(color_.x, color_.y, color_.z);
            for (const auto i : polygon_)
	            glVertex3f(i.x, i.y, i.z);
            glVertex3f(polygon_[0].x, polygon_[0].y, polygon_[0].z);
            glEnd();
        }
    }

    void SimplePainter::DrawQuad(const glm::vec3 vertex0, const glm::vec3 vertex1, const glm::vec3 vertex2,
                                 const glm::vec3 vertex3, const glm::vec3 color)
    {
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_QUADS);
        glVertex3f(vertex0.x, vertex0.y, vertex0.z);
        glVertex3f(vertex1.x, vertex1.y, vertex1.z);
        glVertex3f(vertex2.x, vertex2.y, vertex2.z);
        glVertex3f(vertex3.x, vertex3.y, vertex3.z);
        glEnd();  // End of drawing color-cube
    }

    void SimplePainter::DrawCube(const glm::vec3 vertex0, const glm::vec3 vertex1, const glm::vec3 vertex2,
                                 const glm::vec3 vertex3, const glm::vec3 vertex4, const glm::vec3 vertex5,
                                 const glm::vec3 vertex6, const glm::vec3 vertex7, const glm::vec3 color)
    {
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_QUADS);
        // Up face (y = 1.0f)
        glVertex3f(vertex7.x, vertex7.y, vertex7.z);
        glVertex3f(vertex4.x, vertex4.y, vertex4.z);
        glVertex3f(vertex5.x, vertex5.y, vertex5.z);
        glVertex3f(vertex6.x, vertex6.y, vertex6.z);

        // Bottom face (y = -1.0f)
        glVertex3f(vertex0.x, vertex0.y, vertex0.z);
        glVertex3f(vertex3.x, vertex3.y, vertex3.z);
        glVertex3f(vertex2.x, vertex2.y, vertex2.z);
        glVertex3f(vertex1.x, vertex1.y, vertex1.z);

        // Front face  (z = 1.0f)
        glVertex3f(vertex4.x, vertex4.y, vertex4.z);
        glVertex3f(vertex0.x, vertex0.y, vertex0.z);
        glVertex3f(vertex1.x, vertex1.y, vertex1.z);
        glVertex3f(vertex5.x, vertex5.y, vertex5.z);

        // Back face (z = -1.0f)
        glVertex3f(vertex6.x, vertex6.y, vertex6.z);
        glVertex3f(vertex2.x, vertex2.y, vertex2.z);
        glVertex3f(vertex3.x, vertex3.y, vertex3.z);
        glVertex3f(vertex7.x, vertex7.y, vertex7.z);

        // Left face (x = -1.0f)
        glVertex3f(vertex7.x, vertex7.y, vertex7.z);
        glVertex3f(vertex3.x, vertex3.y, vertex3.z);
        glVertex3f(vertex0.x, vertex0.y, vertex0.z);
        glVertex3f(vertex4.x, vertex4.y, vertex4.z);

        // Right face (x = 1.0f)
        glVertex3f(vertex5.x, vertex5.y, vertex5.z);
        glVertex3f(vertex1.x, vertex1.y, vertex1.z);
        glVertex3f(vertex2.x, vertex2.y, vertex2.z);
        glVertex3f(vertex6.x, vertex6.y, vertex6.z);
        glEnd();  // End of drawing color-cube
    }

    void SimplePainter::DrawCubeLine(const glm::vec3 vertex0, const glm::vec3 vertex1, const glm::vec3 vertex2,
                                     const glm::vec3 vertex3, const glm::vec3 vertex4, const glm::vec3 vertex5,
                                     const glm::vec3 vertex6, const glm::vec3 vertex7, const glm::vec3 color)
    {
        glColor3f(color.x, color.y, color.z);
        // Up Face
        glBegin(GL_LINE_LOOP);
        glVertex3f(vertex7.x, vertex7.y, vertex7.z);
        glVertex3f(vertex4.x, vertex4.y, vertex4.z);
        glVertex3f(vertex5.x, vertex5.y, vertex5.z);
        glVertex3f(vertex6.x, vertex6.y, vertex6.z);
        glVertex3f(vertex7.x, vertex7.y, vertex7.z);
        glEnd();

        // Bottom face (y = -1.0f)
        glBegin(GL_LINE_LOOP);
        glVertex3f(vertex0.x, vertex0.y, vertex0.z);
        glVertex3f(vertex3.x, vertex3.y, vertex3.z);
        glVertex3f(vertex2.x, vertex2.y, vertex2.z);
        glVertex3f(vertex1.x, vertex1.y, vertex1.z);
        glVertex3f(vertex0.x, vertex0.y, vertex0.z);
        glEnd();


        glBegin(GL_LINES);
        glVertex3f(vertex4.x, vertex4.y, vertex4.z);
        glVertex3f(vertex0.x, vertex0.y, vertex0.z);
        glVertex3f(vertex1.x, vertex1.y, vertex1.z);
        glVertex3f(vertex5.x, vertex5.y, vertex5.z);
        glVertex3f(vertex6.x, vertex6.y, vertex6.z);
        glVertex3f(vertex2.x, vertex2.y, vertex2.z);
        glVertex3f(vertex3.x, vertex3.y, vertex3.z);
        glVertex3f(vertex7.x, vertex7.y, vertex7.z);
        glEnd();

    }

    void SimplePainter::DrawCubeLine(const glm::vec3 min_, const glm::vec3 max_, const glm::vec3 color)
    {
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_LINE_LOOP);
        glVertex3f(min_.x, max_.y, min_.z);
        glVertex3f(max_.x, max_.y, min_.z);
        glVertex3f(max_.x, max_.y, max_.z);
        glVertex3f(min_.x, max_.y, max_.z);
        glVertex3f(min_.x, max_.y, min_.z);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3f(min_.x, min_.y, min_.z);
        glVertex3f(max_.x, min_.y, min_.z);
        glVertex3f(max_.x, min_.y, max_.z);
        glVertex3f(min_.x, min_.y, max_.z);
        glVertex3f(min_.x, min_.y, min_.z);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(min_.x, min_.y, min_.z);
        glVertex3f(min_.x, max_.y, min_.z);
        glVertex3f(min_.x, min_.y, max_.z);
        glVertex3f(min_.x, max_.y, max_.z);
        glVertex3f(max_.x, min_.y, min_.z);
        glVertex3f(max_.x, max_.y, min_.z);
        glVertex3f(max_.x, min_.y, max_.z);
        glVertex3f(max_.x, max_.y, max_.z);
        glEnd();
    }

    void SimplePainter::DrawPolygonIntersectCube(glm::vec3 point_, glm::vec3  normal_, glm::vec3 vertex0,
                                                 glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3,
                                                 glm::vec3 vertex4, glm::vec3 vertex5, glm::vec3 vertex6,
                                                 glm::vec3 vertex7)
    {
        std::vector<std::pair<int, int>> lines_index; //fixed
        lines_index.reserve(12);
        lines_index.push_back(std::make_pair<int, int>(0, 1));
        lines_index.push_back(std::make_pair<int, int>(1, 2));
        lines_index.push_back(std::make_pair<int, int>(2, 3));
        lines_index.push_back(std::make_pair<int, int>(3, 0));
        lines_index.push_back(std::make_pair<int, int>(0, 4));
        lines_index.push_back(std::make_pair<int, int>(1, 5));
        lines_index.push_back(std::make_pair<int, int>(2, 6));
        lines_index.push_back(std::make_pair<int, int>(3, 7));
        lines_index.push_back(std::make_pair<int, int>(4, 5));
        lines_index.push_back(std::make_pair<int, int>(5, 6));
        lines_index.push_back(std::make_pair<int, int>(6, 7));
        lines_index.push_back(std::make_pair<int, int>(7, 4));

        glm::vec3 cube_vertices[8] = {
            vertex0, vertex1, vertex2, vertex3, vertex4, vertex5, vertex6, vertex7
        };

        std::vector<glm::vec3> inter_points;
        inter_points.clear();

        double d_value = -glm::dot(point_, normal_);

        for (auto& [first, second] : lines_index)
        {
            glm::vec3 p0 = cube_vertices[first];
            glm::vec3 p1 = cube_vertices[second];
            if (glm::dot(normal_, (p1 - p0)) == 0) continue;
            if (double t = (-d_value - glm::dot(normal_, p0) / glm::dot(normal_, (p1 - p0))); t >= 0 && t <= 1)
            {
                glm::vec3 inter_point = p0 + glm::vec3(t * (p1 - p0).x, t * (p1 - p0).y, t * (p1 - p0).z);
                inter_points.push_back(inter_point);
            }
        }

        if (inter_points.size() > 1)
        {
            std::sort(inter_points.begin() + 1, inter_points.end(), [point_, normal_, &inter_points](const glm::vec3& a, const glm::vec3& b) {
                const glm::vec3 fv = glm::normalize(inter_points[0] - point_);
                const glm::vec3 av = glm::normalize(a - point_);
                const glm::vec3 bv = glm::normalize(b - point_);

                const double cosa = glm::dot(fv, av);
                const double cosb = glm::dot(fv, bv);
                const double sina = glm::dot(normal_, glm::cross(fv, av));
                const double sinb = glm::dot(normal_, glm::cross(fv, bv));
                const double anglea = fmod(glm::degrees(atan2(sina, cosa)) + 360.0, 360.0);
                const double angleb = fmod(glm::degrees(atan2(sinb, cosb)) + 360.0, 360.0);
                return anglea < angleb;
                });

            glBegin(GL_POLYGON);
            for (auto& inter_point : inter_points)
	            glVertex3f(inter_point.x, inter_point.y, inter_point.z);
            glVertex3f(inter_points[0].x, inter_points[0].y, inter_points[0].z);
            glEnd();
        }

    }
    void SimplePainter::DrawPolygon(const std::vector<glm::vec3>& polygon_, const glm::vec3 color_)
    {
        if (!polygon_.empty())
        {
            glColor3f(color_.x, color_.y, color_.z);
            glBegin(GL_POLYGON);
            for (const auto& i : polygon_)
	            glVertex3f(i.x, i.y, i.z);
            glVertex3f(polygon_[0].x, polygon_[0].y, polygon_[0].z);
            glEnd();
        }
    }
    void SimplePainter::DrawPoints(const std::vector<glm::vec3>* points, const float size)
    {
        // using batch optimize skill, instancing
        const unsigned int instance_point_count = static_cast<unsigned int>(points->size()); // point count to render

        unsigned int instancingVBO;
        glGenBuffers(1, &instancingVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instancingVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * instance_point_count, points->data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        unsigned int pointVAO;
        glGenVertexArrays(1, &pointVAO);
        glBindVertexArray(pointVAO);

        // Set attribute 
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, instancingVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glVertexAttribDivisor(0, 1);

        glPointSize(size);
        glBindVertexArray(pointVAO);
        glDrawArraysInstanced(GL_POINTS, 0, 1, instance_point_count); // instance_point_count points of 1 vertices each
        glBindVertexArray(0);

        // Deallocate buffer info.
        glDeleteVertexArrays(1, &pointVAO);
        pointVAO = 0;
        glDeleteBuffers(1, &instancingVBO);
        instancingVBO = 0;
    }

    void SimplePainter::DrawTriangles(const std::vector<glm::vec3>* points)
    {
        const unsigned int  triangles_count = static_cast<unsigned int>(points->size());

        unsigned int triangles_vao, triangles_vbo;
        glGenVertexArrays(1, &triangles_vao);
        glGenBuffers(1, &triangles_vbo);
        glBindVertexArray(triangles_vao);
        glBindBuffer(GL_ARRAY_BUFFER, triangles_vbo);
        glBufferData(GL_ARRAY_BUFFER, triangles_count * sizeof(glm::vec3), points->data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);


        glBindVertexArray(triangles_vao);
        glDrawArrays(GL_TRIANGLES, 0, triangles_count);
        glBindVertexArray(0);

        // Deallocate buffer info.
        glDeleteVertexArrays(1, &triangles_vao);
        glDeleteBuffers(1, &triangles_vbo);
        triangles_vao = 0;
        triangles_vbo = 0;
    }

    void SimplePainter::DrawCubes(const std::vector<glm::vec3>* points, const float size)
    {
        const unsigned int instance_point_count = static_cast<unsigned int>(points->size());

        unsigned int instancing_vbo;
        glGenBuffers(1, &instancing_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, instancing_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * instance_point_count, points->data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        const float cube_vertices[] = {
            //  position(3)      // normal vector(3)
         // back face
         -size, -size, -size,  0.0f,  0.0f, -1.0f, // bottom-left
          size,  size, -size,  0.0f,  0.0f, -1.0f, // top-right
          size, -size, -size,  0.0f,  0.0f, -1.0f, // bottom-right         
          size,  size, -size,  0.0f,  0.0f, -1.0f, // top-right
         -size, -size, -size,  0.0f,  0.0f, -1.0f, // bottom-left
         -size,  size, -size,  0.0f,  0.0f, -1.0f, // top-left
         // front face
         -size, -size,  size,  0.0f,  0.0f,  1.0f, // bottom-left
          size, -size,  size,  0.0f,  0.0f,  1.0f, // bottom-right
          size,  size,  size,  0.0f,  0.0f,  1.0f, // top-right
          size,  size,  size,  0.0f,  0.0f,  1.0f, // top-right
         -size,  size,  size,  0.0f,  0.0f,  1.0f, // top-left
         -size, -size,  size,  0.0f,  0.0f,  1.0f, // bottom-left
         // left face
         -size,  size,  size, -1.0f,  0.0f,  0.0f, // top-right
         -size,  size, -size, -1.0f,  0.0f,  0.0f, // top-left
         -size, -size, -size, -1.0f,  0.0f,  0.0f, // bottom-left
         -size, -size, -size, -1.0f,  0.0f,  0.0f, // bottom-left
         -size, -size,  size, -1.0f,  0.0f,  0.0f, // bottom-right
         -size,  size,  size, -1.0f,  0.0f,  0.0f, // top-right
         // right face
          size,  size,  size,  1.0f,  0.0f,  0.0f, // top-left
          size, -size, -size,  1.0f,  0.0f,  0.0f, // bottom-right
          size,  size, -size,  1.0f,  0.0f,  0.0f, // top-right         
          size, -size, -size,  1.0f,  0.0f,  0.0f, // bottom-right
          size,  size,  size,  1.0f,  0.0f,  0.0f, // top-left
          size, -size,  size,  1.0f,  0.0f,  0.0f, // bottom-left     
          // bottom face
          -size, -size, -size,  0.0f, -1.0f,  0.0f, // top-right
           size, -size, -size,  0.0f, -1.0f,  0.0f, // top-left
           size, -size,  size,  0.0f, -1.0f,  0.0f, // bottom-left
           size, -size,  size,  0.0f, -1.0f,  0.0f, // bottom-left
          -size, -size,  size,  0.0f, -1.0f,  0.0f, // bottom-right
          -size, -size, -size,  0.0f, -1.0f,  0.0f, // top-right
          // top face
          -size,  size, -size,  0.0f,  1.0f,  0.0f, // top-left
           size,  size , size,  0.0f,  1.0f,  0.0f, // bottom-right
           size,  size, -size,  0.0f,  1.0f,  0.0f, // top-right     
           size,  size,  size,  0.0f,  1.0f,  0.0f, // bottom-right
          -size,  size, -size,  0.0f,  1.0f,  0.0f, // top-left
          -size,  size,  size,  0.0f,  1.0f,  0.0f  // bottom-left        
        };

        unsigned int cube_vao, cube_vbo;
        glGenVertexArrays(1, &cube_vao);
        glGenBuffers(1, &cube_vbo);
        glBindVertexArray(cube_vao);
        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

        // Set attribute 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, instancing_vbo);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glVertexAttribDivisor(2, 1);

        // Unbind 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glBindVertexArray(cube_vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instance_point_count); // instance_point_count points of 36 vertices each
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &cube_vao);
        glDeleteBuffers(1, &cube_vbo);
        glDeleteBuffers(1, &instancing_vbo);
        cube_vao = 0;
		cube_vbo = 0;
		instancing_vbo = 0;
    }

    void SimplePainter::RenderCube()
    {
        unsigned int cube_vao = 0;
        unsigned int cube_vbo;

        // initialize (if necessary)
        if (cube_vao == 0)
        {
	        constexpr float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                 // bottom face
                 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 // top face
                 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
            };
            glGenVertexArrays(1, &cube_vao);
            glGenBuffers(1, &cube_vbo);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cube_vao);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        // render Cube
        glBindVertexArray(cube_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &cube_vao);
        glDeleteBuffers(1, &cube_vbo);
        cube_vao = 0;
		cube_vbo = 0;
    }
    void SimplePainter::RenderQuad()
    {
        unsigned int quad_vao = 0;
        unsigned int quad_vbo;
        if (quad_vao == 0)
        {
	        constexpr float quad_vertices[] = {
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
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &quad_vao);
        glDeleteBuffers(1, &quad_vbo);
        quad_vao = 0;
		quad_vbo = 0;
    }

    void SimplePainter::RenderSphere()
    {
        unsigned int sphere_vao = 0;
        unsigned int vbo, ebo;

        glGenVertexArrays(1, &sphere_vao);

        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        constexpr unsigned int X_SEGMENTS = 64;
        constexpr unsigned int Y_SEGMENTS = 64;
        constexpr float n_pi = std::numbers::pi_v<float>;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                const float x_segment = static_cast<float>(x) / static_cast<float>(X_SEGMENTS);
                const float y_segment = static_cast<float>(y) / static_cast<float>(Y_SEGMENTS);
                const float x_pos = std::cos(x_segment * 2.0f * n_pi) * std::sin(y_segment * n_pi);
                const float y_pos = std::cos(y_segment * n_pi);
                const float z_pos = std::sin(x_segment * 2.0f * n_pi) * std::sin(y_segment * n_pi);

                positions.emplace_back(x_pos, y_pos, z_pos);
                uv.emplace_back(x_segment, y_segment);
                normals.emplace_back(x_pos, y_pos, z_pos);
            }
        }

        bool odd_row = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!odd_row) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            odd_row = !odd_row;
        }
        const unsigned int index_count = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (!normals.empty())
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (!uv.empty())
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphere_vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        const unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

        glBindVertexArray(sphere_vao);
        glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, nullptr);


        glDeleteVertexArrays(1, &sphere_vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
		sphere_vao = 0;
		vbo = 0;
		ebo = 0;
    }

    void SimplePainter::Draw2DBox(float vertices_[])
    {
		unsigned int vbo, vao;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices_, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);                                                              
        vao = 0;
		vbo = 0;
    }

    //
    //bool SimplePainter::in_circle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    //{
    //    double ax = a.x - c.x, ay = a.y - c.y;
    //    double bx = b.x - c.x, by = b.y - c.y;
    //    double denominator = 2 * (ax * by - ay * bx);
    //    if (denominator == 0) return false;
    //    double cx = (ax * ax + ay * ay) * by - (bx * bx + by * by) * ay;
    //    double cy = (bx * bx + by * by) * ax - (ax * ax + ay * ay) * bx;
    //    double x = cx / denominator, y = cy / denominator;
    //    glm::vec3 center = { c.x + x, c.y + y };
    //    double radius = sqrt((center.x - a.x) * (center.x - a.x) + (center.y - a.y) * (center.y - a.y));
    //    for (int i = 0; i < interPoints.size(); i++) {
    //        if (i == a.p1 || i == a.p2 || i == a.p3 || i == b.p1 || i == b.p2 || i == b.p3 || i == c.p1 || i == c.p2 || i == c.p3) continue;
    //        if (sqrt((center.x - interPoints[i].x) * (center.x - interPoints[i].x) + (center.y - interPoints[i].y) * (center.y - interPoints[i].y)) < radius) {
    //            return false;
    //        }
    //    }
    //    return true;
    //}
}
