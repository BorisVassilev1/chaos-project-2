#ifdef _WIN32
#include "dx12_nri.hpp"
#include "nriFactory.hpp"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

static DXGI_FORMAT nriFormat2d3d12Format[] = {
	DXGI_FORMAT_UNKNOWN,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_B4G4R4A4_UNORM,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_B5G6R5_UNORM,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_B5G5R5A1_UNORM,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_SNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R8_SINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_SNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R8G8_SINT,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SINT,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_B8G8R8A8_UNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_B8G8R8A8_UNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R10G10B10A2_UINT,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_SNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SINT,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_SNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_R16G16_FLOAT,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SINT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_SINT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_D16_UNORM,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_D32_FLOAT,
	DXGI_FORMAT_R8_UINT,
	(DXGI_FORMAT)-1,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	(DXGI_FORMAT)-1,
	(DXGI_FORMAT)-1,
};

void DX12NRI::createDevice() {
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter);
		 ++adapterIndex) {
		DXGI_ADAPTER_DESC1 desc;
		HRESULT			   hr = adapter->GetDesc1(&desc);
		if (FAILED(hr)) continue;

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)))) {
			std::wcout << L"Selected adapter: " << desc.Description << std::endl;
			break;
		}
	}
	assert(device && "Failed to select D3D12 device.");
}

DX12NRI::DX12NRI() {
	ComPtr<ID3D12Debug>	 spDebugController0;
	ComPtr<ID3D12Debug1> spDebugController1;
	HRESULT				 hr = D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0));
	assert(SUCCEEDED(hr) && "Failed to get D3D12 debug interface.");
	hr = spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1));
	assert(SUCCEEDED(hr) && "Failed to get D3D12 debug1 interface.");
	spDebugController1->SetEnableGPUBasedValidation(true);

	hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));

	if (FAILED(hr)) {
		std::cerr << "Failed to create DXGI Factory." << std::endl;
		return;
	}
	createDevice();
	commandAllocator = DX12NRICommandPool(device.Get());
}

std::unique_ptr<NRIBuffer> DX12NRI::createBuffer(std::size_t size, BufferUsage usage) const {
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension			 = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width				 = size;
	desc.Height				 = 1;
	desc.DepthOrArraySize	 = 1;
	desc.MipLevels			 = 1;
	desc.Format				 = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count	 = 1;
	desc.SampleDesc.Quality	 = 0;
	desc.Layout				 = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags				 = D3D12_RESOURCE_FLAG_NONE;
	if (usage & NRI::BUFFER_USAGE_TRANSFER_SRC) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & NRI::BUFFER_USAGE_TRANSFER_DST) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & NRI::BUFFER_USAGE_STORAGE) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	return std::make_unique<DX12NRIBuffer>(desc, device.Get());
}

NRI::MemoryRequirements DX12NRIBuffer::getMemoryRequirements() {
	D3D12_RESOURCE_ALLOCATION_INFO info = device->GetResourceAllocationInfo(0, 1, &resourceDesc);
	return NRI::MemoryRequirements(info.SizeInBytes, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE, info.Alignment);
}

std::unique_ptr<NRIImage2D> DX12NRI::createImage2D(uint32_t width, uint32_t height, NRI::Format fmt,
												   NRI::ImageUsage usage) const {
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension			 = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width				 = width;
	desc.Height				 = height;
	desc.DepthOrArraySize	 = 1;
	desc.MipLevels			 = 1;
	assert(fmt != Format::FORMAT_UNDEFINED);
	assert(fmt < Format::_FORMAT_NUM);
	desc.Format = nriFormat2d3d12Format[fmt];
	assert(desc.Format != -1);
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags				= D3D12_RESOURCE_FLAG_NONE;
	if (usage & NRI::IMAGE_USAGE_TRANSFER_SRC) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & NRI::IMAGE_USAGE_TRANSFER_DST) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & NRI::IMAGE_USAGE_STORAGE) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & NRI::IMAGE_USAGE_COLOR_ATTACHMENT) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (usage & NRI::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	return std::make_unique<DX12NRIImage2D>(width, height, desc, device.Get());
}

NRI::MemoryRequirements DX12NRIImage2D::getMemoryRequirements() {
	D3D12_RESOURCE_ALLOCATION_INFO info = device->GetResourceAllocationInfo(0, 1, &resourceDesc);
	return NRI::MemoryRequirements(info.SizeInBytes, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE, info.Alignment);
}

void DX12NRIImage2D::bindMemory(NRIAllocation &allocation, std::size_t offset) {
	DX12NRIAllocation &alloc = static_cast<DX12NRIAllocation &>(allocation);
	device->CreatePlacedResource(*alloc, offset, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr,
								 IID_PPV_ARGS(&resource));
	assert(resource && "Failed to create placed resource.");
}

D3D12_HEAP_TYPE nriMemoryType2d3d12HeapType[]{
	D3D12_HEAP_TYPE_UPLOAD,
	D3D12_HEAP_TYPE_READBACK,
	D3D12_HEAP_TYPE_DEFAULT,
};

