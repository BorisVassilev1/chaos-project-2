#include <qguiapplication.h>
#include <iostream>
#include <thread>
#include "src/vk_nri.hpp"

#include <QtWidgets>
#include <qguiapplication_platform.h>
#include <qwindowdefs.h>

int main(int argc, char *argv[]) {
	VulkanNRI nri;
	std::cout << "Vulkan NRI initialized successfully." << std::endl;

	QApplication app(argc, argv);

	auto window = nri.createQWidgetSurface(app);
	QTimer t {};

	window->connect(&t, &QTimer::timeout, [window]() { window->drawFrame(); });
	t.start(16);
	
	QWidget *viewport = QWidget::createWindowContainer(window);

	auto *mainWindow = new QWidget();
	mainWindow->setWindowTitle("BeamCast GUI");

	// Layout: Vulkan container and a Quit button side by side
	auto *layout = new QHBoxLayout(mainWindow);
	layout->setContentsMargins(8, 8, 8, 8);
	layout->setSpacing(8);
	viewport->setMinimumSize(640, 480);
	viewport->setFocusPolicy(Qt::StrongFocus);
	viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	layout->addWidget(viewport, /*stretch*/ 1);
	auto *quitButton = new QPushButton("Quit", mainWindow);
	quitButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	layout->addWidget(quitButton);
	QObject::connect(quitButton, &QPushButton::clicked, &app, &QApplication::quit);
	mainWindow->resize(1024, 600);
	mainWindow->show();

	app.exec();

	nri.synchronize();
	delete viewport;

	return 0;
}
