#pragma once

#include <cstddef>
#include <memory>
#if defined(__linux__)
	#include <glm/glm.hpp>
#else
	#include <glm.hpp>
#endif
#include <QtWidgets>

class NRIBuffer;
class NRIAllocation;
class NRIImage2D;
class NRICommandPool;
class NRICommandQueue;
class NRICommandBuffer;
class NRIQWindow;

/// NRI - Native Rendering Interface
class NRI {
   public:
	virtual ~NRI() {}

	enum BufferUsage {
		BUFFER_USAGE_VERTEX		  = 1 << 0,
		BUFFER_USAGE_INDEX		  = 1 << 1,
		BUFFER_USAGE_UNIFORM	  = 1 << 2,
		BUFFER_USAGE_STORAGE	  = 1 << 3,
		BUFFER_USAGE_TRANSFER_SRC = 1 << 4,
		BUFFER_USAGE_TRANSFER_DST = 1 << 5
	};

	enum Format {
		FORMAT_UNDEFINED			= 0,
		FORMAT_R4G4_UNORM			= 1,
		FORMAT_R4G4B4A4_UNORM		= 2,
		FORMAT_B4G4R4A4_UNORM		= 3,
		FORMAT_R5G6B5_UNORM			= 4,
		FORMAT_B5G6R5_UNORM			= 5,
		FORMAT_R5G5B5A1_UNORM		= 6,
		FORMAT_B5G5R5A1_UNORM		= 7,
		FORMAT_A1R5G5B5_UNORM		= 8,
		FORMAT_R8_UNORM				= 9,
		FORMAT_R8_SNORM				= 10,
		FORMAT_R8_USCALED			= 11,
		FORMAT_R8_SSCALED			= 12,
		FORMAT_R8_UINT				= 13,
		FORMAT_R8_SINT				= 14,
		FORMAT_R8_SRGB				= 15,
		FORMAT_R8G8_UNORM			= 16,
		FORMAT_R8G8_SNORM			= 17,
		FORMAT_R8G8_USCALED			= 18,
		FORMAT_R8G8_SSCALED			= 19,
		FORMAT_R8G8_UINT			= 20,
		FORMAT_R8G8_SINT			= 21,
		FORMAT_R8G8_SRGB			= 22,
		FORMAT_R8G8B8_UNORM			= 23,
		FORMAT_R8G8B8_SNORM			= 24,
		FORMAT_R8G8B8_USCALED		= 25,
		FORMAT_R8G8B8_SSCALED		= 26,
		FORMAT_R8G8B8_UINT			= 27,
		FORMAT_R8G8B8_SINT			= 28,
		FORMAT_R8G8B8_SRGB			= 29,
		FORMAT_B8G8R8_UNORM			= 30,
		FORMAT_B8G8R8_SNORM			= 31,
		FORMAT_B8G8R8_USCALED		= 32,
		FORMAT_B8G8R8_SSCALED		= 33,
		FORMAT_B8G8R8_UINT			= 34,
		FORMAT_B8G8R8_SINT			= 35,
		FORMAT_B8G8R8_SRGB			= 36,
		FORMAT_R8G8B8A8_UNORM		= 37,
		FORMAT_R8G8B8A8_SNORM		= 38,
		FORMAT_R8G8B8A8_USCALED		= 39,
		FORMAT_R8G8B8A8_SSCALED		= 40,
		FORMAT_R8G8B8A8_UINT		= 41,
		FORMAT_R8G8B8A8_SINT		= 42,
		FORMAT_R8G8B8A8_SRGB		= 43,
		FORMAT_B8G8R8A8_UNORM		= 44,
		FORMAT_B8G8R8A8_SNORM		= 45,
		FORMAT_B8G8R8A8_USCALED		= 46,
		FORMAT_B8G8R8A8_SSCALED		= 47,
		FORMAT_B8G8R8A8_UINT		= 48,
		FORMAT_B8G8R8A8_SINT		= 49,
		FORMAT_B8G8R8A8_SRGB		= 50,
		FORMAT_A8B8G8R8_UNORM		= 51,
		FORMAT_A8B8G8R8_SNORM		= 52,
		FORMAT_A8B8G8R8_USCALED		= 53,
		FORMAT_A8B8G8R8_SSCALED		= 54,
		FORMAT_A8B8G8R8_UINT		= 55,
		FORMAT_A8B8G8R8_SINT		= 56,
		FORMAT_A8B8G8R8_SRGB		= 57,
		FORMAT_A2R10G10B10_UNORM	= 58,
		FORMAT_A2R10G10B10_SNORM	= 59,
		FORMAT_A2R10G10B10_USCALED	= 60,
		FORMAT_A2R10G10B10_SSCALED	= 61,
		FORMAT_A2R10G10B10_UINT		= 62,
		FORMAT_A2R10G10B10_SINT		= 63,
		FORMAT_A2B10G10R10_UNORM	= 64,
		FORMAT_A2B10G10R10_SNORM	= 65,
		FORMAT_A2B10G10R10_USCALED	= 66,
		FORMAT_A2B10G10R10_SSCALED	= 67,
		FORMAT_A2B10G10R10_UINT		= 68,
		FORMAT_A2B10G10R10_SINT		= 69,
		FORMAT_R16_UNORM			= 70,
		FORMAT_R16_SNORM			= 71,
		FORMAT_R16_USCALED			= 72,
		FORMAT_R16_SSCALED			= 73,
		FORMAT_R16_UINT				= 74,
		FORMAT_R16_SINT				= 75,
		FORMAT_R16_SFLOAT			= 76,
		FORMAT_R16G16_UNORM			= 77,
		FORMAT_R16G16_SNORM			= 78,
		FORMAT_R16G16_USCALED		= 79,
		FORMAT_R16G16_SSCALED		= 80,
		FORMAT_R16G16_UINT			= 81,
		FORMAT_R16G16_SINT			= 82,
		FORMAT_R16G16_SFLOAT		= 83,
		FORMAT_R16G16B16_UNORM		= 84,
		FORMAT_R16G16B16_SNORM		= 85,
		FORMAT_R16G16B16_USCALED	= 86,
		FORMAT_R16G16B16_SSCALED	= 87,
		FORMAT_R16G16B16_UINT		= 88,
		FORMAT_R16G16B16_SINT		= 89,
		FORMAT_R16G16B16_SFLOAT		= 90,
		FORMAT_R16G16B16A16_UNORM	= 91,
		FORMAT_R16G16B16A16_SNORM	= 92,
		FORMAT_R16G16B16A16_USCALED = 93,
		FORMAT_R16G16B16A16_SSCALED = 94,
		FORMAT_R16G16B16A16_UINT	= 95,
		FORMAT_R16G16B16A16_SINT	= 96,
		FORMAT_R16G16B16A16_SFLOAT	= 97,
		FORMAT_R32_UINT				= 98,
		FORMAT_R32_SINT				= 99,
		FORMAT_R32_SFLOAT			= 100,
		FORMAT_R32G32_UINT			= 101,
		FORMAT_R32G32_SINT			= 102,
		FORMAT_R32G32_SFLOAT		= 103,
		FORMAT_R32G32B32_UINT		= 104,
		FORMAT_R32G32B32_SINT		= 105,
		FORMAT_R32G32B32_SFLOAT		= 106,
		FORMAT_R32G32B32A32_UINT	= 107,
		FORMAT_R32G32B32A32_SINT	= 108,
		FORMAT_R32G32B32A32_SFLOAT	= 109,
		FORMAT_R64_UINT				= 110,
		FORMAT_R64_SINT				= 111,
		FORMAT_R64_SFLOAT			= 112,
		FORMAT_R64G64_UINT			= 113,
		FORMAT_R64G64_SINT			= 114,
		FORMAT_R64G64_SFLOAT		= 115,
		FORMAT_R64G64B64_UINT		= 116,
		FORMAT_R64G64B64_SINT		= 117,
		FORMAT_R64G64B64_SFLOAT		= 118,
		FORMAT_R64G64B64A64_UINT	= 119,
		FORMAT_R64G64B64A64_SINT	= 120,
		FORMAT_R64G64B64A64_SFLOAT	= 121,
		FORMAT_B10G11R11_UFLOAT		= 122,
		FORMAT_E5B9G9R9_UFLOAT		= 123,
		FORMAT_D16_UNORM			= 124,
		FORMAT_X8_D24_UNORM			= 125,
		FORMAT_D32_SFLOAT			= 126,
		FORMAT_S8_UINT				= 127,
		FORMAT_D16_UNORM_S8_UINT	= 128,
		FORMAT_D24_UNORM_S8_UINT	= 129,
		FORMAT_D32_SFLOAT_S8_UINT	= 130,
		_FORMAT_NUM					= 131
	};

