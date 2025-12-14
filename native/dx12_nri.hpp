#pragma once
#include "nri.hpp"

#ifdef _WIN32
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class DX12NRIAllocation : public NRIAllocation {
	ComPtr<ID3D12Heap> heap = nullptr;

   public:
	DX12NRIAllocation() = default;
	DX12NRIAllocation(ID3D12Heap *h) : heap(h) {}
	ID3D12Heap *operator*() const { return heap.Get(); }
};

class DX12NRIBuffer : public NRIBuffer {
	D3D12_RESOURCE_DESC	   resourceDesc = {};
	ID3D12Device		  *device		= nullptr;
	ComPtr<ID3D12Resource> resource		= nullptr;

   public:
	DX12NRIBuffer() = default;
	DX12NRIBuffer(D3D12_RESOURCE_DESC desc, ID3D12Device *dev) : resourceDesc(desc), device(dev) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override {}
};

class DX12NRIImage2D : public NRIImage2D {
	D3D12_RESOURCE_DESC	   resourceDesc = {};
	ID3D12Device		  *device		= nullptr;
	ComPtr<ID3D12Resource> resource		= nullptr;
	uint32_t			   width;
	uint32_t			   height;

   public:
	DX12NRIImage2D() = default;
	DX12NRIImage2D(uint32_t w, uint32_t h, D3D12_RESOURCE_DESC desc, ID3D12Device *dev)
		: resourceDesc(desc), device(dev), resource(nullptr), width(w), height(h) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;

	void clear(NRICommandBuffer &commandBuffer, glm::vec4 color) override {}

	void prepareForPresent(NRICommandBuffer &commandBuffer) override {}
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

   public:
	DX12NRICommandQueue() = default;
	DX12NRICommandQueue(ID3D12Device *device);

	void submit(NRICommandBuffer &commandBuffer) override {}
	void synchronize() override {}

	ID3D12CommandQueue *operator*() const { return commandQueue.Get(); }
};

class DX12NRICommandBuffer : public NRICommandBuffer {
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

   public:
	DX12NRICommandBuffer() = default;
	DX12NRICommandBuffer(ID3D12CommandAllocator *commandAllocator, ID3D12Device *device);

	void begin() override {}
	void end() override { commandList->Close(); }

	ID3D12GraphicsCommandList *operator*() const { return commandList.Get(); }
};

class DX12NRI;

class DX12NRIQWindow : public NRIQWindow {
	ComPtr<IDXGISwapChain3> swapChain = nullptr;
	DX12NRICommandQueue		presentQueue;

	ComPtr<ID3D12Fence> frameFence		= nullptr;
	HANDLE				frameFenceEvent = nullptr;
	UINT64				frameFenceValue = 0;

	DX12NRICommandBuffer commandBuffer;

   protected:
   public:
	DX12NRIQWindow(const DX12NRI &nri, QApplication &app, std::unique_ptr<Renderer> &&renderer);
	void drawFrame() override;

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

	DX12NRICommandPool commandAllocator;

   public:
	DX12NRI();

	std::unique_ptr<NRIBuffer>		  createBuffer(std::size_t size, BufferUsage usage) const override;
	std::unique_ptr<NRIImage2D>		  createImage2D(uint32_t width, uint32_t height, NRI::Format fmt,
													NRI::ImageUsage usage) const override;
	std::unique_ptr<NRIAllocation>	  allocateMemory(MemoryRequirements memoryRequirements) const override;
	std::unique_ptr<NRICommandQueue>  createCommandQueue() const override;
	std::unique_ptr<NRICommandBuffer> createCommandBuffer(const NRICommandPool &commandPool) const override;
	std::unique_ptr<NRICommandPool>	  createCommandPool() const override;
	NRIQWindow *createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) const override;

	void synchronize() const override {}

	auto  getDevice() const { return device.Get(); }
	auto &getFactory() const { return factory; }
	auto &getDefaultCommandPool() const { return commandAllocator; }

   private:
	void createDevice();
};
#endif
