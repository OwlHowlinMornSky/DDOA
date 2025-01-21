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
	virtual void RequireRefresh();
	virtual bool SaveVision();
	virtual const cv::Mat* GetVision();
	virtual bool IsLooking();

	virtual void DrawRectangle(System::Drawing::Rectangle rect, System::Drawing::Color color);
	virtual void ShowDebugWindow();

public:
	void Reset();
	int SetOnWnd(HWND hwnd);
	int SetOnWnd(System::String^ cls, System::String^ title);

private:
	HWND m_hwnd; // ���ھ��
	cv::Mat* m_mat; // ��ȡ����֡�Ĵ洢λ�á�
	ohms::wgc::ICapture* r_capture; // capture����
#ifdef _DEBUG
	cv::Mat* m_matDebug; // ��ȡ����֡�Ĵ洢λ�á�
#endif // _DEBUG
};

}
