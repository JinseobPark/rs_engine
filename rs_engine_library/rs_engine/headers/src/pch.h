#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include <list>
#include <span>
#include <future> // include thread


#include <GL/glew.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../extern/imgui/imgui.h"
#include "../extern/imgui/imgui_impl_glfw.h"
#include "../extern/imgui/imgui_impl_opengl3.h"
#include "../extern/imgui/ImGuiFileDialog.h"
#include "../extern/tinyfiledialogs/tinyfiledialogs.h"

#include "Define/DLL_header.h"
#include "Util/RSLogger.h"
#include "Define/RSDefine.h"

#include "Systems/Timer/RSDebugTimer.h"
