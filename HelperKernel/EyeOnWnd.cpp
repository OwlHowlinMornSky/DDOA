
#include <opencv2/imgproc.hpp>

#include "EyeOnWnd.h"

namespace HelperKernel {

EyeOnWnd::EyeOnWnd() {
	m_mat = new cv::Mat();
	if (!ohms::wgc::ICapture::setup(true)) { // ��ʼ��WGC
		throw gcnew System::SystemException(L"Failed to setup WGC.");
	}
	r_capture = ohms::wgc::ICapture::getInstance();
	m_hwnd = NULL;
}

EyeOnWnd::~EyeOnWnd() {
	m_hwnd = NULL;
	ohms::wgc::ICapture::drop();
	r_capture = nullptr;
	delete m_mat;
	m_mat = nullptr;
}

bool EyeOnWnd::UpdateVision() {
	if (r_capture->isRefreshed()) { // refresh���ٴ������������
		r_capture->askForRefresh();
		if (r_capture->copyMatTo(*m_mat, true)) { // Ҫ��ת��ΪBGR
			if (m_mat->size().width != 960 || m_mat->size().height != 540) { // ȷ����С����Ҫ��
				cv::resize(
					*m_mat, *m_mat,
					cv::Size(960, 540),
					0.0, 0.0, cv::InterpolationFlags::INTER_LINEAR
				);
			}
			return true;
		}
	}
	return false;
}

const cv::Mat* EyeOnWnd::GetVision() {
	return m_mat;
}

bool EyeOnWnd::IsLooking() {
	return NULL != m_hwnd;
}

void EyeOnWnd::Reset() {
	m_hwnd = NULL;
	r_capture->stopCapture(); // ֹͣ��ͼ
}

bool EyeOnWnd::SetOnWnd(HWND hwnd) {
	Reset();
	if (m_hwnd == NULL) {
		return false;
	}
	m_hwnd = hwnd;
	if (!IsWindow(m_hwnd) || IsIconic(m_hwnd) || !r_capture->startCaptureWindow(m_hwnd)) { // ��Щ���ܽ�ͼ�ı�Ҫ����
		m_hwnd = NULL;
		return false;
		//return SetReturnValue::CaptureFailed;
	}
	r_capture->setClipToClientArea(true);
	//if (Settings::WndHandler::DEFAULT.UseHook && !Settings::WndHandler::DEFAULT.UseSendInput)
	//	if (!TryHook())
	//		IHelper::instance()->GuiLogF(ReturnMsgEnum::HookFailed);
	//m_state = StateValue::Game;
	return true;
}

}
