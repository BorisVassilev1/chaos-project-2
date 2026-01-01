#pragma once
#include "nri.hpp"

#ifdef _WIN32
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
	DX12NRIAllocation(ID3D12Heap *h, NRI::MemoryTypeRequest type) : heap(h), type(type) {}

	NRI::MemoryTypeRequest getTypeRequest() const { return type; }
	ID3D12Heap			  *operator*() const { return heap.Get(); }
};

class DX12NRIBuffer : public NRIBuffer {
	D3D12_RESOURCE_DESC	   resourceDesc = {};
	ID3D12Device		  *device		= nullptr;
	ComPtr<ID3D12Resource> resource		= nullptr;

   public:
	DX12NRIBuffer() = default;
	DX12NRIBuffer(const D3D12_RESOURCE_DESC &desc, ID3D12Device *dev) : resourceDesc(desc), device(dev) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;

	void *map(std::size_t offset, std::size_t size) override;
	void  unmap() override;
	void  copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset, std::size_t dstOffset,
				   std::size_t size) override;

	void bindAsVertexBuffer(NRICommandBuffer &commandBuffer, uint32_t binding, std::size_t offset) override;
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

	DX12NRIImage2D(uint32_t w, uint32_t h, D3D12_RESOURCE_DESC desc, DX12NRI &nri, NRI::ImageUsage usage)
		: resourceDesc(desc), resource(nullptr), width(w), height(h), usage(usage), nri(nri) {}
	DX12NRIImage2D(ID3D12Resource *res, uint32_t w, uint32_t h, NRI::ImageUsage usage, DX12NRI &nri);

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;

	void clear(NRICommandBuffer &commandBuffer, glm::vec4 color) override;

	void	 prepareForPresent(NRICommandBuffer &commandBuffer) override;
	uint32_t getWidth() const override { return width; }
	uint32_t getHeight() const override { return height; }

	D3D12_CPU_DESCRIPTOR_HANDLE getViewHandle() const { return viewHandle; }
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

	void beginRendering(NRIImage2D &renderTarget) override;
	void endRendering() override;

	ID3D12CommandAllocator	  *getCommandAllocator() const { return commandAllocator; }
	ID3D12GraphicsCommandList *operator*() const { return commandList.Get(); }
	ID3D12GraphicsCommandList *operator->() const { return commandList.Get(); }
};

class DX12NRIProgram : public NRIProgram {
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

   public:
	DX12NRIProgram(std::vector<NRI::ShaderCreateInfo> &&shaderCreateInfos, ID3D12Device *device);

	void bind(NRICommandBuffer &commandBuffer) override;
	void unbind(NRICommandBuffer &commandBuffer) override;

	void draw(NRICommandBuffer &commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
			  uint32_t firstInstance) override;
};

class DX12NRI;

class DX12NRIQWindow : public NRIQWindow {
	ComPtr<IDXGISwapChain3> swapChain = nullptr;
	DX12NRICommandQueue		presentQueue;

	ComPtr<ID3D12Fence> frameFence		= nullptr;
	HANDLE				frameFenceEvent = nullptr;
	UINT64				frameFenceValue = 0;

	DX12NRICommandBuffer commandBuffer;

	std::vector<DX12NRIImage2D> swapchainImages;

   protected:
	void resizeEvent(QResizeEvent *event) override;
   public:
	DX12NRIQWindow(DX12NRI &nri, QApplication &app, std::unique_ptr<Renderer> &&renderer);
	void				 drawFrame() override;
	DX12NRICommandQueue &getMainQueue() override;

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
	NRIQWindow				   *createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) override;
	std::unique_ptr<NRIProgram> createProgram(std::vector<ShaderCreateInfo> &&shaderInfos) override;

	bool shouldFlipY() const override { return false; }
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
