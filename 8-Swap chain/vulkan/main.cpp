
/*
 Vulkan 没有“默认帧缓冲区”的概念，因此它需要一个基础架构来拥有我们将渲染到的缓冲区，然后再将它们可视化到屏幕上。该基础设施称为交换链，必须在 Vulkan 中显式创建。交换链本质上是一个等待呈现到屏幕的图像队列。我们的应用程序将获取这样的图像以绘制到它，然后将其返回到队列中。队列的具体工作方式以及从队列中呈现图像的条件取决于交换链的设置方式，但交换链的一般目的是使图像的呈现与屏幕的刷新率同步。
 */

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        //启用设备扩展
        auto extensions = getRequiredExtensions();
        //使用交换链需要VK_KHR_swapchain先启用扩展。启用扩展只需要对逻辑设备创建结构进行小的更改：
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

    //创建交换链
    void createSwapChain() {
        //查询交换链支持详情
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        //为交换链选择正确的设置
        //找到最佳交换链的正确设置。需要确定三种类型的设置：

        //交换范围（交换链中图像的分辨率）
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        //演示模式（将图像“交换”到屏幕的条件）
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        //表面格式（颜色深度）
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        //我们还必须决定我们希望在交换链中拥有多少张图像。该实现指定了它运行所需的最小数量：
        //简单地坚持这个最小值意味着我们有时可能必须等待驱动程序完成内部操作，然后才能获取另一个要渲染的图像。因此，建议至少请求一个比最小值多的图像：
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        //我们还应该确保在执行此操作时不超过最大图像数量，其中0是一个特殊值，表示没有最大值：
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        //交换链可以理解为fbo
        //创建交换链对象需要填充一个大型结构
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        //指定surface, fbo必须绑定一个surface
        createInfo.surface = surface;
        
        //在指定交换链应绑定到哪个表面后，指定交换链图像的详细信息：
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1; //imageArrayLayers指定每个图像包含的层数。1除非您正在开发立体 3D 应用程序，否则总是如此
        //imageUsage使用交换链中的图像进行何种操作。在本教程中，我们将直接渲染给它们，这意味着它们被用作颜色附件
        //您也可以先将图像渲染为单独的图像以执行后处理等操作。在这种情况下，您可以使用类似的值 VK_IMAGE_USAGE_TRANSFER_DST_BIT，并使用内存操作将渲染图像传输到交换链图像。
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        
        /*
         接下来，我们需要指定如何处理将跨多个队列族使用的交换链图像。如果图形队列系列与表示队列不同，我们的应用程序就会出现这种情况。我们将从图形队列中的交换链中绘制图像，然后将它们提交到演示队列中。有两种方法可以处理从多个队列访问的图像：

         VK_SHARING_MODE_EXCLUSIVE：图像一次由一个队列族拥有，并且在将其用于另一个队列族之前，必须明确转移所有权。此选项提供最佳性能。
         VK_SHARING_MODE_CONCURRENT：图像可以在多个队列族中使用，无需显式所有权转移。
         
         如果队列族不同，那么我们将在本教程中使用并发模式以避免必须编写所有权章节，因为这些涉及一些稍后会更好解释的概念。并发模式要求您预先指定将使用queueFamilyIndexCount和pQueueFamilyIndices 参数在哪些队列家族之间共享所有权。如果图形队列族和显示队列族相同，大多数硬件都会出现这种情况，那么我们应该坚持独占模式，因为并发模式要求您至少指定两个不同的队列族。
         */
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        //我们可以指定应该对交换链中的图像应用某种变换capabilities，例如 90 度顺时针旋转或水平翻转。要指定您不需要任何转换，只需指定当前转换。
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        //compositeAlpha字段指定是否应使用 Alpha 通道与窗口系统中的其他窗口混合。您几乎总是想简单地忽略 alpha 通道，因此VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR.
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        //clipped成员设置为 VK_TRUE 这意味着我们不关心被遮挡的像素的颜色，例如因为另一个窗口在它们前面。
        //除非您确实需要能够读回这些像素并获得可预测的结果，否则您将通过启用剪辑获得最佳性能。
        createInfo.clipped = VK_TRUE;

        //您的交换链可能会在您的应用程序运行时变得无效或未优化，例如因为调整了窗口大小。在这种情况下，实际上需要从头开始重新创建交换链，并且必须在此字段中指定对旧链的引用。这是一个复杂的话题，我们将在以后的章节中详细了解。现在我们假设我们只会创建一个交换链。
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        //创建交换链
        //参数是逻辑设备、交换链创建信息、可选的自定义分配器和指向存储句柄的变量的指针。这并不奇怪。它应该vkDestroySwapchainKHR在设备之前使用：
        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        
        //检索交换链图像
        //现在已经创建了交换链，所以剩下的就是检索其中VkImage的句柄
        //图像是由交换链的实现创建的，一旦交换链被销毁，它们将被自动清理，因此我们不需要添加任何清理代码。
        //我们只在交换链中指定了最小数量的图像，因此允许实现创建具有更多图像的交换链。这就是为什么我们首先使用 查询图像的最终数量vkGetSwapchainImagesKHR，然后调整容器的大小，最后再次调用它来检索句柄。
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        //为交换链图像选择的格式和范围存储在成员变量中
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            //VK_FORMAT_B8G8R8A8_SRGB意味着我们以 8 位无符号整数的顺序存储 B、G、R 和 alpha 通道，每个像素总共 32 位
            //colorSpace成员指示是否支持 SRGB颜色空间或不使用VK_COLOR_SPACE_SRGB_NONLINEAR_KHR标志。
            //请注意，该标志曾经VK_COLORSPACE_SRGB_NONLINEAR_KHR在旧版本的规范中被调用。
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
            //对于颜色空间，如果可用，我们将使用 SRGB，因为它可以产生更准确的感知颜色。它也是图像的标准色彩空间，就像我们稍后将使用的纹理一样。因此，我们还应该使用 SRGB 颜色格式，其中最常见的一种是VK_FORMAT_B8G8R8A8_SRGB.
        }

        //如果这也失败了，那么我们可以根据它们的“好”程度开始对可用格式进行排名，但在大多数情况下，只需使用指定的第一种格式就可以了。
        return availableFormats[0];
    }

    /*
     演示模式可以说是交换链最重要的设置，因为它代表了将图像显示到屏幕的实际条件。Vulkan 中有四种可能的模式：

     VK_PRESENT_MODE_IMMEDIATE_KHR：您的应用程序提交的图像会立即转移到屏幕上，这可能会导致撕裂。
     VK_PRESENT_MODE_FIFO_KHR：交换链是一个队列，当显示器刷新时，显示器从队列的前面获取图像，程序将渲染的图像插入到队列的后面。如果队列已满，则程序必须等待。这与现代游戏中的垂直同步最为相似。刷新显示的那一刻称为“垂直空白”。
     VK_PRESENT_MODE_FIFO_RELAXED_KHR：此模式仅在应用程序迟到并且队列在最后一个垂直空白处为空的情况下与前一种模式不同。图像最终到达时立即传输，而不是等待下一个垂直空白。这可能会导致明显的撕裂。
     VK_PRESENT_MODE_MAILBOX_KHR: 这是第二种模式的另一种变体。队列已满时不会阻塞应用程序，而是将已排队的图像简单地替换为较新的图像。此模式可用于尽可能快地渲染帧，同时仍避免撕裂，与标准垂直同步相比，延迟问题更少。这就是俗称的“三重缓冲”，虽然单独存在三个缓冲并不一定意味着帧率是解锁的。
     只有VK_PRESENT_MODE_FIFO_KHR模式保证可用，所以我们必须再次编写一个函数来寻找可用的最佳模式：
     */
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    //交换范围, 就是分辨率, 可用的分辨率在VkSurfaceCapabilitiesKHR中定义
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            //告诉vulkan使用, 通过在成员中设置宽度和高度来匹配窗口的分辨率
            return capabilities.currentExtent;
        } else {
            //GLFW 在测量尺寸时使用两个单位：像素和 屏幕坐标。例如，{WIDTH, HEIGHT}我们之前在创建窗口时指定的分辨率是在屏幕坐标中测量的。但是 Vulkan 使用像素，因此交换链范围也必须以像素为单位指定。不幸的是，如果您使用的是高 DPI 显示器（如 Apple 的 Retina 显示器），屏幕坐标与像素不对应。相反，由于更高的像素密度，以像素为单位的窗口分辨率将大于以屏幕坐标为单位的分辨率。因此，如果 Vulkan 没有为我们修复交换范围，我们就不能只使用原始的{WIDTH, HEIGHT}. 相反，我们必须 glfwGetFramebufferSize先查询窗口的分辨率（以像素为单位），然后再将其与最小和最大图像范围进行匹配。
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            //clamp函数在这里用于限制实现所支持的允许的最小和最大范围的width值height。
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    //填充SwapChainSupportDetails
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        //基本上我们需要检查三种属性：
        //基本表面功能（交换链中图像的最小/最大数量，图像的最小/最大宽度和高度）
        //表面格式（像素格式、色彩空间）
        //可用的演示模式

        //基本表面功能（交换链中图像的最小/最大数量，图像的最小/最大宽度和高度）
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        //表面格式（像素格式、色彩空间）
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        //可用的演示模式
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        //数据存储在这里
        return details;
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        //验证交换链支持是否足够。
        //如果给定我们拥有的窗口表面，至少有一种支持的图像格式和一种支持的演示模式，则交换链支持对于本教程来说就足够了。
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    //检查交换链支持
    //由于各种原因，并非所有显卡都能够将图像直接呈现到屏幕上，例如因为它们是为服务器设计的并且没有任何显示输出。其次，由于图像显示与窗口系统和与窗口相关的表面密切相关，因此它实际上并不是 Vulkan 核心的一部分。您必须VK_KHR_swapchain 在查询其支持后启用设备扩展。
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        //1. 获取扩展个数
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        //2. 找到可用的扩展
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
