#pragma once

#include "IEye.h"
#include <ohms/WGC.h>
#include "framework.h"

namespace HelperKernel {

public ref class EyeOnWnd sealed : public IEye {
public:
	EyeOnWnd();
	virtual ~EyeOnWnd();

public:
	virtual bool UpdateVision();
	virtual const cv::Mat* GetVision();
	virtual bool IsLooking();

public:
	void Reset();
	bool SetOnWnd(HWND hwnd);

private:
	HWND m_hwnd; // ���ھ��
	cv::Mat* m_mat; // ��ȡ����֡�Ĵ洢λ�á�
	ohms::wgc::ICapture* r_capture; // capture����
};

}