	enum MemoryTypeRequest { MEMORY_TYPE_UPLOAD = 1 << 0, MEMORY_TYPE_READBACK = 1 << 1, MEMORY_TYPE_DEVICE = 1 << 2 };

	enum ImageUsage {
		IMAGE_USAGE_TRANSFER_SRC			 = 1 << 0,
		IMAGE_USAGE_TRANSFER_DST			 = 1 << 1,
		IMAGE_USAGE_SAMPLED					 = 1 << 2,
		IMAGE_USAGE_STORAGE					 = 1 << 3,
		IMAGE_USAGE_COLOR_ATTACHMENT		 = 1 << 4,
		IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 1 << 5,
		IMAGE_USAGE_TRANSIENT_ATTACHMENT	 = 1 << 6,
		IMAGE_USAGE_INPUT_ATTACHMENT		 = 1 << 7
	};

	struct MemoryRequirements {
		std::size_t		  size;
		std::size_t		  alignment = 0;
		MemoryTypeRequest typeRequest;

		MemoryRequirements(std::size_t s, MemoryTypeRequest tr, std::size_t a = 0)
			: size(s), alignment(a), typeRequest(tr) {}

		MemoryRequirements &setTypeRequest(MemoryTypeRequest tr);
	};

	virtual std::unique_ptr<NRIBuffer>		  createBuffer(std::size_t size, BufferUsage usage) const	   = 0;
	virtual std::unique_ptr<NRIImage2D>		  createImage2D(uint32_t width, uint32_t height, Format fmt,
															ImageUsage usage) const						   = 0;
	virtual std::unique_ptr<NRIAllocation>	  allocateMemory(MemoryRequirements memoryRequirements) const  = 0;
	virtual std::unique_ptr<NRICommandQueue>  createCommandQueue() const								   = 0;
	virtual std::unique_ptr<NRICommandBuffer> createCommandBuffer(const NRICommandPool &commandPool) const = 0;
	virtual std::unique_ptr<NRICommandPool>	  createCommandPool() const									   = 0;

