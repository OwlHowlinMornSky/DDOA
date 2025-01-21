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

#include <opencv2/core/mat.hpp>

namespace HelperKernel {

/**
 * @brief �ۡ�
 */
public interface class IEye {
public:
	/**
	 * @brief Ҫ��ˢ����Ұ��
	 */
	void RequireRefresh();
	/**
	 * @brief ����Ұ���ݱ��档
	 * @return ������µ�������Ϊ true����������δˢ�� ���� false��
	 */
	bool SaveVision();
	/**
	 * @brief ��ȡ��ǰ����Ұ��
	 * @return ��ǰ��Ұ������Ϊnullptr����û����Ұ����
	 */
	const cv::Mat* GetVision();
	/**
	 * @brief ����Ƿ����ڹ۲졣
	 * @return �Ƿ����ڹ۲졣
	 */
	bool IsLooking();

	void BeginDebugDraw();
	void DrawRectangle(System::Drawing::Rectangle rect, System::Drawing::Color color);
	void ShowDebugWindow();
	void DestroyDebugWindows();
};

}
