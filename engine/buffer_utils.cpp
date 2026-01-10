#include "buffer_utils.hpp"


std::tuple<std::vector<std::size_t>, NRI::MemoryRequirements> getBufferOffsets(const std::vector<NRIBuffer *> &buffers) {
	std::vector<NRI::MemoryRequirements> memReqs;
	for (auto buffer : buffers) {
		memReqs.push_back(buffer->getMemoryRequirements());
	}

	std::size_t				 totalSize = 0;
	std::vector<std::size_t> offsets;
	for (const auto &req : memReqs) {
		if (totalSize % req.alignment != 0) { totalSize += req.alignment - (totalSize % req.alignment); }
		offsets.push_back(totalSize);

		totalSize += req.size;
	}
	return {offsets, NRI::MemoryRequirements(totalSize, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE, 0)};
}


std::unique_ptr<NRIAllocation> allocateBindMemory(NRI &nri, const std::vector<NRIBuffer *> &buffers,
												  NRI::MemoryTypeRequest memoryTypeRequest) {
	auto [offsets, memReq] = getBufferOffsets(buffers);

	auto allocation = nri.allocateMemory(memReq.setTypeRequest(memoryTypeRequest));
	for (std::size_t i = 0; i < buffers.size(); i++) {
		buffers[i]->bindMemory(*allocation, offsets[i]);
	}
	return allocation;
}
