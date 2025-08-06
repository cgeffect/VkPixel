#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <optional>
#include <set>

#include <glm/glm.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
};