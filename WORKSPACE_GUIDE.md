# Workspace 설정 파일을 사용한 빌드 가이드

## 🎯 Workspace 방식의 장점

이제 `particle_master.code-workspace` 파일을 사용하여:
- RSEngine과 SeobJJangGames를 별도 폴더로 관리
- Developer Command Prompt 환경 자동 로드
- 멀티 프로젝트를 하나의 workspace에서 관리

---

## 📂 Workspace 열기

### 방법 1: Workspace 파일로 열기 (권장)

1. **파일 탐색기에서**:
   - `particle_master.code-workspace` 파일을 더블클릭
   
2. **VS Code에서**:
   - `File` → `Open Workspace from File...`
   - `particle_master.code-workspace` 선택

### 방법 2: 명령줄에서

```cmd
cd C:\Users\WhatSseob\Desktop\particle_master
code particle_master.code-workspace
```

---

## 🔧 Developer Command Prompt 환경

Workspace를 열면 자동으로 "Developer Command Prompt" 프로필이 사용됩니다.

### 터미널 열기

1. **Ctrl + `** (백틱) 또는 `Terminal` → `New Terminal`
2. 자동으로 VsDevCmd.bat이 실행되어 MSBuild 사용 가능

### 수동으로 환경 확인

터미널에서 다음 명령으로 확인:

```cmd
where msbuild
where cl
```

정상적이면 Visual Studio 경로가 표시됩니다.

---

## 🛠️ 빌드하기

### 빌드 단축키

- **Ctrl + Shift + B**: 기본 빌드 태스크 실행 (Debug x64)
- **Ctrl + Shift + P** → "Tasks: Run Build Task": 태스크 선택

### 빌드 태스크 목록

| 태스크 | 설명 |
|--------|------|
| **Build Debug (x64)** | 전체 솔루션 Debug 빌드 (기본) |
| **Build Release (x64)** | 전체 솔루션 Release 빌드 |
| Build RSEngine Debug (x64) | RSEngine 라이브러리만 빌드 |
| Build SeobJJangGames Debug (x64) | 게임만 빌드 (RSEngine 자동 빌드) |
| Clean Debug (x64) | Debug 빌드 아티팩트 제거 |
| Rebuild Debug (x64) | Clean + Build |

### 터미널에서 직접 빌드

```cmd
# Debug 빌드
msbuild rsengine_playground\SPH_MINT.sln /p:Configuration=Debug /p:Platform=x64 /m

# Release 빌드
msbuild rsengine_playground\SPH_MINT.sln /p:Configuration=Release /p:Platform=x64 /m

# RSEngine만 빌드
msbuild rsengine_playground\SPH_MINT.sln /t:RSEngine /p:Configuration=Debug /p:Platform=x64 /m
```

---

## 🐛 디버깅

### 디버깅 시작

1. **F5**: 빌드 후 디버깅 시작
2. **Ctrl + F5**: 디버깅 없이 실행

### 디버그 구성

- **(MSVC) Debug - SeobJJangGames**: Debug 빌드 디버깅
- **(MSVC) Release - SeobJJangGames**: Release 빌드 실행

### 중단점 설정

1. 소스 파일 열기 (예: `rsengine_playground/main.cpp`)
2. 줄 번호 왼쪽 클릭
3. F5로 디버깅 시작

---

## 📁 Workspace 폴더 구조

Workspace에는 3개의 폴더가 등록되어 있습니다:

```
particle_master (workspace root)
├── 📁 RSEngine          - 엔진 라이브러리 소스
├── 📁 SeobJJangGames    - 게임 애플리케이션 소스
└── 📁 Root              - 전체 프로젝트 루트
```

VS Code 좌측 Explorer에서 각 폴더를 독립적으로 탐색할 수 있습니다.

---

## 🔍 IntelliSense 설정

`.vscode/c_cpp_properties.json`이 자동으로 적용되어:
- 헤더 파일 자동 완성
- 코드 네비게이션 (Go to Definition)
- 오류 표시

설정된 Include Path:
- `rs_engine_library/extern/` (GLFW, GLEW, GLM 등)
- `rs_engine_library/rs_engine/headers/` (빌드된 RSEngine 헤더)
- `RSEngine/src/` (엔진 소스)
- `rsengine_playground/src/` (게임 소스)

---

## ⚠️ 문제 해결

### 1. MSBuild를 찾을 수 없음

**증상**: `'msbuild'은(는) 내부 또는 외부 명령이 아닙니다`

**해결**:
1. VS Code를 **완전히 종료**
2. `particle_master.code-workspace` 파일을 다시 열기
3. 새 터미널 열기 (Ctrl + `)
4. `where msbuild` 명령으로 확인

### 2. Workspace가 제대로 열리지 않음

**해결**:
- `File` → `Close Workspace`
- `File` → `Open Workspace from File...`
- `particle_master.code-workspace` 선택

### 3. 터미널이 PowerShell로 열림

**해결**:
1. 터미널 우측 상단 `+` 옆 드롭다운 클릭
2. "Developer Command Prompt" 선택
3. 또는 터미널 종료 후 새로 열기

### 4. 빌드 경로 오류

모든 빌드 태스크는 `${workspaceFolder:Root}`를 기준으로 실행됩니다.
- Root = `C:\Users\WhatSseob\Desktop\particle_master`

---

## 🚀 빠른 시작 체크리스트

1. ✅ `particle_master.code-workspace` 파일로 VS Code 열기
2. ✅ 터미널 열기 (Ctrl + `) - Developer Command Prompt 자동 실행
3. ✅ `where msbuild` 명령으로 환경 확인
4. ✅ **Ctrl + Shift + B** 눌러 빌드
5. ✅ **F5** 눌러 디버깅 시작

---

## 📝 추가 설정 (선택사항)

### 기본 터미널 프로필 변경

`.vscode/settings.json` 또는 User Settings에서:

```json
{
    "terminal.integrated.defaultProfile.windows": "Developer Command Prompt"
}
```

### 빌드 시 자동 저장

```json
{
    "files.autoSave": "onFocusChange"
}
```

### 빌드 출력 자동 표시

```json
{
    "task.autoRunTask": "build"
}
```

---

## 🎮 빌드 완료 후

빌드가 성공하면:

```
rsengine_playground/OutputFiles/Debug/
├── SeobJJangGames.exe    <- 실행 파일
├── RSEngineD.pdb          <- 디버그 심볼
└── resources/             <- 복사된 리소스
    ├── GLSL/
    ├── Font/
    └── TestObject/
```

**F5**를 눌러 게임을 실행하세요! 🎉

---

**이전 방식과의 차이점**:
- ❌ 매번 Developer Command Prompt에서 VS Code 실행할 필요 없음
- ✅ Workspace 파일만 열면 자동으로 환경 설정
- ✅ 멀티 폴더 구조로 프로젝트 관리 편리
- ✅ 모든 경로가 workspace 기준으로 일관되게 관리
