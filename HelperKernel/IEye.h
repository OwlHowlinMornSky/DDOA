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

	void DrawRectangle(System::Drawing::Rectangle rect, System::Drawing::Color color);
	void ShowDebugWindow();
};

}
