# RSEngine + SeobJJangGames 빌드 가이드

## 목차
1. [프로젝트 구조 개요](#프로젝트-구조-개요)
2. [빌드 환경 요구사항](#빌드-환경-요구사항)
3. [빌드 환경 구성](#빌드-환경-구성)
4. [VSCode에서 빌드하기](#vscode에서-빌드하기)
5. [VSCode에서 실행 및 디버깅](#vscode에서-실행-및-디버깅)
6. [문제 해결](#문제-해결)

---

## 프로젝트 구조 개요

이 프로젝트는 두 개의 주요 컴포넌트로 구성되어 있습니다:

### 1. RSEngine (엔진 라이브러리)
- **위치**: `RSEngine/`
- **타입**: Static Library (.lib)
- **역할**: 게임 엔진 핵심 기능 제공
  - OpenGL 렌더링 시스템
  - SPH 파티클 시뮬레이션
  - 물리 엔진
  - 리소스 관리 시스템
  - 이벤트 시스템

### 2. SeobJJangGames (게임 애플리케이션)
- **위치**: `rsengine_playground/`
- **타입**: Application (.exe)
- **역할**: RSEngine을 사용하는 게임 애플리케이션
- **의존성**: RSEngine 라이브러리에 의존

### 빌드 의존성 관계
```
SeobJJangGames.exe
    └── RSEngine.lib (Static Library)
        ├── OpenGL (glfw3_mt.lib, glew32s.lib)
        ├── FreeType (freetype.lib)
        └── External Dependencies
```

---

## 빌드 환경 요구사항

### 필수 소프트웨어

1. **Visual Studio 2022** (또는 2019)
   - C++ 데스크톱 개발 워크로드 설치
   - Windows 10 SDK
   - MSVC v143 (또는 v142) 도구 집합
   - CMake 도구 (선택사항)

2. **Visual Studio Code** (최신 버전)
   - 필수 확장 프로그램:
     - C/C++ (Microsoft)
     - C/C++ Extension Pack
     - CMake Tools (선택사항)

3. **MSBuild**
   - Visual Studio와 함께 설치됨
   - PATH에 자동 등록되어 있어야 함

### 라이브러리 의존성

프로젝트는 다음 라이브러리들을 사용합니다:

| 라이브러리 | 용도 | 위치 |
|----------|------|------|
| **GLFW** | 윈도우/입력 관리 | `rs_engine_library/extern/` |
| **GLEW** | OpenGL 확장 로더 | `rs_engine_library/extern/` |
| **GLM** | 수학 라이브러리 | `rs_engine_library/extern/glm/` |
| **FreeType** | 폰트 렌더링 | `rs_engine_library/extern/image/freetype/` |
| **ImGui** | UI 라이브러리 | `RSEngine/extern/imgui/` |
| **STB Image** | 이미지 로딩 | `rs_engine_library/extern/image/` |

**중요**: 모든 라이브러리는 이미 `rs_engine_library/` 폴더에 포함되어 있습니다.

---

## 빌드 환경 구성

### 1. Developer Command Prompt 설정

VSCode에서 MSBuild를 사용하려면 Visual Studio의 개발자 도구가 PATH에 등록되어 있어야 합니다.

#### 방법 1: Developer Command Prompt에서 VSCode 실행 (권장)

1. **시작 메뉴**에서 "Developer Command Prompt for VS 2022" 검색
2. 실행 후 프로젝트 폴더로 이동:
   ```cmd
   cd C:\Users\WhatSseob\Desktop\particle_master
   ```
3. VSCode 실행:
   ```cmd
   code .
   ```

이 방법으로 실행하면 MSBuild와 모든 Visual Studio 도구가 자동으로 PATH에 등록됩니다.

#### 방법 2: VSCode 통합 터미널에서 환경 설정

VSCode를 일반적으로 실행한 경우, 터미널에서 다음 명령 실행:

```powershell
# Visual Studio 2022 환경 설정
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1"

# 또는 Visual Studio 2019인 경우
& "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\Launch-VsDevShell.ps1"
```

### 2. 프로젝트 구조 확인

빌드 전에 다음 구조가 올바른지 확인하세요:

```
particle_master/
├── RSEngine/                      # 엔진 프로젝트
│   ├── RSEngine.vcxproj
│   ├── src/                       # 엔진 소스 코드
│   ├── extern/                    # 엔진 전용 외부 라이브러리 (imgui 등)
│   └── resources/                 # 엔진 리소스 (셰이더, 폰트 등)
│
├── rsengine_playground/           # 게임 프로젝트
│   ├── SPH_MINT.sln              # **솔루션 파일** (중요!)
│   ├── SeobJJangGames.vcxproj    # 게임 프로젝트 파일
│   ├── main.cpp                   # 게임 진입점
│   ├── src/                       # 게임 소스 코드
│   └── resources/                 # 게임 리소스
│
├── rs_engine_library/             # 공유 라이브러리 저장소
│   ├── extern/                    # 외부 라이브러리 (GLFW, GLEW, GLM 등)
│   │   ├── GL/
│   │   ├── glfw/
│   │   ├── glm/
│   │   ├── image/freetype/
│   │   └── lib/                   # 라이브러리 파일 (.lib)
│   └── rs_engine/                 # 빌드된 RSEngine 출력
│       ├── lib/                   # RSEngineD.lib / RSEngine.lib
│       └── headers/               # 복사된 헤더 파일
│
├── .vscode/                       # VSCode 설정 (자동 생성됨)
│   ├── tasks.json                # 빌드 태스크
│   └── launch.json               # 디버그 설정
│
└── OutputFiles/                   # 게임 실행 파일 출력
    ├── Debug/
    │   └── SeobJJangGames.exe
    └── Release/
        └── SeobJJangGames.exe
```

### 3. 빌드 구성 이해하기

프로젝트는 여러 빌드 구성을 지원합니다:

#### RSEngine 빌드 구성

| 구성 | 플랫폼 | 타입 | 출력 | 용도 |
|------|--------|------|------|------|
| **Debug** | x64 | Static Library | RSEngineD.lib | 개발 및 디버깅 |
| **Release** | x64 | Static Library | RSEngine.lib | 최종 릴리스 |
| Debug_DLL | x64 | Dynamic Library | RSEngineD.dll | DLL 버전 (선택사항) |
| Release_DLL | x64 | Dynamic Library | RSEngine.dll | DLL 버전 (선택사항) |

**권장 구성**: Debug (x64) / Release (x64) - Static Library

#### SeobJJangGames 빌드 구성

| 구성 | 플랫폼 | 출력 | 특징 |
|------|--------|------|------|
| **Debug** | x64 | SeobJJangGames.exe | - 디버그 심볼 포함<br>- 콘솔 윈도우 표시<br>- 메모리 누수 검사 활성화 |
| **Release** | x64 | SeobJJangGames.exe | - 최적화 활성화<br>- 디버그 정보 제거<br>- 콘솔 숨김 (윈도우 서브시스템) |

---

## VSCode에서 빌드하기

VSCode 설정 파일(`.vscode/tasks.json`)이 이미 구성되어 있습니다.

### 빌드 태스크 목록

1. **Build Debug (x64)** - 기본 빌드 (Ctrl+Shift+B)
   - RSEngine과 SeobJJangGames 모두 빌드
   
2. **Build Release (x64)** - 릴리스 빌드
   
3. **Build RSEngine Debug (x64)** - 엔진만 빌드
   
4. **Build SeobJJangGames Debug (x64)** - 게임만 빌드 (엔진 자동 빌드)

5. **Clean Debug (x64)** - 빌드 아티팩트 정리

6. **Rebuild Debug (x64)** - 전체 리빌드

### 빌드 방법

#### 방법 1: 키보드 단축키 사용 (가장 빠름)

1. **Ctrl + Shift + B** 누르기
2. "Build Debug (x64)" 선택 (기본값)

#### 방법 2: 명령 팔레트 사용

1. **Ctrl + Shift + P** (또는 F1) 눌러 명령 팔레트 열기
2. "Tasks: Run Build Task" 입력
3. 원하는 빌드 태스크 선택

#### 방법 3: 터미널에서 직접 실행

VSCode 통합 터미널 (Ctrl + `)에서:

```powershell
# 전체 솔루션 빌드 (Debug)
msbuild rsengine_playground/SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /m

# 전체 솔루션 빌드 (Release)
msbuild rsengine_playground/SPH_MINT.sln /p:Configuration=Release /p:Platform=x64 /m

# RSEngine만 빌드
msbuild rsengine_playground/SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /t:RSEngine /m

# SeobJJangGames만 빌드
msbuild rsengine_playground/SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /t:SeobJJangGames /m

# 클린 빌드
msbuild rsengine_playground/SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /t:Clean
msbuild rsengine_playground/SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /t:Rebuild /m
```

### 빌드 출력 확인

빌드가 성공하면 다음 위치에 파일이 생성됩니다:

#### Debug 빌드
```
rs_engine_library/rs_engine/lib/RSEngineD.lib          # 엔진 라이브러리
rsengine_playground/OutputFiles/Debug/SeobJJangGames.exe  # 게임 실행 파일
rsengine_playground/OutputFiles/Debug/resources/       # 복사된 리소스
```

#### Release 빌드
```
rs_engine_library/rs_engine/lib/RSEngine.lib           # 엔진 라이브러리
rsengine_playground/OutputFiles/Release/SeobJJangGames.exe  # 게임 실행 파일
rsengine_playground/OutputFiles/Release/resources/     # 복사된 리소스
```

### Post-Build 이벤트

빌드 시 자동으로 수행되는 작업들:

#### RSEngine 빌드 후
```cmd
# 라이브러리 복사
copy RSEngineD.lib → rs_engine_library/rs_engine/lib/

# 헤더 파일 복사 (라이브러리 배포용)
xcopy src/*.h → rs_engine_library/rs_engine/headers/src/
xcopy extern/*.h → rs_engine_library/rs_engine/headers/extern/
```

#### SeobJJangGames 빌드 후
```cmd
# 리소스 폴더 복사
xcopy resources → OutputFiles/Debug/resources/
```

---

## VSCode에서 실행 및 디버깅

### 실행 구성

`.vscode/launch.json` 파일에 두 가지 실행 구성이 설정되어 있습니다:

1. **(MSVC) Debug - SeobJJangGames**
   - Debug 빌드 실행 및 디버깅
   - 중단점, 변수 검사 가능

2. **(MSVC) Release - SeobJJangGames**
   - Release 빌드 실행
   - 최적화된 성능

### 디버깅 방법

#### 방법 1: F5 키 사용 (권장)

1. `main.cpp` 또는 게임 소스 파일 열기
2. **F5** 키 누르기
3. 자동으로 빌드 후 디버깅 시작

#### 방법 2: 디버그 패널 사용

1. 좌측 사이드바에서 **디버그 아이콘** 클릭 (또는 Ctrl+Shift+D)
2. 상단 드롭다운에서 구성 선택:
   - "(MSVC) Debug - SeobJJangGames"
3. **녹색 재생 버튼** 클릭 또는 F5

#### 방법 3: 빌드 없이 실행

디버깅 없이 실행하려면:
- **Ctrl + F5** (디버거 연결 없이 실행)

### 중단점 설정

1. 소스 코드 파일 열기 (예: `main.cpp`, `GameStateManager.cpp`)
2. 줄 번호 왼쪽 클릭하여 중단점 설정
3. F5로 디버깅 시작
4. 중단점에서 멈추면:
   - **F10**: Step Over (다음 줄로)
   - **F11**: Step Into (함수 안으로)
   - **Shift+F11**: Step Out (함수 밖으로)
   - **F5**: Continue (계속 실행)

### 디버깅 패널 활용

디버깅 중 좌측 패널에서 확인 가능한 정보:

- **Variables**: 현재 스코프의 변수 값
- **Watch**: 감시할 표현식 추가
- **Call Stack**: 함수 호출 스택
- **Breakpoints**: 설정된 모든 중단점

### 디버그 콘솔

VSCode 하단의 **DEBUG CONSOLE** 탭에서:
- 프로그램 출력 확인
- 표현식 평가 (예: `myVector.size()`)
- 변수 값 조회

---

## 문제 해결

### 1. MSBuild를 찾을 수 없음

**증상**:
```
'msbuild'은(는) 내부 또는 외부 명령, 실행할 수 있는 프로그램, 또는
배치 파일이 아닙니다.
```

**해결 방법**:
1. Developer Command Prompt에서 VSCode 실행 (위 참조)
2. 또는 수동으로 PATH 추가:

```powershell
# PowerShell에서
$env:Path += ";C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin"

# 영구 설정 (시스템 환경 변수)
# 제어판 → 시스템 → 고급 시스템 설정 → 환경 변수
# Path에 추가: C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin
```

### 2. 헤더 파일을 찾을 수 없음

**증상**:
```
fatal error C1083: Cannot open include file: 'RSEngine.h'
```

**해결 방법**:
1. RSEngine을 먼저 빌드했는지 확인
2. 헤더 파일 경로 확인:
   ```
   rs_engine_library/rs_engine/headers/src/
   rs_engine_library/rs_engine/headers/extern/
   ```
3. `.vcxproj` 파일의 `IncludePath` 확인

### 3. 라이브러리 링크 오류

**증상**:
```
LINK : fatal error LNK1104: cannot open file 'RSEngineD.lib'
```

**해결 방법**:
1. RSEngine을 먼저 빌드
2. 라이브러리 파일 확인:
   - Debug: `rs_engine_library/rs_engine/lib/RSEngineD.lib`
   - Release: `rs_engine_library/rs_engine/lib/RSEngine.lib`
3. 빌드 순서 확인: RSEngine → SeobJJangGames

### 4. 실행 파일이 시작되지 않음

**증상**:
- 프로그램이 즉시 종료됨
- "DLL을 찾을 수 없습니다" 오류

**해결 방법**:
1. **리소스 폴더 확인**:
   ```
   OutputFiles/Debug/resources/
   ```
   이 폴더가 없으면 빌드가 제대로 완료되지 않은 것입니다.

2. **작업 디렉터리 확인**:
   `launch.json`의 `cwd` 설정이 올바른지 확인:
   ```json
   "cwd": "${workspaceFolder}/rsengine_playground/OutputFiles/Debug"
   ```

3. **리소스 수동 복사**:
   ```powershell
   xcopy /E /I /Y rsengine_playground\resources rsengine_playground\OutputFiles\Debug\resources
   ```

### 5. Python merge_shaders.py 오류

**증상**:
```
'python'은(는) 내부 또는 외부 명령이 아닙니다
```

**해결 방법**:
1. Python 3.x 설치
2. PATH에 Python 추가
3. 또는 Pre-Build 이벤트 비활성화 (이미 병합된 셰이더 사용)

### 6. IntelliSense 오류 (빨간 밑줄)

IntelliSense가 헤더를 찾지 못하는 경우:

**해결 방법**:
1. `.vscode/c_cpp_properties.json` 생성:

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/rs_engine_library/extern",
                "${workspaceFolder}/rs_engine_library/extern/image/freetype",
                "${workspaceFolder}/rs_engine_library/rs_engine/headers/src",
                "${workspaceFolder}/rs_engine_library/rs_engine/headers/extern",
                "${workspaceFolder}/RSEngine/src",
                "${workspaceFolder}/RSEngine/extern",
                "${workspaceFolder}/rsengine_playground/src"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE",
                "GLEW_STATIC"
            ],
            "windowsSdkVersion": "10.0.22000.0",
            "compilerPath": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.35.32215/bin/Hostx64/x64/cl.exe",
            "cStandard": "c17",
            "cppStandard": "c++20",
            "intelliSenseMode": "windows-msvc-x64"
        }
    ],
    "version": 4
}
```

2. VSCode 재시작

### 7. 빌드는 성공하지만 실행 시 충돌

**해결 방법**:
1. **Debug 빌드로 실행**하여 크래시 위치 확인
2. **메모리 누수 검사**:
   ```cpp
   // main.cpp에 이미 포함됨
   #if defined(DEBUG) || defined(_DEBUG)
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   #endif
   ```
3. **리소스 파일 경로** 확인:
   - 셰이더 파일: `resources/GLSL/`
   - 폰트 파일: `resources/Font/`
   - 모델 파일: `resources/TestObject/`

---

## 추가 팁

### 빠른 빌드

병렬 빌드 활성화 (`/m` 플래그 사용 중):
```powershell
msbuild SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /m:8
# /m:8 = 최대 8개 프로세스로 병렬 빌드
```

### 특정 프로젝트만 빌드

```powershell
# RSEngine만 빌드
msbuild SPH_MINT.sln /t:RSEngine /p:Configuration=Debug /p:Platform=x64

# SeobJJangGames만 빌드 (RSEngine 자동 빌드됨)
msbuild SPH_MINT.sln /t:SeobJJangGames /p:Configuration=Debug /p:Platform=x64
```

### 빌드 로그 저장

```powershell
msbuild SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /fl /flp:logfile=build.log;verbosity=detailed
```

### 성능 프로파일링

Release 빌드로 실행 시:
1. Visual Studio Profiler 사용
2. 또는 외부 프로파일러 (Intel VTune, AMD uProf)

---

## 요약

### 빌드 순서
1. **Developer Command Prompt에서 VSCode 실행**
2. **Ctrl+Shift+B** → "Build Debug (x64)" 선택
3. 빌드 완료 대기 (RSEngine → SeobJJangGames)
4. **F5** 키로 디버깅 시작

### 주요 파일
- **솔루션**: `rsengine_playground/SPH_MINT.sln`
- **엔진 라이브러리**: `rs_engine_library/rs_engine/lib/RSEngineD.lib`
- **실행 파일**: `rsengine_playground/OutputFiles/Debug/SeobJJangGames.exe`
- **리소스**: `rsengine_playground/OutputFiles/Debug/resources/`

### 문제 발생 시
1. Developer Command Prompt 환경 확인
2. RSEngine이 먼저 빌드되었는지 확인
3. 라이브러리 파일 경로 확인
4. 리소스 폴더 복사 여부 확인

---

**빌드에 성공하셨다면, F5를 눌러 게임을 즐겨보세요!** 🚀

**추가 문서**:
- [RSEngine README](RSEngine/README.md) - 엔진 아키텍처 및 기능 설명
- [API 문서](https://yoursite.com/docs) - Doxygen 생성 문서
