#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

class Renderer
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            return attributeDescriptions;
        }
    };

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    static std::vector<char> readFile(const std::string &filename)
    {
        std::fstream file(filename, std::ios::in | std::ios::binary);
        std::vector<char> data(std::filesystem::file_size(filename));
        file.read(data.data(), std::filesystem::file_size(filename));
        return data;
    }

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphiscFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphiscFamily.has_value() and presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct RenderModel
    {
        std::vector<Vertex> Vertices;
        std::vector<uint16_t> Indices;

        size_t VertexOffset;
        size_t IndexOffset;

        RenderModel(const std::vector<Vertex> &Vertices, const std::vector<uint16_t> &Indices)
            : Vertices(Vertices), Indices(Indices) {}

        RenderModel() {}
    };

    struct RenderTarget
    {
        uint16_t ModelID;

        glm::mat4 Transform;

        std::vector<VkDescriptorSet> UniformDescriptorSet;
        std::vector<VkDeviceMemory> UniformDeviceMemory;
        std::vector<VkBuffer> UniformBuffer;

        RenderTarget(uint16_t ModelID) : ModelID(ModelID) {}
        RenderTarget() : ModelID(0) {}
    };

    GLFWwindow *window = nullptr;
    VkInstance instance{};
    VkDebugUtilsMessengerEXT debugMessenger{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
    VkQueue graphicsQueue{};
    VkQueue presentQueue{};
    VkSwapchainKHR swapChain{};
    VkSurfaceKHR surface{};
    VkRenderPass renderPass{};
    VkDescriptorSetLayout descriptorSetLayout{};
    VkDescriptorPool descriptorPool{};
    VkPipelineLayout pipelineLayout{};
    VkPipeline graphicsPipeline{};
    VkCommandPool commandPool{};
    VkBuffer vertexBuffer{};
    VkDeviceMemory vertexBufferMemory{};
    VkBuffer indexBuffer{};
    VkDeviceMemory indexBufferMemory{};

    std::vector<VkImage> swapChainImages{};
    std::vector<VkImageView> swapChainImageViews{};
    std::vector<VkFramebuffer> swapChainFramebuffers{};
    std::vector<VkCommandBuffer> commandBuffers{};
    std::vector<VkDescriptorSet> descriptorSets{};

    std::vector<VkBuffer> uniformBuffers{};
    std::vector<VkDeviceMemory> uniformBuffersMemory{};

    std::vector<VkSemaphore> imageAvailableSemaphore{};
    std::vector<VkSemaphore> renderFinishedSemaphore{};
    std::vector<VkFence> inFlightFences{};
    std::vector<VkFence> imagesInFlight{};

    VkFormat swapChainImageFormat{};
    VkExtent2D swapChainExtent{};

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const bool COLOR_ALPHA_BLEND = false;
    const size_t MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    size_t currentFrame = 0;

    glm::mat4 ViewTransform{};
    glm::mat4 ProjectionTransform{};

    std::map<uint16_t, RenderModel> mRenderModels{};
    std::map<uint16_t, RenderTarget> mRenderTargets{};

    bool mRenderModelsDirty = false;
    bool mRenderTargetsDirty = false;

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0};

    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void RebuildBuffers();
    void RenderModelCreate(uint16_t ID, const RenderModel &Model);
    void RenderModelDelete(uint16_t ID);
    void CreateDummyModel();
    void CreateUniformBuffers(RenderTarget &Target);
    void CreateDescriptorSets(RenderTarget &Target);
    void RenderTargetCreate(uint16_t ID, uint16_t ModelID);
    void RenderTargetDelete(uint16_t ID);
    void RenderTargetUpdate(uint16_t ID, glm::mat4 Transform);
    void ViewTransformUpdate(glm::mat4 Transform);
    void ProjectionTransformUpdate(glm::mat4 Transform);
    void UpdateUniformBuffer(uint32_t currentImage);

    void initWindow();
    void createInstance();
    void createSurface();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createDescriptorPool();
    void createCommandBuffers();
    void createSyncObjects();
    void initVulkan();
    void cleanupSwapChain();
    void recreateSwapChain();
    void drawFrame();
    void mainLoop();
    void cleanup();

    bool checkValidationLayerSupport();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);

    std::vector<const char *> getRequiredExtensions();
    VkShaderModule createShaderModule(const std::vector<char> &code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

public:
    void run();
};