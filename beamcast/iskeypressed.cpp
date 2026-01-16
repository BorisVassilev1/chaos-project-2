#include "iskeypressed.hpp"

namespace beamcast {
bool IsKeyPressed::isKeyPressed(int keyCode) const {
	// if (keyCode < 0 || keyCode >= 256) { return false; }
	switch (keyCode) {
		case Qt::Key_Shift: return app.queryKeyboardModifiers() & Qt::ShiftModifier;
		case Qt::Key_Control: return app.queryKeyboardModifiers() & Qt::ControlModifier;
		case Qt::Key_Alt: return app.queryKeyboardModifiers() & Qt::AltModifier;
		case Qt::Key_Meta: return app.queryKeyboardModifiers() & Qt::MetaModifier;
		default:
			if (keyCode < 0 || keyCode >= 256) {
				dbLog(dbg::LOG_WARNING, "isKeyPressed: keyCode out of range: ", keyCode);
				return false;
			}
			return keyStates[keyCode];
	}
}

bool IsKeyPressed::keyEvent(QKeyEvent *event) {
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
}	  // namespace beamcast
