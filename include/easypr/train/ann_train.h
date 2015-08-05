#ifndef EASYPR_TRAIN_ANNTRAIN_H_
#define EASYPR_TRAIN_ANNTRAIN_H_

#include <opencv2/opencv.hpp>
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"

namespace easypr{

  const char* kChinese[] = {
    "zh_cuan" /* �� */, "zh_e" /* �� */, "zh_gan" /* ��*/,
    "zh_gan1" /*��*/, "zh_gui" /* �� */, "zh_gui1" /* �� */,
    "zh_hei" /* �� */, "zh_hu" /* �� */, "zh_ji" /* �� */,
    "zh_jin" /* �� */, "zh_jing" /* �� */, "zh_jl" /* �� */,
    "zh_liao" /* �� */, "zh_lu" /* ³ */, "zh_meng" /* �� */,
    "zh_min" /* �� */, "zh_ning" /* �� */, "zh_qing" /* �� */,
    "zh_qiong" /* �� */, "zh_shan" /* �� */, "zh_su" /* �� */,
    "zh_sx" /* �� */, "zh_wan" /* �� */, "zh_xiang" /* �� */,
    "zh_xin" /* �� */, "zh_yu" /* ԥ */, "zh_yu1" /* �� */,
    "zh_yue" /* �� */, "zh_yun" /* �� */, "zh_zang" /* �� */,
    "zh_zhe" /* �� */
  };

  const char kCharacters[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', /* û��I */
    'J', 'K', 'L', 'M', 'N', /* û��O */ 'P', 'Q',
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
  };

  const int kPredictSize = 10;

  class AnnTrain{
  public:
    explicit AnnTrain(const char* chars_folder, const char* zhchars_folder, const char* xml);

    void train(const int & neurons = 40);

  private:
    void getTrainData();

    cv::Ptr<cv::ml::ANN_MLP> ann_;
    cv::Ptr<cv::ml::TrainData> train_data_;

    const char* ann_xml_;
    const char* chars_folder_;
    const char* zhchars_folder_;
  };

}

#endif // EASYPR_TRAIN_ANNTRAIN_H_