#ifdef _WIN32
	#include <windows.h>
	#include <wrl/client.h>
	#include "dx12_nri.hpp"
	#include <dxcapi.h>
	#include "nriFactory.hpp"
	#include "../beamcast/utils.hpp"

	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "d3d12.lib")
	#pragma comment(lib, "dxcompiler.lib")

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

DescriptorAllocator::DescriptorAllocator(ID3D12Device *device) : device(device) {
	const UINT numDescriptors = 1024;
	// RTV Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors				= numDescriptors;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HRESULT hr						= device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap));
		assert(SUCCEEDED(hr) && "Failed to create RTV descriptor heap.");
	}
	// DSV Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors				= numDescriptors;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HRESULT hr						= device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dsvHeap));
		assert(SUCCEEDED(hr) && "Failed to create DSV descriptor heap.");
	}
	// Sampler Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.NumDescriptors				= numDescriptors;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		HRESULT hr						= device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&samplerHeap));
		assert(SUCCEEDED(hr) && "Failed to create Sampler descriptor heap.");
	}
	// CBV/SRV/UAV Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors				= numDescriptors;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		HRESULT hr						= device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&cbvSrvUavHeap));
		assert(SUCCEEDED(hr) && "Failed to create CBV/SRV/UAV descriptor heap.");
	}
}

ID3D12DescriptorHeap *DescriptorAllocator::getRtvHeap() const { return rtvHeap.Get(); }
ID3D12DescriptorHeap *DescriptorAllocator::getDsvHeap() const { return dsvHeap.Get(); }
ID3D12DescriptorHeap *DescriptorAllocator::getSamplerHeap() const { return samplerHeap.Get(); }
ID3D12DescriptorHeap *DescriptorAllocator::getCbvSrvUavHeap() const { return cbvSrvUavHeap.Get(); }

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::allocateRtv() {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	static UINT					offset = 0;
	handle.ptr += offset * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	offset++;
	assert(handle.ptr && "Failed to allocate RTV descriptor.");
	assert(offset < rtvHeap->GetDesc().NumDescriptors && "RTV Descriptor Heap overflow.");
	return handle;
}
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::allocateDsv() {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	static UINT					offset = 0;
	handle.ptr += offset * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	offset++;
	assert(handle.ptr && "Failed to allocate DSV descriptor.");
	assert(offset < dsvHeap->GetDesc().NumDescriptors && "DSV Descriptor Heap overflow.");
	return handle;
}
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::allocateSampler() {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = samplerHeap->GetCPUDescriptorHandleForHeapStart();
	static UINT					offset = 0;
	handle.ptr += offset * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	offset++;
	assert(handle.ptr && "Failed to allocate Sampler descriptor.");
	assert(offset < samplerHeap->GetDesc().NumDescriptors && "Sampler Descriptor Heap overflow.");
	return handle;
}
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::allocateCbvSrvUav() {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart();
	static UINT					offset = 0;
	handle.ptr += offset * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	offset++;
	assert(handle.ptr && "Failed to allocate CBV/SRV/UAV descriptor.");
	assert(offset < cbvSrvUavHeap->GetDesc().NumDescriptors && "CBV/SRV/UAV Descriptor Heap overflow.");
	return handle;
}

D3D12_HEAP_TYPE nriMemoryType2d3d12HeapType[]{
	D3D12_HEAP_TYPE_UPLOAD,
	D3D12_HEAP_TYPE_READBACK,
	D3D12_HEAP_TYPE_DEFAULT,
};

DX12NRIAllocation::DX12NRIAllocation(DX12NRI &nri, NRI::MemoryRequirements memoryRequirements)
	: type(memoryRequirements.typeRequest) {
	D3D12_HEAP_DESC heapDesc				 = {};
	heapDesc.SizeInBytes					 = memoryRequirements.size;
	heapDesc.Properties.Type				 = nriMemoryType2d3d12HeapType[memoryRequirements.typeRequest];
	heapDesc.Properties.CPUPageProperty		 = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDesc.Alignment						 = memoryRequirements.alignment;
	heapDesc.Flags							 = D3D12_HEAP_FLAG_NONE;

	HRESULT hr = nri.getDevice()->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap));
	if (FAILED(hr)) { std::cerr << "Failed to allocate memory of size: " << memoryRequirements.size << std::endl; }
	assert(SUCCEEDED(hr) && "Failed to create heap.");
}

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
	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	if (FAILED(hr)) {
		std::cerr << "Failed to get D3D12 debug interface." << std::endl;
	} else {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
		hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));
		if (FAILED(hr)) {
			std::cerr << "Failed to get DXGI debug interface." << std::endl;
		} else {
			dxgiDebug->EnableLeakTrackingForThread();
		}
	}

	hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));

	if (FAILED(hr)) {
		std::cerr << "Failed to create DXGI Factory." << std::endl;
		return;
	}
	createDevice();
	commandAllocator	= DX12NRICommandPool(device.Get());
	descriptorAllocator = DescriptorAllocator(device.Get());
}

