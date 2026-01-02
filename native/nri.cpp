#include "nri.hpp"

NRIProgramBuilder &NRIProgramBuilder::addShaderModule(const NRI::ShaderCreateInfo &shaderInfo) {
	shaderStagesInfo.push_back(shaderInfo);
	return *this;
}
NRIProgramBuilder &NRIProgramBuilder::setVertexBindings(const std::vector<NRI::VertexBinding> &bindings) {
	vertexBindings = bindings;
	return *this;
}
NRIProgramBuilder &NRIProgramBuilder::setPrimitiveType(NRI::PrimitiveType primitiveType) {
	this->primitiveType = primitiveType;
	return *this;
}
NRIProgramBuilder &NRIProgramBuilder::setPushConstantRanges(const std::vector<NRI::PushConstantRange> &ranges) {
	pushConstantRanges = ranges;
	return *this;
}

NRIQWindow::NRIQWindow(NRI &nri, std::unique_ptr<Renderer> &&rendererPtr) : renderer(std::move(rendererPtr)), nri(nri) {
	using namespace std::chrono_literals;
	timer = Timer([this]() {
		std::cout << "Drawing frame..." << std::endl;
		drawFrame();
		std::cout << "finished drawing frame" << std::endl;
		
		auto elapsed = std::chrono::steady_clock::now() - lastFrameTime;
		lastFrameTime = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(elapsed).count();
		_deltaTime = deltaTime;
		std::cout << "Frame Time: " << (deltaTime * 1000.0f) << " ms" << std::endl;

		for (const auto &cb : frameCallbacks)
			cb();

	}, 0ms);
}

void NRIQWindow::startFrameTimer() { 
	std::cout << "starting timer" << std::endl;
	timer.start(); 
}

void NRIQWindow::addFrameCallback(const frameCallback &cb) { frameCallbacks.push_back(cb); }
void NRIQWindow::addResizeCallback(const resizeCallback &cb) { resizeCallbacks.push_back(cb); }
void NRIQWindow::addKeyCallback(const keyCallback &cb) { keyCallbacks.push_back(cb); }
void NRIQWindow::addMouseCallback(const mouseCallback &cb) { mouseCallbacks.push_back(cb); }

void NRIQWindow::closeEvent(QCloseEvent *event) {
	QWindow::closeEvent(event);
	std::cout << "Closing NRIQWindow, stopping timer..." << std::endl;
	timer.stop();
}

void NRIQWindow::resizeEvent(QResizeEvent *event) {
	QWindow::resizeEvent(event);
	for (const auto &cb : resizeCallbacks)
		cb(event);
}

void NRIQWindow::keyPressEvent(QKeyEvent *event) {
	QWindow::keyPressEvent(event);
	for (const auto &cb : keyCallbacks)
		cb(event);
}

void NRIQWindow::keyReleaseEvent(QKeyEvent *event) {
	QWindow::keyReleaseEvent(event);
	for (const auto &cb : keyCallbacks)
		cb(event);
}

void NRIQWindow::mousePressEvent(QMouseEvent *event) {
	QWindow::mousePressEvent(event);
	for (const auto &cb : mouseCallbacks)
		cb(event);
}

void NRIQWindow::mouseReleaseEvent(QMouseEvent *event) {
	QWindow::mouseReleaseEvent(event);
	for (const auto &cb : mouseCallbacks)
		cb(event);
}

void NRIQWindow::mouseMoveEvent(QMouseEvent *event) {
	QWindow::mouseMoveEvent(event);
	for (const auto &cb : mouseCallbacks)
		cb(event);
}
