//#pragma once
//#include <map>
//#include "Define/RSPipelineList.h"
//
//namespace PJS_SIMULATOR
//{
//	class PipelineManager
//	{
//	public:
//		PipelineManager() {}
//		~PipelineManager() {}
//
//		void Initialize();
//		void Shutdown();
//
//		void Use(RSPipelineType type_);
//		void SetData(std::string valueName, glm::mat4 data);
//		void SetData(std::string valueName, glm::vec3 data);
//		void SetData(std::string valueName, glm::vec4 data);
//		void SetData(std::string valueName, glm::float32 data);
//		void SetData(std::string valueName, int data);
//		void SetData(std::string valueName, unsigned int data);
//		void SetTexture(std::string valueName, unsigned int data, unsigned int index);
//
//		const unsigned int GetID() { return m_pipeline->GetID(); }
//
//	private:
//		std::map<RSShaderNames, Pipeline*> m_pipelines_map_;
//	};
//}
