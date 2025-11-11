#include "pch.h"
#include "RSShader.h"
#include <windows.h>

using namespace std;

namespace RS_Shader
{
	char* TextFileRead(const char* fn) {

		FILE* fp;
		char* content = nullptr;

		if (fn != NULL) {

			if (fopen_s(&fp, fn, "rt") == 0) {
				size_t count = 0;

				fseek(fp, 0, SEEK_END);
				count = ftell(fp);
				rewind(fp);
				if (count > 0) {
					content = (char*)malloc(sizeof(char) * (count + 1));
					count = fread_s(content, sizeof(char) * (count + 1), sizeof(char), count, fp);
					content[count] = '\0';
				}
				fclose(fp);
			}
			else {
				std::cout << errno << "\n";
				RS_WARN("no file %s", fn);
				//std::cout << "no file " << fn << std::endl;

			}

		}
		return content;
	}


	void CheckCompileErrors(const GLuint shader, const std::string& type, const std::string& path_ = nullptr)
	{
		int success;
		char info_log[512];

		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, info_log);
				RS_WARN("ERROR::SHADER_COMPILATION_ERROR of type: %s shader : %d\n path : %s\n %s -- --------------------------------------------------- -- ", type.c_str(), shader, path_.c_str(),info_log);
			}
			else
			{
				//std::cout << "SHADER::COMPILE::" << shader << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, info_log);
        RS_WARN("ERROR::SHADER_COMPILATION_ERROR of type: %s shader : %d\n path : %s\n %s -- --------------------------------------------------- -- ",
                type.c_str(), shader, path_.c_str(), info_log);
			}
		}
	}


	void RSShader::CompileShader()
	{
		bool is_use_geometry = (geo_path_ != nullptr);
		//char* vs = NULL, * fs = NULL, *gs = NULL; //?

		std::string vertex_code;
		std::string fragment_code;
		std::string geometry_code;
		std::ifstream v_shader_file;
		std::ifstream f_shader_file;
		std::ifstream g_shader_file;


		v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		g_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		if (b_from_path)
		{
			try
			{
				// open files
				v_shader_file.open(vertex_path_);
				f_shader_file.open(frag_path_);
				std::stringstream v_shader_stream, f_shader_stream;
				// read file's buffer contents into streams
				v_shader_stream << v_shader_file.rdbuf();
				f_shader_stream << f_shader_file.rdbuf();
				// close file handlers
				v_shader_file.close();
				f_shader_file.close();
				// convert stream into string
				vertex_code = v_shader_stream.str();
				fragment_code = f_shader_stream.str();

				if (is_use_geometry)
				{
					g_shader_file.open(geo_path_);
					std::stringstream g_shader_stream;
					g_shader_stream << g_shader_file.rdbuf();
					g_shader_file.close();
					geometry_code = g_shader_stream.str();
				}
			}
			catch (std::ifstream::failure& e)
			{
				RS_WARN("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: %s", e.what());
			}
		}
		
		const char* v_shader_code = b_from_path ? vertex_code.c_str() : vertex_path_;
		const char* f_shader_code = b_from_path ? fragment_code.c_str() : frag_path_;

		GLuint v, f, g;
		v = glCreateShader(GL_VERTEX_SHADER);
		f = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(v, 1, &v_shader_code, NULL);
		glShaderSource(f, 1, &f_shader_code, NULL);

		glCompileShader(v);
		glCompileShader(f);

		if (is_use_geometry)
		{
			const char* gShaderCode = b_from_path ? geometry_code.c_str() : geo_path_;
			g = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(g, 1, &gShaderCode, NULL);
			glCompileShader(g);
		}

		//free(vs); free(fs); //?

		ID = glCreateProgram();
		glAttachShader(ID, v);
		glAttachShader(ID, f);
		if (is_use_geometry) glAttachShader(ID, g);

		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM", vertex_path_);

		glDeleteShader(v);
		glDeleteShader(f);
		if (is_use_geometry) glDeleteShader(g);

	}

	void RSShader::CompileShaderFromFile(const char* vertex_path, const char* fragment_path)
	{
		char* vs = NULL, * fs = NULL;

		const GLuint v = glCreateShader(GL_VERTEX_SHADER);
		const GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

		if (vertex_path != NULL)
			vs = TextFileRead(vertex_path);
		if (fragment_path != NULL)
			fs = TextFileRead(fragment_path);

		const char* vv = vs;
		const char* ff = fs;

		glShaderSource(v, 1, &vv, NULL);
		glShaderSource(f, 1, &ff, NULL);

		free(vs); free(fs);

		glCompileShader(v);
		glCompileShader(f);

		ID = glCreateProgram();
		glAttachShader(ID, v);
		glAttachShader(ID, f);

		glLinkProgram(ID);

		glDeleteShader(v);
		glDeleteShader(f);

		int success;

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			RS_WARN("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
		}

	}

	RSShader::RSShader(const char* vertex_path, const char* fragment_path, const bool is_path_, const char* geometry_path)
	{
		vertex_path_ = const_cast<char*>(vertex_path);
		frag_path_ = const_cast<char*>(fragment_path);
		geo_path_ = (geometry_path != nullptr) ? const_cast<char*>(geometry_path) : nullptr;
    b_from_path = is_path_;
	}

	//RSShader::RSShader(const int vertex_id, const int fragment_id)
	//{
	//	HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(vertex_id), RT_RCDATA);
	//	if (!resource) {
	//		std::cerr << "Failed to find resource!" << std::endl;
	//	}
	//}


	RSShader::~RSShader() = default;

	void RSShader::Initialize(const char* vertex_path, const char* fragment_path)
	{
		vertex_path_ = const_cast<char*>(vertex_path);
		frag_path_ = const_cast<char*>(fragment_path);
	}

	void RSShader::Use() const
	{
		glUseProgram(ID);
	}

	void RSShader::SetBool(const std::string& name, const bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void RSShader::SetInt(const std::string& name, const int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void RSShader::SetUInt(const std::string& name, const unsigned int value) const
	{
		glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
	}
	void RSShader::SetFl32(const std::string& name, const float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void RSShader::SetMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void RSShader::SetVec3(const std::string& name, const glm::vec3 vec) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}

	void RSShader::SetIVec3(const std::string& name, const glm::ivec3 vec) const
	{
		glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}

	void RSShader::SetVec4(const std::string& name, const glm::vec4 vec) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}

	void RSShader::SetIVec4(const std::string& name, const glm::ivec4 vec) const
	{
		glUniform4iv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}

	void RSShader::SetVec2(const std::string& name, const glm::vec2 vec) const
	{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}

	ComputeShader::ComputeShader(const char* compute_path, const bool is_from_path)
	{
		compute_path_ = (compute_path != nullptr) ? const_cast<char*>(compute_path) : nullptr;
    b_from_path = is_from_path;
	}

	void ComputeShader::CompileShader()
	{
		std::string compute_code;
		if (b_from_path)
		{
			std::ifstream c_shader_file;
			c_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				c_shader_file.open(compute_path_);
				std::stringstream c_shader_stream;
				c_shader_stream << c_shader_file.rdbuf();
				c_shader_file.close();
				compute_code = c_shader_stream.str();
			}
			catch (std::ifstream::failure& e)
			{
				RS_WARN("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: %s", e.what());
			}
		}

		const char* c_shader_code = b_from_path ? compute_code.c_str() : compute_path_;

		// compute shader
		const GLuint compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &c_shader_code, NULL);
		glCompileShader(compute);
		CheckCompileErrors(compute, "COMPUTE", compute_path_);

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, compute);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM", compute_path_);

		glDeleteShader(compute);

	}

	void ComputeShader::Use() const
	{
		glUseProgram(ID);
	}
	void ComputeShader::SetMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void ComputeShader::SetVec3(const std::string& name, const glm::vec3 vec) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}
	void ComputeShader::SetIVec3(const std::string& name, const glm::ivec3 vec) const
	{
		glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}
	void ComputeShader::SetVec4(const std::string& name, const glm::vec4 vec) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}
	void ComputeShader::SetIVec4(const std::string& name, const glm::ivec4 vec) const
	{
		glUniform4iv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
	}
	void ComputeShader::SetBool(const std::string& name, const bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
	}
	void ComputeShader::SetInt(const std::string& name, const int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void ComputeShader::SetUInt(const std::string& name, const unsigned int value) const
	{
		glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
	}
	void ComputeShader::SetFl32(const std::string& name, const float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
}
