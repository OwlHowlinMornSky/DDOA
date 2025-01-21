#pragma once

#include "IHand.h"
#include "framework.h"

namespace HelperKernel {

public ref class HandOnWnd sealed : public IHand {
public:
	HandOnWnd();
	virtual ~HandOnWnd();

public:
	virtual bool IsOperating();

	virtual void MoveCursorTo(Drawing::Point target);
	virtual void MoveMouseWheel(bool isDown, unsigned int cnt);
	virtual Drawing::Point GetCursorPosition();

	virtual void ClickAt(Drawing::Point target);

	virtual int SetUserCursorInterceptionEnabled(bool enabled);
	virtual bool GetUserCursorInterceptionEnabled();

public:
	void Reset();
	int SetOnWnd(HWND hwnd);
	int SetOnWnd(System::String^ cls, System::String^ title);

protected:
	int InitHookMod();
	int TryHook();
	void DropHook();
	void DropHookMod();

private:
	HWND m_hwnd; // ���ھ��
	Drawing::Point m_screenSize; // ��Ļ��С��
	Drawing::Point m_lastMousePoint; // ���λ�ü�¼��
	Drawing::Rectangle m_workArea; // �������򣨼���ȥ����������

	HMODULE m_hmod; // �ҹ�ģ������
	FARPROC m_hookproc; // �ҹ����̡�
	HHOOK m_hhook; // �ҹ������
};

}
