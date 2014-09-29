#include "../include/plate_locate.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

const float DEFAULT_ERROR = 0.6;
const float DEFAULT_ASPECT = 3.75; 

CPlateLocate::CPlateLocate()
{
	//cout << "CPlateLocate" << endl;
	m_GaussianBlurSize = DEFAULT_GAUSSIANBLUR_SIZE;
	m_MorphSizeWidth = DEFAULT_MORPH_SIZE_WIDTH;
	m_MorphSizeHeight = DEFAULT_MORPH_SIZE_HEIGHT;

	m_error = DEFAULT_ERROR;
	m_aspect = DEFAULT_ASPECT;
	m_verifyMin = DEFAULT_VERIFY_MIN;
	m_verifyMax = DEFAULT_VERIFY_MAX;

	m_angle = DEFAULT_ANGLE;
}

//! ��minAreaRect��õ���С��Ӿ��Σ����ݺ�Ƚ����ж�
bool CPlateLocate::verifySizes(RotatedRect mr)
{
	float error = m_error;
	//Spain car plate size: 52x11 aspect 4,7272
	//China car plate size: 440mm*140mm��aspect 3.142857
	float aspect = m_aspect;
	//Set a min and max area. All other patchs are discarded
	//int min= 1*aspect*1; // minimum area
	//int max= 2000*aspect*2000; // maximum area
	int min= 44*14*m_verifyMin; // minimum area
	int max= 44*14*m_verifyMax; // maximum area
	//Get only patchs that match to a respect ratio.
	float rmin= aspect-aspect*error;
	float rmax= aspect+aspect*error;

	int area= mr.size.height * mr.size.width;
	float r = (float)mr.size.width / (float)mr.size.height;
	if(r < 1)
	{
		r= (float)mr.size.height / (float)mr.size.width;
	}

	if(( area < min || area > max ) || ( r < rmin || r > rmax ))
	{
		return false;
	}
	else
	{
		return true;
	}
}

//! ��ʾ�������ɵĳ���ͼ�񣬱����ж��Ƿ�ɹ���������ת��
Mat CPlateLocate::showResultMat(Mat src, Size rect_size, Point2f center)
{
	Mat img_crop;
	getRectSubPix(src, rect_size, center, img_crop);

	Mat resultResized;
	resultResized.create(HEIGHT, WIDTH, TYPE);

	resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);

	return resultResized;
}

//! ��λ����ͼ��
//! src ԭʼͼ��
//! resultVec һ��Mat���������洢����ץȡ����ͼ��
//! �ɹ�����0�����򷵻�-1
int CPlateLocate::plateLocate(Mat src, vector<Mat>& resultVec)
{
	Mat src_blur, src_gray;
	Mat grad;

	int scale = SOBEL_SCALE;
	int delta = SOBEL_DELTA;
	int ddepth = SOBEL_DDEPTH;

	if( !src.data )
	{ return -1; }

	//��˹���⡣Size�е�����Ӱ�쳵�ƶ�λ��Ч����
	GaussianBlur( src, src_blur, Size(m_GaussianBlurSize, m_GaussianBlurSize), 
		0, 0, BORDER_DEFAULT );

	/// Convert it to gray
	cvtColor( src_blur, src_gray, CV_RGB2GRAY );

	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );

	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );

	/// Total Gradient (approximate)
	addWeighted( abs_grad_x, SOBEL_X_WEIGHT, abs_grad_y, SOBEL_Y_WEIGHT, 0, grad );

	Mat img_threshold;
	threshold(grad, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
	//threshold(grad, img_threshold, 75, 255, CV_THRESH_BINARY);

	Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight) );
	morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);
	
	//Find ���� of possibles plates
	vector< vector< Point> > contours;
	findContours(img_threshold,
		contours, // a vector of contours
		CV_RETR_EXTERNAL, // ��ȡ�ⲿ����
		CV_CHAIN_APPROX_NONE); // all pixels of each contours

	//Start to iterate to each contour founded
	vector<vector<Point> >::iterator itc = contours.begin();
	
	vector<RotatedRect> rects;
	//Remove patch that are no inside limits of aspect ratio and area.
	int t = 0;
	while (itc != contours.end())
	{
		//Create bounding rect of object
		RotatedRect mr = minAreaRect(Mat(*itc));

		//large the rect for more
		if( !verifySizes(mr))
		{
			itc = contours.erase(itc);
		}
		else
		{
			++itc;
			rects.push_back(mr);
		}
	}

	for(int i=0; i< rects.size(); i++)
	{
		RotatedRect minRect = rects[i];
		if(verifySizes(minRect))
		{	
			// rotated rectangle drawing 
			// Get rotation matrix
			// ��ת�ⲿ�ִ���ȷʵ���Խ�ĳЩ��б�ĳ��Ƶ�������
			// ������Ҳ���󽫸������ĳ��Ƹ����б�������ۺϿ��ǣ����ǲ�ʹ����δ��롣
			// 2014-08-14,�����µ���һ��ͼƬ�з����кܶ೵������б�ģ���˾����ٴγ���
			// ��δ��롣
			float r = (float)minRect.size.width / (float)minRect.size.height;
			float angle = minRect.angle;
			Size rect_size = minRect.size;
			if (r < 1)
			{
				angle = 90 + angle;
				swap(rect_size.width, rect_size.height);
			}
			//���ץȡ�ķ�����ת����m_angle�Ƕȣ����ǳ��ƣ���������
			if (angle - m_angle < 0 && angle + m_angle > 0)
			{
				//Create and rotate image
				Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);
				Mat img_rotated;
				warpAffine(src, img_rotated, rotmat, src.size(), CV_INTER_CUBIC);

				Mat resultMat;
				resultMat = showResultMat(img_rotated, rect_size, minRect.center);

				resultVec.push_back(resultMat);
			}
		}
	}
	return 0;
}

}	/*! \namespace easypr*/