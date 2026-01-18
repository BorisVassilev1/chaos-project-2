#pragma once
#include "nri.hpp"

#if defined(_WIN32) && defined(NRI_DX12)
	#include <dxgi1_6.h>
	#include <d3d12.h>
	#include <wrl.h>
	#include <dxgidebug.h>

using Microsoft::WRL::ComPtr;

class DX12NRI;

class DescriptorAllocator {
	ComPtr<ID3D12DescriptorHeap> rtvHeap	   = nullptr;
	ComPtr<ID3D12DescriptorHeap> dsvHeap	   = nullptr;
	ComPtr<ID3D12DescriptorHeap> samplerHeap   = nullptr;
	ComPtr<ID3D12DescriptorHeap> cbvSrvUavHeap = nullptr;
	ID3D12Device				*device		   = nullptr;

   public:
	DescriptorAllocator() = default;
	DescriptorAllocator(ID3D12Device *device);

	ID3D12DescriptorHeap *getRtvHeap() const;
	ID3D12DescriptorHeap *getDsvHeap() const;
	ID3D12DescriptorHeap *getSamplerHeap() const;
	ID3D12DescriptorHeap *getCbvSrvUavHeap() const;

	D3D12_CPU_DESCRIPTOR_HANDLE allocateRtv();
	D3D12_CPU_DESCRIPTOR_HANDLE allocateDsv();
	D3D12_CPU_DESCRIPTOR_HANDLE allocateSampler();
	D3D12_CPU_DESCRIPTOR_HANDLE allocateCbvSrvUav();
};

class DX12NRIAllocation : public NRIAllocation {
	ComPtr<ID3D12Heap>	   heap = nullptr;
	NRI::MemoryTypeRequest type;

   public:
	DX12NRIAllocation() = default;
	DX12NRIAllocation(DX12NRI &nri, NRI::MemoryRequirements memoryRequirements);

	NRI::MemoryTypeRequest getTypeRequest() const { return type; }
	ID3D12Heap			  *operator*() const { return heap.Get(); }
};

class DX12NRIBuffer : public NRIBuffer {
	D3D12_RESOURCE_DESC	   resourceDesc = {};
	ID3D12Device		  *device		= nullptr;
	ComPtr<ID3D12Resource> resource		= nullptr;
	std::size_t			   size			= 0;
	std::size_t			   offset		= 0;

   public:
	DX12NRIBuffer() = default;
	DX12NRIBuffer(const D3D12_RESOURCE_DESC &desc, ID3D12Device *dev)
		: resourceDesc(desc), device(dev), size(desc.Width) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;

	void *map(std::size_t offset, std::size_t size) override;
	void  unmap() override;

	std::size_t getSize() const override;
	std::size_t getOffset() const override;

	void copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset, std::size_t dstOffset,
				  std::size_t size) override;

	void bindAsVertexBuffer(NRICommandBuffer &commandBuffer, uint32_t binding, std::size_t offset,
							std::size_t stride) override;
	void bindAsIndexBuffer(NRICommandBuffer &commandBuffer, std::size_t offset, NRI::IndexType indexType) override;

	ID3D12Resource *operator*() const { return resource.Get(); }
};

class DX12NRIImage2D : public NRIImage2D {
	D3D12_RESOURCE_DESC	   resourceDesc = {};
	ComPtr<ID3D12Resource> resource		= nullptr;
	ComPtr<ID3D12Resource> viewResource = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE viewHandle = {};

	uint32_t		width;
	uint32_t		height;
	NRI::ImageUsage usage;

	D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_COMMON;

	DX12NRI &nri;

   public:
	void transitionState(NRICommandBuffer &commandBuffer, D3D12_RESOURCE_STATES newState);

	DX12NRIImage2D(DX12NRIImage2D &&other) = default;
	DX12NRIImage2D &operator=(DX12NRIImage2D &&other) = default;

	DX12NRIImage2D(DX12NRI &nri, uint32_t width, uint32_t height, NRI::Format fmt, NRI::ImageUsage usage);

