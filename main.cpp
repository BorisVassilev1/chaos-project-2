#include <iostream>

#include "beamcast/gui.hpp"

int main(int argc, char *argv[]) {
	beamcast::BeamcastApplication app(argc, argv);
	return app.exec();
}
