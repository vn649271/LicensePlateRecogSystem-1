#include "easypr/chars_identify.h"
#include "easypr/core_func.h"
/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

//�й�����
const char strCharacters[] = {
        '0', '1', '2', '3', '4', '5', \
    '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', /* û��I */\
    'J', 'K', 'L', 'M', 'N', /* û��O */ 'P', 'Q', 'R', 'S', 'T', \
    'U', 'V', 'W', 'X', 'Y', 'Z'
};
const int numCharacter = 34; /* û��I��0,10��������24��Ӣ���ַ�֮�� */

//���¶�����ѵ��ʱ�õ��������ַ����ݣ�����ȫ�棬��Щʡ��û��ѵ����������û���ַ�
//��Щ���������2�ı�ʾ��ѵ��ʱ�������ַ���һ�ֱ��Σ�Ҳ��Ϊѵ�����ݴ洢
const std::string strChinese[] = {
        "zh_cuan" /* �� */, "zh_e" /* �� */, "zh_gan" /* ��*/, \
    "zh_gan1"/*��*/, "zh_gui"/* �� */, "zh_gui1"/* �� */, \
    "zh_hei" /* �� */, "zh_hu" /* �� */, "zh_ji" /* �� */, \
    "zh_jin" /* �� */, "zh_jing" /* �� */, "zh_jl" /* �� */, \
    "zh_liao" /* �� */, "zh_lu" /* ³ */, "zh_meng" /* �� */, \
    "zh_min" /* �� */, "zh_ning" /* �� */, "zh_qing" /* �� */, \
    "zh_qiong" /* �� */, "zh_shan" /* �� */, "zh_su" /* �� */, \
    "zh_sx" /* �� */, "zh_wan" /* �� */, "zh_xiang" /* �� */, \
    "zh_xin" /* �� */, "zh_yu" /* ԥ */, "zh_yu1" /* �� */, \
    "zh_yue" /* �� */, "zh_yun" /* �� */, "zh_zang" /* �� */, \
    "zh_zhe" /* �� */};

const int numAll = 65;

/* 34+20=54 */

CCharsIdentify::CCharsIdentify() {
  m_predictSize = 10;
  m_path = "resources/model/ann.xml";
  LoadModel();

  if (m_map.empty()) {
    m_map.insert(std::pair<std::string, std::string>("zh_cuan", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_e", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_gan", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_gan1", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_gui", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_gui1", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_hei", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_hu", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_ji", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_jin", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_jing", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_jl", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_liao", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_lu", "³"));
    m_map.insert(std::pair<std::string, std::string>("zh_meng", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_min", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_ning", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_qing", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_qiong", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_shan", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_su", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_sx", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_wan", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_xiang", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_xin", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_yu", "ԥ"));
    m_map.insert(std::pair<std::string, std::string>("zh_yu1", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_yue", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_yun", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_zang", "��"));
    m_map.insert(std::pair<std::string, std::string>("zh_zhe", "��"));
  }
}


void CCharsIdentify::LoadModel() {
  ann.clear();
  ann.load(m_path.c_str(), "ann");
}

void CCharsIdentify::LoadModel(std::string s) {
  ann.clear();
  ann.load(s.c_str(), "ann");
}


int CCharsIdentify::classify(cv::Mat f, bool isChinses, bool isSpeci) {
  int result = -1;
  cv::Mat output(1, numAll, CV_32FC1);
  ann.predict(f, output);//ʹ��ann���ַ����ж�

  if (!isChinses)//�������ַ����ж�
  {
    if (isSpeci) {
      result = 0;
      float maxVal = -2;
      for (int j = 10; j < numCharacter; j++) {
        float val = output.at<float>(j);
        //cout << "j:" << j << "val:"<< val << endl;
        if (val > maxVal) {
          maxVal = val;
          result = j;
        }
      }

    }
    else {
      result = 0;
      float maxVal = -2;
      for (int j = 0; j < numCharacter; j++) {
        float val = output.at<float>(j);
        //cout << "j:" << j << "val:"<< val << endl;
        if (val > maxVal)//��������ַ�Ȩ�������Ǹ���Ҳ����ͨ��ann��Ϊ����ܵ��ַ�
        {
          maxVal = val;
          result = j;
        }
      }
    }
  }
  else//�����ֺ�Ӣ����ĸ���ж�
  {

    result = numCharacter;
    float maxVal = -2;
    for (int j = numCharacter; j < numAll; j++) {
      float val = output.at<float>(j);
      //cout << "j:" << j << "val:"<< val << endl;
      if (val > maxVal) {
        maxVal = val;
        result = j;
      }
    }

  }
  return result;
}

int CCharsIdentify::classify(cv::Mat f) {
  int result = -1;
  cv::Mat output(1, numAll, CV_32FC1);
  ann.predict(f, output);//ʹ��ann���ַ����ж�



  float maxVal = -2;
  for (int j = 0; j < numCharacter; j++) {
    float val = output.at<float>(j);
    //cout << "j:" << j << "val:"<< val << endl;
    if (val > maxVal)
      //��������ַ�Ȩ�������Ǹ���Ҳ����ͨ��ann��Ϊ����ܵ��ַ�
    {
      maxVal = val;
      result = j;
    }
  }


  for (int j = numCharacter; j < numAll; j++) {
    float val = output.at<float>(j);
    //cout << "j:" << j << "val:"<< val << endl;
    if (val > maxVal) {
      maxVal = val;
      result = j;
    }
  }


  return result;
}

std::string CCharsIdentify::charsIdentify(cv::Mat input) {
  cv::Mat f = features(input, m_predictSize);
  std::string result = "";
  int index = classify(f);//ʹ��ann���б��Ǹ��ַ�

  if (index >= numCharacter) {
    std::string s = strChinese[index - numCharacter];
    std::string province = m_map[s];
    return s;
  }
  else {
    char s = strCharacters[index];
    char szBuf[216];
    sprintf(szBuf, "%c", s);
    return szBuf;
  }

}

//���뵱���ַ�Mat,�����ַ���std::string
std::string CCharsIdentify::charsIdentify(cv::Mat input, bool isChinese,
                                          bool isSpeci) {
  cv::Mat f = features(input, m_predictSize);
  std::string result = "";
  int index = classify(f, isChinese, isSpeci);//ʹ��ann���б��Ǹ��ַ�

  if (!isChinese) {
    result = result + strCharacters[index];
  }
  else {
    std::string s = strChinese[index - numCharacter];
    std::string province = m_map[s];
    result = province + result;
  }

  return result;
}


}  /*! \namespace easypr*/
