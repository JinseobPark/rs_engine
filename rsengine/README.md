# RSEngine

![logo](doxy_scripts/logo.jpg)

**A Rock & Stone Engine**

RSEngine (Realtime-Simulation Engine) is a custom game engine built with C++20, featuring GPU-accelerated SPH (Smoothed Particle Hydrodynamics) fluid simulation and a modular component-based architecture.

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Architecture](#architecture)
- [Core Systems](#core-systems)
- [SPH Particle Simulation](#sph-particle-simulation)
- [Build Requirements](#build-requirements)
- [Quick Start](#quick-start)
- [Documentation](#documentation)
- [Project Structure](#project-structure)
- [License](#license)

---

## Overview

RSEngine is designed for real-time physics simulation and visualization, with a primary focus on fluid dynamics using SPH methods. The engine provides:

- **Modular System Architecture**: Component-based design with easy extensibility
- **GPU-Accelerated Simulation**: OpenGL Compute Shader-based particle physics
- **Real-time Rendering**: PBR (Physically Based Rendering) material system
- **Multiple Solver Support**: Static/Dynamic, Brute-force/Grid-based approaches
- **Data Visualization**: VTK file loading, point clipping, property-based coloring

**Version**: 0.0.6  
**Author**: Jinseob Park  
**Language**: C++20  
**Graphics API**: OpenGL 4.3+

---

## Key Features

### Fluid Simulation
- **SPH Solvers**: 
  - Static Brute Force (accurate, small-scale)
  - Static Grid (optimized for large-scale)
  - Dynamic Brute Force (inflow/outflow support)
- **Real-time Parameters**: Adjustable density, viscosity, pressure, gravity
- **Collision Detection**: Mesh-based collision with custom properties
- **Visualization Modes**: Color-coded by velocity, density, pressure, acceleration

### Rendering System
- **PBR Materials**: Metallic/Roughness workflow
- **Deferred/Forward Rendering**: Dual pipeline support
- **Particle Rendering**: GPU instancing for millions of particles
- **Legend System**: Dynamic color mapping with min/max value calculation
- **Clipping Tools**: Plane/Cube/Sphere-based particle filtering

### Engine Architecture
- **System Components**: Application, Input, Physics, Graphics, Timer, Event Handler
- **Resource Management**: Centralized singleton managers for all assets
- **Fixed/Variable FPS**: Configurable update loop
- **Event System**: Decoupled communication between systems

---

## Architecture

### System Initialization Flow

```
RSEngine::Initialize()
  |-> AddSystem(Application)
  |-> AddSystem(Timer)
  |-> AddSystem(Input)
  |-> AddSystem(Physics)
  |-> AddSystem(EventHandler)
  |-> AddSystem(Graphics)
  \-> AddSystem(State)
    |
    Initialize() each system
    |
    RSResourceManager::Load()
    RSResourceManager::Initialize()
    |
PostInitialize() each system
```

### Update Loop

```cpp
while (is_running)
{
  // Fixed or variable delta time
    float dt = IsFixedFps() ? fixed_delta_time : actual_delta_time;
    
    if (IsUpdateable())
 {
   RSResourceManager::Update(dt);
        
        for (each system)
    system->Update(dt);
   
      EventHandler->UpdatePost(dt);
        Graphics->UpdatePost(dt);
    }
}
```

---

## Core Systems

### 1. RSSystemComponent (Base Class)

All engine systems derive from this base:

```cpp
class RSSystemComponent
{
public:
    virtual void Initialize(HWND h_window = nullptr) = 0;
    virtual void PostInitialize() {}
    virtual void Update(float dt) = 0;
    virtual void Shutdown() = 0;
};
```

### 2. RSResourceManager (Singleton)

Centralized management of all resources:

| Manager | Responsibility |
|---------|---------------|
| `ShaderManager` | GLSL shader compilation & linking |
| `TextureManager` | Texture loading & caching |
| `MaterialManager` | PBR material properties |
| `MeshManager` | Geometry data management |
| `ObjectManager` | Scene object hierarchy |
| `ParticleManager` | SPH simulator lifecycle |
| `WidgetManager` | UI/ImGui widgets |
| `Camera` | View/Projection matrices |

### 3. RSEventHandler

Event-driven architecture for loose coupling:

```cpp
// Add events from anywhere
mRSENGINE->AddEvent(EventType::CAMERA_RESET);
mRSENGINE->AddEvent(EventType::OBJECT_MOVE, "PlayerName", position_data);
```

---

## SPH Particle Simulation

### Solver Types

| Solver | Description | Use Case |
|--------|-------------|----------|
| `R_STATIC_BRUTE_FORCE` | All-pair comparison | Small-scale, high accuracy |
| `R_STATIC_GRID` | Spatial hashing | Large-scale static scenes |
| `R_DYNAMIC_BRUTE_FORCE` | With inflow/outflow | Continuous fluid streams |

### Simulation Properties

```cpp
struct RSParticleProperty {
    float particle_radius;        // Particle size
    float density;       // Rest density (default: 1000.0)
    float viscosity;     // Fluid viscosity (default: 0.1)
    float gravity;                // Gravitational acceleration (default: 9.8)
    float mass;    // Particle mass
    float gas_constant;           // Pressure stiffness (default: 2000.0)
    float smoothing_length;       // SPH kernel radius
};
```

### Visualization Options

```cpp
enum class RSRenderPropertyType {
    R_COLOR,        // Solid color
    R_VELOCITY,     // Color by velocity magnitude
    R_ACCELERATION, // Color by acceleration magnitude
 R_DENSITY,      // Color by fluid density
    R_PRESSURE      // Color by pressure
};
```

### Dynamic Flow System

**Inflow Zone**: Generates particles at specified rate and velocity
- Maximum 50,000 particles
- Configurable speed & direction
- Grid-based spawning

**Outflow Zone**: Removes particles from simulation
- Zone-based detection
- Automatic particle deactivation

### Usage Example

```cpp
// Create and configure simulator
RS_Particle::RSParticleSimulator simulator;

RSInitSettingParticleSystem settings;
settings.init_particle_setting_min = glm::vec3(-10, 0, -10);
settings.init_particle_setting_max = glm::vec3(10, 5, 10);
settings.init_particle_setting_space = 0.15f;
settings.boundary_min = glm::vec3(-10, -30, -10);
settings.boundary_max = glm::vec3(10, 30, 10);

simulator.SetInitSettingParticleSystem(settings);
simulator.SetSolverType(RSSolverType::R_STATIC_GRID);
simulator.Initialize();

// Update loop
simulator.Update(deltaTime);
simulator.ForwardDraw();
```

---

## Build Requirements

### Development Environment
- **IDE**: Visual Studio 2022
- **C++ Standard**: C++20
- **Platform**: Windows 10/11

### Dependencies
- **GLEW**: OpenGL Extension Wrangler Library
- **GLFW**: Window and input management
- **GLM**: Mathematics library for graphics
- **ImGui**: Immediate mode GUI

### GPU Requirements
- OpenGL 4.3+ support
- Compute Shader capability
- Minimum 2GB VRAM for large-scale simulations

### GPU Optimization

The engine automatically selects high-performance GPU:

```cpp
// Force NVIDIA discrete GPU
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

// Force AMD discrete GPU
extern "C" {
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
```

---

## Quick Start

### Basic Engine Initialization

```cpp
#include "RSEngine.h"

int main()
{
    RS_Engine::RSEngine engine;
    
    // Initialize engine systems
    engine.Initialize();
    
    // Configure FPS
    engine.SetFixedFps(true);
    engine.SetFps(100.0f);

    // Run main loop
    engine.Run();
    
    // Cleanup
    engine.Shutdown();
    
    return 0;
}
```

### Creating Game Objects

```cpp
auto objectManager = RSResourceManager::GetInstance()->GetObjectManager();

// Create object with components
auto player = objectManager->CreateObject("Player");
player->AddComponent(new RSModel());
player->AddComponent(new RSRigidbody());
player->GetTransform()->SetPosition(glm::vec3(0, 1, 0));
player->GetTransform()->SetScale(glm::vec3(1, 1, 1));
```

### Loading Materials (PBR)

```cpp
auto materialManager = RSResourceManager::GetInstance()->GetMaterialManager();

materialManager->CreateMaterial(
    "metal_surface",
    texture_map,      // Albedo texture
    normal_map,       // Normal map
    0.8f,          // Metallic
    0.3f,             // Roughness
    0.5f  // Ambient occlusion
);
```

---

## Documentation

### Doxygen Document Generation

#### Prerequisites (WSL/Linux)

1. **Install WSL** (Windows Subsystem for Linux):
   ```bash
   wsl --install -d debian
   ```

2. **Enter WSL** and configure username & password

3. **Update system**:
   ```bash
   sudo apt update && sudo apt upgrade -y
   ```

4. **Install dependencies**:
   ```bash
   sudo apt install doxygen graphviz python3 texlive-full make
   ```

#### Generate Documentation

5. **Navigate to script directory**:
   ```bash
   cd doxy_scripts
   chmod +x ./*
   ```

6. **Run generation script**:
   ```bash
   ./gen-doc.sh
   ```

7. **View results**:
   - HTML: `doxy_output/html/index.html`
   - PDF: `refman.pdf`

#### Documentation Features
- **Doxygen Awesome CSS**: Modern, clean UI
- **Dark mode support**
- **Interactive table of contents**
- **Korean language support** (`kotex` package)
- **Code copy buttons**

---

## Technical Details

### Shader Pipeline

GLSL shaders are automatically converted to C++ headers:

```python
# merge_shaders.py
# Scans .vert, .frag, .comp files
# �� Generates RSMergedShaders.h/cpp

const char* shader_name_vs = R"(
 #version 430 core
    // GLSL code...
)";
```

### Compute Shader Example (SPH Density)

```glsl
#version 430 core
layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer PositionBuffer {
    vec4 positions[];  // xyz: position, w: density
};

uniform uint particleCount;
uniform float smoothingLength;

void main() {
uint id = gl_GlobalInvocationID.x;
    if (id >= particleCount) return;
    
 vec3 pos = positions[id].xyz;
    float density = 0.0;
    
    // Neighbor search
    for (uint i = 0; i < particleCount; i++) {
   float dist = length(pos - positions[i].xyz);
        if (dist < smoothingLength) {
            density += mass * poly6_kernel(dist);
        }
    }
    
positions[id].w = density;
}
```

---

## Configuration

### FPS Control

```cpp
// Fixed timestep mode
engine.SetFixedFps(true);
engine.SetFps(100.0f);           // 100 FPS
// Delta time = 0.01 seconds

// Variable timestep mode
engine.SetFixedFps(false);
// Uses actual frame time
```

### Window Resize Handling

```cpp
// Automatically handled by Application system
void RSResourceManager::Resize(int width, int height)
{
    GetCamera()->UpdateAspect(width, height);
    GetBufferManager()->ResizeBuffers(width, height);
    GetWidgetManager()->ResizeWidgets();
    GetParticleManager()->GetParticleSimulator()->UpdateRenderProperty();
}
```

---

## Related Projects

- **SeobJJangGames**: Game layer built on RSEngine

---

## License

Project-specific license. See individual library licenses for third-party dependencies.

---

## Acknowledgments

- **Doxygen Awesome CSS**: MIT License
- **GLFW, GLEW, GLM, ImGui**: Respective licenses

---

<div align="center">

**Rock & Stone!**

*Realtime-Simulation Engine for Physics-Based Visualization*

[Documentation](#documentation)   [Quick Start](#quick-start)   [Architecture](#architecture)

</div>
