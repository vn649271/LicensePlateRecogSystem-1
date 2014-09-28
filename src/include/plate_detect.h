//////////////////////////////////////////////////////////////////////////
// Name:	    plate_detect Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:  
// Defines CPlateDetect
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_DETECT_H__
#define __PLATE_DETECT_H__

#include "prep.h"
#include "plate_locate.h"
#include "plate_judge.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateDetect 
{
public:
	CPlateDetect();

	//! ���ƶ�λ���ж�
	int plateDetect(Mat, vector<Mat>&);

	//! װ��SVMģ��
	void LoadModel(string s);

private:
	//�����ƶ�λ
	CPlateLocate* m_plateLocate;

	//! �����ж�
	CPlateJudge* m_plateJudge;
};

}	/*! \namespace easypr*/

#endif /* endif __PLATE_DETECT_H__ */