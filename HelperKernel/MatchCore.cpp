#include "MatchCore.h"

#include <opencv2/imgproc.hpp>

namespace {

constexpr int g_nMatchMethod = cv::TM_SQDIFF_NORMED; // opencv matchʱ�õķ�����
constexpr bool g_useColorDiff = false; // check�����õ�ѡ�

/**
 * @brief �Ƚ�����mat����������ֵ����������
 * @param matSample ����
 * @param matTemplate ģ��
 * @return ����̶ȣ���Ҫ������Ч�����ʹ�ã���
*/
inline float matDiffrencePixel(const cv::Mat& matSample, const cv::Mat& matTemplate) {
	assert(matSample.size() == matTemplate.size()); // ȷ������mat��Сһ�¡�
	cv::Mat matResault;
	cv::absdiff(matSample, matTemplate, matResault); // ������Բ�
	cv::threshold(matResault, matResault, 48.0, 255.0, cv::ThresholdTypes::THRESH_BINARY); // ��ֵ
	cv::cvtColor(matResault, matResault, cv::COLOR_BGR2GRAY); // ת��Ϊ�Ҷ�
	return (static_cast<float>(cv::countNonZero(matResault))); // ��Ϊ��ɫ��������
}
/**
 * @brief �Ƚ�����mat����������ֵ��������ɫ��
 * @param matSample ����
 * @param matTemplate ģ��
 * @return ����̶ȣ���Ҫ������Ч�����ʹ�ã�.
*/
inline float matDiffrenceColor(const cv::Mat& matSample, const cv::Mat& matTemplate) {
	assert(matSample.size() == matTemplate.size()); // ȷ������mat��Сһ�¡�
	cv::Mat matResault;
	cv::absdiff(matSample, matTemplate, matResault); // ������Բ�ֵ��
	cv::threshold(matResault, matResault, 32.0, 255.0, cv::ThresholdTypes::THRESH_BINARY); // ��ֵ
	cv::cvtColor(matResault, matResault, cv::COLOR_BGR2GRAY); // תΪ�Ҷ�
	float res = 0.0f;
	// �ѻҶ�ֵȫ��������
	for (cv::MatIterator_<unsigned char> i = matResault.begin<unsigned char>(),
		n = matResault.end<unsigned char>(); i != n; ++i) {
		res += static_cast<float>(*i);
	}
	return (res / 255.0f); // �Ҷ� ���� 255�����ֵ��
}

/**
 * @brief �ж�����mat�����Ƿ��ڹ涨��ֵ��
 * @param matSample ����
 * @param matTemplate ģ��
 * @param thres ������ֵ
 * @return ����̶�С����ֵ��Ϊtrue�����㹻���ƣ�
*/
inline bool check(const cv::Mat& matSample, const cv::Mat& matTemplate, float thres) {
	assert(thres > 0.0f && thres < 100.0f);
	assert(matSample.size() == matTemplate.size());
	float diff;
	if constexpr (g_useColorDiff)
		diff = matDiffrenceColor(matSample, matTemplate) / matTemplate.size().area(); // �����������������
	else
		diff = matDiffrencePixel(matSample, matTemplate) / matTemplate.size().area();
	return (diff < thres);
}

inline bool checkWithMask(const cv::Mat& matSample, const cv::Mat& matTemplate, const cv::Mat& matMask, float thres) {
	assert(thres > 0.0f && thres < 100.0f);
	assert(matSample.size() == matTemplate.size());
	float diff;
	if constexpr (g_useColorDiff)
		diff = matDiffrenceColor(matSample, matTemplate) / cv::countNonZero(matMask); // �������������Ч����������
	else
		diff = matDiffrencePixel(matSample, matTemplate) / cv::countNonZero(matMask);
	return (diff < thres);
}

}

namespace HelperKernel {

MatchCore::MatchCore() {
	throw gcnew System::NotImplementedException();
}

MatchCore::~MatchCore() {
	if (m_target) delete m_target;
	m_target = nullptr;
	if (m_mask) delete m_mask;
	m_mask = nullptr;
}

bool MatchCore::Match(IEye^ eye, float threshold) {
	const cv::Mat* vision = eye->GetVision();
	if (nullptr == vision) return false;
	const cv::Mat& sample = *vision;

	cv::Mat srcImage; // ����
	cv::Rect searchRect(m_searchRect.X, m_searchRect.Y, m_searchRect.Width, m_searchRect.Height);

	if (nullptr != m_mask) {
		sample(searchRect).copyTo(srcImage, *m_mask);
	}
	else {
		sample(searchRect).copyTo(srcImage);
	}

	if (m_isFloatingArea) {
		cv::Size targetSize = m_target->size();
		// ����match����Ĵ�С
		cv::Size size = srcImage.size() - targetSize;
		size += { 1, 1 };
		assert(size.height > 0 && size.width > 0);

		cv::Mat resultImage; // match���
		resultImage.create(size, CV_32FC1);

		cv::matchTemplate(srcImage, *m_target, resultImage, g_nMatchMethod); // match

		// Ѱ�����ƥ���
		double minValue, maxValue;
		cv::Point minLocation, maxLocation, matchLocation;
		cv::minMaxLoc(resultImage, &minValue, &maxValue, &minLocation, &maxLocation);
		if constexpr (g_nMatchMethod == cv::TM_SQDIFF || g_nMatchMethod == cv::TM_SQDIFF_NORMED)
			matchLocation = minLocation;
		else
			matchLocation = maxLocation;

		// ��ƥ�䵽�ĵ�ȷ��ƥ�䵽�����������ԭʼ���룩
		cv::Rect prevMatch = cv::Rect(matchLocation + searchRect.tl(), targetSize);
		sample(prevMatch).copyTo(srcImage);

		m_prevMatchedRect = Drawing::Rectangle(prevMatch.x, prevMatch.y, prevMatch.width, prevMatch.height);
	}
	else {
		m_prevMatchedRect = m_searchRect;
	}

	// �����������Ƿ�������ֵ
	bool res =
		(nullptr != m_mask)
		?
		checkWithMask(srcImage, *m_target, *m_mask, threshold)
		:
		check(srcImage, *m_target, threshold)
		;
	return res;

	return false;
}

bool MatchCore::IsFloatingArea() {
	return m_isFloatingArea;
}

Drawing::Rectangle MatchCore::GetSearchRect() {
	return m_searchRect;
}

Drawing::Rectangle MatchCore::GetPreviousMatchedRect() {
	return m_prevMatchedRect;
}

}
