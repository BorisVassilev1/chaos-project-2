#pragma once

#include "../native/nri.hpp"

std::pair<std::unique_ptr<NRIImage2D>, std::unique_ptr<NRIAllocation>> createImage2D(std::string_view fileName,
																					 NRI &nri, NRICommandQueue &queue,
																					 NRI::ImageUsage usage = NRI::ImageUsage::IMAGE_USAGE_SAMPLED);
