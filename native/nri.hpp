#pragma once

#include <cstddef>
#include <memory>
#include <glm/glm.hpp>
#include <QtWidgets>
#include <iostream>

class NRIBuffer;
class NRIAllocation;
class NRIImage2D;
class NRICommandPool;
class NRICommandQueue;
class NRICommandBuffer;
class NRIQWindow;
class NRIProgram;
class NRIGraphicsProgram;
class NRIComputeProgram;
class NRIProgramBuilder;
class Renderer;

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

	enum MemoryTypeRequest {
		MEMORY_TYPE_UPLOAD	 = 0,
		MEMORY_TYPE_READBACK = 1,
		MEMORY_TYPE_DEVICE	 = 2,
		_MEMORY_TYPE_NUM	 = 3
	};

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

	enum ShaderType {
		SHADER_TYPE_VERTEX					= 0,
		SHADER_TYPE_FRAGMENT				= 1,
		SHADER_TYPE_COMPUTE					= 2,
		SHADER_TYPE_GEOMETRY				= 3,
		SHADER_TYPE_TESSELLATION_CONTROL	= 4,
		SHADER_TYPE_TESSELLATION_EVALUATION = 5,
		SHADER_TYPE_RAYGEN					= 6,
		SHADER_TYPE_ANY_HIT					= 7,
		SHADER_TYPE_CLOSEST_HIT				= 8,
		SHADER_TYPE_MISS					= 9,
		SHADER_TYPE_INTERSECTION			= 10,
		SHADER_TYPE_MESH					= 11,
		SHADER_TYPE_TASK					= 12,
		_SHADER_TYPE_NUM
	};

	struct MemoryRequirements {
		std::size_t		  size;
		std::size_t		  alignment = 0;
		MemoryTypeRequest typeRequest;

		MemoryRequirements(std::size_t s, MemoryTypeRequest tr, std::size_t a = 0)
			: size(s), alignment(a), typeRequest(tr) {}

		MemoryRequirements &setTypeRequest(MemoryTypeRequest tr);
	};

	virtual std::unique_ptr<NRIBuffer>		  createBuffer(std::size_t size, BufferUsage usage)		 = 0;
	virtual std::unique_ptr<NRIImage2D>		  createImage2D(uint32_t width, uint32_t height, Format fmt,
															ImageUsage usage)						 = 0;
	virtual std::unique_ptr<NRIAllocation>	  allocateMemory(MemoryRequirements memoryRequirements)	 = 0;
	virtual std::unique_ptr<NRICommandQueue>  createCommandQueue()									 = 0;
	virtual std::unique_ptr<NRICommandBuffer> createCommandBuffer(const NRICommandPool &commandPool) = 0;
	virtual std::unique_ptr<NRICommandPool>	  createCommandPool()									 = 0;
	virtual NRICommandPool					 &getDefaultCommandPool()								 = 0;

	struct ShaderCreateInfo {
		std::string		sourceFile;		/// path to the shader source file
		std::string		entryPoint;		/// entry point function name
		NRI::ShaderType shaderType;		/// type of the shader (vertex, fragment, etc.)
	};

	enum VertexInputRate { VERTEX_INPUT_RATE_VERTEX = 0, VERTEX_INPUT_RATE_INSTANCE = 1 };

	struct VertexAttribute {
		uint32_t		location;	   /// shader-specified location
		NRI::Format		format;		   /// format of the attribute
		uint32_t		offset;		   /// offset in vertex buffer
		VertexInputRate inputRate;	   /// input rate (per-vertex or per-instance)
		std::string		name;		   /// optional name of the attribute
	};

	struct VertexBinding {
		uint32_t					 binding;		/// binding index
		uint32_t					 stride;		/// size of one vertex
		VertexInputRate				 inputRate;		/// input rate (per-vertex or per-instance)
		std::vector<VertexAttribute> attributes;
	};

	enum PrimitiveType {
		PRIMITIVE_TYPE_TRIANGLES	  = 0,
		PRIMITIVE_TYPE_TRIANGLE_STRIP = 1,
		PRIMITIVE_TYPE_LINES		  = 2,
		PRIMITIVE_TYPE_LINE_STRIP	  = 3,
		PRIMITIVE_TYPE_POINTS		  = 4
	};

	struct PushConstantRange {
		uint32_t offset;
		uint32_t size;
	};

	virtual std::unique_ptr<NRIProgramBuilder> createProgramBuilder()								  = 0;
	virtual NRIQWindow *createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) = 0;

	virtual bool shouldFlipY() const = 0;

	virtual void synchronize() const = 0;
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
	virtual void				   *map(std::size_t offset, std::size_t size)				  = 0;
	virtual void					unmap()													  = 0;

	virtual void copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset,
						  std::size_t dstOffset, std::size_t size) = 0;

	virtual void bindAsVertexBuffer(NRICommandBuffer &commandBuffer, uint32_t binding, std::size_t offset) = 0;
};

