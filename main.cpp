#include <iostream>
#include "src/nri.hpp"
#include "src/vk_nri.hpp"

#include <QtWidgets>

class BeamcastApplication : public QApplication {
	std::unique_ptr<NRI> nri;
	NRIQWindow			*window;
	QLabel				*fpsLabel;
	QTimer				 timer;


	QElapsedTimer framesLastSecondTimer;
	int frameCountLastSecond = 0;

	QWidget *viewport;

   public:
	BeamcastApplication(int &argc, char **argv) : QApplication(argc, argv), nri(new VulkanNRI()) {
		window	 = nri->createQWidgetSurface(*this);
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

		connect(&timer, &QTimer::timeout, [this]() { render(); });
		timer.start(0);
		framesLastSecondTimer.start();
	}

	~BeamcastApplication() {
		window->destroy();
		delete window;
		delete viewport;
	}

	void render() {
		auto elapsed = framesLastSecondTimer.elapsed();
		if (elapsed >= 1000) {
			fpsLabel->setText(QString("FPS: %1").arg(frameCountLastSecond));
			frameCountLastSecond = 0;
			framesLastSecondTimer.restart();
		}
		++frameCountLastSecond;

		window->drawFrame(); 
	}
};

int main(int argc, char *argv[]) {
	BeamcastApplication app(argc, argv);
	return app.exec();
}
