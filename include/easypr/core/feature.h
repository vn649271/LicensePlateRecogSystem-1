#ifndef EASYPR_CORE_FEATURE_H_
#define EASYPR_CORE_FEATURE_H_

#include <opencv2/opencv.hpp>

namespace easypr {

//! ��ó��Ƶ�������
cv::Mat getTheFeatures(cv::Mat in);

//! EasyPR��getFeatures�ص�����
//! ���ڴӳ��Ƶ�image����svm��ѵ������features
typedef void (*svmCallback)(const cv::Mat& image, cv::Mat& features);

//!  EasyPR��getFeatures�ص�����
//! �������ǻ�ȡ��ֱ��ˮƽ��ֱ��ͼͼֵ
void getHistogramFeatures(const cv::Mat& image, cv::Mat& features);

//! �������ǻ�ȡSIFT������
void getSIFTFeatures(const cv::Mat& image, cv::Mat& features);

//! �������ǻ�ȡHOG������
void getHOGFeatures(const cv::Mat& image, cv::Mat& features);

//! �������ǻ�ȡHSV�ռ�������ֱ��ͼ������
void getHSVHistFeatures(const cv::Mat& image, cv::Mat& features);

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_FEATURE_H_