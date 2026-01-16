#pragma once

#include <QApplication>
#include <QKeyEvent>
#include <iostream>

#include "utils.hpp"

namespace beamcast {
class IsKeyPressed {
	bool keyStates[256] = {false};

	QApplication &app;

   public:
	IsKeyPressed(QApplication &app) : app(app) {}

	bool isKeyPressed(int keyCode) const;
	bool keyEvent(QKeyEvent *event);
};
}	  // namespace beamcast
