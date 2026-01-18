#pragma once

#include <memory>
#include <vector>
#include "../native/nri.hpp"

namespace beamcast {
std::tuple<std::vector<std::size_t>, nri::MemoryRequirements> getBufferOffsets(const std::vector<nri::Buffer *> &buffers);

std::unique_ptr<nri::Allocation> allocateBindMemory(nri::NRI &nri, const std::vector<nri::Buffer *> &buffers,
												  nri::MemoryTypeRequest memoryTypeRequest);
std::unique_ptr<nri::Allocation> allocateBindMemory(nri::NRI &nri, const std::vector<std::size_t> &bufferOffsets, nri::MemoryRequirements memReq,
												  const std::vector<nri::Buffer *> &buffers);
}	  // namespace beamcast
