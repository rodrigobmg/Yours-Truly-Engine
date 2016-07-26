//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////

#pragma once
#ifndef YTE_TextureLoader_hpp
#define YTE_TextureLoader_hpp

#include <assert.h>
#include <memory>

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"
#include "stb/stb_image.h"


#include "YTE/Core/Types.hpp"

namespace YTE
{
  template<typename FlagType>
  u32 GetMemoryType(u32 aTypeBits, vk::PhysicalDeviceMemoryProperties aDeviceProperties, FlagType aProperties)
  {
    for (uint32_t i = 0; i < 32; i++)
    {
      if ((aTypeBits & 1) == 1)
      {
        if ((aDeviceProperties.memoryTypes[i].propertyFlags & aProperties) == aProperties)
        {
          return i;
        }
      }
      aTypeBits >>= 1;
    }

    // todo : throw error
    return 0;
  }

  struct Texture
  {
    vk::Sampler sampler;
    vk::Image image;
    vk::ImageLayout imageLayout;
    vk::DeviceMemory deviceMemory;
    vk::ImageView view;
    u32 width;
    u32 height;
    u32 mipLevels;
    u32 layerCount;
    vk::DescriptorImageInfo descriptor;
  };

  class TextureLoader
  {
    public:

    template <typename Type = u32>
    struct Pixel
    {
      Type mR;
      Type mG;
      Type mB;
      Type mA;
    };

    struct STBImageHolder
    {
      struct STBDeleter
      {
        void operator ()(stbi_uc* aPixels)
        {
          stbi_image_free(aPixels);
        }
      };

      std::unique_ptr<stbi_uc, STBDeleter> mPixels;
      //std::vector<Pixel<u64>> mConvertedPixels;
      i32 mWidth;
      i32 mHeight;
      i32 mChannels;
    };

    TextureLoader(vk::PhysicalDevice aPhysicalDevice, vk::Device aDevice, vk::Queue aQueue, vk::CommandPool aCommandPool);

    ~TextureLoader();

    // Create an image memory barrier for changing the layout of
    // an image and put it into an active command buffer
    void setImageLayout(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, vk::ImageSubresourceRange subresourceRange);
    
    // Fixed sub resource on first mip level and layer
    void setImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);

    // Load a 2D texture
    Texture loadTexture(const std::string &aFilename);
    Texture loadTexture(std::vector<std::string> &aFilename);

    // Clean up vulkan resources used by a texture object
    void destroyTexture(Texture texture);

    void createImageView(vk::Image aImage, vk::Format aFormat, vk::ImageView &aImageView);
    void createTextureImageView(Texture &aTexture);
    void createTextureSampler(Texture &aTexture);

    void createImage(u32 aWidth, u32 aHeight, u32 aImageCount, vk::Format aFormat, vk::ImageTiling aTiling, vk::ImageUsageFlags aUsage, vk::MemoryPropertyFlags properties, vk::Image& aImage, vk::DeviceMemory &aImageMemory);

    void copyImage(vk::Image srcImage, vk::Image dstImage, u32 width, u32 height);
    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);
    void transitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    Texture createTextureImage(const std::string &aTextureFile);
    Texture createTextureImage(std::vector<std::string> &aTextureFile);
    Texture createTextureImage(std::vector<STBImageHolder> &aPixels);


    Texture SetupTexture(Texture &aTexture);

    vk::CommandBuffer createCommandBuffer(vk::CommandBufferLevel level, bool begin);

    private:
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue queue;
    vk::CommandBuffer cmdBuffer;
    vk::CommandPool cmdPool;
    vk::PhysicalDeviceMemoryProperties deviceMemoryProperties;
  };
};

#endif
