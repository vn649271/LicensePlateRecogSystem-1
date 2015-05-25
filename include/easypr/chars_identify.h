//////////////////////////////////////////////////////////////////////////
// Name:	    chars_identify Header
// Version:		1.0
// Date:	    2014-09-25
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsIdentify 
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_IDENTIFY_H__
#define __CHARS_IDENTIFY_H__

#include <opencv2/opencv.hpp>
#include <string>

namespace easypr {

class CCharsIdentify {
 public:
  CCharsIdentify();

  //! �ַ�����
  std::string charsIdentify(cv::Mat, bool, bool);

  std::string charsIdentify(cv::Mat input);

  //! �ַ�����
  int classify(cv::Mat, bool, bool);

  int classify(cv::Mat f);

  //! װ��ANNģ��
  void LoadModel();

  //! װ��ANNģ��
  void LoadModel(std::string s);

  //! �������ȡģ��·��
  inline void setModelPath(std::string path) { m_path = path; }

  inline std::string getModelPath() const { return m_path; }

 private:
  //��ʹ�õ�ANNģ��
  CvANN_MLP ann;

  //! ģ�ʹ洢·��
  std::string m_path;

  //! �����ߴ�
  int m_predictSize;

  //! ʡ�ݶ�Ӧmap
  std::map<std::string, std::string> m_map;
};

}  /* \namespace easypr  */

#endif /* endif __CHARS_IDENTIFY_H__ */