DX12NRI::~DX12NRI() {
	if (dxgiDebug) {
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
									 DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
}

std::unique_ptr<NRIBuffer> DX12NRI::createBuffer(std::size_t size, BufferUsage usage) {
	std::cout << "Creating Buffer - Size: " << size << ", Usage: " << usage << std::endl;
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
	if (usage & NRI::BUFFER_USAGE_STORAGE) desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	return std::make_unique<DX12NRIBuffer>(desc, device.Get());
}

NRI::MemoryRequirements DX12NRIBuffer::getMemoryRequirements() {
	D3D12_RESOURCE_ALLOCATION_INFO info = device->GetResourceAllocationInfo(0, 1, &resourceDesc);
	return NRI::MemoryRequirements(info.SizeInBytes, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE, info.Alignment);
}

void DX12NRIBuffer::bindMemory(NRIAllocation &allocation, std::size_t offset) {
	DX12NRIAllocation	 &alloc		   = static_cast<DX12NRIAllocation &>(allocation);
	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
	if (alloc.getTypeRequest() == NRI::MemoryTypeRequest::MEMORY_TYPE_UPLOAD) {
		initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	HRESULT hr =
		device->CreatePlacedResource(*alloc, offset, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr) && "Failed to bind buffer memory.");
	this->offset = offset;
}

void *DX12NRIBuffer::map(std::size_t offset, std::size_t size) {
	void	   *mappedData = nullptr;
	D3D12_RANGE readRange  = {};
	readRange.Begin		   = offset;
	readRange.End		   = offset + size;
	HRESULT hr			   = resource->Map(0, &readRange, &mappedData);
	assert(SUCCEEDED(hr) && "Failed to map buffer memory.");
	return static_cast<uint8_t *>(mappedData) + offset;
}

void DX12NRIBuffer::unmap() { resource->Unmap(0, nullptr); }

std::size_t DX12NRIBuffer::getSize() const { return size; }
std::size_t DX12NRIBuffer::getOffset() const { return offset; }

void DX12NRIBuffer::copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset,
							 std::size_t dstOffset, std::size_t size) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	DX12NRIBuffer		 &dxSrcBuffer = static_cast<DX12NRIBuffer &>(srcBuffer);

	dxCmdBuffer.begin();
	dxCmdBuffer->CopyBufferRegion(resource.Get(), dstOffset, dxSrcBuffer.resource.Get(), srcOffset, size);
}

void DX12NRIBuffer::bindAsVertexBuffer(NRICommandBuffer &commandBuffer, uint32_t binding, std::size_t offset,
									   std::size_t stride) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	dxCmdBuffer.begin();
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation			= resource->GetGPUVirtualAddress() + offset;
	vbView.SizeInBytes				= static_cast<UINT>(resourceDesc.Width);
	vbView.StrideInBytes			= stride;	  // TODO: THIS IS BAD!!! FIX IT!!!

	dxCmdBuffer->IASetVertexBuffers(binding, 1, &vbView);
}

DXGI_FORMAT nriIndexTypeToDxgiFormat[]{
	DXGI_FORMAT_R16_UINT,	  // INDEX_TYPE_UINT16
	DXGI_FORMAT_R32_UINT	  // INDEX_TYPE_UINT32
};

void DX12NRIBuffer::bindAsIndexBuffer(NRICommandBuffer &commandBuffer, std::size_t offset, NRI::IndexType indexType) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	dxCmdBuffer.begin();
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation		   = resource->GetGPUVirtualAddress() + offset;
	ibView.SizeInBytes			   = static_cast<UINT>(resourceDesc.Width);
	assert(indexType < NRI::IndexType::_INDEX_TYPE_NUM);
	ibView.Format = nriIndexTypeToDxgiFormat[indexType];
	assert(ibView.Format != DXGI_FORMAT_UNKNOWN && "Unsupported index type.");
	dxCmdBuffer->IASetIndexBuffer(&ibView);
}

std::unique_ptr<NRIImage2D> DX12NRI::createImage2D(uint32_t width, uint32_t height, NRI::Format fmt,
												   NRI::ImageUsage usage) {
	return std::make_unique<DX12NRIImage2D>(*this, width, height, fmt, usage);
}

