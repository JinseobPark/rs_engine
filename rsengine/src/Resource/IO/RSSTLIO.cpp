/******************************************************************************/
/*!
\file   RSPipelineList.cpp
\author Jinseob Park
\date   2024/08/01

This file contains loading STL file for Rendering System.

*/
/******************************************************************************/
#include "pch.h"
#include "RSSTLIO.h"
//#include <memory>
//#include <map>

namespace RS_FILE_IO
{
	bool RSSTLIO::ImportStl(const std::string& path_, RSMeshData& data_)
	{
		FILE* fp = fopen(path_.c_str(), "rb");
		if (!fp) return false;

		// Read the 80-byte header
		char header[80];
		fread(header, sizeof(char), 80, fp);

		// Read the number of triangles
		uint32_t num_triangles;
		fread(&num_triangles, sizeof(uint32_t), 1, fp);
		const size_t is_binary_size = static_cast<size_t>(num_triangles) * static_cast<size_t>(50) + static_cast<size_t>(84);

		_fseeki64(fp, 0, SEEK_END);
		const size_t f_size = static_cast<size_t>(_ftelli64(fp));

		// Go back to the beginning of the file
		rewind(fp);

		bool result;

		if (is_binary_size == f_size)
			result = ReadDataBinary(fp, data_);
		else
			result = ReadDataAscii(fp, data_);

		// Close the file
		fclose(fp);

		return result;
	}

	bool RSSTLIO::ImportStl(const std::wstring& path_, RSMeshData& data_)
	{
		FILE* fp = _wfopen(path_.c_str(), L"rb");
		if (!fp) return false;

		// Read the 80-byte header
		char header[80];
		fread(header, sizeof(char), 80, fp);

		// Read the number of triangles
		uint32_t num_triangles;
		fread(&num_triangles, sizeof(uint32_t), 1, fp);
		const size_t is_binary_size = static_cast<size_t>(num_triangles) * static_cast<size_t>(50) + static_cast<size_t>(84);

		_fseeki64(fp, 0, SEEK_END);
		const size_t f_size = static_cast<size_t>(_ftelli64(fp));

		// Go back to the beginning of the file
		rewind(fp);

		bool result;

		if (is_binary_size == f_size)
			result = ReadDataBinary(fp, data_);
		else
			result = ReadDataAscii(fp, data_);

		// Close the file
		fclose(fp);

		return result;
	}




	bool RSSTLIO::ImportEmbeddedMeshData(const unsigned char mesh_data[], RSMeshData& data_)
	{
    int n_tra_size = 0;

    auto max = glm::vec3(-FLT_MAX);
    auto min = glm::vec3(FLT_MAX);

    data_.vertices.clear();

    // read 4 bytes from 81th byte to triangle size
    memcpy(&n_tra_size, mesh_data + 80, sizeof(n_tra_size));

    std::unordered_map<RSVertex, unsigned int, RSVertexHash> unique_vertices;
    unsigned int total_vertices = 0;

		for (int i = 0; i < n_tra_size; ++i)
		{
			//skip normal


			for (int n = 0; n < 3; ++n)
			{
				RSVertex vertex;
				memcpy(&vertex.position, mesh_data + 84 + i * 50 + 3 * sizeof(float) + n * 3 * sizeof(float), 3 * sizeof(float));
				if (!unique_vertices.contains(vertex))
				{
					unique_vertices[vertex] = total_vertices;
					data_.vertices.push_back(vertex);
					data_.indices.push_back(total_vertices);
					total_vertices++;
				}
				else
				{
					data_.indices.push_back(unique_vertices[vertex]);
				}
				max = glm::max(max, vertex.position);
				min = glm::min(min, vertex.position);
			}
		}

    data_.vertices.shrink_to_fit();
    data_.max = max;
    data_.min = min;

    //normal calculate
    for (int i = 0; i < data_.indices.size(); i += 3)
    {
      glm::vec3 v0 = data_.vertices[data_.indices[i]].position;
      glm::vec3 v1 = data_.vertices[data_.indices[i + 1]].position;
      glm::vec3 v2 = data_.vertices[data_.indices[i + 2]].position;
      glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
      data_.vertices[data_.indices[i]].normal += normal;
      data_.vertices[data_.indices[i + 1]].normal += normal;
      data_.vertices[data_.indices[i + 2]].normal += normal;
    }

    for (auto& vert : data_.vertices)
    {
      vert.normal = glm::normalize(vert.normal);
    }

    return true;
	}

