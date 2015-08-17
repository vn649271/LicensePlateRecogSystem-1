//////////////////////////////////////////////////////////////////////////
// Name:	    chars_recognise Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsRecognise
//////////////////////////////////////////////////////////////////////////
#ifndef EASYPR_CORE_CHARSRECOGNISE_H_
#define EASYPR_CORE_CHARSRECOGNISE_H_

#include "easypr/core/chars_segment.h"
#include "easypr/core/chars_identify.h"

namespace easypr {

class CCharsRecognise {
 public:
  CCharsRecognise();

  ~CCharsRecognise();

  std::string charsRecognise(cv::Mat plate);

  //! ��ó�����ɫ
  inline std::string getPlateColor(cv::Mat input) const {
    std::string color = "δ֪";
    Color result = getPlateType(input, true);
    if (BLUE == result) color = "����";
    if (YELLOW == result) color = "����";
    return color;
  }

  //! ���ñ���
  inline void setLiuDingSize(int param) {
    m_charsSegment->setLiuDingSize(param);
  }
  inline void setColorThreshold(int param) {
    m_charsSegment->setColorThreshold(param);
  }
  inline void setBluePercent(float param) {
    m_charsSegment->setBluePercent(param);
  }
  inline float getBluePercent() const {
    return m_charsSegment->getBluePercent();
  }
  inline void setWhitePercent(float param) {
    m_charsSegment->setWhitePercent(param);
  }
  inline float getWhitePercent() const {
    return m_charsSegment->getWhitePercent();
  }

 private:
  //���ַ��ָ�
  CCharsSegment* m_charsSegment;
};

} /* \namespace easypr  */

#endif  // EASYPR_CORE_CHARSRECOGNISE_H_