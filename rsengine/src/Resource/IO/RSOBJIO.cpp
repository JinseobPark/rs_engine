#include "pch.h"
#include "RSOBJIO.h"
#include <sstream>
#include <fstream>

namespace RS_FILE_IO
{
	bool RSOBJIO::ImportObjOld(const std::string& path_, RSMeshData& data_)
	{
		std::ifstream file(path_);
		if (!file.is_open())
		{
			return false;
		}

		std::vector<glm::vec3> positions, normals;
		std::vector<glm::vec2> tex_coord;
		std::unordered_map<RSVertex, unsigned int, RSVertexHash> unique_vertices;

		std::string line;
		while (getline(file, line))
		{
			std::istringstream ss(line);
			std::string line_header;
			ss >> line_header;

			if (line_header == "v")
			{
				glm::vec3 position;
				ss >> position.x >> position.y >> position.z;
				positions.push_back(position);
			}
			else if (line_header == "vt")
			{
				glm::vec2 texCoord;
				ss >> texCoord.x >> texCoord.y;
				tex_coord.push_back(texCoord);
			}
			else if (line_header == "vn")
			{
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (line_header == "f")
			{
				std::string vertex_text;
				for (int i = 0; i < 3; i++)
				{
					ss >> vertex_text;
					std::ranges::replace(vertex_text, '/', ' ');
					std::istringstream vertex_stream(vertex_text);
					int vertex_idx, tex_idx, normal_idx;
					vertex_stream >> vertex_idx >> tex_idx >> normal_idx;

					RSVertex vertex(positions[vertex_idx - 1], normals[normal_idx - 1], tex_coord[tex_idx - 1]);
					if (!unique_vertices.contains(vertex))
					{
						unique_vertices[vertex] = static_cast<unsigned int>(data_.vertices.size());
						data_.vertices.push_back(vertex);
					}
					data_.indices.push_back(unique_vertices[vertex]);
				}
			}
		}
		return true;
	}

	bool RSOBJIO::ImportObj(const std::string& path_, RSMeshData& data_)
	{
		std::wifstream file(path_.c_str());
		std::wstring mesh_matlib;

		std::vector<std::wstring> mesh_mtllibs;
		std::vector<int> position_indices, normal_indices, uv_indices;

		bool has_uv = false;
		bool has_normal = false;

		std::wstring temp_mesh_material;


		if (file)
		{
			std::unordered_map<RSVertex, unsigned int, RSVertexHash> unique_vertices;

			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texcoords;
			std::wstring face;
			wchar_t check_char;
			int temp_uv_index = 0;
			int temp_normal_index = 0;
			int temp_position_index = 0;
			int triangle_count = 0, total_vertices = 0, total_triangles = 0;

			while (file)
			{
				check_char = file.get();
				switch (check_char)
				{
				case '#':
					check_char = file.get();
					while (check_char != '\n')
						check_char = file.get();
					break;
				case 'v':
					check_char = file.get();
					if (check_char == ' ')
					{
						glm::vec3 temp_vertex;
						file >> temp_vertex.x >> temp_vertex.y >> temp_vertex.z;
						positions.push_back(temp_vertex);
					}
					else if (check_char == 't')
					{
						glm::vec2 temp_uv;
						file >> temp_uv.x >> temp_uv.y;
						texcoords.push_back(temp_uv);
						has_uv = true;
					}
					else if (check_char == 'n')
					{
						glm::vec3 temp_normal;
						file >> temp_normal.x >> temp_normal.y >> temp_normal.z;
						normals.push_back(temp_normal);
						has_normal = true;
					}
					break;
				case 'f':
					check_char = file.get();
					if (check_char == ' ')
					{
						face = L"";
						triangle_count = 0;

						check_char = file.get();
						while (check_char != '\n')
						{
							face += check_char;
							check_char = file.get();
							if (check_char == ' ')
								triangle_count++;
						}

						// check for space at the end of the line
						if(face[face.length() - 1] == ' ')
							triangle_count--;	

						triangle_count -= 1;

						std::wstringstream ss(face);

						if (!face.empty())
						{
							std::wstring vert_def;
							int first_vertex_index, last_vertex_index; // for holding the first and last vertex index

							for (int i = 0; i < 3; ++i)
							{
								ss >> vert_def; //Get vertex definition

								std::wstring vert_part;
								int which_part = 0; // (v, vt, vn)

								//parse this string
								for (int j = 0; j < vert_def.length(); ++j)
								{
									if(vert_def[j] != '/')
										vert_part += vert_def[j];

									if (vert_def[j] == '/' || j == vert_def.length() - 1)
									{
										std::wistringstream wstring_to_int(vert_part);

										if (which_part == 0) // position
										{
											wstring_to_int >> temp_position_index;
											temp_position_index -= 1; // in wavefront obj all indices start at 1, not zero

											if (j == vert_def.length() - 1)
												temp_normal_index = temp_uv_index = 0;
										}
										else if (which_part == 1) // uv
										{
											if (!vert_part.empty())
											{
												wstring_to_int >> temp_uv_index;
												temp_uv_index -= 1;
											}
											else
												temp_uv_index = 0;

											if(j == vert_def.length() - 1)
												temp_normal_index = 0;
										}
										else if (which_part == 2) // normal
										{
											wstring_to_int >> temp_normal_index;
											temp_normal_index -= 1;
										}

										vert_part = L"";
										which_part++;
									}

								}


								// Check if this vertex is already in the list of vertices
								RSVertex vertex;
									vertex.position = positions[temp_position_index];
									vertex.normal = has_normal ? normals[temp_normal_index] : glm::vec3(0.0f);
									vertex.texture_coordinates = has_uv ? texcoords[temp_uv_index] : glm::vec2(0.0f);

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

								if (i == 0)
									first_vertex_index = unique_vertices[vertex];
								else if (i == 2)
									last_vertex_index = unique_vertices[vertex];
							}
							
							total_triangles += 1;

							for (int k = 0; k < triangle_count - 1; ++k)
							{
								data_.indices.push_back(first_vertex_index);
								data_.indices.push_back(last_vertex_index);
								ss >> vert_def; //Get vertex definition

								std::wstring vert_part;
								int which_part = 0; // (v, vt, vn)

								//parse this string
								for (int j = 0; j < vert_def.length(); ++j)
								{
									if(vert_def[j] != '/')
										vert_part += vert_def[j];

									if (vert_def[j] == '/' || j == vert_def.length() - 1)
									{
										std::wistringstream wstring_to_int(vert_part);

										if (which_part == 0) // position
										{
											wstring_to_int >> temp_position_index;
											temp_position_index -= 1; // in wavefront obj all indices start at 1, not zero

											if (j == vert_def.length() - 1)
												temp_normal_index = temp_uv_index = 0;
										}
										else if (which_part == 1) // uv
										{
											if (!vert_part.empty())
											{
												wstring_to_int >> temp_uv_index;
												temp_uv_index -= 1;
											}
											else
												temp_uv_index = 0;

											if(j == vert_def.length() - 1)
												temp_normal_index = 0;
										}
										else if (which_part == 2) // normal
										{
											wstring_to_int >> temp_normal_index;
											temp_normal_index -= 1;
										}

										vert_part = L"";
										which_part++;
									}
								}

								// Check if this vertex is already in the list of vertices
								RSVertex vertex;
									vertex.position = positions[temp_position_index];
									vertex.normal = has_normal ? normals[temp_normal_index] : glm::vec3(0.0f);
									vertex.texture_coordinates = has_uv ? texcoords[temp_uv_index] : glm::vec2(0.0f);

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

								last_vertex_index = unique_vertices[vertex];
								total_triangles += 1;
							}

						}
					}
					break;	
				default:
					break;
				}
			}
			data_.vertices.shrink_to_fit();
			data_.indices.shrink_to_fit();
			file.close();
		}
		else
		{
			return false;
		}

		if (!has_uv)
			std::cout << "texcoord is not exist\n";
		else
		{
			data_.has_texcoord = true;
		}
		if (!has_normal)
		{
			std::vector<glm::vec3> accumulatedNormals(data_.vertices.size(), glm::vec3(0.0f));
			for (int i = 0; i < data_.indices.size(); i += 3)
			{
				glm::vec3 v0 = data_.vertices[data_.indices[i]].position;
				glm::vec3 v1 = data_.vertices[data_.indices[i + 1]].position;
				glm::vec3 v2 = data_.vertices[data_.indices[i + 2]].position;

				glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

				accumulatedNormals[data_.indices[i]] += normal;
				accumulatedNormals[data_.indices[i + 1]] += normal;
				accumulatedNormals[data_.indices[i + 2]] += normal;
			}

			for (int i = 0; i < accumulatedNormals.size(); i++)
			{
				data_.vertices[i].normal = glm::normalize(accumulatedNormals[i]);
			}
		}

		// Set min, max data
		auto min =  glm::vec3(FLT_MAX);
		auto max = glm::vec3(-FLT_MAX);

		for (auto& vert : data_.vertices)
		{
			min = glm::min(min, vert.position);
			max = glm::max(max, vert.position);
		}

		data_.min = min;
		data_.max = max;


		return true;
	}

	bool RSOBJIO::ImportObj(std::wstring path_, RSMeshData& data_)
	{
		std::wifstream file(path_.c_str());
		std::wstring mesh_matlib;


		std::vector<std::wstring> mesh_mtllibs;
		std::vector<int> position_indices, normal_indices, uv_indices;

		bool has_uv = false;
		bool has_normal = false;
		bool compute_normal = false;

		std::wstring temp_mesh_material;



		if (file)
		{
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texcoords;
			int temp_position_index = 0, temp_normal_index = 0, temp_uv_index = 0;
			int triangle_count = 0, total_vertices = 0, total_triangles = 0;
			wchar_t check_char;
			std::wstring face;
			std::unordered_map<RSVertex, unsigned int, RSVertexHash> unique_vertices;
			while (file)
			{
				check_char = file.get();
				switch (check_char)
				{
				case '#':
					check_char = file.get();
					while (check_char != '\n')
						check_char = file.get();
					break;
				case 'v':
					check_char = file.get();
					if (check_char == ' ')
					{
						glm::vec3 temp_vertex;
						file >> temp_vertex.x >> temp_vertex.y >> temp_vertex.z;
						positions.push_back(temp_vertex);
					}
					else if (check_char == 't')
					{
						glm::vec2 temp_uv;
						file >> temp_uv.x >> temp_uv.y;
						texcoords.push_back(temp_uv);
						has_uv = true;
					}
					else if (check_char == 'n')
					{
						glm::vec3 temp_normal;
						file >> temp_normal.x >> temp_normal.y >> temp_normal.z;
						normals.push_back(temp_normal);
						has_normal = true;
					}
					break;
				case 'f':
					check_char = file.get();
					if (check_char == ' ')
					{
						face = L"";
						triangle_count = 0;

						check_char = file.get();
						while (check_char != '\n')
						{
							face += check_char;
							check_char = file.get();
							if (check_char == ' ')
								triangle_count++;
						}

						// check for space at the end of the line
						if (face[face.length() - 1] == ' ')
							triangle_count--;

						triangle_count -= 1;

						std::wstringstream ss(face);

						if (!face.empty())
						{
							std::wstring vert_def;
							int first_vertex_index, last_vertex_index; // for holding the first and last vertex index

							for (int i = 0; i < 3; ++i)
							{
								ss >> vert_def; //Get vertex definition

								std::wstring vert_part;
								int which_part = 0; // (v, vt, vn)

								//parse this string
								for (int j = 0; j < vert_def.length(); ++j)
								{
									if (vert_def[j] != '/')
										vert_part += vert_def[j];

									if (vert_def[j] == '/' || j == vert_def.length() - 1)
									{
										std::wistringstream wstring_to_int(vert_part);

										if (which_part == 0) // position
										{
											wstring_to_int >> temp_position_index;
											temp_position_index -= 1; // in wavefront obj all indices start at 1, not zero

											if (j == vert_def.length() - 1)
												temp_normal_index = temp_uv_index = 0;
										}
										else if (which_part == 1) // uv
										{
											if (!vert_part.empty())
											{
												wstring_to_int >> temp_uv_index;
												temp_uv_index -= 1;
											}
											else
												temp_uv_index = 0;

											if (j == vert_def.length() - 1)
												temp_normal_index = 0;
										}
										else if (which_part == 2) // normal
										{
											wstring_to_int >> temp_normal_index;
											temp_normal_index -= 1;
										}

										vert_part = L"";
										which_part++;
									}

								}


								// Check if this vertex is already in the list of vertices
								RSVertex vertex;
								vertex.position = positions[temp_position_index];
								vertex.normal = has_normal ? normals[temp_normal_index] : glm::vec3(0.0f);
								vertex.texture_coordinates = has_uv ? texcoords[temp_uv_index] : glm::vec2(0.0f);

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

								if (i == 0)
									first_vertex_index = unique_vertices[vertex];
								else if (i == 2)
									last_vertex_index = unique_vertices[vertex];
							}

							total_triangles += 1;

							for (int k = 0; k < triangle_count - 1; ++k)
							{
								data_.indices.push_back(first_vertex_index);
								data_.indices.push_back(last_vertex_index);
								ss >> vert_def; //Get vertex definition

								std::wstring vert_part;
								int which_part = 0; // (v, vt, vn)

								//parse this string
								for (int j = 0; j < vert_def.length(); ++j)
								{
									if (vert_def[j] != '/')
										vert_part += vert_def[j];

									if (vert_def[j] == '/' || j == vert_def.length() - 1)
									{
										std::wistringstream wstring_to_int(vert_part);

										if (which_part == 0) // position
										{
											wstring_to_int >> temp_position_index;
											temp_position_index -= 1; // in wavefront obj all indices start at 1, not zero

											if (j == vert_def.length() - 1)
												temp_normal_index = temp_uv_index = 0;
										}
										else if (which_part == 1) // uv
										{
											if (!vert_part.empty())
											{
												wstring_to_int >> temp_uv_index;
												temp_uv_index -= 1;
											}
											else
												temp_uv_index = 0;

											if (j == vert_def.length() - 1)
												temp_normal_index = 0;
										}
										else if (which_part == 2) // normal
										{
											wstring_to_int >> temp_normal_index;
											temp_normal_index -= 1;
										}

										vert_part = L"";
										which_part++;
									}
								}

								// Check if this vertex is already in the list of vertices
								RSVertex vertex;
								vertex.position = positions[temp_position_index];
								vertex.normal = has_normal ? normals[temp_normal_index] : glm::vec3(0.0f);
								vertex.texture_coordinates = has_uv ? texcoords[temp_uv_index] : glm::vec2(0.0f);

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

								last_vertex_index = unique_vertices[vertex];
								total_triangles += 1;
							}

						}
					}
					break;
				default:
					break;
				}
			}
			data_.vertices.shrink_to_fit();
			data_.indices.shrink_to_fit();
			file.close();
		}
		else
		{
			return false;
		}

		if (!has_uv)
			std::cout << "texcoord is not exist\n";
		else
		{
			data_.has_texcoord = true;
		}
		if (!has_normal)
		{
			std::vector<glm::vec3> accumulatedNormals(data_.vertices.size(), glm::vec3(0.0f));
			for (int i = 0; i < data_.indices.size(); i += 3)
			{
				glm::vec3 v0 = data_.vertices[data_.indices[i]].position;
				glm::vec3 v1 = data_.vertices[data_.indices[i + 1]].position;
				glm::vec3 v2 = data_.vertices[data_.indices[i + 2]].position;

				glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

				accumulatedNormals[data_.indices[i]] += normal;
				accumulatedNormals[data_.indices[i + 1]] += normal;
				accumulatedNormals[data_.indices[i + 2]] += normal;
			}

			for (int i = 0; i < accumulatedNormals.size(); i++)
			{
				data_.vertices[i].normal = glm::normalize(accumulatedNormals[i]);
			}
		}

		// Set min, max data
		auto min = glm::vec3(FLT_MAX);
		auto max = glm::vec3(-FLT_MAX);

		for (auto& vert : data_.vertices)
		{
			min = glm::min(min, vert.position);
			max = glm::max(max, vert.position);
		}

		data_.min = min;
		data_.max = max;


		return true;
	}

}
