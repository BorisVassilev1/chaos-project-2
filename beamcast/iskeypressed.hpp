#pragma once

#include <QApplication>
#include <QKeyEvent>
#include <iostream>

#include "utils.hpp"

class IsKeyPressed {
	bool keyStates[256] = {false};

	QApplication &app;

   public:
	IsKeyPressed(QApplication &app) : app(app) {}

	bool isKeyPressed(int keyCode) const {
		//if (keyCode < 0 || keyCode >= 256) { return false; }
		switch (keyCode) {
			case Qt::Key_Shift:
				return app.queryKeyboardModifiers() & Qt::ShiftModifier;
			case Qt::Key_Control:
				return app.queryKeyboardModifiers() & Qt::ControlModifier;
			case Qt::Key_Alt:
				return app.queryKeyboardModifiers() & Qt::AltModifier;
			case Qt::Key_Meta:
				return app.queryKeyboardModifiers() & Qt::MetaModifier;
			default: 
				assert(keyCode >= 0 && keyCode < 256 && "Key code out of range in IsKeyPressed::isKeyPressed");
				return keyStates[keyCode];
		}
	}

	bool keyEvent(QKeyEvent *event) {
		if (!event) { return false; }
		if (event->key() < 0 || event->key() >= 256) { return false; }
		if (event->type() == QEvent::KeyPress) {
			keyStates[event->key()] = true;
			return true;
		} else if (event->type() == QEvent::KeyRelease) {
			keyStates[event->key()] = false;
			return true;
		}
		return false;
	}
};
