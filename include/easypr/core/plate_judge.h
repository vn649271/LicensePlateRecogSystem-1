#ifndef EASYPR_CORE_PLATEJUDGE_H_
#define EASYPR_CORE_PLATEJUDGE_H_

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

#endif  // EASYPR_CORE_PLATEJUDGE_H_