DX12NRIImage2D::DX12NRIImage2D(DX12NRI &nri, uint32_t width, uint32_t height, NRI::Format fmt, NRI::ImageUsage usage)
	: nri(nri), width(width), height(height), usage(usage) {
	resourceDesc.Dimension		  = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width			  = width;
	resourceDesc.Height			  = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels		  = 1;
	assert(fmt != NRI::Format::FORMAT_UNDEFINED);
	assert(fmt < NRI::Format::_FORMAT_NUM);
	resourceDesc.Format = nriFormat2d3d12Format[fmt];
	assert(resourceDesc.Format != -1);
	resourceDesc.SampleDesc.Count	= 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags				= D3D12_RESOURCE_FLAG_NONE;
	if (usage & NRI::IMAGE_USAGE_TRANSFER_SRC) resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & NRI::IMAGE_USAGE_TRANSFER_DST) resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & NRI::IMAGE_USAGE_STORAGE) resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (usage & NRI::IMAGE_USAGE_COLOR_ATTACHMENT) resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (usage & NRI::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
}

DX12NRIImage2D::DX12NRIImage2D(ID3D12Resource *res, uint32_t w, uint32_t h, NRI::ImageUsage usage, DX12NRI &nri)
	: resourceDesc(res->GetDesc()), resource(res), width(w), height(h), usage(usage), nri(nri) {
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format						  = resourceDesc.Format;
	rtvDesc.ViewDimension				  = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice			  = 0;
	rtvDesc.Texture2D.PlaneSlice		  = 0;

	viewHandle = nri.getDescriptorAllocator().allocateRtv();
	nri.getDevice()->CreateRenderTargetView(resource.Get(), &rtvDesc, viewHandle);
}

NRI::MemoryRequirements DX12NRIImage2D::getMemoryRequirements() {
	D3D12_RESOURCE_ALLOCATION_INFO info = nri.getDevice()->GetResourceAllocationInfo(0, 1, &resourceDesc);
	return NRI::MemoryRequirements(info.SizeInBytes, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE, info.Alignment);
}

void DX12NRIImage2D::bindMemory(NRIAllocation &allocation, std::size_t offset) {
	DX12NRIAllocation &alloc	  = static_cast<DX12NRIAllocation &>(allocation);
	D3D12_CLEAR_VALUE  clearValue = {};
	if (resourceDesc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT || resourceDesc.Format == DXGI_FORMAT_D32_FLOAT) {
		clearValue.DepthStencil = {1.0f, 0};
		clearValue.Format		= resourceDesc.Format;
	} else {
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 0.0f;
		clearValue.Format	= resourceDesc.Format;
	}
	nri.getDevice()->CreatePlacedResource(*alloc, offset, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue,
										  IID_PPV_ARGS(&resource));
	// TODO: We need a full-featured view creation system, not this implicit bullshit
	if (resourceDesc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT || resourceDesc.Format == DXGI_FORMAT_D32_FLOAT) {
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format						  = resourceDesc.Format;
		dsvDesc.ViewDimension				  = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice			  = 0;
		dsvDesc.Flags						  = D3D12_DSV_FLAG_NONE;

		viewHandle = nri.getDescriptorAllocator().allocateDsv();
		nri.getDevice()->CreateDepthStencilView(resource.Get(), &dsvDesc, viewHandle);
	} else {
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format						  = resourceDesc.Format;
		rtvDesc.ViewDimension				  = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice			  = 0;
		rtvDesc.Texture2D.PlaneSlice		  = 0;

		viewHandle = nri.getDescriptorAllocator().allocateRtv();
		nri.getDevice()->CreateRenderTargetView(resource.Get(), &rtvDesc, viewHandle);
	}
}

void DX12NRIImage2D::transitionState(NRICommandBuffer &commandBuffer, D3D12_RESOURCE_STATES newState) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);

	dxCmdBuffer.begin();
	if (currentState == newState) return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = resource.Get();
	barrier.Transition.StateBefore = currentState;
	barrier.Transition.StateAfter  = newState;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	dxCmdBuffer->ResourceBarrier(1, &barrier);
	currentState = newState;
}

void DX12NRIImage2D::clear(NRICommandBuffer &commandBuffer, glm::vec4 color) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	dxCmdBuffer.begin();
	transitionState(commandBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);

	float clearColor[4] = {color.r, color.g, color.b, color.a};
	dxCmdBuffer->ClearRenderTargetView(viewHandle, clearColor, 0, nullptr);
}