	DX12NRIImage2D(ID3D12Resource *res, uint32_t w, uint32_t h, NRI::ImageUsage usage, DX12NRI &nri);

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;

	void clear(NRICommandBuffer &commandBuffer, glm::vec4 color) override;
	void copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset,
				  uint32_t srcRowPitch) override;

	void	 prepareForPresent(NRICommandBuffer &commandBuffer) override;
	uint32_t getWidth() const override { return width; }
	uint32_t getHeight() const override { return height; }
	NRIResourceHandle			getImageViewHandle() override;

	D3D12_CPU_DESCRIPTOR_HANDLE getViewHandle() const { return viewHandle; }
	ID3D12Resource			   *operator*() const { return resource.Get(); }
};

class DX12NRICommandPool : public NRICommandPool {
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;

   public:
	DX12NRICommandPool() = default;
	DX12NRICommandPool(ID3D12Device *device);
	ID3D12CommandAllocator *operator*() const { return commandAllocator.Get(); }
};

class DX12NRICommandQueue : public NRICommandQueue {
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;

	ComPtr<ID3D12Fence> fence	   = nullptr;
	HANDLE				fenceEvent = nullptr;
	uint64_t			fenceValue = 0;

   public:
	DX12NRICommandQueue() = default;
	DX12NRICommandQueue(ID3D12Device *device);

	SubmitKey submit(NRICommandBuffer &commandBuffer) override;
	void	  wait(SubmitKey key) override;

	ID3D12CommandQueue *operator*() const { return commandQueue.Get(); }
};

class DX12NRICommandBuffer : public NRICommandBuffer {
	ComPtr<ID3D12GraphicsCommandList> commandList	   = nullptr;
	ID3D12CommandAllocator			 *commandAllocator = nullptr;
	bool							  opened		   = false;

   public:
	DX12NRICommandBuffer() = default;
	DX12NRICommandBuffer(ID3D12CommandAllocator *commandAllocator, ID3D12Device *device);

	void begin() override {
		if (!opened) {
			HRESULT hr = commandList->Reset(commandAllocator, nullptr);
			assert(SUCCEEDED(hr) && "Failed to reset command list");
			opened = true;
		}
	}
	void end() override {
		if (opened) {
			HRESULT hr = commandList->Close();
			assert(SUCCEEDED(hr) && "Failed to close command list");
			opened = false;
		}
	}

	ID3D12CommandAllocator	  *getCommandAllocator() const { return commandAllocator; }
	ID3D12GraphicsCommandList *operator*() const { return commandList.Get(); }
	ID3D12GraphicsCommandList *operator->() const { return commandList.Get(); }
};

class DX12NRIProgram : virtual NRIProgram {
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

   public:
	DX12NRIProgram(ComPtr<ID3D12PipelineState> &&pso, ComPtr<ID3D12RootSignature> &&rootSig)
		: pipelineState(pso), rootSignature(rootSig) {}

	void bind(NRICommandBuffer &commandBuffer) override;
	void unbind(NRICommandBuffer &commandBuffer) override;

	void setPushConstants(NRICommandBuffer &commandBuffer, const void *data, std::size_t size,
						  std::size_t offset) override;
};

class DX12NRIGraphicsProgram : public DX12NRIProgram, public NRIGraphicsProgram {
   public:
	using DX12NRIProgram::DX12NRIProgram;

	void draw(NRICommandBuffer &commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
			  uint32_t firstInstance) override;
	void drawIndexed(NRICommandBuffer &commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
					 int32_t vertexOffset, uint32_t firstInstance) override;
};

class DX12NRIComputeProgram : public DX12NRIProgram, public NRIComputeProgram {
   public:
	using DX12NRIProgram::DX12NRIProgram;

	void dispatch(NRICommandBuffer &commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
				  uint32_t groupCountZ) override;
};

class DX12NRIProgramBuilder : public NRIProgramBuilder {
	DX12NRI &nri;

	std::vector<ComPtr<ID3DBlob>> compileShaderModules(const std::vector<NRI::ShaderCreateInfo> &shaderCreateInfos);

