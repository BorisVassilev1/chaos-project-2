#pragma once

#include <map>
#include <functional>

#include "nri.hpp"

class NRIFactory {
	using NRICreateFunction = NRI *(*)();

	std::map<std::string, NRICreateFunction> nriInfos;

   public:
	NRIFactory() = default;

	static NRIFactory &getInstance() {
		static NRIFactory instance;
		return instance;
	}

	void registerNRI(const char *name, NRICreateFunction createFunction) { nriInfos.emplace(name, createFunction); }

	NRI *createNRI(const std::string &name) const {
		auto it = nriInfos.find(name);
		if (it != nriInfos.end()) {
			return it->second();
		} else throw std::runtime_error("Native Rendering Interface not found: " + name);
		return nullptr;
	}

	std::vector<std::string> getAvailableNRIs() const {
		std::vector<std::string> names;
		for (const auto &pair : nriInfos) {
			names.push_back(pair.first);
		}
		return names;
	}

	std::string selectNRIGUI() const {
		std::vector<std::string> availableNRIs = getAvailableNRIs();
		QDialog					 dialog{};
		dialog.setWindowTitle("Select an Option");

		QVBoxLayout *layout = new QVBoxLayout(&dialog);

		std::string selectedString;

		// Create a button for each string
		for (const std::string &option : availableNRIs) {
			QPushButton *button = new QPushButton(QString::fromStdString(option), &dialog);
			layout->addWidget(button);

			// Connect button click to capture the string and close dialog
			QObject::connect(button, &QPushButton::clicked, [&dialog, &selectedString, option]() {
				selectedString = option;
				dialog.accept();
			});
		}

		// Execute dialog and wait for user action
		dialog.exec();

		return selectedString;
	}
};