void DX12NRIImage2D::prepareForPresent(NRICommandBuffer &commandBuffer) {
	static_cast<DX12NRICommandBuffer &>(commandBuffer).begin();
	transitionState(commandBuffer, D3D12_RESOURCE_STATE_PRESENT);
}

std::unique_ptr<NRIAllocation> DX12NRI::allocateMemory(MemoryRequirements memoryRequirements) {
	return std::make_unique<DX12NRIAllocation>(*this, memoryRequirements);
}

DX12NRICommandQueue::DX12NRICommandQueue(ID3D12Device *device) {
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags					  = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type					  = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hr					  = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(hr) && "Failed to create command queue.");
	hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr) && "Failed to create command queue fence.");
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert(fenceEvent && "Failed to create fence event.");
}

NRICommandQueue::SubmitKey DX12NRICommandQueue::submit(NRICommandBuffer &commandBuffer) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	ID3D12CommandList	 *cmdLists[]  = {(*dxCmdBuffer)};
	commandBuffer.end();
	commandQueue->ExecuteCommandLists(1, cmdLists);
	fenceValue++;
	HRESULT hr = commandQueue->Signal(fence.Get(), fenceValue);
	assert(SUCCEEDED(hr) && "Failed to signal command queue fence.");
	return static_cast<NRICommandQueue::SubmitKey>(fenceValue);
}

void DX12NRICommandQueue::wait(SubmitKey key) {
	if (fence->GetCompletedValue() < static_cast<uint64_t>(key)) {
		HRESULT hr = fence->SetEventOnCompletion(static_cast<uint64_t>(key), fenceEvent);
		assert(SUCCEEDED(hr) && "Failed to set event on fence completion.");
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

std::unique_ptr<NRICommandQueue> DX12NRI::createCommandQueue() {
	return std::make_unique<DX12NRICommandQueue>(device.Get());
}

std::unique_ptr<NRICommandBuffer> DX12NRI::createCommandBuffer(const NRICommandPool &commandPool) {
	DX12NRICommandPool pool = static_cast<const DX12NRICommandPool &>(commandPool);
	return std::make_unique<DX12NRICommandBuffer>(*pool, device.Get());
}

DX12NRICommandBuffer::DX12NRICommandBuffer(ID3D12CommandAllocator *cmdAlloc, ID3D12Device *device)
	: commandAllocator(cmdAlloc) {
	HRESULT hr =
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, nullptr, IID_PPV_ARGS(&commandList));
	commandList->Close();
	assert(SUCCEEDED(hr) && "Failed to create command list.");
}

DX12NRIProgramBuilder::DX12NRIProgramBuilder(DX12NRI &nri) : nri(nri) {}

std::vector<ComPtr<ID3DBlob>> DX12NRIProgramBuilder::compileShaderModules(
	const std::vector<NRI::ShaderCreateInfo> &shaderCreateInfos) {
	IDxcCompiler3 *compiler = nullptr;
	HRESULT		   hr		= DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	assert(SUCCEEDED(hr) && "Failed to create DX Compiler.");

	IDxcUtils *utils;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
	assert(SUCCEEDED(hr) && "Failed to create DX Utils.");

	std::vector<ComPtr<ID3DBlob>> shaderBlobs;
	for (const auto &shaderCreateInfo : shaderCreateInfos) {
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		std::vector<uint8_t> sourceCode;
		IDxcBlobEncoding	*sourceBlob;
		std::wstring wSourceFile = std::wstring(shaderCreateInfo.sourceFile.begin(), shaderCreateInfo.sourceFile.end());
		HRESULT		 hr			 = utils->LoadFile(wSourceFile.c_str(), nullptr, &sourceBlob);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to load shader source file: " + std::string(shaderCreateInfo.sourceFile));
		}

		std::vector<LPCWSTR> arguments;
		arguments.push_back(L"-E");
		std::wstring entryPoint = std::wstring(shaderCreateInfo.entryPoint.begin(), shaderCreateInfo.entryPoint.end());
		arguments.push_back(entryPoint.c_str());
		arguments.push_back(L"-D");
		arguments.push_back(L"DX12");
		arguments.push_back(L"-T");

		switch (shaderCreateInfo.shaderType) {
			case NRI::ShaderType::SHADER_TYPE_VERTEX: arguments.push_back(L"vs_6_0"); break;
			case NRI::ShaderType::SHADER_TYPE_FRAGMENT: arguments.push_back(L"ps_6_0"); break;
			default: throw std::runtime_error("Unsupported shader stage!");
		}

		DxcBuffer buffer{};
		buffer.Ptr		= sourceBlob->GetBufferPointer();
		buffer.Size		= sourceBlob->GetBufferSize();
		buffer.Encoding = 0;

		std::cout << "Compiling shader: " << shaderCreateInfo.sourceFile << std::endl;

		IDxcResult *result;
		hr = compiler->Compile(&buffer, arguments.data(), static_cast<UINT32>(arguments.size()), nullptr,
							   IID_PPV_ARGS(&result));
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to compile shader: " + std::string(shaderCreateInfo.sourceFile));
		}

		IDxcBlobEncoding *errorBuffer;
		result->GetErrorBuffer(&errorBuffer);
		if (errorBuffer != nullptr && errorBuffer->GetBufferSize() > 0) {
			std::string errorMessage(reinterpret_cast<const char *>(errorBuffer->GetBufferPointer()),
									 errorBuffer->GetBufferSize());
			std::cerr << "Shader compilation warnings/errors: " << errorMessage << std::endl;
		}

		if (FAILED(hr)) {
			if (errorBlob) {
				std::cerr << "Shader compilation error: " << static_cast<const char *>(errorBlob->GetBufferPointer())
						  << std::endl;
			}
			assert(false && "Failed to compile shader.");
		}
		hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
		assert(SUCCEEDED(hr) && "Failed to get compiled shader blob.");

		shaderBlobs.push_back(shaderBlob);
	}
	return shaderBlobs;
}

