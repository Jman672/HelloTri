#pragma once

#include <memory>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_platform.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

const std::vector validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

std::vector<const char*> deviceExtensions = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}
private:
	GLFWwindow* window = nullptr;
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}
	void initVulkan() {
		createInstance();	
		//this should be `setupDebugMessenger();`
		pickPhysicalDevice();

	}

	void pickPhysicalDevice(){
		auto devices = instance.enumeratePhysicalDevices();
	
		const auto devIter = std::ranges::find_if(devices,
    		[&](auto const & device) {
        		auto queueFamilies = device.getQueueFamilyProperties();
        		bool isSuitable = device.getProperties().apiVersion >= VK_API_VERSION_1_3;
			const auto qfpIter = std::ranges::find_if(queueFamilies,
            		[]( vk::QueueFamilyProperties const & qfp )
                    		{
                        		return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
                    		} );
            		isSuitable = isSuitable && ( qfpIter != queueFamilies.end() );
            		auto extensions = device.enumerateDeviceExtensionProperties( );
            		bool found = true;
            		for (auto const & extension : deviceExtensions) {
                		auto extensionIter = std::ranges::find_if(extensions, [extension](auto const & ext) {return strcmp(ext.extensionName, extension) == 0;});
                		found = found &&  extensionIter != extensions.end();
            		}
            		isSuitable = isSuitable && found;
            		printf("\n");
            		if (isSuitable) {
               		 	physicalDevice = device;
            		}
            		return isSuitable;
    		});
    		if (devIter == devices.end()) {
        		throw std::runtime_error("failed to find a suitable GPU!");
    		}
	}

	//This is where the debugMessengere function will go
	
	//TODO: reread the message callback section of the validation layers page and add that stuff eventually
	//	for more verbose-ness of code and proper formatting
	void createInstance(){
		//defines the info for the app
		constexpr vk::ApplicationInfo appInfo{ 
			.pApplicationName = "Hello Triangle",
			.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
			.pEngineName	    = "No Engine",
			.engineVersion	    = VK_MAKE_VERSION( 1, 0, 0 ),
			.apiVersion	    = vk::ApiVersion14 };
	
		//This section will get the required layers provided in the define statements
		std::vector<char const*> requiredLayers;
		if (enableValidationLayers) {
			requiredLayers.assign(validationLayers.begin(), validationLayers.end());
		}
		//This section will make sure that the provided layers are supported
		auto layerProperties = context.enumerateInstanceLayerProperties();
    		if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
        		return std::ranges::none_of(layerProperties,
                	                   [requiredLayer](auto const& layerProperty)
                	                   { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
    		}))
    		{
        		throw std::runtime_error("One or more required layers are not supported!");
    		};



		uint32_t glfwExtensionCount = 0;

		auto glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );
		auto extensionProperties = context.enumerateInstanceExtensionProperties();

		//not really sure icl
		for(uint32_t i = 0; i < glfwExtensionCount; ++i){
			if(std::ranges::none_of(extensionProperties, 
						[glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
						{ return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
			{
				throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
			}
		}

		//create info for the instance, adding in the val layers and extensions
		vk::InstanceCreateInfo createInfo{ 
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
			.ppEnabledLayerNames = requiredLayers.data(),
			.enabledExtensionCount = glfwExtensionCount,
			.ppEnabledExtensionNames = glfwExtensions};

		//This section is for ensuring that the required properties are supported
		uint32_t count = 0;
		for(uint32_t i = 0; i < glfwExtensionCount; i++){
			for(const auto& extension : extensionProperties){
				if(std::string(extension.extensionName) == std::string(glfwExtensions[i])){
					count++;
					break;
				}
			}
		}
		if(count != glfwExtensionCount){
			std::cout << count << "\t" << glfwExtensionCount << "\n";
			throw std::runtime_error("Some extensions not supported");
		}
		
		//This makes the actual instance once everything is ready and confirmed
		instance = vk::raii::Instance(context, createInfo);
	}

	void mainLoop(){
		while(!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		glfwDestroyWindow(window);

		glfwTerminate();
	}
};
