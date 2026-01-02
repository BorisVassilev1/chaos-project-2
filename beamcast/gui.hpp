#pragma once
#include <qtmetamacros.h>
#include <iostream>

#include <QWidget>
#include <QApplication>

#include "../native/nri.hpp"

class BeamcastApplication : public QApplication {
	std::unique_ptr<NRI> nri;
	NRIQWindow			*window;
	QLabel				*fpsLabel;

	QElapsedTimer framesLastSecondTimer;
	int			  frameCountLastSecond = 0;

	QWidget *viewport;

	NRI *chooseNRI();

   public:
	BeamcastApplication(int &argc, char **argv);
	~BeamcastApplication();

	void frameCallback();
};