class NRIImage2D {
   public:
	virtual ~NRIImage2D() {}

	virtual NRI::MemoryRequirements getMemoryRequirements()									  = 0;
	virtual void					bindMemory(NRIAllocation &allocation, std::size_t offset) = 0;

	virtual void clear(NRICommandBuffer &commandBuffer, glm::vec4 color) = 0;
	virtual void prepareForPresent(NRICommandBuffer &commandBuffer)		 = 0;

	virtual uint32_t getWidth() const  = 0;
	virtual uint32_t getHeight() const = 0;
};

class NRICommandPool {
   public:
	virtual ~NRICommandPool() {}
};

class NRICommandQueue {
   public:
	virtual ~NRICommandQueue() {}

	using SubmitKey = uint64_t;

	virtual SubmitKey submit(NRICommandBuffer &commandBuffer) = 0;
	virtual void	  wait(SubmitKey)						  = 0;
};

class NRICommandBuffer {
   public:
	virtual ~NRICommandBuffer() {}

	virtual void begin() = 0;
	virtual void end()	 = 0;

	virtual void beginRendering(NRIImage2D &renderTarget) = 0;
	virtual void endRendering()							  = 0;
};

class NRIProgramBuilder {
   protected:
	std::vector<NRI::ShaderCreateInfo>	shaderStagesInfo;
	std::vector<NRI::VertexBinding>		vertexBindings;
	NRI::PrimitiveType					primitiveType;
	std::vector<NRI::PushConstantRange> pushConstantRanges;

   public:
	virtual ~NRIProgramBuilder() {}

	NRIProgramBuilder &addShaderModule(const NRI::ShaderCreateInfo &shaderInfo);
	NRIProgramBuilder &setVertexBindings(const std::vector<NRI::VertexBinding> &bindings);
	NRIProgramBuilder &setPrimitiveType(NRI::PrimitiveType primitiveType);
	NRIProgramBuilder &setPushConstantRanges(const std::vector<NRI::PushConstantRange> &ranges);
	virtual std::unique_ptr<NRIGraphicsProgram> buildGraphicsProgram() = 0;
	virtual std::unique_ptr<NRIComputeProgram>	buildComputeProgram()  = 0;
};

/// NRIProgram - represents a GPU program (shader)
/// should contain shader modules, pipeline state and layout
class NRIProgram {
   public:
	virtual ~NRIProgram() {}

	virtual void bind(NRICommandBuffer &commandBuffer)	 = 0;
	virtual void unbind(NRICommandBuffer &commandBuffer) = 0;

	virtual void setPushConstants(NRICommandBuffer &commandBuffer, const void *data, std::size_t size,
								  std::size_t offset) = 0;
};

class NRIGraphicsProgram : virtual public NRIProgram {
   public:
	virtual void draw(NRICommandBuffer &commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
					  uint32_t firstVertex, uint32_t firstInstance) = 0;
};

class NRIComputeProgram : virtual public NRIProgram {
   public:
	virtual void dispatch(NRICommandBuffer &commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
						  uint32_t groupCountZ) = 0;
};

class Renderer : public QWidget {
   protected:
	NRI &nri;

   public:
	Renderer(NRI &nri) : nri(nri) {}
	virtual void initialize(NRIQWindow &window)								= 0;
	virtual void render(NRIImage2D &currentImage, NRICommandBuffer &cmdBuf) = 0;
	virtual ~Renderer() {}
};

class NRIQWindow : public QWindow {
   protected:
	std::unique_ptr<Renderer> renderer;
	std::chrono::steady_clock::time_point lastFrameTime;
	float _deltaTime = 0.0f;

	using frameCallback = std::function<void()>;
	using resizeCallback = std::function<void(QResizeEvent *)>;
	using keyCallback	 = std::function<void(QKeyEvent *)>;
	using mouseCallback	 = std::function<void(QMouseEvent *)>;
	std::vector<frameCallback> frameCallbacks;
	std::vector<resizeCallback> resizeCallbacks;
	std::vector<keyCallback> keyCallbacks;
	std::vector<mouseCallback> mouseCallbacks;
	QTimer					   timer;
	NRI						  &nri;

   public:
	NRIQWindow(NRI &nri, std::unique_ptr<Renderer> &&rendererPtr);

	void startFrameTimer();
	void addFrameCallback(const frameCallback &cb);
	void addResizeCallback(const resizeCallback &cb);
	void addKeyCallback(const keyCallback &cb);
	void addMouseCallback(const mouseCallback &cb);
	void closeEvent(QCloseEvent *event) override ;
	void resizeEvent(QResizeEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

	virtual void drawFrame() = 0;

	auto deltaTime() const { return _deltaTime; }

	auto					&getRenderer() { return renderer; }
	virtual NRICommandQueue &getMainQueue() = 0;
};
