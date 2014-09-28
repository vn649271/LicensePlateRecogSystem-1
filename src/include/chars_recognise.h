//////////////////////////////////////////////////////////////////////////
// Name:	    chars_recognise Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsRecognise
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_RECOGNISE_H__
#define __CHARS_RECOGNISE_H__

#include "prep.h"
#include "chars_segment.h"
#include "chars_identify.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CCharsRecognise 
{
public:
	CCharsRecognise();

	//! �ַ��ָ���ʶ��
	int charsRecognise(Mat, String&);

	//! װ��ANNģ��
	void LoadModel(string s);

private:
	//���ַ��ָ�
	CCharsSegment* m_charsSegment;

	//! �ַ�ʶ��
	CCharsIdentify* m_charsIdentify;
};

}	/* \namespace easypr  */

#endif /* endif __CHARS_RECOGNISE_H__ */