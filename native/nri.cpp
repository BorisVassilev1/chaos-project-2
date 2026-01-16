#include "nri.hpp"

namespace nri {

ResourceHandle::ResourceHandle(ResourceType type, bool writable, uint32_t index)
	: handle((type << 30) | (static_cast<uint32_t>(writable) << 29) | (index & 0x1FFFFFFF)) {}

ResourceHandle ResourceHandle::INVALID_HANDLE = ResourceHandle(0xFFFFFFFF);

ProgramBuilder &ProgramBuilder::addShaderModule(const ShaderCreateInfo &shaderInfo) {
	shaderStagesInfo.push_back(shaderInfo);
	return *this;
}
ProgramBuilder &ProgramBuilder::setVertexBindings(const std::vector<VertexBinding> &bindings) {
	vertexBindings = bindings;
	return *this;
}
ProgramBuilder &ProgramBuilder::setPrimitiveType(PrimitiveType primitiveType) {
	this->primitiveType = primitiveType;
	return *this;
}
ProgramBuilder &ProgramBuilder::setPushConstantRanges(const std::vector<PushConstantRange> &ranges) {
	pushConstantRanges = ranges;
	return *this;
}

QWindow::QWindow(NRI &nri, std::unique_ptr<Renderer> &&rendererPtr) : renderer(std::move(rendererPtr)), nri(nri) {
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

void QWindow::startFrameTimer() {
	dbLog(dbg::LOG_INFO, "starting timer");
	timer.start();
}

void QWindow::addFrameCallback(const frameCallback &cb) { frameCallbacks.push_back(cb); }
void QWindow::addResizeCallback(const resizeCallback &cb) { resizeCallbacks.push_back(cb); }
void QWindow::addKeyCallback(const keyCallback &cb) { keyCallbacks.push_back(cb); }
void QWindow::addMouseCallback(const mouseCallback &cb) { mouseCallbacks.push_back(cb); }

void QWindow::closeEvent(::QCloseEvent *event) {
	::QWindow::closeEvent(event);
	dbLog(dbg::LOG_INFO, "Closing QWindow, stopping timer...");
	timer.stop();
}

void QWindow::resizeEvent(::QResizeEvent *event) {
	::QWindow::resizeEvent(event);
	for (const auto &cb : resizeCallbacks)
		cb(event);
}

void QWindow::keyPressEvent(::QKeyEvent *event) {
	::QWindow::keyPressEvent(event);
	for (const auto &cb : keyCallbacks)
		cb(event);
}

void QWindow::keyReleaseEvent(::QKeyEvent *event) {
	::QWindow::keyReleaseEvent(event);
	for (const auto &cb : keyCallbacks)
		cb(event);
}

void QWindow::mousePressEvent(::QMouseEvent *event) {
	::QWindow::mousePressEvent(event);
	for (const auto &cb : mouseCallbacks)
		cb(event);
}

void QWindow::mouseReleaseEvent(::QMouseEvent *event) {
	::QWindow::mouseReleaseEvent(event);
	for (const auto &cb : mouseCallbacks)
		cb(event);
}

void QWindow::mouseMoveEvent(::QMouseEvent *event) {
	::QWindow::mouseMoveEvent(event);
	for (const auto &cb : mouseCallbacks)
		cb(event);
}

ResourceHandle ImageView::getHandle() {
	if (handle == ResourceHandle::INVALID_HANDLE) {
		handle = createHandle();
		if (handle == ResourceHandle::INVALID_HANDLE) {
			dbLog(dbg::LOG_ERROR, "Failed to create ImageView handle! This is not fatal, but usage may fail.");
		}
	}
	return handle;
}
}	  // namespace nri