   public:
	DX12NRIProgramBuilder(DX12NRI &nri);
	std::unique_ptr<NRIGraphicsProgram> buildGraphicsProgram() override;
	std::unique_ptr<NRIComputeProgram>	buildComputeProgram() override;
	std::unique_ptr<NRIRayTracingProgram> buildRayTracingProgram() override;
};

class DX12NRIQWindow : public NRIQWindow {
	ComPtr<IDXGISwapChain3> swapChain = nullptr;
	DX12NRICommandQueue		presentQueue;

	ComPtr<ID3D12Fence> frameFence		= nullptr;
	HANDLE				frameFenceEvent = nullptr;
	UINT64				frameFenceValue = 0;

	DX12NRICommandBuffer commandBuffer;

	std::vector<DX12NRIImage2D>	  swapchainImages;

	std::optional<DX12NRIImage2D> depthImage;
	std::optional<DX12NRIAllocation> depthImageMemory;

   protected:
	void resizeEvent(QResizeEvent *event) override;

   public:
	DX12NRIQWindow(DX12NRI &nri, QApplication &app, std::unique_ptr<Renderer> &&renderer);
	void				 drawFrame() override;
	DX12NRICommandQueue &getMainQueue() override;

	void beginRendering(NRICommandBuffer &cmdBuf, NRIImage2D &renderTarget) override;
	void endRendering(NRICommandBuffer &cmdBuf) override;

	const auto &getSwapChain() const { return swapChain; }
	auto	   &getSwapChain() { return swapChain; }
	const auto &getPresentQueue() const { return presentQueue; }
	auto	   &getPresentQueue() { return presentQueue; }
	auto	   &getFrameFence() { return frameFence; }
};

class DX12NRI : public NRI {
	ComPtr<IDXGIFactory4> factory = nullptr;
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	ComPtr<ID3D12Device>  device  = nullptr;

	DX12NRICommandPool	commandAllocator;
	DescriptorAllocator descriptorAllocator;

	ComPtr<ID3D12Debug6> debugController;
	ComPtr<IDXGIDebug1>	 dxgiDebug;

   public:
	DX12NRI();
	~DX12NRI();

	std::unique_ptr<NRIBuffer>		  createBuffer(std::size_t size, BufferUsage usage) override;
	std::unique_ptr<NRIImage2D>		  createImage2D(uint32_t width, uint32_t height, NRI::Format fmt,
													NRI::ImageUsage usage) override;
	std::unique_ptr<NRIAllocation>	  allocateMemory(MemoryRequirements memoryRequirements) override;
	std::unique_ptr<NRICommandQueue>  createCommandQueue() override;
	std::unique_ptr<NRICommandBuffer> createCommandBuffer(const NRICommandPool &commandPool) override;
	std::unique_ptr<NRICommandPool>	  createCommandPool() override;
	NRIQWindow *createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) override;
	std::unique_ptr<NRIProgramBuilder> createProgramBuilder() override;
	std::unique_ptr<NRIBLAS> createBLAS(NRIBuffer &vertexBuffer, NRI::Format vertexFormat, std::size_t vertexOffset,
										uint32_t vertexCount, std::size_t vertexStride, NRIBuffer &indexBuffer,
										NRI::IndexType indexType, std::size_t indexOffset) override;
	std::unique_ptr<NRITLAS> createTLAS(const std::span<const NRIBLAS *>	 &blases,
										std::optional<std::span<glm::mat3x4>> transforms = std::nullopt) override;

	bool shouldFlipY() const override { return false; }
	bool supportsRayTracing() const override { return false; }
	bool supportsTextures() const override { return false; }
	void synchronize() const override {}

	auto				getDevice() const { return device.Get(); }
	auto			   &getFactory() const { return factory; }
	const auto		   &getDefaultCommandPool() const { return commandAllocator; }
	DX12NRICommandPool &getDefaultCommandPool() override { return commandAllocator; }
	const auto		   &getDescriptorAllocator() const { return descriptorAllocator; }
	auto			   &getDescriptorAllocator() { return descriptorAllocator; }

   private:
	void createDevice();
};
#endif
