#include "easypr/core/chars_identify.h"
#include "easypr/core/core_func.h"
#include "easypr/train/ann_train.h"

namespace easypr {

  std::map<string, string> zh_map = {
    { "zh_cuan", "��" },
    { "zh_e", "��" },
    { "zh_gan", "��" },
    { "zh_gan1", "��" },
    { "zh_gui", "��" },
    { "zh_gui1", "��" },
    { "zh_hei", "��" },
    { "zh_hu", "��" },
    { "zh_ji", "��" },
    { "zh_jin", "��" },
    { "zh_jing", "��" },
    { "zh_jl", "��" },
    { "zh_liao", "��" },
    { "zh_lu", "³" },
    { "zh_meng", "��" },
    { "zh_min", "��" },
    { "zh_ning", "��" },
    { "zh_qing", "��" },
    { "zh_qiong", "��" },
    { "zh_shan", "��" },
    { "zh_su", "��" },
    { "zh_sx", "��" },
    { "zh_wan", "��" },
    { "zh_xiang", "��" },
    { "zh_xin", "��" },
    { "zh_yu", "ԥ" },
    { "zh_yu1", "��" },
    { "zh_yue", "��" },
    { "zh_yun", "��" },
    { "zh_zang", "��" },
    { "zh_zhe", "��" }
  };

  CharsIdentify* CharsIdentify::instance_ = nullptr;

  CharsIdentify* CharsIdentify::instance(){
    if (!instance_){
      instance_ = new CharsIdentify;
    }
    return instance_;
  }

  CharsIdentify::CharsIdentify() {
    ann_ = ml::ANN_MLP::load<ml::ANN_MLP>("resources/model/ann.xml");
  }

  string CharsIdentify::identify(Mat input, bool isChinese, bool isSpeci) {
    Mat f = features(input, kPredictSize);

    string result;
    int index = classify(f, isChinese, isSpeci);

    if (!isChinese) {
      result = result + kCharacters[index];
    }
    else {
      string s = kChinese[index - sizeof(kCharacters)];
      string province = zh_map[s];
      result = province + result;
    }

    return result;
  }

  int CharsIdentify::classify(Mat f, bool isChinses, bool isSpeci) {
    int result = -1;
    Mat output(1, sizeof(kCharacters) + sizeof(kChinese), CV_32FC1);
    ann_->predict(f, output);  //ʹ��ann���ַ����ж�

    if (!isChinses)  // �����ֺ�Ӣ����ĸ���ж�
    {
      if (isSpeci) {
        // A-Z
        result = 0;
        float maxVal = -2;
        for (int j = 10; j < sizeof(kCharacters); j++) {
          float val = output.at<float>(j);
          // cout << "j:" << j << "val:"<< val << endl;
          if (val > maxVal) {
            maxVal = val;
            result = j;
          }
        }
      }
      else {
        // 0-9
        result = 0;
        float maxVal = -2;
        for (int j = 0; j < sizeof(kCharacters); j++) {
          float val = output.at<float>(j);
          // cout << "j:" << j << "val:"<< val << endl;
          if (val >
            maxVal)  //��������ַ�Ȩ�������Ǹ���Ҳ����ͨ��ann��Ϊ����ܵ��ַ�
          {
            maxVal = val;
            result = j;
          }
        }
      }
    }
    else  // �������ַ����ж�
    {
      result = sizeof(kCharacters);
      float maxVal = -2;
      for (int j = sizeof(kCharacters); j < sizeof(kCharacters) + sizeof(sizeof(kChinese)); j++) {
        float val = output.at<float>(j);
        // cout << "j:" << j << "val:"<< val << endl;
        if (val > maxVal) {
          maxVal = val;
          result = j;
        }
      }
    }
    return result;
  }

  //int CCharsIdentify::classify(Mat f) {
  //  int result = -1;
  //  Mat output(1, numAll, CV_32FC1);
  //  ann.predict(f, output);  //ʹ��ann���ַ����ж�

  //  float maxVal = -2;
  //  for (int j = 0; j < numCharacter; j++) {
  //    float val = output.at<float>(j);
  //    // cout << "j:" << j << "val:"<< val << endl;
  //    if (val > maxVal)
  //      //��������ַ�Ȩ�������Ǹ���Ҳ����ͨ��ann��Ϊ����ܵ��ַ�
  //    {
  //      maxVal = val;
  //      result = j;
  //    }
  //  }

  //  for (int j = numCharacter; j < numAll; j++) {
  //    float val = output.at<float>(j);
  //    // cout << "j:" << j << "val:"<< val << endl;
  //    if (val > maxVal) {
  //      maxVal = val;
  //      result = j;
  //    }
  //  }

  //  return result;
  //}

  //string CCharsIdentify::charsIdentify(Mat input) {
  //  Mat f = features(input, m_predictSize);
  //  string result = "";
  //  int index = classify(f);  //ʹ��ann���б��Ǹ��ַ�

  //  if (index >= numCharacter) {
  //    string s = strChinese[index - numCharacter];
  //    string province = m_map[s];
  //    return s;
  //  }
  //  else {
  //    char s = strCharacters[index];
  //    char szBuf[216];
  //    sprintf(szBuf, "%c", s);
  //    return szBuf;
  //  }
  //}

}
