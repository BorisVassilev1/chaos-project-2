#pragma once

#include <map>
#include <functional>

#include "nri.hpp"

class NRIFactory {
	using NRICreateFunction = std::function<std::unique_ptr<NRI>()>;

	std::map<std::string, NRICreateFunction> nriInfos;

   public:
	NRIFactory() = default;

	static NRIFactory &getInstance();

	void registerNRI(const char *name, NRICreateFunction createFunction);
	std::unique_ptr<NRI> createNRI(const std::string &name) const;
	std::vector<std::string> getAvailableNRIs() const;
	std::string selectNRIGUI() const;
};