D3D12_INPUT_CLASSIFICATION nriVertexInputRate2d3d12InputClass[] = {
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
};

D3D12_PRIMITIVE_TOPOLOGY_TYPE nriPrimitiveTopology2d3d12TopologyType[] = {
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,	 // TRIANGLE_STRIP
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,	 // LINE_STRIP
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
};

std::unique_ptr<NRIGraphicsProgram> DX12NRIProgramBuilder::buildGraphicsProgram() {
	auto shaderBlobs = compileShaderModules(shaderStagesInfo);

	std::vector<D3D12_ROOT_PARAMETER1> pushConstantRanges;
	for (const auto &pushConstantRange : this->pushConstantRanges) {
		if (pushConstantRange.size % 4 != 0 || pushConstantRange.offset % 4 != 0) {
			dbLog(dbg::LOG_WARNING,
				  "Push constant range size and offset must be multiple of 4 bytes for DX12.\n"
				  "The range size and offset will be rounded up to the nearest multiple of 4.")
		}
		D3D12_ROOT_CONSTANTS rootConstants = {};
		rootConstants.Num32BitValues	   = pushConstantRange.size / 4 + (pushConstantRange.size % 4 != 0);
		rootConstants.RegisterSpace		   = 0;
		rootConstants.ShaderRegister	   = pushConstantRange.offset / 4 + (pushConstantRange.offset % 4 != 0);
		dbLog(dbg::LOG_DEBUG, "Push Constant Range - Offset (in 32-bit values): ", rootConstants.ShaderRegister,
			  ", Size (in 32-bit values): ", rootConstants.Num32BitValues);
		dbLog(dbg::LOG_DEBUG, "NRI Push Constant Range - Offset (in bytes): ", pushConstantRange.offset,
			  ", Size (in bytes): ", pushConstantRange.size);
		D3D12_ROOT_PARAMETER1 rootParam = {};
		rootParam.ParameterType			= D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		rootParam.Constants				= rootConstants;
		rootParam.ShaderVisibility		= D3D12_SHADER_VISIBILITY_ALL;
		pushConstantRanges.push_back(rootParam);
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};

	D3D12_ROOT_SIGNATURE_DESC1 rootSignatureDesc{};
	rootSignatureDesc.Flags			= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.NumParameters = pushConstantRanges.size();
	rootSignatureDesc.pParameters	= pushConstantRanges.data();
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc{};
	versionedRootSignatureDesc.Version	= D3D_ROOT_SIGNATURE_VERSION_1_1;
	versionedRootSignatureDesc.Desc_1_1 = rootSignatureDesc;

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	ComPtr<ID3D12RootSignature> rootSignature;
	HRESULT hr = D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, &rootSignatureBlob, &errorBlob);
	assert(SUCCEEDED(hr) && "Failed to serialize root signature.");
	hr = nri.getDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
											  rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr) && "Failed to create root signature.");

	psoDesc.pRootSignature					  = rootSignature.Get();
	psoDesc.BlendState.RenderTarget[0]		  = D3D12_RENDER_TARGET_BLEND_DESC{FALSE,
																		   FALSE,
																		   D3D12_BLEND_ONE,
																		   D3D12_BLEND_ZERO,
																		   D3D12_BLEND_OP_ADD,
																		   D3D12_BLEND_ONE,
																		   D3D12_BLEND_ZERO,
																		   D3D12_BLEND_OP_ADD,
																		   D3D12_LOGIC_OP_NOOP,
																		   D3D12_COLOR_WRITE_ENABLE_ALL};
	psoDesc.BlendState.AlphaToCoverageEnable  = FALSE;
	psoDesc.BlendState.IndependentBlendEnable = FALSE;

	psoDesc.SampleMask		= UINT_MAX;
	psoDesc.RasterizerState = D3D12_RASTERIZER_DESC{D3D12_FILL_MODE_SOLID,
													D3D12_CULL_MODE_NONE,
													FALSE,
													D3D12_DEFAULT_DEPTH_BIAS,
													D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
													D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
													FALSE,
													FALSE,
													FALSE,
													0};
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements{};
	for (auto &binding : this->vertexBindings) {
		for (auto &attribute : binding.attributes) {
			D3D12_INPUT_ELEMENT_DESC elementDesc = {};
			elementDesc.SemanticName			 = attribute.name.c_str();
			elementDesc.SemanticIndex			 = 0;
			elementDesc.AlignedByteOffset		 = attribute.offset;
			elementDesc.Format					 = nriFormat2d3d12Format[attribute.format];
			elementDesc.InputSlot				 = binding.binding;
			elementDesc.AlignedByteOffset		 = static_cast<UINT>(attribute.offset);
			elementDesc.InputSlotClass			 = nriVertexInputRate2d3d12InputClass[binding.inputRate];
			elementDesc.InstanceDataStepRate	 = 0;
			inputElements.push_back(elementDesc);
		}
	}

	psoDesc.InputLayout = {inputElements.data(), static_cast<UINT>(inputElements.size())};

	psoDesc.DepthStencilState	  = D3D12_DEPTH_STENCIL_DESC{TRUE,
														 D3D12_DEPTH_WRITE_MASK_ALL,
														 D3D12_COMPARISON_FUNC_LESS,
														 FALSE,
														 D3D12_DEFAULT_STENCIL_READ_MASK,
														 D3D12_DEFAULT_STENCIL_WRITE_MASK,
														 D3D12_STENCIL_OP_KEEP,
														 D3D12_STENCIL_OP_KEEP,
														 D3D12_STENCIL_OP_KEEP,
														 D3D12_COMPARISON_FUNC_ALWAYS,
														 D3D12_STENCIL_OP_KEEP,
														 D3D12_STENCIL_OP_KEEP,
														 D3D12_STENCIL_OP_KEEP,
														 D3D12_COMPARISON_FUNC_ALWAYS};
	psoDesc.PrimitiveTopologyType = nriPrimitiveTopology2d3d12TopologyType[this->primitiveType];
	assert(psoDesc.PrimitiveTopologyType != D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED &&
		   "Unsupported primitive topology type.");
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0]	 = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat		 = DXGI_FORMAT_D32_FLOAT;

	int i = 0;
	for (const auto &shaderBlob : shaderBlobs) {
		const auto &shaderStageInfo = shaderStagesInfo[i++];
		switch (shaderStageInfo.shaderType) {
			case NRI::ShaderType::SHADER_TYPE_VERTEX:
				psoDesc.VS = {shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize()};
				dbLog(dbg::LOG_DEBUG, "Vertex shader size: ", shaderBlob->GetBufferSize(), " bytes");
				break;
			case NRI::ShaderType::SHADER_TYPE_FRAGMENT:
				psoDesc.PS = {shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize()};
				dbLog(dbg::LOG_DEBUG, "Pixel shader size: ", shaderBlob->GetBufferSize(), " bytes");
				break;
			default: throw std::runtime_error("Unsupported shader stage!");
		}
	}

	psoDesc.NumRenderTargets = 1;

	ComPtr<ID3D12PipelineState> pipelineState;
	hr = nri.getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(hr) && "Failed to create graphics pipeline state.");

	return std::make_unique<DX12NRIGraphicsProgram>(std::move(pipelineState), std::move(rootSignature));
}

