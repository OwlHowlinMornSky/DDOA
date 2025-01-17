#pragma once

#include <opencv2/core/mat.hpp>

namespace HelperKernel {

/**
 * @brief �ۡ�
 */
public interface class IEye {
public:
	/**
	 * @brief ˢ����Ұ��
	 * @return ������µ���Ұ��Ϊ true��������Ұδˢ�� ���� false��
	 */
	bool UpdateVision();
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
};

}
