#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED // Vulkan uses left handed coordinate system in world space

#define GLM_INCLUDE "../Deps/glm/glm/glm.hpp"
#define GLM_INCLUDE_QUATERNION "../Deps/glm/glm/gtx/quaternion.hpp"
#define GLM_INCLUDE_QUATERNION2 "../Deps/glm/glm/gtc/quaternion.hpp"
#define GLM_INCLUDE_MATRIX_TRANSFORM "../Deps/glm/glm/gtc/matrix_transform.hpp"
#define GLM_INCLUDE_STRING_CAST "../Deps/glm/glm/gtx/string_cast.hpp"
#define GLFW_INCLUDE "../Deps/glfw/include/GLFW/glfw3.h"
#define IMGUI_INCLUDE "../Deps/imgui/imgui.h"
#define IMGUI_INTERNAL_INCLUDE "../Deps/imgui/imgui_internal.h"
#define IMGUI_GLFW_INCLUDE "../Deps/imgui/examples/imgui_impl_glfw.h"
#define IMGUI_VULKAN_INCLUDE "../Deps/imgui/examples/imgui_impl_vulkan.h"
#define STB_INCLUDE "../Deps/stb/stb.h"
#define STB_INCLUDE_IMAGE "../Deps/stb/stb_image.h"
#define SPIRV_CROSS_INCLUDE "../Deps/SPIRV-Cross/spirv_reflect.hpp"
#define ASSIMP_IMPORTER_INCLUDE "../Deps/assimp/include/assimp/Importer.hpp"
#define ASSIMP_SCENE_INCLUDE "../Deps/assimp/include/assimp/scene.h"
#define ASSIMP_POSTPROCESS_INCLUDE "../Deps/assimp/include/assimp/postprocess.h"
#define VOLK_SOURCE "../Deps/volk/volk.c"
#define VOLK_INCLUDE "../Deps/volk/volk.h"

# define VULKAN_INCLUDE VOLK_INCLUDE


// Unusable API
// #define GLSLANG_INCLUDE "../Deps/glslang/glslang/Public/ShaderLang.h"
// #define GLSLANG_RESOURCES_INCLUDE "../Deps/glslang/StandAlone/ResourceLimits.h"
// #define GLSLANG_DIRSTACK_INCLUDE "../Deps/glslang/StandAlone/DirStackFileIncluder.h"
// #define GLSLANG_SPV_INCLDUE "../Deps/glslang/SPIRV/GlslangToSpv.h"