std::unique_ptr<NRIComputeProgram> DX12NRIProgramBuilder::buildComputeProgram() {
	auto							  shaderBlobs = compileShaderModules(std::move(shaderStagesInfo));
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
	assert(shaderBlobs.size() == 1 && "Compute program must have exactly one shader stage!");
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	ComPtr<ID3DBlob>			rootSignatureBlob;
	ComPtr<ID3DBlob>			errorBlob;
	ComPtr<ID3D12RootSignature> rootSignature;
	HRESULT						hr =
		D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignatureBlob, &errorBlob);
	assert(SUCCEEDED(hr) && "Failed to serialize root signature.");
	hr = nri.getDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
											  rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr) && "Failed to create root signature.");
	psoDesc.pRootSignature = rootSignature.Get();
	ComPtr<ID3D12PipelineState> pipelineState;
	hr = nri.getDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(hr) && "Failed to create compute pipeline state.");
	return std::make_unique<DX12NRIComputeProgram>(std::move(pipelineState), std::move(rootSignature));
}

void DX12NRIProgram::bind(NRICommandBuffer &commandBuffer) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	assert(pipelineState && "Pipeline state is not created.");
	assert(rootSignature && "Root signature is not created.");
	dxCmdBuffer.begin();
	dxCmdBuffer->SetPipelineState(pipelineState.Get());
	dxCmdBuffer->SetGraphicsRootSignature(rootSignature.Get());
	dxCmdBuffer->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DX12NRIProgram::unbind(NRICommandBuffer &commandBuffer) {
	// Nothing to do here for now
}