	virtual NRIQWindow *createQWidgetSurface(QApplication &app) const = 0;
};

inline constexpr NRI::BufferUsage operator|(NRI::BufferUsage a, NRI::BufferUsage b) {
	return static_cast<NRI::BufferUsage>(static_cast<int>(a) | static_cast<int>(b));
}

inline constexpr NRI::ImageUsage operator|(NRI::ImageUsage a, NRI::ImageUsage b) {
	return static_cast<NRI::ImageUsage>(static_cast<int>(a) | static_cast<int>(b));
}

class NRIAllocation {
   public:
	virtual ~NRIAllocation() {}
};

class NRIBuffer {
   public:
	virtual ~NRIBuffer() {}

	virtual NRI::MemoryRequirements getMemoryRequirements()									  = 0;
	virtual void					bindMemory(NRIAllocation &allocation, std::size_t offset) = 0;
};

class NRIImage2D {
   public:
	virtual ~NRIImage2D() {}

	virtual NRI::MemoryRequirements getMemoryRequirements()									  = 0;
	virtual void					bindMemory(NRIAllocation &allocation, std::size_t offset) = 0;

	virtual void clear(NRICommandBuffer &commandBuffer, glm::vec4 color) = 0;
};

class NRICommandPool {
   public:
	virtual ~NRICommandPool() {}
};

class NRICommandQueue {
   public:
	virtual ~NRICommandQueue() {}

	virtual void submit(NRICommandBuffer &commandBuffer) = 0;
	virtual void synchronize()							 = 0;
};

class NRICommandBuffer {
   public:
	virtual ~NRICommandBuffer() {}
};

class NRIQWindow : public QWindow {
   public:
	virtual ~NRIQWindow() {}

	virtual void drawFrame() = 0;
};
