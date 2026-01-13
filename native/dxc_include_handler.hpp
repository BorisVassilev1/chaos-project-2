#pragma once
#include <filesystem>
#include <unordered_set>
#ifdef _WIN32
	#define NOMINMAX
	#include <wrl.h>
	//#include <dxcapi.h>
	#define CComPtr Microsoft::WRL::ComPtr
	#undef MemoryBarrier
	#include <dxc/dxcapi.h>
#else
	#include <dxc/dxcapi.h>
#endif
#include "../beamcast/utils.hpp"
#include "unicode.hpp"
#include <iostream>

// https://simoncoenen.com/blog/programming/graphics/DxcCompiling
class CustomIncludeHandler : public IDxcIncludeHandler {
	CComPtr<IDxcUtils> pUtils;

   public:
	CustomIncludeHandler(CComPtr<IDxcUtils> utils) : pUtils(utils) {}

	HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR							  pFilename,
										 _COM_Outptr_result_maybenull_ IDxcBlob **ppIncludeSource) override {
		CComPtr<IDxcBlobEncoding> pEncoding;
		std::filesystem::path	  path = std::filesystem::canonical(WIDE_TO_UNICODE(pFilename));
		if (IncludedFiles.find(path.string()) != IncludedFiles.end()) {
			// Return empty string blob if this file has been included before
			static const char nullStr[] = " ";
			pUtils->CreateBlobFromPinned(nullStr, ARRAYSIZE(nullStr), DXC_CP_ACP, &pEncoding);
			*ppIncludeSource = pEncoding.Detach();
			return S_OK;
		}

		HRESULT hr = pUtils->LoadFile(pFilename, nullptr, &pEncoding);
		if (SUCCEEDED(hr)) {
			IncludedFiles.insert(path.string());
			*ppIncludeSource = pEncoding.Detach();
		}
		return hr;
	}

	void reset() { IncludedFiles.clear(); }

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override { return E_NOINTERFACE; }
	ULONG STDMETHODCALLTYPE	  AddRef(void) override { return 0; }
	ULONG STDMETHODCALLTYPE	  Release(void) override { return 0; }

	std::unordered_set<std::string> IncludedFiles;
};
