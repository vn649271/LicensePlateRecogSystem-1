#ifndef __PLATE_JUDGE_H__
#define __PLATE_JUDGE_H__

#include "easypr/core/plate.h"
#include "easypr/core/feature.h"
#include "easypr/core/core_func.h"

namespace easypr {

class PlateJudge {
 public:
  static PlateJudge* instance();
   
   //! �Զ�����ƽ���SVM�ж�
  int plateJudge(const std::vector<CPlate>&, std::vector<CPlate>&);

  //! �����ж�
  int plateJudge(const std::vector<Mat>&, std::vector<Mat>&);

  //! �����жϣ�һ��ͼ��
  int plateJudge(const Mat& inMat, int& result);

 private:
  PlateJudge();

  static PlateJudge* instance_;

  cv::Ptr<ml::SVM> svm_;

};

}

#endif