void DX12NRIProgram::setPushConstants(NRICommandBuffer &commandBuffer, const void *data, std::size_t size,
									  std::size_t offset) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	dxCmdBuffer.begin();
	dxCmdBuffer->SetGraphicsRoot32BitConstants(0, static_cast<UINT>(size / 4), data, static_cast<UINT>(offset / 4));
}

void DX12NRIGraphicsProgram::draw(NRICommandBuffer &commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
								  uint32_t firstVertex, uint32_t firstInstance) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	dxCmdBuffer.begin();
	dxCmdBuffer->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}

void DX12NRIGraphicsProgram::drawIndexed(NRICommandBuffer &commandBuffer, uint32_t indexCount, uint32_t instanceCount,
										 uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	dxCmdBuffer.begin();
	dxCmdBuffer->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void DX12NRIComputeProgram::dispatch(NRICommandBuffer &commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
									 uint32_t groupCountZ) {
	DX12NRICommandBuffer &dxCmdBuffer = static_cast<DX12NRICommandBuffer &>(commandBuffer);
	dxCmdBuffer.begin();
	dxCmdBuffer->Dispatch(groupCountX, groupCountY, groupCountZ);
}

std::unique_ptr<NRICommandPool> DX12NRI::createCommandPool() {
	return std::make_unique<DX12NRICommandPool>(device.Get());
}

DX12NRICommandPool::DX12NRICommandPool(ID3D12Device *device) {
	HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr) && "Failed to create command allocator.");
}

NRIQWindow *DX12NRI::createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) {
	auto *window = new DX12NRIQWindow(*this, app, std::move(renderer));
	window->getRenderer()->initialize(*window);
	window->startFrameTimer();
	return window;
}

