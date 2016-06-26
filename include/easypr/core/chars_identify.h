#ifndef EASYPR_CORE_CHARSIDENTIFY_H_
#define EASYPR_CORE_CHARSIDENTIFY_H_

#include "easypr/util/kv.h"
#include "easypr/core/character.hpp"
#include <memory>
#include "opencv2/opencv.hpp"

namespace easypr {

class CharsIdentify {
 public:
  static CharsIdentify* instance();

  int classify(cv::Mat f, float& maxVal, bool isChinses = false);
  void classify(cv::Mat featureRows, std::vector<int>& out_maxIndexs,
    std::vector<float>& out_maxVals, std::vector<bool> isChineseVec);
  void classify(std::vector<CCharacter>& charVec);


  std::pair<std::string, std::string> identify(cv::Mat input, bool isChinese = false);
  int identify(std::vector<cv::Mat> inputs, std::vector<std::pair<std::string, std::string>>& outputs,
    std::vector<bool> isChineseVec);

  std::pair<std::string, std::string> identifyChinese(cv::Mat input);

  bool isCharacter(cv::Mat input, std::string& label, float& maxVal, bool isChinese = false);

  void LoadModel(std::string path);
 private:
  CharsIdentify();

  static CharsIdentify* instance_;
  cv::Ptr<cv::ml::ANN_MLP> ann_;
  std::shared_ptr<Kv> kv_;
};
}

#endif  //  EASYPR_CORE_CHARSIDENTIFY_H_