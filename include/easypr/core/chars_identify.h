#ifndef __CHARS_IDENTIFY_H__
#define __CHARS_IDENTIFY_H__

#include <opencv2/opencv.hpp>
#include "easypr/util/util.h"

using namespace cv;
using namespace std;

namespace easypr {

  class CharsIdentify {
  public:
    static CharsIdentify* instance();

    string identify(Mat input, bool isChinese, bool isSpeci);

  private:
    CharsIdentify();

    static CharsIdentify* instance_;

    //! �ַ�����
    int classify(Mat f, bool isChinses, bool isSpeci);

    //��ʹ�õ�ANNģ��
    cv::Ptr<cv::ml::ANN_MLP> ann_;
  };

}

#endif