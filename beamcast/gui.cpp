#include "gui.hpp"

#include "../native/nriFactory.hpp"
#include "renderer.hpp"

NRI *BeamcastApplication::chooseNRI() {
	auto	   &factory		   = NRIFactory::getInstance();
	std::string selectedString = factory.selectNRIGUI();

	if (!selectedString.empty()) {
		return factory.createNRI(selectedString);
	} else throw std::runtime_error("No NRI selected!");
}

BeamcastApplication::BeamcastApplication(int &argc, char **argv) : QApplication(argc, argv), nri(chooseNRI()) {
	window	 = nri->createQWidgetSurface(*this, std::make_unique<BeamcastRenderer>(*nri, *this));
	viewport = QWidget::createWindowContainer(window);
	viewport->setMinimumSize(640, 480);
	viewport->setFocusPolicy(Qt::StrongFocus);
	viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QWidget *mainWindow = new QWidget();
	mainWindow->setWindowTitle("BeamCast GUI");
	// Layout: Vulkan viewport with Quit button and FPS label on the right
	auto *layout = new QGridLayout(mainWindow);
	layout->setContentsMargins(8, 8, 8, 8);
	layout->setSpacing(8);
	layout->addWidget(viewport, 0, 0, 1, 10);

	QPushButton *quitButton = new QPushButton("Quit", mainWindow);
	quitButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	layout->addWidget(quitButton, 1, 0);
	QObject::connect(quitButton, &QPushButton::clicked, this, &BeamcastApplication::quit);
	mainWindow->resize(1024, 600);

	fpsLabel = new QLabel("FPS: 0", mainWindow);
	fpsLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	layout->addWidget(fpsLabel, 1, 1, Qt::AlignLeft);

	mainWindow->show();
	window->addFrameCallback([this]() { this->frameCallback(); });
	framesLastSecondTimer.start();
}

BeamcastApplication::~BeamcastApplication() {
	window->destroy();
	delete window;
	delete viewport;
}

void BeamcastApplication::frameCallback() {
	auto elapsed = framesLastSecondTimer.elapsed();
	if (elapsed >= 1000) {
		fpsLabel->setText(QString("FPS: %1").arg(frameCountLastSecond));
		frameCountLastSecond = 0;
		framesLastSecondTimer.restart();
	}
	++frameCountLastSecond;
}
