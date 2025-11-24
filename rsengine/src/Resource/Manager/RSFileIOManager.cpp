#include "pch.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/Input/RSInputClass.h"
#include "RSResourceManager.h"
#include "RSFileIOManager.h"
#include <filesystem>
#include <locale>
#include <codecvt>
#pragma execution_character_set("utf-8")

namespace RS_FILE_IO
{
	using RS_Input::mRSINPUT;
	using RS_Input::mRSKEYBIND;

	// Helper function to convert wstring to string
	inline std::string WideToUtf8(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();

		const int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
		                                            static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
		std::string result(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
			static_cast<int>(wstr.size()), &result[0], size_needed, nullptr, nullptr);
		return result;
	}

	// Helper function to convert wstring to ANSI string
	inline std::string WideToAnsi(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();

		int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
			static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
		std::string result(size_needed, 0);
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
			static_cast<int>(wstr.size()), &result[0], size_needed, nullptr, nullptr);
		return result;
	}

	constexpr unsigned int FileExtensionHash(const char* s)
	{
		return *s ? static_cast<unsigned int>(*s) + 33 * FileExtensionHash(s + 1) : 5381;
	}
	RSFileIOManger::RSFileIOManger() = default;

	RSFileIOManger::~RSFileIOManger() = default;

	void RSFileIOManger::Load()
	{
		LinkManagers();

		LoadInitFiles();
	}

	void RSFileIOManger::Initialize()
	{

	}

	void RSFileIOManger::Update(float)
	{
		if (mRSINPUT->IsTriggered(mRSKEYBIND->GetKey(RSKeyList::FILE_DIALOG_OPEN)) && mRSINPUT->IsPressed(RSKey::CTRL_L))
			RSResourceManager::GetInstance()->GetFileIOManager()->TryFileIO();

		if (mRSINPUT->IsTriggered(mRSKEYBIND->GetKey(RSKeyList::SAVE_FILE_DIALOG)) && mRSINPUT->IsPressed(RSKey::CTRL_L))
      RSResourceManager::GetInstance()->GetFileIOManager()->SaveFileDialog();

		if (RSResourceManager::GetInstance()->GetFileIOManager()->IsFileIODone())
		{
			OpenFile();
			RS_INFO("File IO result : {0}", RSResourceManager::GetInstance()->GetFileIOManager()->GetFileIOResult());
		}
	}

	void RSFileIOManger::Shutdown()
	{
		//CloseFileDialog();
	}

	bool RSFileIOManger::OpenFileDialog()
	{
		const wchar_t* filter_patterns[] = { L"*.png", L"*.jpg", L"*.stl", L"*.obj", L"*.mesh_obj", L"*.bin", L"*.vtk", L"*.ply", L"*.las" };

		const wchar_t* file_path = tinyfd_openFileDialogW(L"Open File", L"", 9, filter_patterns, nullptr, 0);
		if (!file_path)
		{
#if 0
			bool result = true;
			while (result)
			{
				const char* input_text = tinyfd_inputBox(
					"MINT ATTACK",
					//u8"감히 취소를 눌러? 민트 맛좀 봐라!",
					"Write 'I LOVE MINT'",
					"RIGHT NOW!");
				if (input_text == nullptr)
					result = true;
				else
				{
					std::string input_text_str = input_text;

					if (const auto result_text = "I LOVE MINT"; input_text_str != result_text)
						result = true;
					else
						result = false;
				}
			}
#endif
			return false;
		}

		m_file_path = file_path;
		return true;
	}

	bool RSFileIOManger::SaveFileDialog(const std::wstring& extension_)
	{
		const wchar_t* filter_patterns[] = {L"*.bin" };

    const wchar_t* file_path = tinyfd_saveFileDialogW(L"Save File", L"", 1, filter_patterns, nullptr);

		if (!file_path)
		{
#if 0
			bool result = true;
			while (result)
			{
				const char* input_text = tinyfd_inputBox(
					"MINT ATTACK",
					//u8"감히 취소를 눌러? 민트 맛좀 봐라!",
					"Write 'I LOVE MINT'",
					"RIGHT NOW!");
				if (input_text == nullptr)
					result = true;
				else
				{
					std::string input_text_str = input_text;

					if (const auto result_text = "I LOVE MINT"; input_text_str != result_text)
						result = true;
					else
						result = false;
				}
			}
#endif
			return false;
		}
		else
		{
			std::wstring file_path_str = file_path;

      std::wstring file_extension = file_path_str.substr(file_path_str.find_last_of('.') + 1);
			
      // if extension_ isn't empty and same with file_extension,
      // file_extension change to extension_. also file_path_str add extension_
      if (!extension_.empty() && extension_ != file_extension)
      {
        file_path_str += L"." + std::wstring(extension_.begin(), extension_.end());
        file_extension = extension_;
      }
			

      if (file_extension == L"bin")
      {
        RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->ExportPointData(file_path_str);
        return true;
      }
      else
      {
        RS_WARN("Unsupported file extension : %s", file_extension.c_str());
        return false;
      }

		}
		return true;



	}

	void RSFileIOManger::TryFileIO()
	{
		if (IsLoaderReady())
		{
			SetLoaderReady(false);
			m_file_io_result = std::async(std::launch::async, &RSFileIOManger::OpenFileDialog, this);
		}
	}
	bool RSFileIOManger::OpenFile()
	{
		bool result = false;

		// separate the file extension from m_file_path
		const std::wstring file_path = GetFilePath();
		if (file_path.empty())
			return false;

		// file_path convert to utf-8
		const auto file_path_asc = WideToAnsi(file_path);
		const auto file_path_uni = WideToUtf8(file_path);
		//file_path_str = W2A(file_path.c_str());

		//std::string file_path_str(file_path.begin(), file_path.end());
		const std::string file_extension = file_path_uni.substr(file_path_uni.find_last_of('.') + 1);
		std::string file_name = file_path_uni.substr(file_path_uni.find_last_of('\\') + 1);

		// get rid of extension from file_name
		file_name = file_name.substr(0, file_name.find_last_of('.'));


		if (file_extension == "png" || file_extension == "jpg")
		{
			result = RSResourceManager::GetInstance()->GetTextureManager()->LoadTextures(file_name, file_path_asc);
		}
		else if (file_extension == "obj" || file_extension == ".mesh_obj")
		{
			result = RSResourceManager::GetInstance()->GetMeshManager()->LoadMesh(file_name, file_path_asc);
		}//LoadMeshSTL2
		else if (file_extension == "stl")
		{
			result = RSResourceManager::GetInstance()->GetMeshManager()->LoadMeshStl2(file_name, file_path_asc);
		}
		else if (file_extension == "bin" || file_extension == "ply" || file_extension == "las")
		{

			if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistPointClipper())
			{
				RS_WARN("Point Clipper level didn't load.");
				result = false;
			}
			else
			{
				if (RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->isLoaded())
					RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->ResetData();

				RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->LoadPointData(file_path_asc);
				result = true;
			}
		}
		else if (file_extension == "vtk")
		{
			if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistVtkViewer())
			{
				RS_WARN("VTK Viewer didn't load.");
				result = false;
			}
			else
			{
				RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadVtkFile(file_path_asc);
				result = true;
			}
		}
		else
		{
			RS_WARN("Unsupported file extension : %s", file_extension.c_str());
			result = false;
		}

		return result;
	}

	bool RSFileIOManger::OpenFile(std::string path_)
	{
		bool result = false;

		// path_ is utf-8
		const auto file_path = std::wstring(path_.begin(), path_.end());
		if (file_path.empty())
			return false;

		// file_path convert to utf-8

		const auto file_path_asc = WideToAnsi(file_path);
		const auto file_path_uni = WideToUtf8(file_path);
		//file_path_str = W2A(file_path.c_str());

		//std::string file_path_str(file_path.begin(), file_path.end());
		const std::string file_extension = file_path_uni.substr(file_path_uni.find_last_of('.') + 1);
		std::string file_name = file_path_uni.substr(file_path_uni.find_last_of('\\') + 1);

		// get rid of extension from file_name
		file_name = file_name.substr(0, file_name.find_last_of("."));


		if (file_extension == "png" || file_extension == "jpg")
		{
			result = RSResourceManager::GetInstance()->GetTextureManager()->LoadTextures(file_name, file_path_asc);
		}
		else if (file_extension == "obj" || file_extension == ".mesh_obj")
		{
			result = RSResourceManager::GetInstance()->GetMeshManager()->LoadMesh(file_name, file_path_asc);
		}//LoadMeshSTL2
		else if (file_extension == "stl")
		{
			result = RSResourceManager::GetInstance()->GetMeshManager()->LoadMeshStl2(file_name, file_path_asc);
		}
		else if (file_extension == "bin" || file_extension == "ply" || file_extension == "las")
		{

			if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistPointClipper())
			{
				RS_WARN("Point Clipper level didn't load.");
				result = false;
			}
			else
			{
				if (RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->isLoaded())
					RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->ResetData();

				RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->LoadPointData(file_path_asc);
				result = true;
			}
		}
		else if (file_extension == "vtk")
		{
			if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistVtkViewer())
			{
				RS_WARN("VTK Viewer didn't load.");
				result = false;
			}
			else
			{
				RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadVtkFile(file_path_asc);
				result = true;
			}
		}
		else
		{
			RS_WARN("Unsupported file extension : %s", file_extension.c_str());
			result = false;
		}

		return result;
	}

	bool RSFileIOManger::OpenFile(const std::wstring& path_)
	{
		bool result = false;

		// path_ is utf-8

		// file_path convert to utf-8
		const auto file_path_asc = WideToAnsi(path_);
		const auto file_path_uni = WideToUtf8(path_);
		//file_path_str = W2A(file_path.c_str());

		//std::string file_path_str(file_path.begin(), file_path.end());
		const std::string file_extension = file_path_uni.substr(file_path_uni.find_last_of('.') + 1);
		std::string file_name = file_path_uni.substr(file_path_uni.find_last_of('\\') + 1);

		// get rid of extension from file_name
		file_name = file_name.substr(0, file_name.find_last_of('.'));


		if (file_extension == "png" || file_extension == "jpg")
		{
			result = RSResourceManager::GetInstance()->GetTextureManager()->LoadTextures(file_name, file_path_asc);
		}
		else if (file_extension == "obj" || file_extension == ".mesh_obj")
		{
			result = RSResourceManager::GetInstance()->GetMeshManager()->LoadMesh(file_name, file_path_asc);
		}//LoadMeshSTL2
		else if (file_extension == "stl")
		{
			result = RSResourceManager::GetInstance()->GetMeshManager()->LoadMeshStl2(file_name, file_path_asc);
		}
		else if (file_extension == "bin" || file_extension == "ply" || file_extension == "las")
		{

			if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistPointClipper())
			{
				RS_WARN("Point Clipper level didn't load.");
				result = false;
			}
			else
			{
				if (RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->isLoaded())
					RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->ResetData();

				RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->LoadPointData(file_path_asc);
				result = true;
			}
		}
		else if (file_extension == "vtk")
		{
			if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistVtkViewer())
			{
				RS_WARN("VTK Viewer didn't load.");
				result = false;
			}
			else
			{
				RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadVtkFile(file_path_asc);
				result = true;
			}
		}
		else
		{
			RS_WARN("Unsupported file extension : %s", file_extension.c_str());
			result = false;
		}

		return result;
	}

	bool RSFileIOManger::OpenFile(const char* path_)
	{
		bool result = false;

		//path_ is utf-8
		//USES_CONVERSION;
		const std::string file_path_asc = path_;// std::string(W2A(file_path.c_str()));
		const std::string file_path_uni = path_;// std::string(W2A_CP(file_path.c_str(), CP_UTF8));
		//file_path_str = W2A(file_path.c_str());

		//std::string file_path_str(file_path.begin(), file_path.end());
		const std::string file_extension = file_path_uni.substr(file_path_uni.find_last_of('.') + 1);
		std::string file_name = file_path_uni.substr(file_path_uni.find_last_of('\\') + 1);

		// get rid of extension from file_name
		file_name = file_name.substr(0, file_name.find_last_of('.'));


		if (file_extension == "png" || file_extension == "jpg")
		{
			result = RSResourceManager::GetInstance()->GetTextureManager()->LoadTextures(file_name, file_path_asc);
		}
		else if (file_extension == "obj" || file_extension == ".mesh_obj")
		{
			result = RSResourceManager::GetInstance()->GetMeshManager()->LoadMesh(file_name, file_path_asc);
		}//LoadMeshSTL2
		else if (file_extension == "stl")
		{
			result = RSResourceManager::GetInstance()->GetMeshManager()->LoadMeshStl2(file_name, file_path_asc);
		}
		else if (file_extension == "bin" || file_extension == "ply" || file_extension == "las")
		{

			if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistPointClipper())
			{
				RS_WARN("Point Clipper level didn't load.");
				result = false;
			}
			else
			{
				if (RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->isLoaded())
					RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->ResetData();

				RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->LoadPointData(file_path_asc);
				result = true;
			}
		}
		else if (file_extension == "vtk")
		{
			if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistVtkViewer())
			{
				RS_WARN("VTK Viewer didn't load.");
				result = false;
			}
			else
			{
				RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadVtkFile(file_path_asc);
				result = true;
			}
		}
		else
		{
			RS_WARN("Unsupported file extension : %s", file_extension.c_str());
			result = false;
		}


		return result;
	}



	void RSFileIOManger::LoadInitFiles()
	{
		LoadTextures();
		LoadMeshes();
	}

	void RSFileIOManger::LinkManagers()
	{
		m_texture_manager = RSResourceManager::GetInstance()->GetTextureManager();
		m_mesh_manager = RSResourceManager::GetInstance()->GetMeshManager();

	}

	void RSFileIOManger::LoadTextures() const
	{
		// Load Textures
		std::string path = "resources/InitLoadFiles/Textures/";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::string file_name = entry.path().stem().string();
			std::ranges::transform(file_name, file_name.begin(), ::tolower);

			std::filesystem::path file_extenstion = entry.path().extension();
			std::string file_extenstion_str = file_extenstion.string();
			std::ranges::transform(file_extenstion_str, file_extenstion_str.begin(), ::tolower);

			if (file_extenstion == L".jpg" || file_extenstion == L".png" || file_extenstion == L".jpeg")
			{
				// Check if the file is already loaded
				if (!m_texture_manager->IsExistTexture(file_name))
				{
					// It includes the file name and the path
					m_texture_manager->LoadTextures(file_name, entry.path().string());
					RS_INFO("%s loaded.", file_name.c_str());
				}
				else
				{
					RS_WARN("RSTexture already loaded : %s", file_name.c_str());
				}
			}
		}

		RS_INFO("Textures End");
	}

	void RSFileIOManger::LoadMeshes() const
	{
		//std::string path = "resources/InitLoadFiles/Meshes/";
		// Load Meshes
		std::string path = "resources/InitLoadFiles/Meshes/";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::string file_name = entry.path().stem().string();
			std::ranges::transform(file_name, file_name.begin(), ::tolower);

			std::filesystem::path file_extension = entry.path().extension();
			std::string file_extenstion_str = file_extension.string();
			std::ranges::transform(file_extenstion_str, file_extenstion_str.begin(), ::tolower);

			// TODO : Refactor this part by switch statement
			if (file_extension == L".stl")// || file_extenstion == L".png" || file_extenstion == L".jpeg")
			{
				// Check if the file is already loaded
				if (!m_mesh_manager->IsExistMesh(file_name))
				{
					// It includes the file name and the path
					m_mesh_manager->LoadMeshStl2(file_name, entry.path().wstring());
					RS_INFO("%s loaded.", file_name.c_str());
				}
				else
					RS_WARN("STL file already loaded : %s", file_name.c_str());
			}
			else if (file_extension == L".obj" || file_extension == ".mesh_obj")
			{
				// Check if the file is already loaded
				if (!m_mesh_manager->IsExistMesh(file_name))
				{
					// It includes the file name and the path
					m_mesh_manager->LoadMesh(file_name, entry.path().wstring());
					RS_INFO("%s loaded.", file_name.c_str());
				}
				else
					RS_WARN("STL file already loaded : %s", file_name.c_str());
			}

			// Add more file extension here
		}
		RS_INFO("Meshes End");
	}

	bool RSFileIOManger::IsFileIODone()
	{
		if (m_file_io_result.valid() && m_file_io_result.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			return true;
		}
		return false;
	}
	bool RSFileIOManger::GetFileIOResult()
	{
		SetLoaderReady(true);
		return m_file_io_result.get();
	}

	void RSFileIOManger::CloseFileDialog()
	{
		// Program did shut down. So, close the file dialog. and future is closed.
		m_file_io_result = std::async(std::launch::async, &RSFileIOManger::ReturnFalseIoResult, this);
		if (m_file_io_result.valid() && m_file_io_result.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			m_file_io_result.get();

	}
	bool RSFileIOManger::ReturnFalseIoResult()
	{
		return false;
	}
}

