#pragma once
#include <qtmetamacros.h>
#include <iostream>

#include <QWidget>
#include <QApplication>

#include "../native/nri.hpp"

namespace beamcast {
class BeamcastApplication : public QApplication {
	std::unique_ptr<nri::NRI> nri;
	nri::QWindow			 *window;
	QLabel					 *fpsLabel;

	QElapsedTimer framesLastSecondTimer;
	int			  frameCountLastSecond = 0;

	QWidget *viewport;

	std::unique_ptr<nri::NRI> chooseNRI();

   public:
	BeamcastApplication(int &argc, char **argv);
	~BeamcastApplication();

	void frameCallback();
};
}	  // namespace beamcast
