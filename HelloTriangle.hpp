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

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}
	void initVulkan() {
		createInstance();	
	}
	
	void createInstance(){
		//defines the info for the app
		constexpr vk::ApplicationInfo appInfo{ 
			.pApplicationName = "Hello Triangle",
			.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
			.pEngineName	    = "No Engine",
			.engineVersion	    = VK_MAKE_VERSION( 1, 0, 0 ),
			.apiVersion	    = vk::ApiVersion14 };
		
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

		//create info for the instance
		vk::InstanceCreateInfo createInfo{ 
			.pApplicationInfo = &appInfo,
			.enabledExtensionCount = glfwExtensionCount,
			.ppEnabledExtensionNames = glfwExtensions };

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
