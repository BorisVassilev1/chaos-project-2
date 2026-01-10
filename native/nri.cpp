#include "nri.hpp"

NRIResourceHandle::NRIResourceHandle(ResourceType type, bool writable, uint32_t index)
	: handle((type << 30) | (static_cast<uint32_t>(writable) << 29) | (index & 0x1FFFFFFF)) {}

NRIResourceHandle NRIResourceHandle::INVALID_HANDLE = NRIResourceHandle(0xFFFFFFFF);

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
	connect(&timer, &QTimer::timeout, [this]() {
		drawFrame();

		auto elapsed	= std::chrono::steady_clock::now() - lastFrameTime;
		lastFrameTime	= std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(elapsed).count();
		_deltaTime		= deltaTime;
		++framesSinceLastPrint;

		if (lastFrameTime - lastPrintTime >= 1s) {
			lastPrintTime = lastFrameTime;
			dbLogR(dbg::LOG_INFO, "FPS: ", framesSinceLastPrint, "Last Delta Time: ", (deltaTime * 1000.0f), " ms");
			framesSinceLastPrint = 0;
		}

		for (const auto &cb : frameCallbacks)
			cb();
	});
}

void NRIQWindow::startFrameTimer() {
	dbLog(dbg::LOG_INFO, "starting timer");
	timer.start();
}

void NRIQWindow::addFrameCallback(const frameCallback &cb) { frameCallbacks.push_back(cb); }
void NRIQWindow::addResizeCallback(const resizeCallback &cb) { resizeCallbacks.push_back(cb); }
void NRIQWindow::addKeyCallback(const keyCallback &cb) { keyCallbacks.push_back(cb); }
void NRIQWindow::addMouseCallback(const mouseCallback &cb) { mouseCallbacks.push_back(cb); }

void NRIQWindow::closeEvent(QCloseEvent *event) {
	QWindow::closeEvent(event);
	dbLog(dbg::LOG_INFO, "Closing NRIQWindow, stopping timer...");
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
