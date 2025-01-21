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
#include "MatchCore.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

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

/**
 * @brief �ж�����mat�����Ƿ��ڹ涨��ֵ�ڣ���������Ӧ��������
 * @param matSample ����
 * @param matTemplate ģ��
 * @param matMask ����
 * @param thres ������ֵ
 * @return ���ַ�Χ�� ����̶�С����ֵ��Ϊtrue�����㹻���ƣ�
 */
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

MatchCore::MatchCore() :
	m_isFloatingArea(false),
	m_searchRect(),
	m_prevMatchedRect(),
	m_pattern(nullptr),
	m_mask(nullptr) {}

MatchCore::~MatchCore() {
	if (m_pattern) delete m_pattern;
	m_pattern = nullptr;
	if (m_mask) delete m_mask;
	m_mask = nullptr;
}

bool MatchCore::Match(IEye^ eye, float threshold) {
	const cv::Mat* vision = eye->GetVision();
	if (nullptr == vision) return false;
	const cv::Mat& sample = *vision;

	cv::Mat srcImage; // ����
	cv::Rect searchRect(m_searchRect.X, m_searchRect.Y, m_searchRect.Width, m_searchRect.Height);

	switch ((m_isFloatingArea ? 2 : 0) | (m_mask ? 1 : 0)) {
	case 0: // �����ֵĹ̶�����
		sample(searchRect).copyTo(srcImage);
		m_prevMatchedRect = m_searchRect;
		return check(srcImage, *m_pattern, threshold); // ��������Ƿ�������ֵ
	case 1: // �����ֵĹ̶�����
		sample(searchRect).copyTo(srcImage, *m_mask);
		m_prevMatchedRect = m_searchRect;
		return checkWithMask(srcImage, *m_pattern, *m_mask, threshold); // ��������Ƿ�������ֵ
	case 2: // �����ֵĸ�������
		sample(searchRect).copyTo(srcImage);
		{
			cv::Size targetSize = m_pattern->size();
			// ����match����Ĵ�С
			cv::Size size = srcImage.size() - targetSize;
			size += { 1, 1 };
			assert(size.height > 0 && size.width > 0);

			cv::Mat resultImage; // match���
			resultImage.create(size, CV_32FC1);
			cv::matchTemplate(srcImage, *m_pattern, resultImage, g_nMatchMethod); // match

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

			m_prevMatchedRect = Rectangle(prevMatch.x, prevMatch.y, prevMatch.width, prevMatch.height);
		}
		return check(srcImage, *m_pattern, threshold); // �����������Ƿ�������ֵ
	case 3: // �����ֵĸ�������
		sample(searchRect).copyTo(srcImage);
		{
			cv::Size targetSize = m_pattern->size();
			// ����match����Ĵ�С
			cv::Size size = srcImage.size() - targetSize;
			size += { 1, 1 };
			assert(size.height > 0 && size.width > 0);

			cv::Mat resultImage; // match���
			resultImage.create(size, CV_32FC1);
			cv::matchTemplate(srcImage, *m_pattern, resultImage, g_nMatchMethod, m_mask ? *m_mask : cv::noArray()); // match

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

			m_prevMatchedRect = Rectangle(prevMatch.x, prevMatch.y, prevMatch.width, prevMatch.height);
		}
		// �����������Ƿ�������ֵ
		return checkWithMask(srcImage, *m_pattern, *m_mask, threshold);
	default:
		break;
	}
	return false;
}

bool MatchCore::IsFloatingArea() {
	return m_isFloatingArea;
}

Rectangle MatchCore::GetSearchRect() {
	return m_searchRect;
}

Rectangle MatchCore::GetPreviousMatchedRect() {
	return m_prevMatchedRect;
}

bool MatchCore::LoadPattern(System::String^ filepath) {
	if (m_pattern) delete m_pattern;

	cli::array<wchar_t>^ wArray = filepath->ToCharArray();
	cli::array<unsigned char, 1>^ arr = System::Text::Encoding::UTF8->GetBytes(wArray);

	int len = arr->Length;
	char* cstr = new char[len + 2];
	System::IntPtr pcstr(cstr);
	System::Runtime::InteropServices::Marshal::Copy(arr, 0, pcstr, len);
	cstr[len] = 0;
	cstr[len + 1] = 0;

	m_pattern = new cv::Mat(cv::imread(cstr));
	if (m_pattern->empty())
		return false;
	if (!m_isFloatingArea) {
		m_searchRect.Width = m_pattern->size().width;
		m_searchRect.Height = m_pattern->size().height;
	}

	if (m_mask && (m_pattern->size() != m_mask->size())) {
		return false;
	}
	return true;
}

bool MatchCore::LoadMask(System::String^ filepath) {
	if (m_mask) delete m_mask;

	cli::array<wchar_t>^ wArray = filepath->ToCharArray();
	cli::array<unsigned char, 1>^ arr = System::Text::Encoding::UTF8->GetBytes(wArray);

	int len = arr->Length;
	char* cstr = new char[len + 2];
	System::IntPtr pcstr(cstr);
	System::Runtime::InteropServices::Marshal::Copy(arr, 0, pcstr, len);
	cstr[len] = 0;
	cstr[len + 1] = 0;

	m_mask = new cv::Mat(cv::imread(cstr, cv::IMREAD_GRAYSCALE));
	if (m_mask->empty())
		return false;

	if (m_pattern && (m_pattern->size() != m_mask->size())) {
		return false;
	}
	return true;
}

}
