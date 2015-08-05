//////////////////////////////////////////////////////////////////////////
// Name:	    chars_identify Header
// Version:		1.0
// Date:	    2014-09-25
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsIdentify
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_IDENTIFY_H__
#define __CHARS_IDENTIFY_H__

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
    */
namespace easypr {

  class CCharsIdentify {
  public:
    CCharsIdentify();

    //! �ַ��ָ�
    string charsIdentify(Mat input, bool isChinese, bool isSpeci);

  private:
    //! �ַ�����
    int classify(Mat f, bool isChinses, bool isSpeci);

    //��ʹ�õ�ANNģ��
    cv::Ptr<ml::ANN_MLP> ann_;
  };

} /* \namespace easypr  */

#endif /* endif __CHARS_IDENTIFY_H__ */