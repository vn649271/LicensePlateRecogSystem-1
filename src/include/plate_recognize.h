//////////////////////////////////////////////////////////////////////////
// Name:	    plate_recognize Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CPlateRecognize
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_RECOGNIZE_H__
#define __PLATE_RECOGNIZE_H__

#include "prep.h"
#include "plate_detect.h"
#include "chars_recognise.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateRecognize : public CPlateDetect, public CCharsRecognise
{
public:
	CPlateRecognize();

	//! ���Ƽ�����ַ�ʶ��
	int plateRecognize(Mat, vector<string>&);

	////! ���ƶ�λ���ж�
	//int plateDetect(Mat, vector<Mat>&);

	////! �ַ��ָ���ʶ��
	//int charsRecognise(Mat, String&);

	////! װ��SVM
	//void LoadSVM(string s);

	////! װ��ANNģ��
	//void LoadANN(string s);

private:
	////�����Ƽ��
	//CPlateDetect* m_plateDetect;

	////! �ַ�ʶ��
	//CCharsRecognise* m_charsRecognise;
};

}	/* \namespace easypr  */

#endif /* endif __PLATE_RECOGNITION_H__ */