std::unique_ptr<NRIAllocation> DX12NRI::allocateMemory(MemoryRequirements memoryRequirements) const {
	D3D12_HEAP_DESC heapDesc				 = {};
	heapDesc.SizeInBytes					 = memoryRequirements.size;
	heapDesc.Properties.Type				 = nriMemoryType2d3d12HeapType[memoryRequirements.typeRequest];
	heapDesc.Properties.CPUPageProperty		 = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDesc.Alignment						 = memoryRequirements.alignment;
	heapDesc.Flags							 = D3D12_HEAP_FLAG_NONE;

	ID3D12Heap *heap = nullptr;
	HRESULT		hr	 = device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap));
	assert(SUCCEEDED(hr) && "Failed to create heap.");

	return std::make_unique<DX12NRIAllocation>(heap);
}

DX12NRICommandQueue::DX12NRICommandQueue(ID3D12Device *device) {
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags					  = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type					  = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hr					  = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(hr) && "Failed to create command queue.");
}

std::unique_ptr<NRICommandQueue> DX12NRI::createCommandQueue() const {
	return std::make_unique<DX12NRICommandQueue>(device.Get());
}

std::unique_ptr<NRICommandBuffer> DX12NRI::createCommandBuffer(const NRICommandPool &commandPool) const {
	DX12NRICommandPool pool = static_cast<const DX12NRICommandPool &>(commandPool);
	return std::make_unique<DX12NRICommandBuffer>(*pool, device.Get());
}

DX12NRICommandBuffer::DX12NRICommandBuffer(ID3D12CommandAllocator *cmdAlloc, ID3D12Device *device) {
	HRESULT hr =
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr) && "Failed to create command list.");
}

std::unique_ptr<NRICommandPool> DX12NRI::createCommandPool() const {
	return std::make_unique<DX12NRICommandPool>(device.Get());
}

DX12NRICommandPool::DX12NRICommandPool(ID3D12Device *device) {
	HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr) && "Failed to create command allocator.");
}

NRIQWindow *DX12NRI::createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) const {
	return new DX12NRIQWindow(*this, app, std::move(renderer));
}

DX12NRIQWindow::DX12NRIQWindow(const DX12NRI &nri, QApplication &app, std::unique_ptr<Renderer> &&renderer)
	: NRIQWindow((NRI &)nri, std::move(renderer)),
	  swapChain(nullptr),
	  presentQueue(),
	  frameFence(nullptr),
	  frameFenceEvent(nullptr),
	  frameFenceValue(0),
	  commandBuffer() {
	nri.getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameFence));
	frameFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert(frameFenceEvent && "Failed to create fence event.");

	presentQueue  = DX12NRICommandQueue(nri.getDevice());
	commandBuffer = DX12NRICommandBuffer(*nri.getDefaultCommandPool(), nri.getDevice());

	HWND hwnd = reinterpret_cast<HWND>(winId());
	RECT rect;
	GetWindowRect(hwnd, &rect);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width					= rect.right - rect.left;
	swapChainDesc.Height				= rect.bottom - rect.top;
	swapChainDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferCount			= 2;
	swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count		= 1;

	ComPtr<IDXGISwapChain1> swapChain1;
	HRESULT					hr =
		nri.getFactory()->CreateSwapChainForHwnd(*presentQueue, hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
	assert(SUCCEEDED(hr) && "Failed to create swap chain.");
	hr = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain));
	assert(SUCCEEDED(hr) && "Failed to get swap chain interface.");
}

void DX12NRIQWindow::drawFrame() {
	// UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	// HRESULT					hr = swapChain->GetBuffer(backBufferIndex, IID_PPV_ARGS(&backBuffer));
	// assert(SUCCEEDED(hr) && "Failed to get back buffer.");
	//
	// renderer->render(swapchainImages[backBufferIndex], commandBuffer);

	//// Execute command buffer
	// presentQueue.submit(commandBuffer);
	//// Present
	// hr = swapChain->Present(1, 0);
	// assert(SUCCEEDED(hr) && "Failed to present swap chain.");
	//// Wait for GPU to finish
	// frameFenceValue++;
	// hr = (*presentQueue)->Signal(frameFence.Get(), frameFenceValue);
	// assert(SUCCEEDED(hr) && "Failed to signal fence.");
	// if (frameFence->GetCompletedValue() < frameFenceValue) {
	//	hr = frameFence->SetEventOnCompletion(frameFenceValue, frameFenceEvent);
	//	assert(SUCCEEDED(hr) && "Failed to set event on fence completion.");
	//	WaitForSingleObject(frameFenceEvent, INFINITE);
	// }
}

int __reg_dx12_nri = []() {
	NRIFactory::getInstance().registerNRI("DirectX 12", []() -> NRI * { return new DX12NRI(); });
	return 0;
}();

#endif
