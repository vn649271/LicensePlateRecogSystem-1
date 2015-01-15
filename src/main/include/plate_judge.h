//////////////////////////////////////////////////////////////////////////
// Name:	    plate_judge Header
// Version:		1.0
// Date:	    2014-09-22
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:  
// Defines CPlateLocate
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_JUDGE_H__
#define __PLATE_JUDGE_H__

#include "prep.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateJudge 
{
public:
	CPlateJudge();

	//! �����ж�
	int plateJudge(const vector<Mat>&, vector<Mat>&);

	//! ֱ��ͼ����
	Mat histeq(Mat);

	//! װ��SVMģ��
	void LoadModel();

	//! װ��SVMģ��
	void LoadModel(string s);

	//! �������ȡģ��·��
	inline void setModelPath(string path){	m_path = path;	}
	inline string getModelPath() const{	 return m_path;	}

private:
	//��ʹ�õ�SVMģ��
	CvSVM svm;

	//! ģ�ʹ洢·��
	string m_path;
};

}	/*! \namespace easypr*/

#endif /* endif __PLATE_JUDGE_H__ */