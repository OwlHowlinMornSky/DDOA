﻿#pragma once

#include <inspectable.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <d3d11_4.h>

#include <mutex>
#include <atomic>

#include <opencv2/opencv.hpp>

namespace ohms::capture::wgc {

class CaptureCore final {
public:
	CaptureCore(
		winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device,
		winrt::Windows::Graphics::Capture::GraphicsCaptureItem const& item,
		HWND targetWindow);

	~CaptureCore() {
		Close();
	}

	void StartCapture();

	void Close();

	void setNeedRefresh();

	void setDecimationMode(bool val);

	void setShowScale(int val);

	void setClipClientArea(bool val);

	const cv::Mat& getCapMat();;

protected:
	void OnFrameArrived(
		winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
		winrt::Windows::Foundation::IInspectable const& args);

	void createTexture();

protected:
	std::atomic_bool m_closed;

	winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item;
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool;
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session;

	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device;
	winrt::com_ptr<ID3D11DeviceContext> m_d3dContext;
	winrt::com_ptr<ID3D11Device> m_d3dDevice;
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker m_frameArrived;

	ID3D11Texture2D* m_texture;
	winrt::Windows::Graphics::SizeInt32 m_lastSize;
	winrt::Windows::Graphics::SizeInt32 m_lastTexSize;

	std::atomic<bool> m_img_needRefresh;
	std::atomic<bool> m_img_decimation;
	std::atomic<bool> m_img_client;
	std::atomic<int> m_img_scale;

	D3D11_BOX m_client_box;
	HWND m_target_window;

	cv::Mat m_cap;
	std::mutex m_mutex_cap;
};

} // namespace ohms::capture::wgc
