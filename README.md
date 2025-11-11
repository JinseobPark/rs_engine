# RSEngine + SeobJJangGames Project

![RSEngine Logo](RSEngine/doxy_scripts/logo.jpg)

**Game Development Project Based on Rock & Stone Engine**

[한국어](#korean) | [English](#english)

---

<a name="english"></a>
## English Version

This repository consists of two main projects:
- **RSEngine**: A game engine library for real-time physics simulation and rendering
- **SeobJJangGames**: A game application utilizing RSEngine

---

## Table of Contents

- [Project Overview](#project-overview)
- [Key Features](#key-features)
- [Project Structure](#project-structure)
- [Build Environment](#build-environment)
- [Build Instructions](#build-instructions)
- [Running the Application](#running-the-application)
- [Technologies Used](#technologies-used)
- [Documentation](#documentation)
- [License](#license)

---

## Project Overview

### RSEngine (Realtime-Simulation Engine)

RSEngine is a custom game engine developed in C++20, featuring GPU-accelerated SPH (Smoothed Particle Hydrodynamics) fluid simulation and a modular component-based architecture.

**Version**: 0.0.6  
**Developer**: Jinseob Park  
**Language**: C++20  
**Graphics API**: OpenGL 4.3+  
**Type**: Static Library (.lib)

#### Core Features

- **GPU-Accelerated Physics Simulation**: Particle physics based on OpenGL Compute Shaders
- **Real-time Fluid Dynamics**: SPH-based fluid simulation
- **PBR Rendering System**: Physically Based Rendering material system
- **Modular Architecture**: Component-based design with excellent extensibility
- **Data Visualization**: VTK file loading, point clipping, attribute-based coloring

### SeobJJangGames

A game application built on RSEngine, providing various game states and simulation features.

**Type**: Application (.exe)  
**Dependencies**: RSEngine Static Library

#### Main Game States

- **Jordy Roll Game**: Physics-based gameplay
- **Particle Simulator**: Real-time particle simulation tool
- **VTK Loader**: Scientific data visualization
- **Point Clipper**: Point cloud editing tool

---

## Key Features

### RSEngine Engine Features

#### 1. Fluid Simulation (SPH)
- **Multiple Solver Support**:
  - Static Brute Force (accuracy-focused, small-scale)
  - Static Grid (optimized, large-scale)
  - Dynamic Brute Force (supports inflow/outflow)
- **Real-time Parameter Adjustment**: Density, viscosity, pressure, gravity
- **Collision Detection**: Mesh-based collision system
- **Visualization Modes**: Color coding based on velocity, density, pressure, acceleration

#### 2. Rendering System
- **PBR Materials**: Metallic/Roughness workflow
- **Dual Pipeline**: Deferred/Forward rendering
- **Particle Rendering**: Handles millions of particles with GPU instancing
- **Legend System**: Dynamic color mapping and min/max value calculation
- **Clipping Tools**: Particle filtering based on Plane/Cube/Sphere

#### 3. Engine Architecture
- **System Components**: Application, Input, Physics, Graphics, Timer, Event Handler
- **Resource Management**: Centralized singleton managers
- **Fixed/Variable FPS**: Configurable update loop
- **Event System**: Decoupled communication between systems

#### 4. Data Processing
- **VTK File Loading**: Supports scientific data visualization formats
- **Point Clipping**: Data filtering in 3D space
- **Attribute-based Rendering**: Visualization based on data values

### SeobJJangGames Game Features

#### 1. Game State Management
- **State-based Architecture**: Scene management through game state manager
- **Dynamic State Transitions**: Runtime state switching support
- **Game Data Management**: Centralized game data with singleton pattern

#### 2. GPU Optimization
- **NVIDIA Optimus Support**: Automatic dedicated GPU selection
- **AMD PowerXpress Support**: High-performance GPU activation

#### 3. Debugging Support
- **Memory Leak Detection**: CRT debug heap integration
- **Developer Console**: Runtime debugging tools

---

## Project Structure

```
particle_master/
├── RSEngine/                    # Engine library project
│   ├── src/                    # Engine source code
│   ├── RSEngine/               # Engine header files
│   ├── extern/                 # External libraries (ImGui, etc.)
│   ├── resources/              # Engine resources (shaders, textures)
│   ├── doxy_scripts/           # Doxygen documentation scripts
│   ├── doxygen-awesome-css/    # Documentation stylesheets
│   ├── RSEngine.vcxproj        # Visual Studio project
│   └── README.md               # Engine documentation
│
├── rsengine_playground/         # Game application project
│   ├── src/                    # Game source code
│   │   ├── Level/             # Level-related code
│   │   ├── LevelData/         # Level data
│   │   ├── Widget/            # UI widgets
│   │   ├── GameData.h/cpp     # Game data management
│   │   └── GameStateManager.h/cpp  # Game state management
│   ├── resources/              # Game resources
│   │   ├── GLSL/              # Shader files
│   │   ├── Font/              # Font files
│   │   ├── Textures/          # Textures
│   │   └── TestObject/        # Test objects
│   ├── OutputFiles/            # Build outputs
│   ├── SPH_MINT.sln           # Visual Studio solution
│   ├── SeobJJangGames.vcxproj # Visual Studio project
│   └── main.cpp               # Application entry point
│
├── rs_engine_library/          # Engine external libraries
│   ├── extern/                # Third-party libraries
│   │   ├── glm/              # Math library
│   │   ├── image/            # Image loading (STB, FreeType)
│   │   ├── glfw3_mt.lib      # GLFW (window/input)
│   │   └── glew32s.lib       # GLEW (OpenGL extensions)
│   └── rs_engine/             # Engine headers/libraries
│
├── BUILD_GUIDE.md              # Detailed build guide
├── WORKSPACE_GUIDE.md          # Workspace guide
└── particle_master.code-workspace  # VSCode workspace settings
```

### Build Dependency Chain

```
SeobJJangGames.exe
    └── RSEngine.lib (Static Library)
        ├── OpenGL
        │   ├── glfw3_mt.lib (window/input management)
        │   └── glew32s.lib (OpenGL extension loader)
        ├── GLM (math library)
        ├── FreeType (font rendering)
        ├── ImGui (UI library)
        └── STB Image (image loading)
```

---

## Build Environment

### Required Software

#### Visual Studio 2022 (or 2019)
- **Workload**: Desktop development with C++
- **Components**:
  - Windows 10 SDK
  - MSVC v143 (or v142) toolset
  - MSBuild

#### Visual Studio Code (Optional)
- **Extensions**:
  - C/C++ (Microsoft)
  - C/C++ Extension Pack

### Library Dependencies

All required libraries are included in the `rs_engine_library/` folder:

| Library | Version | Purpose | Location |
|---------|---------|---------|----------|
| **GLFW** | 3.x | Window/input management | `rs_engine_library/extern/` |
| **GLEW** | 2.x | OpenGL extension loader | `rs_engine_library/extern/` |
| **GLM** | 0.9.9+ | Math library | `rs_engine_library/extern/glm/` |
| **FreeType** | 2.x | Font rendering | `rs_engine_library/extern/image/freetype/` |
| **ImGui** | 1.x | UI library | `RSEngine/extern/imgui/` |
| **STB Image** | Latest | Image loading | `rs_engine_library/extern/image/` |

### System Requirements

- **OS**: Windows 10/11 (64-bit)
- **CPU**: x64 architecture
- **GPU**: Graphics card with OpenGL 4.3+ support
- **RAM**: Minimum 8GB (16GB recommended)
- **Storage**: 2GB or more

---

## Build Instructions

### Building with Visual Studio

#### 1. Open Solution
```
rsengine_playground/SPH_MINT.sln
```

#### 2. Select Build Configuration
- **Debug (x64)**: Debug build
- **Release (x64)**: Optimized release build

#### 3. Build Order
1. **Build RSEngine Project** (Static Library)
   - Right-click `RSEngine` project in Solution Explorer
   - Select "Build"
   
2. **Build SeobJJangGames Project** (Application)
   - Right-click `SeobJJangGames` project in Solution Explorer
   - Select "Build"

Or build entire solution:
- Menu: `Build → Build Solution` (Ctrl+Shift+B)

### Building with VSCode

#### 1. Launch VSCode from Developer Command Prompt

Search for "Developer Command Prompt for VS 2022" in Start Menu and run:

```cmd
cd C:\Users\WhatSseob\Desktop\particle_master
code .
```

#### 2. Use VSCode Build Tasks

**Ctrl+Shift+P** → "Tasks: Run Task":

- **Build Debug (x64)**: Full solution debug build (default)
- **Build Release (x64)**: Full solution release build
- **Build RSEngine Debug (x64)**: Engine only debug build
- **Build RSEngine Release (x64)**: Engine only release build
- **Build SeobJJangGames Debug (x64)**: Game only debug build
- **Build SeobJJangGames Release (x64)**: Game only release build

Or use shortcut **Ctrl+Shift+B** to run default build task

#### 3. Build Directly from Terminal

```cmd
msbuild rsengine_playground/SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /m
```

### Build Output Locations

After build completion, executable locations:

- **Debug Build**:
  ```
  rsengine_playground/OutputFiles/Debug/SeobJJangGames.exe
  ```

- **Release Build**:
  ```
  rsengine_playground/OutputFiles/Release/SeobJJangGames.exe
  ```

---

## Running the Application

### Running from Visual Studio

1. Set `SeobJJangGames` as startup project
2. **F5** (Debug) or **Ctrl+F5** (Run without debugging)

### Running from VSCode

1. After successful build, run from terminal:

```cmd
# Run Debug
.\rsengine_playground\OutputFiles\Debug\SeobJJangGames.exe

# Run Release
.\rsengine_playground\OutputFiles\Release\SeobJJangGames.exe
```

2. Or run directly from Windows Explorer

### Pre-run Checklist

- ✅ Verify build completed successfully
- ✅ Ensure `resources/` folder is in same directory as executable
- ✅ Verify OpenGL 4.3+ compatible GPU driver is installed

---

## Technologies Used

### Programming Languages
- **C++20**: Utilizing latest C++ standards
- **GLSL**: OpenGL Shading Language (Compute/Vertex/Fragment Shaders)

### Graphics API & Libraries
- **OpenGL 4.3+**: Core rendering API
- **GLFW 3.x**: Cross-platform window library
- **GLEW 2.x**: OpenGL extension management
- **GLM**: OpenGL mathematics library

### UI & Text
- **ImGui**: Immediate mode GUI library
- **FreeType**: Font rendering engine

### Image Processing
- **STB Image**: Lightweight image loader

### Development Tools
- **Visual Studio 2022**: Primary IDE
- **MSBuild**: Build system
- **Doxygen**: Code documentation generator
- **Visual Studio Code**: Alternative editor

### Architecture Patterns
- **Component-Based Architecture**: Modular system design
- **Singleton Pattern**: Resource managers
- **State Pattern**: Game state management
- **Event System**: Decoupled inter-system communication

---

## Documentation

### RSEngine Documentation

- **README**: `RSEngine/README.md`
- **Doxygen Documentation**: 
  - Generate: Run `RSEngine/doxy_scripts/gen_doc.sh`
  - Location: `RSEngine/docs/html/index.html`
- **References**: `RSEngine/reference.txt`

### Build Guides

- **Detailed Build Guide**: `BUILD_GUIDE.md`
- **Workspace Guide**: `WORKSPACE_GUIDE.md`

### Key Documentation Topics

#### RSEngine Architecture
```
RSEngine::Initialize()
  |-> AddSystem(Application)
  |-> AddSystem(Timer)
  |-> AddSystem(Input)
  |-> AddSystem(Physics)
  |-> AddSystem(EventHandler)
  |-> AddSystem(Graphics)
  \-> AddSystem(State)
```

#### Update Loop
```cpp
while (is_running)
{
    float dt = IsFixedFps() ? fixed_delta_time : actual_delta_time;
    
    if (IsUpdateable())
    {
        Update(dt);      // Update each system
        PostUpdate();    // Post update
    }
    
    Render();           // Rendering
    PostRender();       // Post rendering
}
```

### Game State System

Available game states in SeobJJangGames:

| State ID | Name | Description |
|----------|------|-------------|
| `SRG_JORDY` | Jordy Roll Game | Physics-based rolling game |
| `SRG_PARTICLE_SIMULATOR` | Particle Simulator | SPH particle simulator |
| `SRG_VTK_LOADER` | VTK Loader | VTK data loader |
| `SRG_POINT_CLIPPER` | Point Clipper | Point cloud clipping tool |

---

## Troubleshooting

### Build Errors

#### 1. "MSBuild not found"
**Solution**: Launch VSCode from Developer Command Prompt
```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
cd C:\Users\WhatSseob\Desktop\particle_master
code .
```

#### 2. "External libraries not found"
**Check**:
- Verify `rs_engine_library/extern/` folder exists
- Check library paths in project properties
- Try rebuilding entire solution

#### 3. "LNK2019: unresolved external symbol"
**Solution**:
1. Build RSEngine project first
2. Verify build configuration (Debug/Release) and platform (x64) match
3. Try clean build: `Clean Debug (x64)` → `Build Debug (x64)`

### Runtime Errors

#### 1. "OpenGL error" or "GPU initialization failed"
**Solution**:
- Install latest GPU drivers
- Verify OpenGL 4.3+ support
- Configure dedicated GPU in NVIDIA/AMD control panel

#### 2. "Resources not found"
**Solution**:
- Ensure `resources/` folder is in same directory as executable
- Or run from `rsengine_playground/` directory

#### 3. "DLL not found"
**Solution**:
- Static library build requires no additional DLLs
- Try rebuilding
- Verify Windows SDK installation

### Debugging

#### Memory Leak Detection (Debug build)
```cpp
// Enabled in main.cpp
#if defined(DEBUG) || defined(_DEBUG)
_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
// _CrtSetBreakAlloc(number); // Break at specific allocation
#endif
```

#### Log Checking
```cpp
RS_MSG("Message");      // General message
RS_WARNING("Warning");  // Warning
RS_ERROR("Error");      // Error
```

---

## Performance Optimization

### RSEngine Optimization Features

- **GPU Instancing**: Efficient rendering of massive particles
- **Compute Shaders**: GPU parallel computation
- **Grid-based SPH**: Improved time complexity from O(n²) to O(n)
- **Frustum Culling**: Skip rendering off-screen objects

### Recommended Settings

**Small-scale Simulation (< 10,000 particles)**:
- Static Brute Force Solver
- High Quality rendering

**Large-scale Simulation (> 10,000 particles)**:
- Static Grid Solver
- Instanced rendering

---

## Contributing

### Developer
- **Jinseob Park** - Lead developer and architecture designer

### How to Contribute
1. Create an issue or feature request
2. Fork and create a feature branch
3. Commit changes
4. Submit Pull Request

---

## References

### Technical Documentation
- [OpenGL Documentation](https://www.opengl.org/documentation/)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [ImGui Documentation](https://github.com/ocornut/imgui)

### SPH Simulation
- Smoothed Particle Hydrodynamics papers and resources
- Fluid dynamics simulation techniques

### Project Resources
- **Repository**: GitHub - particle_master
- **Branch**: refactor/revive
- **Owner**: JinseobPark

---

## Author

**Jinseob Park (WhatSseob)**
- GitHub: [@JinseobPark](https://github.com/JinseobPark)
- Repository: [particle_master](https://github.com/JinseobPark/particle_master)

---

**Built with Rock & Stone!**

*Last Updated: November 10, 2025*

---
---

<a name="korean"></a>
## 한국어 버전

이 저장소는 두 개의 주요 프로젝트로 구성되어 있습니다:
- **RSEngine**: 실시간 물리 시뮬레이션 및 렌더링을 위한 게임 엔진 라이브러리
- **SeobJJangGames**: RSEngine을 활용한 게임 애플리케이션

---

## 목차

- [프로젝트 개요](#프로젝트-개요)
- [주요 기능](#주요-기능)
- [프로젝트 구조](#프로젝트-구조)
- [빌드 환경](#빌드-환경)
- [빌드 방법](#빌드-방법)
- [실행 방법](#실행-방법)
- [사용 기술](#사용-기술)
- [문서](#문서)
- [라이센스](#라이센스)

---

## 프로젝트 개요

### RSEngine (Realtime-Simulation Engine)

RSEngine은 C++20으로 개발된 커스텀 게임 엔진으로, GPU 가속 SPH(Smoothed Particle Hydrodynamics) 유체 시뮬레이션과 모듈식 컴포넌트 기반 아키텍처를 특징으로 합니다.

**버전**: 0.0.6  
**개발자**: Jinseob Park  
**언어**: C++20  
**그래픽 API**: OpenGL 4.3+  
**타입**: Static Library (.lib)

#### 핵심 특징

- **GPU 가속 물리 시뮬레이션**: OpenGL Compute Shader 기반 파티클 물리
- **실시간 유체 역학**: SPH 방식의 유체 시뮬레이션
- **PBR 렌더링 시스템**: Physically Based Rendering 머티리얼 시스템
- **모듈식 아키텍처**: 컴포넌트 기반 설계로 확장성 우수
- **데이터 시각화**: VTK 파일 로딩, 포인트 클리핑, 속성 기반 컬러링

### SeobJJangGames

RSEngine을 기반으로 개발된 게임 애플리케이션으로, 다양한 게임 스테이트와 시뮬레이션 기능을 제공합니다.

**타입**: Application (.exe)  
**의존성**: RSEngine Static Library

#### 주요 게임 스테이트

- **Jordy Roll Game**: 물리 기반 게임플레이
- **Particle Simulator**: 실시간 파티클 시뮬레이션 도구
- **VTK Loader**: 과학 데이터 시각화
- **Point Clipper**: 포인트 클라우드 편집 도구

---

## 주요 기능

### RSEngine 엔진 기능

#### 1. 유체 시뮬레이션 (SPH)
- **다중 솔버 지원**:
  - Static Brute Force (정확도 우선, 소규모)
  - Static Grid (최적화, 대규모)
  - Dynamic Brute Force (유입/유출 지원)
- **실시간 파라미터 조정**: 밀도, 점성, 압력, 중력
- **충돌 감지**: 메시 기반 충돌 시스템
- **시각화 모드**: 속도, 밀도, 압력, 가속도 기반 컬러 코딩

#### 2. 렌더링 시스템
- **PBR 머티리얼**: Metallic/Roughness 워크플로우
- **듀얼 파이프라인**: Deferred/Forward 렌더링
- **파티클 렌더링**: GPU 인스턴싱으로 수백만 파티클 처리
- **범례 시스템**: 동적 컬러 매핑 및 최소/최대값 계산
- **클리핑 도구**: Plane/Cube/Sphere 기반 파티클 필터링

#### 3. 엔진 아키텍처
- **시스템 컴포넌트**: Application, Input, Physics, Graphics, Timer, Event Handler
- **리소스 관리**: 중앙화된 싱글톤 매니저
- **고정/가변 FPS**: 설정 가능한 업데이트 루프
- **이벤트 시스템**: 시스템 간 분리된 통신

#### 4. 데이터 처리
- **VTK 파일 로딩**: 과학 데이터 시각화 형식 지원
- **포인트 클리핑**: 3D 공간에서 데이터 필터링
- **속성 기반 렌더링**: 데이터 값에 따른 시각화

### SeobJJangGames 게임 기능

#### 1. 게임 스테이트 관리
- **상태 기반 아키텍처**: 게임 스테이트 매니저를 통한 씬 관리
- **동적 상태 전환**: 런타임 상태 전환 지원
- **게임 데이터 관리**: 싱글톤 패턴의 중앙화된 게임 데이터

#### 2. GPU 최적화
- **NVIDIA Optimus 지원**: 전용 GPU 자동 선택
- **AMD PowerXpress 지원**: 고성능 GPU 활성화

#### 3. 디버깅 지원
- **메모리 누수 감지**: CRT 디버그 힙 통합
- **개발자 콘솔**: 런타임 디버깅 도구

---

## 프로젝트 구조

```
particle_master/
├── RSEngine/                    # 엔진 라이브러리 프로젝트
│   ├── src/                    # 엔진 소스 코드
│   ├── RSEngine/               # 엔진 헤더 파일
│   ├── extern/                 # 외부 라이브러리 (ImGui 등)
│   ├── resources/              # 엔진 리소스 (쉐이더, 텍스처)
│   ├── doxy_scripts/           # Doxygen 문서 생성 스크립트
│   ├── doxygen-awesome-css/    # 문서 스타일시트
│   ├── RSEngine.vcxproj        # Visual Studio 프로젝트
│   └── README.md               # 엔진 문서
│
├── rsengine_playground/         # 게임 애플리케이션 프로젝트
│   ├── src/                    # 게임 소스 코드
│   │   ├── Level/             # 레벨 관련 코드
│   │   ├── LevelData/         # 레벨 데이터
│   │   ├── Widget/            # UI 위젯
│   │   ├── GameData.h/cpp     # 게임 데이터 관리
│   │   └── GameStateManager.h/cpp  # 게임 스테이트 관리
│   ├── resources/              # 게임 리소스
│   │   ├── GLSL/              # 쉐이더 파일
│   │   ├── Font/              # 폰트 파일
│   │   ├── Textures/          # 텍스처
│   │   └── TestObject/        # 테스트 오브젝트
│   ├── OutputFiles/            # 빌드 출력물
│   ├── SPH_MINT.sln           # Visual Studio 솔루션
│   ├── SeobJJangGames.vcxproj # Visual Studio 프로젝트
│   └── main.cpp               # 애플리케이션 진입점
│
├── rs_engine_library/          # 엔진 외부 라이브러리
│   ├── extern/                # 서드파티 라이브러리
│   │   ├── glm/              # 수학 라이브러리
│   │   ├── image/            # 이미지 로딩 (STB, FreeType)
│   │   ├── glfw3_mt.lib      # GLFW (윈도우/입력)
│   │   └── glew32s.lib       # GLEW (OpenGL 확장)
│   └── rs_engine/             # 엔진 헤더/라이브러리
│
├── BUILD_GUIDE.md              # 상세 빌드 가이드
├── WORKSPACE_GUIDE.md          # 워크스페이스 가이드
└── particle_master.code-workspace  # VSCode 워크스페이스 설정
```

### 빌드 의존성 관계

```
SeobJJangGames.exe
    └── RSEngine.lib (Static Library)
        ├── OpenGL
        │   ├── glfw3_mt.lib (윈도우/입력 관리)
        │   └── glew32s.lib (OpenGL 확장 로더)
        ├── GLM (수학 라이브러리)
        ├── FreeType (폰트 렌더링)
        ├── ImGui (UI 라이브러리)
        └── STB Image (이미지 로딩)
```

---

## 빌드 환경

### 필수 소프트웨어

#### Visual Studio 2022 (또는 2019)
- **워크로드**: C++ 데스크톱 개발
- **컴포넌트**:
  - Windows 10 SDK
  - MSVC v143 (또는 v142) 도구 집합
  - MSBuild

#### Visual Studio Code (선택사항)
- **확장 프로그램**:
  - C/C++ (Microsoft)
  - C/C++ Extension Pack

### 라이브러리 의존성

모든 필수 라이브러리는 `rs_engine_library/` 폴더에 포함되어 있습니다:

| 라이브러리 | 버전 | 용도 | 위치 |
|----------|------|------|------|
| **GLFW** | 3.x | 윈도우/입력 관리 | `rs_engine_library/extern/` |
| **GLEW** | 2.x | OpenGL 확장 로더 | `rs_engine_library/extern/` |
| **GLM** | 0.9.9+ | 수학 라이브러리 | `rs_engine_library/extern/glm/` |
| **FreeType** | 2.x | 폰트 렌더링 | `rs_engine_library/extern/image/freetype/` |
| **ImGui** | 1.x | UI 라이브러리 | `RSEngine/extern/imgui/` |
| **STB Image** | Latest | 이미지 로딩 | `rs_engine_library/extern/image/` |

### 시스템 요구사항

- **OS**: Windows 10/11 (64-bit)
- **CPU**: x64 아키텍처
- **GPU**: OpenGL 4.3+ 지원 그래픽 카드
- **RAM**: 최소 8GB (16GB 권장)
- **저장공간**: 2GB 이상

---

## 빌드 방법

### Visual Studio에서 빌드

#### 1. 솔루션 열기
```
rsengine_playground/SPH_MINT.sln
```

#### 2. 빌드 구성 선택
- **Debug (x64)**: 디버깅용 빌드
- **Release (x64)**: 최적화된 릴리스 빌드

#### 3. 빌드 순서
1. **RSEngine 프로젝트 빌드** (Static Library)
   - 솔루션 탐색기에서 `RSEngine` 프로젝트 우클릭
   - "빌드" 선택
   
2. **SeobJJangGames 프로젝트 빌드** (Application)
   - 솔루션 탐색기에서 `SeobJJangGames` 프로젝트 우클릭
   - "빌드" 선택

또는 전체 솔루션 빌드:
- 메뉴: `빌드 → 솔루션 빌드` (Ctrl+Shift+B)

### VSCode에서 빌드

#### 1. Developer Command Prompt에서 VSCode 실행

**시작 메뉴**에서 "Developer Command Prompt for VS 2022" 검색 후 실행:

```cmd
cd C:\Users\WhatSseob\Desktop\particle_master
code .
```

#### 2. VSCode 빌드 태스크 사용

**Ctrl+Shift+P** → "Tasks: Run Task" 선택:

- **Build Debug (x64)**: 전체 솔루션 디버그 빌드 (기본)
- **Build Release (x64)**: 전체 솔루션 릴리스 빌드
- **Build RSEngine Debug (x64)**: 엔진만 디버그 빌드
- **Build RSEngine Release (x64)**: 엔진만 릴리스 빌드
- **Build SeobJJangGames Debug (x64)**: 게임만 디버그 빌드
- **Build SeobJJangGames Release (x64)**: 게임만 릴리스 빌드

또는 단축키 **Ctrl+Shift+B**로 기본 빌드 태스크 실행

#### 3. 터미널에서 직접 빌드

```cmd
msbuild rsengine_playground/SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /m
```

### 빌드 출력 위치

빌드 완료 후 실행 파일 위치:

- **Debug 빌드**:
  ```
  rsengine_playground/OutputFiles/Debug/SeobJJangGames.exe
  ```

- **Release 빌드**:
  ```
  rsengine_playground/OutputFiles/Release/SeobJJangGames.exe
  ```

---

## 실행 방법

### Visual Studio에서 실행

1. `SeobJJangGames` 프로젝트를 시작 프로젝트로 설정
2. **F5** (디버그 실행) 또는 **Ctrl+F5** (디버그 없이 실행)

### VSCode에서 실행

1. 빌드 완료 후 터미널에서 실행:

```cmd
# Debug 실행
.\rsengine_playground\OutputFiles\Debug\SeobJJangGames.exe

# Release 실행
.\rsengine_playground\OutputFiles\Release\SeobJJangGames.exe
```

2. 또는 Windows 탐색기에서 직접 실행

### 실행 전 확인사항

- ✅ 빌드가 성공적으로 완료되었는지 확인
- ✅ `resources/` 폴더가 실행 파일과 같은 디렉토리에 있는지 확인
- ✅ OpenGL 4.3 이상 지원 GPU 드라이버 설치 확인

---

## 사용 기술

### 프로그래밍 언어
- **C++20**: 최신 C++ 표준 활용
- **GLSL**: OpenGL Shading Language (Compute/Vertex/Fragment Shaders)

### 그래픽스 API & 라이브러리
- **OpenGL 4.3+**: 코어 렌더링 API
- **GLFW 3.x**: 크로스 플랫폼 윈도우 라이브러리
- **GLEW 2.x**: OpenGL 확장 관리
- **GLM**: OpenGL 수학 라이브러리

### UI & 텍스트
- **ImGui**: 즉시 모드 GUI 라이브러리
- **FreeType**: 폰트 렌더링 엔진

### 이미지 처리
- **STB Image**: 경량 이미지 로더

### 개발 도구
- **Visual Studio 2022**: 주 IDE
- **MSBuild**: 빌드 시스템
- **Doxygen**: 코드 문서 생성
- **Visual Studio Code**: 대체 편집기

### 아키텍처 패턴
- **Component-Based Architecture**: 모듈식 시스템 설계
- **Singleton Pattern**: 리소스 매니저
- **State Pattern**: 게임 스테이트 관리
- **Event System**: 분리된 시스템 간 통신

---

## 문서

### RSEngine 문서

- **README**: `RSEngine/README.md`
- **Doxygen 문서**: 
  - 생성: `RSEngine/doxy_scripts/gen_doc.sh` 실행
  - 위치: `RSEngine/docs/html/index.html`
- **참고 자료**: `RSEngine/reference.txt`

### 빌드 가이드

- **상세 빌드 가이드**: `BUILD_GUIDE.md`
- **워크스페이스 가이드**: `WORKSPACE_GUIDE.md`

### 주요 문서 내용

#### RSEngine 아키텍처
```
RSEngine::Initialize()
  |-> AddSystem(Application)
  |-> AddSystem(Timer)
  |-> AddSystem(Input)
  |-> AddSystem(Physics)
  |-> AddSystem(EventHandler)
  |-> AddSystem(Graphics)
  \-> AddSystem(State)
```

#### 업데이트 루프
```cpp
while (is_running)
{
    float dt = IsFixedFps() ? fixed_delta_time : actual_delta_time;
    
    if (IsUpdateable())
    {
        Update(dt);      // 각 시스템 업데이트
        PostUpdate();    // 포스트 업데이트
    }
    
    Render();           // 렌더링
    PostRender();       // 포스트 렌더링
}
```

### 게임 스테이트 시스템

SeobJJangGames에서 사용 가능한 게임 스테이트:

| 스테이트 ID | 이름 | 설명 |
|-----------|------|------|
| `SRG_JORDY` | Jordy Roll Game | 물리 기반 롤링 게임 |
| `SRG_PARTICLE_SIMULATOR` | Particle Simulator | SPH 파티클 시뮬레이터 |
| `SRG_VTK_LOADER` | VTK Loader | VTK 데이터 로더 |
| `SRG_POINT_CLIPPER` | Point Clipper | 포인트 클라우드 클리핑 도구 |

---

## 문제 해결

### 빌드 오류

#### 1. "MSBuild를 찾을 수 없음"
**해결책**: Developer Command Prompt에서 VSCode 실행
```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
cd C:\Users\WhatSseob\Desktop\particle_master
code .
```

#### 2. "외부 라이브러리를 찾을 수 없음"
**확인 사항**:
- `rs_engine_library/extern/` 폴더 존재 확인
- 프로젝트 속성에서 라이브러리 경로 확인
- 솔루션 전체 리빌드 시도

#### 3. "LNK2019: 해결되지 않은 외부 기호"
**해결책**:
1. RSEngine 프로젝트 먼저 빌드
2. 빌드 구성(Debug/Release)과 플랫폼(x64) 일치 확인
3. 클린 빌드 시도: `Clean Debug (x64)` → `Build Debug (x64)`

### 실행 오류

#### 1. "OpenGL 오류" 또는 "GPU 초기화 실패"
**해결책**:
- 최신 GPU 드라이버 설치
- OpenGL 4.3 이상 지원 확인
- NVIDIA/AMD 제어판에서 전용 GPU 사용 설정

#### 2. "리소스를 찾을 수 없음"
**해결책**:
- `resources/` 폴더가 실행 파일과 같은 디렉토리에 있는지 확인
- 또는 `rsengine_playground/` 디렉토리에서 실행

#### 3. "DLL을 찾을 수 없음"
**해결책**:
- Static Library 빌드이므로 추가 DLL 불필요
- 재빌드 시도
- Windows SDK 설치 확인

### 디버깅

#### 메모리 누수 확인 (Debug 빌드)
```cpp
// main.cpp에서 활성화됨
#if defined(DEBUG) || defined(_DEBUG)
_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
// _CrtSetBreakAlloc(number); // 특정 할당 번호에서 중단
#endif
```

#### 로그 확인
```cpp
RS_MSG("메시지");      // 일반 메시지
RS_WARNING("경고");    // 경고
RS_ERROR("오류");      // 오류
```

---

## 성능 최적화

### RSEngine 최적화 기능

- **GPU 인스턴싱**: 대량의 파티클 효율적 렌더링
- **Compute Shader**: GPU 병렬 연산 활용
- **Grid-based SPH**: O(n²) → O(n) 시간 복잡도 개선
- **Frustum Culling**: 화면 밖 객체 렌더링 생략

### 권장 설정

**소규모 시뮬레이션 (< 10,000 파티클)**:
- Static Brute Force Solver
- High Quality 렌더링

**대규모 시뮬레이션 (> 10,000 파티클)**:
- Static Grid Solver
- Instanced 렌더링

---

## 기여

### 개발자
- **Jinseob Park** - 주 개발자 및 아키텍처 설계

### 기여 방법
1. 이슈 생성 또는 기능 제안
2. Fork 후 기능 브랜치 생성
3. 변경사항 커밋
4. Pull Request 제출

---

## 참고 자료

### 기술 문서
- [OpenGL Documentation](https://www.opengl.org/documentation/)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [ImGui Documentation](https://github.com/ocornut/imgui)

### SPH 시뮬레이션
- Smoothed Particle Hydrodynamics 관련 논문 및 자료
- 유체 역학 시뮬레이션 기술

### 프로젝트 리소스
- **저장소**: GitHub - particle_master
- **브랜치**: refactor/revive
- **소유자**: JinseobPark

---

## 저자

**Jinseob Park (WhatSseob)**
- GitHub: [@JinseobPark](https://github.com/JinseobPark)
- Repository: [particle_master](https://github.com/JinseobPark/particle_master)

---

**Built with Rock & Stone!**

*마지막 업데이트: 2025년 11월 10일*
