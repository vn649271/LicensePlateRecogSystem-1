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

#include "easypr/plate.h"
#include "easypr/feature.h"
#include "easypr/core_func.h"

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
  int plateJudge(const std::vector<Mat>&, std::vector<Mat>&);

  //! �����жϣ�һ��ͼ��
  int plateJudge(const Mat& inMat, int& result);

  //! ֱ��ͼ����
  Mat histeq(Mat);

  //! װ��SVMģ��
  void LoadModel();

  //! װ��SVMģ��
  void LoadModel(std::string s);

  //! �������ȡģ��·��
  inline void setModelPath(std::string path) { m_path = path; }
  inline std::string getModelPath() const { return m_path; }

 private:
  //! ʹ�õ�SVMģ��
  cv::Ptr<ml::SVM> svm;

  //! EasyPR��getFeatures�ص�����
  //! ���ڴӳ��Ƶ�image����svm��ѵ������features
  svmCallback m_getFeatures;

  //! ģ�ʹ洢·��
  std::string m_path;
};

} /*! \namespace easypr*/

#endif /* endif __PLATE_JUDGE_H__ */