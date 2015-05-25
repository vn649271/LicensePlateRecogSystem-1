//////////////////////////////////////////////////////////////////////////
// Name:	    plate_judge Header
// Version:		1.1
// Date:	    2014-09-28
// MDate:	    2015-01-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:  
// Defines CPlateLocate
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_JUDGE_H__
#define __PLATE_JUDGE_H__

#include "plate.h"
#include "feature.h"
#include "core_func.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateJudge {
 public:
  CPlateJudge();

  //! �Զ�����ƽ���SVM�ж�
  int plateJudge(const std::vector<CPlate>&, std::vector<CPlate>&);

  //! �����ж�
  int plateJudge(const std::vector<cv::Mat>&, std::vector<cv::Mat>&);

  //! �����жϣ�һ��ͼ��
  int plateJudge(const cv::Mat& inMat, int& result);

  //! ֱ��ͼ����
  cv::Mat histeq(cv::Mat);

  //! װ��SVMģ��
  void LoadModel(const char* model);

 private:
  //��ʹ�õ�SVMģ��
  CvSVM svm;

  // ! EasyPR��getFeatures�ص�����
  // �����ڴӳ��Ƶ�image����svm��ѵ������features
  svmCallback m_getFeatures;
};

}  /*! \namespace easypr*/

#endif /* endif __PLATE_JUDGE_H__ */