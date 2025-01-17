#pragma once

#include "IEye.h"

using namespace System::Drawing;

namespace HelperKernel {

public ref class MatchCore {
protected:
	MatchCore();
	virtual ~MatchCore();

private:
	bool IsFloatingArea();

protected:
	/**
	 * @brief �����ͼ�����顣
	 * @param eye �ۡ�
	 * @param threshold ͨ��֮��ֵ�������С�ڸ�ֵ �� ƥ��ɹ���
	 * @return ƥ���Ƿ�ɹ���
	 */
	bool Match(IEye^ eye, float threshold);

	/**
	 * @brief ��ȡ����֮����
	 * @return ����֮����
	 */
	Rectangle GetSearchRect();
	/**
	 * @brief ��ȡ�ϴγɹ�ƥ��ʱ�������������򸡶�ʱ �÷����������壬��Ϊ��������̶�ʱ ����ֵ������������
	 * @return �ϴγɹ�ƥ��ʱ������
	 */
	Rectangle GetPreviousMatchedRect();

	/**
	 * @brief ����ģ��Mat��
	 * @param filepath ģ��ͼƬ�ļ�·����
	 * @return �����Ƿ�ɹ���
	 */
	bool LoadPattern(System::String^ filepath);
	/**
	 * @brief ��������Mat��
	 * @param filepath ����ͼƬ�ļ�·����
	 * @return �����Ƿ�ɹ���
	 */
	bool LoadMask(System::String^ filepath);

protected:
	bool m_isFloatingArea; // �Ƿ񸡶���Χ���ҡ�
	Rectangle m_searchRect; // ����֮��Χ��
	Rectangle m_prevMatchedRect; // �ϴγɹ�ƥ��֮��Χ��
	cv::Mat* m_pattern; // ����ƥ��֮ͼ����
	cv::Mat* m_mask;   // ����ƥ��֮���֡�
};

}