	bool RSSTLIO::ReadDataBinary(FILE* fp_, RSMeshData& data_)
	{
		int n_tra_size = 0;

		auto max = glm::vec3(-FLT_MAX);
		auto min = glm::vec3(FLT_MAX);

		if (char header[80]; fread(header, sizeof(char), 80, fp_) < 80)
			return false;

		data_.vertices.clear();

		fread(&n_tra_size, sizeof(n_tra_size), 1, fp_);
		std::unordered_map<RSVertex, unsigned int, RSVertexHash> unique_vertices;
		unsigned int total_vertices = 0;
		for (int i = 0; i < n_tra_size; ++i)
		{
			//skip normal
			_fseeki64(fp_, 3 * sizeof(float), SEEK_CUR);

			for (int n = 0; n < 3; ++n)
			{
				RSVertex vertex;
				fread(&vertex.position, sizeof(float), 3, fp_);

				if (!unique_vertices.contains(vertex))
				{
					unique_vertices[vertex] = total_vertices;
					data_.vertices.push_back(vertex);
					data_.indices.push_back(total_vertices);
					total_vertices++;
				}
				else
				{
					data_.indices.push_back(unique_vertices[vertex]);
				}

				max = glm::max(max, vertex.position);
				min = glm::min(min, vertex.position);
			}
			_fseeki64(fp_, 2, SEEK_CUR);
		}


		data_.vertices.shrink_to_fit();
		data_.max = max;
		data_.min = min;

		//normal calculate
		for (int i = 0; i < data_.indices.size(); i += 3)
		{
			glm::vec3 v0 = data_.vertices[data_.indices[i]].position;
			glm::vec3 v1 = data_.vertices[data_.indices[i + 1]].position;
			glm::vec3 v2 = data_.vertices[data_.indices[i + 2]].position;

			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

			data_.vertices[data_.indices[i]].normal += normal;
			data_.vertices[data_.indices[i + 1]].normal += normal;
			data_.vertices[data_.indices[i + 2]].normal += normal;
		}

		for (auto& vert : data_.vertices)
		{
			vert.normal = glm::normalize(vert.normal);
		}

		return true;
	}


	bool RSSTLIO::ReadDataAscii(FILE* fp_, RSMeshData& data_)
	{
		char buf[256];
		char dels[] = " ";
		data_.vertices.clear();

		auto min = glm::vec3(FLT_MAX);
		auto max = glm::vec3(-FLT_MAX);

		std::unordered_map<RSVertex, unsigned int, RSVertexHash> unique_vertices;
		unsigned int vertex_stack = 0;
		while (fgets(buf, 256, fp_))
		{
			if (strlen(buf) == 0)
				continue;

			char* word = strtok(buf, dels);
			if (strcmp(word, "solid") == 0)
			{
			}
			else if (strcmp(word, "endsolid") == 0)
			{
			}
			else if (strcmp(word, "facet") == 0)
			{
			}
			else if (strcmp(word, "outer") == 0)
			{
			}
			else if (strcmp(word, "loop") == 0)
			{
			}
			else if (strcmp(word, "endloop") == 0)
			{
			}
			else if (strcmp(word, "endfacet") == 0)
			{
			}
			else if (strcmp(word, "vertex") == 0)
			{
				RSVertex vertex;
				for (int i = 0; i < 3; ++i)
				{
					word = strtok(nullptr, dels);
					vertex.position[i] = static_cast<float>(atof(word));
				}

				if (!unique_vertices.contains(vertex))
				{
					data_.vertices.push_back(vertex);
					unsigned int index = static_cast<unsigned int>(data_.vertices.size()) - 1;
					data_.indices.push_back(index);
					unique_vertices[vertex] = static_cast<unsigned int>(data_.vertices.size());
				}
				else
				{
					data_.indices.push_back(unique_vertices[vertex]);
				}

				max = glm::max(max, vertex.position);
				min = glm::min(min, vertex.position);

			}
		}
		data_.vertices.shrink_to_fit();

		//normal calculate
		std::vector<glm::vec3> accumulated_normals(data_.vertices.size(), glm::vec3(0.0f));
		for (int i = 0; i < data_.indices.size(); i += 3)
		{
			glm::vec3& v0 = data_.vertices[data_.indices[i]].position;
			glm::vec3& v1 = data_.vertices[data_.indices[i + 1]].position;
			glm::vec3& v2 = data_.vertices[data_.indices[i + 2]].position;

			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

			accumulated_normals[data_.indices[i]] += normal;
			accumulated_normals[data_.indices[i + 1]] += normal;
			accumulated_normals[data_.indices[i + 2]] += normal;
		}

		for (size_t i = 0; i < accumulated_normals.size(); i++)
		{
			if (glm::length(accumulated_normals[i]) != 0) { 
				data_.vertices[i].normal = glm::normalize(accumulated_normals[i]);
			}
			else {
				data_.vertices[i].normal = glm::vec3(0.0f, 0.0f, 1.0f);
			}
		}

		data_.max = max;
		data_.min = min;
		return true;
	}



}