void DX12NRIQWindow::resizeEvent(QResizeEvent *event) {
	NRIQWindow::resizeEvent(event);

	HWND hwnd = reinterpret_cast<HWND>(winId());
	RECT rect;
	GetWindowRect(hwnd, &rect);
	dbLog(dbg::LOG_INFO, "Resizing swap chain to: ", rect.right - rect.left, "x", rect.bottom - rect.top);

	uint32_t width	= rect.right - rect.left;
	uint32_t height = rect.bottom - rect.top;

	if (!swapChain) return;

	const UINT bufferCount = static_cast<UINT>(swapchainImages.size());
	swapchainImages.clear();
	swapChain->ResizeBuffers(bufferCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	depthImage.emplace(static_cast<DX12NRI &>(nri), width, height, NRI::FORMAT_D32_SFLOAT,
					   NRI::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT);
	depthImageMemory = DX12NRIAllocation(static_cast<DX12NRI &>(nri), depthImage->getMemoryRequirements());
	depthImage->bindMemory(*depthImageMemory, 0);

	// get swap chain images
	for (UINT i = 0; i < bufferCount; i++) {
		ComPtr<ID3D12Resource> backBuffer;
		HRESULT				   hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
		if (FAILED(hr)) {
			std::cerr << "failed to get swapchain image " << i << std::endl;
			return;
		}

		NRI::ImageUsage usage =
			NRI::IMAGE_USAGE_COLOR_ATTACHMENT | NRI::IMAGE_USAGE_TRANSFER_SRC | NRI::IMAGE_USAGE_TRANSFER_DST;

		swapchainImages.push_back(DX12NRIImage2D(backBuffer.Get(), width, height, usage, (DX12NRI &)nri));
	}
}

DX12NRIQWindow::DX12NRIQWindow(DX12NRI &nri, QApplication &app, std::unique_ptr<Renderer> &&renderer)
	: NRIQWindow((NRI &)nri, std::move(renderer)),
	  swapChain(nullptr),
	  presentQueue(),
	  frameFence(nullptr),
	  frameFenceEvent(nullptr),
	  frameFenceValue(0),
	  commandBuffer(),
	  depthImage(std::nullopt) {
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

	// get swap chain images
	const UINT bufferCount = swapChainDesc.BufferCount;

	for (UINT i = 0; i < bufferCount; i++) {
		ComPtr<ID3D12Resource> backBuffer;
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
		assert(SUCCEEDED(hr) && "Failed to get back buffer.");

		NRI::ImageUsage usage =
			NRI::IMAGE_USAGE_COLOR_ATTACHMENT | NRI::IMAGE_USAGE_TRANSFER_SRC | NRI::IMAGE_USAGE_TRANSFER_DST;

		swapchainImages.push_back(
			DX12NRIImage2D(backBuffer.Get(), swapChainDesc.Width, swapChainDesc.Height, usage, nri));
	}
	depthImage.emplace(nri, swapChainDesc.Width, swapChainDesc.Height, NRI::FORMAT_D32_SFLOAT,
					   NRI::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT);
	depthImageMemory = DX12NRIAllocation(nri, depthImage->getMemoryRequirements());
	depthImage->bindMemory(*depthImageMemory, 0);
}

void DX12NRIQWindow::drawFrame() {
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	if (swapchainImages.empty()) return;
	auto &backBuffer = swapchainImages[backBufferIndex];
	commandBuffer.getCommandAllocator()->Reset();

	renderer->render(swapchainImages[backBufferIndex], commandBuffer);

	// Execute command buffer
	presentQueue.submit(commandBuffer);
	// Present
	HRESULT hr = swapChain->Present(1, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
		std::cerr << "Device lost. Need to recreate swap chain and all resources." << std::endl;
		exit(-1);
	}
	assert(SUCCEEDED(hr) && "Failed to present swap chain.");
	// Wait for GPU to finish
	frameFenceValue++;
	hr = (*presentQueue)->Signal(frameFence.Get(), frameFenceValue);
	assert(SUCCEEDED(hr) && "Failed to signal fence.");
	if (frameFence->GetCompletedValue() < frameFenceValue) {
		hr = frameFence->SetEventOnCompletion(frameFenceValue, frameFenceEvent);
		assert(SUCCEEDED(hr) && "Failed to set event on fence completion.");
		WaitForSingleObject(frameFenceEvent, INFINITE);
	}
}

DX12NRICommandQueue &DX12NRIQWindow::getMainQueue() { return presentQueue; }

void DX12NRIQWindow::beginRendering(NRICommandBuffer &cmdBuf, NRIImage2D &renderTarget) {
	DX12NRIImage2D		 &dxRenderTarget = static_cast<DX12NRIImage2D &>(renderTarget);
	DX12NRICommandBuffer *commandBuffer	 = static_cast<DX12NRICommandBuffer *>(&cmdBuf);
	commandBuffer->begin();
	dxRenderTarget.transitionState(*commandBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
	depthImage->transitionState(*commandBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2] = {dxRenderTarget.getViewHandle(), depthImage->getViewHandle()};
	(**commandBuffer)->OMSetRenderTargets(1, &rtvHandles[0], FALSE, &rtvHandles[1]);
	std::array<FLOAT, 4> clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	(**commandBuffer)->ClearRenderTargetView(rtvHandles[0], clearColor.data(), 0, nullptr);
	(**commandBuffer)->ClearDepthStencilView(rtvHandles[1], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	D3D12_VIEWPORT viewport = {
		0.0f, 0.0f, static_cast<FLOAT>(dxRenderTarget.getWidth()), static_cast<FLOAT>(dxRenderTarget.getHeight()),
		0.0f, 1.0f};
	D3D12_RECT scissorRect = {0, 0, static_cast<LONG>(dxRenderTarget.getWidth()),
							  static_cast<LONG>(dxRenderTarget.getHeight())};

	(**commandBuffer)->RSSetViewports(1, &viewport);
	(**commandBuffer)->RSSetScissorRects(1, &scissorRect);
}

void DX12NRIQWindow::endRendering(NRICommandBuffer &cmdBuf) {
	// Nothing to do here for now
}

std::unique_ptr<NRIProgramBuilder> DX12NRI::createProgramBuilder() {
	return std::make_unique<DX12NRIProgramBuilder>(*this);
}

int __reg_dx12_nri = []() {
	NRIFactory::getInstance().registerNRI("DirectX 12", []() -> NRI * { return new DX12NRI(); });
	return 0;
}();

#endif
