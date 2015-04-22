#include "../include/plate_recognize.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

CPlateRecognize::CPlateRecognize()
{
	//cout << "CPlateRecognize" << endl;
	//m_plateDetect= new CPlateDetect();
	//m_charsRecognise = new CCharsRecognise();
}

////! װ��SVMģ��
//void CPlateRecognize::LoadSVM(string strSVM)
//{
//	m_plateDetect->LoadModel(strSVM.c_str());
//}
//
////! װ��ANNģ��
//void CPlateRecognize::LoadANN(string strANN)
//{
//	m_charsRecognise->LoadModel(strANN.c_str());
//}
//
//int CPlateRecognize::plateDetect(Mat src, vector<Mat>& resultVec)
//{
//	int result = m_plateDetect->plateDetect(src, resultVec);
//	return result;
//}
//
//int CPlateRecognize::charsRecognise(Mat plate, string& plateLicense)
//{
//	int result = m_charsRecognise->charsRecognise(plate, plateLicense);
//	return result;
//}

int CPlateRecognize::plateRecognize(Mat src, vector<string>& licenseVec,int index)
{
	// ���Ʒ��鼯��
	vector<CPlate> plateVec;
	
	
	// ���������Debugģʽ����������ʾ���е�ͼƬ
	bool showDetectArea = getPDDebug();
	showDetectArea=0;
	// ������ȶ�λ��ʹ����ɫ��Ϣ�����Sobel
	int resultPD = plateDetectDeep(src, plateVec, showDetectArea, 0);

	Mat result;
	src.copyTo(result);

	if (resultPD == 0)
	{
		int num = plateVec.size();

		int resultCR = 0;

		int index = 0;
		for (int j = 0; j < num; j++)
		{
			CPlate item = plateVec[j];
			
			Mat plate = item.getPlateMat();
			
			//��ȡ������ɫ
			string plateType = getPlateColor(plate);

			//��ȡ���ƺ�
			string plateIdentify = "";
			int resultCR = charsRecognise(plate, plateIdentify);
			if (resultCR == 0)
			{
				string license = plateType + ":" + plateIdentify;
				licenseVec.push_back(license);


				//int height = 36;
				//int width = 136;
				//if(height*index + height < result.rows)
				//{
				//	Mat imageRoi = result(Rect(0, 0 + height*index, width, height));
				//	addWeighted(imageRoi, 0, plate, 1, 0, imageRoi);
				//	 
				//	CvxText text("simhei.ttf");
				//	float p = 0.5;

				//	CvScalar size(cvScalar(8,0.5,0.1));

				//	text.setFont(NULL, &size, NULL, &p);   // ͸������

				//	text.putText(result, license.c_str(), Point(width,height*(index+1)));


				//	
				//}
				//index++;


				RotatedRect minRect = item.getPlatePos();
				Point2f rect_points[4]; 
				minRect.points( rect_points );

				if(item.bColored)
				{
					for (int j = 0; j < 4; j++)
					{
						line(result, rect_points[j], rect_points[(j + 1) % 4], Scalar(255, 255, 0), 2, 8);
						//��ɫ��λ���ƣ���ɫ����
					}
				}
				else
				{
					for( int j = 0; j < 4; j++ )
					{
						line(result, rect_points[j], rect_points[(j+1)%4], Scalar(0,0,255), 2, 8 );//sobel��λ���ƣ���ɫ����
					}

				}
				
			
			}
		}
	}


	showResult(result);


	return resultPD;
}

}	/*! \namespace easypr*/

