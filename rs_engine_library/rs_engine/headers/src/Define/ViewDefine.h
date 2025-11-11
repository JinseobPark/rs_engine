#pragma once

namespace SPH_MINT
{

	enum RenderingParticle
	{
		Rendering_Particle = 0x00000001,
		Rendering_Trace = 0x00000002,
		Rendering_Vector = 0x00000004,
		Rendering_ScreenSpaceFluid = 0x00000008,
		Rendering_Analysis = 0x00000010
	};

	enum VisualData
	{
		VisualData_Velocity = 0x00000001,
		VisualData_Dencity = 0x00000002,
		VisualData_Pressure = 0x00000004,
		VisualData_Position = 0x00000008,
		VisualData_LifeTime = 0x00000010,
		VisualData_FoamValue = 0x00000020,
		VisualData_Visible = 0x00000040,
		VisualData_Index = 0x00000080,
		VisualData_Concentration = 0x00000100,
		VisualData_Temperature = 0x00000200,
		VisualData_AxisVel = 0x00000400,
		VisualData_Vorticity = 0x00000800,
		VisualData_Torque = 0x00001000,
		VisualData_FEpsilon = 0x00002000,
		VisualData_Stress = 0x00004000,
		VisualData_Strain = 0x00008000,

		VisualData_UDF = 0x10000000,
	};

	enum Draw
	{
		Draw_Color = 0x00000001,		// 색상으로 그리기
		Draw_Legend = 0x00000002,		// 값으로 그리기				
	};

	/// 카메라
	enum Camera
	{
		Perspective = 0x00000001,	// 원근 카메라
		Ortho = 0x00000002,	// 직교 카메라

		CameraTypeMask = 0x000000FF,


		FreeControl = 0x00000100,	// 자유 카메라
		TargetControl = 0x00000200,	// 타겟 고정 카메라

		CameraControlMask = 0x0000FF00
	};

	/// 구조물
	enum Struct
	{
		Triangle = 0x00000001,	// 삼각형
		Line = 0x00000002,	// 라인

		PrimitiveTypeMask = 0x000000FF,


		Standard = 0x00000100,	// 일반 상태
		Select = 0x00000200,	// 선택 상태
		Hide = 0x00000400,	// 숨김 상태
		Active = 0x00000800,	// 비활성화 상태
		Transparent = 0x00001000,	// 투명 상태

		StateTypeMask = 0x0000FF00,


		Mesh = 0x00010000,	// 본체
		Axis = 0x00020000,	// 축
		Boundary = 0x00040000,	// 외각 선
		Outline = 0x00080000,	// 선택 라인
		Normal = 0x00100000,	// 노말

		EnableTypeMask = 0x00FF0000,


		StructObject = 0x01000000,	// 구조물
		MeasurementObject = 0x02000000,	// 초기 입자
		SourceObject = 0x04000000,	// 생성판
		InitialObject = 0x08000000,	// 계측 박스
		SectionObject = 0x10000000,	// 단면
		ZoneObject = 0x20000000,	// 존
		DrainObject = 0x40000000,	// 소멸판

		ObejctTypeMask = 0xFF000000,
	};

	enum State
	{
		StateNone = 0x00000000,	// 아무것도 그리지 않음

		StateStructMesh = 0x00000001,	// 구조물의 메쉬(본체)
		StateStructAxis = 0x00000002,	// 구조물의 지역 축
		StateStructBoundary = 0x00000004,	// 구조물의 외각 박스
		StateStructOutline = 0x00000008,	// 구조물의 선택 라인
		StateStructNormal = 0x00000010,	// 구조물의 노말

		StateStructMask = 0x000000FF,	// 구조물 상태 마스크


		StateVisualStructObject = 0x00000100,	// 구조물 오브젝트
		StateVisualMeasurementObject = 0x00000200,	// 계측 오브젝트 
		StateVisualSourceObject = 0x00000400,	// 생성판 오브젝트
		StateVisualInitialObject = 0x00000800,	// 초기입자 오브젝트
		StateVisualSectionObject = 0x00001000,	// 단면 오브젝트
		StateVisualSystemBoundary = 0x00002000,	// 해석영역
		StateVisualAnalysisResult = 0x00004000,	// 해석결과
		StateVisualGlobalAxis = 0x00008000,	// 전역 축
		StateVisualZoneObject = 0x00010000,	// 영역 오브젝트
		StateVisualDrainObject = 0x00020000,	// 존 오브젝트

		StateVisualMask = 0x000FFF00,	// 가시화 상태 마스크

		StateALL = 0xFFFFFFFF
	};
}