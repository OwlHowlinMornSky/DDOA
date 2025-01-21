/*
*    DDOA-DOAXVV-OPEN-ASSISTANT
*
*     Copyright 2023-2025  Tyler Parret True
*
*    Licensed under the Apache License, Version 2.0 (the "License");
*    you may not use this file except in compliance with the License.
*    You may obtain a copy of the License at
*
*        http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software
*    distributed under the License is distributed on an "AS IS" BASIS,
*    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*    See the License for the specific language governing permissions and
*    limitations under the License.
*
* @Authors
*    Tyler Parret True <mysteryworldgod@outlook.com><https://github.com/OwlHowlinMornSky>
*/
#pragma once

#include "IEye.h"

using namespace System::Drawing;

namespace HelperKernel {

public ref class MatchCore {
protected:
	MatchCore();
	virtual ~MatchCore();

public:
	bool IsFloatingArea();

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

protected:
	/**
	 * @brief �����ͼ�����顣
	 * @param eye �ۡ�
	 * @param threshold ͨ��֮��ֵ�������С�ڸ�ֵ �� ƥ��ɹ���
	 * @return ƥ���Ƿ�ɹ���
	 */
	bool Match(IEye^ eye, float threshold);

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
