//////////////////////////////////////////////////////////////////////////
// Name:	    plate_recognize Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CPlateRecognize
//////////////////////////////////////////////////////////////////////////
#ifndef EASYPR_CORE_PLATERECOGNIZE_H_
#define EASYPR_CORE_PLATERECOGNIZE_H_

#include "easypr/core/plate_detect.h"
#include "easypr/core/chars_recognise.h"

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

  class CPlateRecognize : public CPlateDetect, public CCharsRecognise {
  public:
    CPlateRecognize();

    //! ���Ƽ�����ַ�ʶ��
    int plateRecognize(Mat src, std::vector<CPlate> &licenseVec);
    int plateRecognize(Mat src, std::vector<std::string> &licenseVec);

    int plateRecognizeAsText(Mat src, std::vector<CPlate> &licenseVec);
    int plateRecognizeAsTextNM(Mat src, std::vector<CPlate> &licenseVec);

    //! ����ģʽ�빤ҵģʽ�л�

    inline void setLifemode(bool param) { CPlateDetect::setPDLifemode(param); }

    //! �Ƿ�������ģʽ

    inline void setDebug(bool param) { CPlateDetect::setPDDebug(param); }
  };

} /* \namespace easypr  */

#endif  // EASYPR_CORE_PLATERECOGNIZE_H_