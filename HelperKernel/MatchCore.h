#pragma once

#include "IEye.h"

using namespace System;

namespace HelperKernel {

public ref class MatchCore {
public:
	MatchCore();
	virtual ~MatchCore();

protected:
	bool Match(IEye^ eye, float threshold);

	bool IsFloatingArea();

	Drawing::Rectangle GetSearchRect();
	Drawing::Rectangle GetPreviousMatchedRect();

protected:
	bool m_isFloatingArea; // �Ƿ񸡶���Χ���ҡ�
	Drawing::Rectangle m_searchRect; // ����֮��Χ��
	Drawing::Rectangle m_prevMatchedRect; // �ϴγɹ�ƥ��֮��Χ��
	cv::Mat* m_target; // ����ƥ��֮ͼ����
	cv::Mat* m_mask;   // ����ƥ��֮���֡�
};

}
