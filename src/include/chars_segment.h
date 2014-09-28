//////////////////////////////////////////////////////////////////////////
// Name:	    chars_segment Header
// Version:		1.0
// Date:	    2014-09-19
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsSegment
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_SEGMENT_H__
#define __CHARS_SEGMENT_H__

#include "prep.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CCharsSegment 
{
public:
	CCharsSegment();

	//! �ַ��ָ�
	int charsSegment(Mat, vector<Mat>& );

	//! �ַ��ߴ���֤
	bool verifySizes(Mat r);

	//! �ַ�Ԥ����
	Mat preprocessChar(Mat in);

	//! ����ֱ��ͼ
	Mat ProjectedHistogram(Mat img, int t);

	//! �����ַ����ض�����
	Mat features(Mat in, int sizeData);

	//! ֱ��ͼ���⣬Ϊ�жϳ�����ɫ��׼��
	Mat histeq(Mat in);

	//! ��ó�����ɫ
	int getPlateType(Mat input);

	//! ȥ��Ӱ���ַ�ʶ�������
	Mat clearLiuDing(Mat img);

	//! �������⳵��������²������ַ���λ�úʹ�С
	Rect GetChineseRect(const Rect rectSpe);

	//! �ҳ�ָʾ���е��ַ���Rect��������A7003X������A��λ��
	int GetSpecificRect(const vector<Rect>& vecRect);

	//! �����������������
	//  1.�������ַ�Rect��ߵ�ȫ��Rectȥ�����������ؽ������ַ���λ�á�
	//  2.�������ַ�Rect��ʼ������ѡ��6��Rect���������ȥ��
	int RebuildRect(const vector<Rect>& vecRect, vector<Rect>& outRect, int specIndex);

	//! ��Rect��λ�ô����ҽ�������
	int SortRect(const vector<Rect>& vecRect, vector<Rect>& out);

	//! preprocessChar���ó���
	static const int CHAR_SIZE = 20;
	static const int HORIZONTAL = 1;
	static const int VERTICAL = 0;

private:
	int m_NiuDingSize;
	int m_theMatWidth;
};

}	/* \namespace easypr  */

#endif /* endif __CHARS_SEGMENT_H__ */