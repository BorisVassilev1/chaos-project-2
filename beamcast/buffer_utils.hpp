#pragma once

#include <memory>
#include <vector>
#include "../native/nri.hpp"

std::tuple<std::vector<std::size_t>, NRI::MemoryRequirements> getBufferOffsets(const std::vector<NRIBuffer *> &buffers);


std::unique_ptr<NRIAllocation> allocateBindMemory(NRI &nri, const std::vector<NRIBuffer *> &buffers,
												  NRI::MemoryTypeRequest memoryTypeRequest);
