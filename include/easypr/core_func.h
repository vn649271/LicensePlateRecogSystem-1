#ifndef __CORE_FUNC_H__
#define __CORE_FUNC_H__

#include <opencv2/opencv.hpp>

namespace easypr {

	enum Color{ BLUE, YELLOW };

	//! ����һ��ͼ������ɫģ���ȡ��Ӧ�Ķ�ֵͼ
	//! ����RGBͼ��, ��ɫģ�壨��ɫ����ɫ��
	//! ����Ҷ�ͼ��ֻ��0��255����ֵ��255����ƥ�䣬0����ƥ�䣩
	cv::Mat colorMatch(const cv::Mat& src, cv::Mat& match, const Color r, const bool adaptive_minsv);

	//! �ж�һ�����Ƶ���ɫ
	//! ���복��mat����ɫģ��
	//! ����true��fasle
	bool plateColorJudge(const cv::Mat&  src, const Color r, const bool adaptive_minsv);

	bool bFindLeftRightBound(cv::Mat& bound_threshold,int& posLeft,int& posRight);
	bool bFindLeftRightBound1(cv::Mat& bound_threshold,int& posLeft,int& posRight);
	bool bFindLeftRightBound2(cv::Mat& bound_threshold,int& posLeft,int& posRight);
	
	//ȥ�������Ϸ���ť��
	//����ÿ��Ԫ�صĽ�Ծ�������С��X��Ϊ��������������ȫ����0��Ϳ�ڣ�
	//X���Ƽ�ֵΪ���ɸ���ʵ�ʵ���
	bool clearLiuDing(cv::Mat& img);
	void clearLiuDingOnly(cv::Mat& img);
	void clearLiuDing(cv::Mat mask,int& top,int& bottom);

	//! ��ó�����ɫ
	Color getPlateType(const cv::Mat&  src, const bool adaptive_minsv);

	//! ֱ��ͼ����
	cv::Mat histeq(cv::Mat in);
	cv::Mat features(cv::Mat in, int sizeData);
	int ThresholdOtsu(cv::Mat mat);


	// ����ȡ��ֱ��ˮƽ����ֱ��ͼ
	cv::Mat ProjectedHistogram(cv::Mat img, int t);

}	/*! \namespace easypr*/

#endif
/* endif __CORE_FUNC_H__ */