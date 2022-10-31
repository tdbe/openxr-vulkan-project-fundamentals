#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Context;
struct GLFWwindow;
class Headset;
class Renderer;

class MirrorView final
{
public:
  MirrorView(const Context* context);
  ~MirrorView();

  void onWindowResize();

  bool connect(const Headset* headset, const Renderer* renderer);
  void processWindowEvents() const;
  bool render(uint32_t swapchainImageIndex);
  void present();

  bool isValid() const;
  bool windowShouldClose() const;
  VkSurfaceKHR getSurface() const;

private:
  bool valid = true;

  const Context* context = nullptr;
  const Headset* headset = nullptr;
  const Renderer* renderer = nullptr;

  GLFWwindow* window = nullptr;

  VkSurfaceKHR surface = nullptr;
  VkSwapchainKHR swapchain = nullptr;
  std::vector<VkImage> swapchainImages;
  VkExtent2D renderSize = { 0u, 0u };

  uint32_t destinationImageIndex = 0u;
  bool resizeDetected = false;

  bool recreateSwapchain();
};