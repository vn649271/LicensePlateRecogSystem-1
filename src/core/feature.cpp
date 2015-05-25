// ����ļ�������EasyPR�������������ɵĺ���
// ���������ռ�Ϊeasypr
// ��������е�������easypr�Ŀ������޸�

#include "easypr/feature.h"
#include "easypr/core_func.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

//! ��ȡ��ֱ��ˮƽ��ֱ��ͼͼֵ
cv::Mat getTheFeatures(cv::Mat in) {
  const int VERTICAL = 0;
  const int HORIZONTAL = 1;

  //Histogram features
  cv::Mat vhist = ProjectedHistogram(in, VERTICAL);
  cv::Mat hhist = ProjectedHistogram(in, HORIZONTAL);

  //Last 10 is the number of moments components
  int numCols = vhist.cols + hhist.cols;

  cv::Mat out = cv::Mat::zeros(1, numCols, CV_32F);

  //Asign values to feature,��������Ϊˮƽ����ֱֱ��ͼ
  int j = 0;
  for (int i = 0; i < vhist.cols; i++) {
    out.at<float>(j) = vhist.at<float>(i);
    j++;
  }
  for (int i = 0; i < hhist.cols; i++) {
    out.at<float>(j) = hhist.at<float>(i);
    j++;
  }

  return out;
}


//! EasyPR��getFeatures�ص�����
//! ������������ֱ��ͼ���������Ļص�����
void getHisteqFeatures(const cv::Mat& image, cv::Mat& features) {
  features = histeq(image);
}

//! EasyPR��getFeatures�ص�����
//! �������ǻ�ȡ��ֱ��ˮƽ��ֱ��ͼͼֵ
void getHistogramFeatures(const cv::Mat& image, cv::Mat& features) {
  cv::Mat grayImage;
  cvtColor(image, grayImage, CV_RGB2GRAY);

  //grayImage = histeq(grayImage);

  cv::Mat img_threshold;
  threshold(grayImage, img_threshold, 0, 255,
            CV_THRESH_OTSU + CV_THRESH_BINARY);
  features = getTheFeatures(img_threshold);
}


//! EasyPR��getFeatures�ص�����
//! �������ǻ�ȡSITF������
void getSIFTFeatures(const cv::Mat& image, cv::Mat& features) {
  //������
}


//! EasyPR��getFeatures�ص�����
//! �������ǻ�ȡHOG������
void getHOGFeatures(const cv::Mat& image, cv::Mat& features) {
  //������
}

//! EasyPR��getFeatures�ص�����
//! �������ǻ�ȡHSV�ռ�������ֱ��ͼ������
void getHSVHistFeatures(const cv::Mat& image, cv::Mat& features) {
  //TODO
}

}  /* \namespace easypr  */
