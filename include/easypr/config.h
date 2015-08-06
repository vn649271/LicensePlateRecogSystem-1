#ifndef EASYPR_TRAIN_TRAIN_H_
#define EASYPR_TRAIN_TRAIN_H_

namespace easypr{

  static const char *kDefaultSvmPath = "resources/model/svm.xml";
  static const char *kDefaultAnnPath = "resources/model/ann.xml";

  static const int kPredictSize = 10;

  static const char * kChinese[] = {
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

  static const char kCharacters[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', /* û��I */
    'J', 'K', 'L', 'M', 'N', /* û��O */ 'P', 'Q',
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
  };

}



#endif // EASYPR_TRAIN_TRAIN_H_