#pragma once

#include "../native/nri.hpp"

namespace beamcast {

std::pair<std::unique_ptr<nri::Image2D>, std::unique_ptr<nri::Allocation>> createImage2D(
	std::string_view fileName, nri::NRI &nri, nri::CommandQueue &queue,
	nri::ImageUsage usage = nri::ImageUsage::IMAGE_USAGE_SAMPLED);
}	  // namespace beamcast
