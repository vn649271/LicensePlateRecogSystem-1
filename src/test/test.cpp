#include "../include/plate_locate.h"
#include "../include/plate_judge.h"
#include "../include/chars_segment.h"
#include "../include/chars_identify.h"

#include "../include/plate_detect.h"
#include "../include/chars_recognise.h"

#include "../include/plate_recognize.h"

using namespace easypr;

int test_plate_locate();
int test_plate_judge();
int test_chars_segment();
int test_chars_identify();
int test_plate_detect();
int test_chars_recognise();
int test_plate_recognize();
int testMain();

//����Ҫ���Ե�ͼƬ��ַд������
const string img_plate_locate = "image/test.jpg";
const string img_plate_detect = "image/test.jpg";
const string img_chars_segment = "image/test.jpg";
const string img_plate_recognize = "image/test.jpg";

const string testOption[] = 
	{
		"1. test plate_locate(���ƶ�λ);"		/* ���ƶ�λ */,
		"2. test plate_judge(�����ж�);"		/* �����ж� */,  
		"3. test plate_detect(���Ƽ��);"		/* ���Ƽ�⣨�������ƶ�λ�복���жϣ� */, 
		"4. test chars_segment(�ַ��ָ�);"		/* �ַ��ָ� */, 
		"5. test chars_identify(�ַ�����);"		/* �ַ����� */,  
		"6. test chars_recognise(�ַ�ʶ��);"		/* �ַ�ʶ�𣨰����ַ��ָ����ַ����� */,
		"7. test plate_recognize(����ʶ��);"		/* ����ʶ�� */, 
		"8. test all(����ȫ��);"		/* ����ȫ�� */,
		"9. ����;"		/* �˳� */,
	};

const int testOptionCount = 9;

int testMain()
{
	bool isExit = false;
	while (isExit != true)
	{
		stringstream selectOption(stringstream::in | stringstream::out);
		selectOption << "EasyPR Test:"<< endl;
		for(int i = 0; i < testOptionCount; i++)
		{
			selectOption << testOption[i] << endl;
		}

		cout << "////////////////////////////////////"<< endl;
		cout << selectOption.str();
		cout << "////////////////////////////////////"<< endl;
		cout << "��ѡ��һ�����:";

		int select = -1;
		bool isRepeat = true;
		while (isRepeat)
		{
			cin >> select;
			isRepeat = false;
			switch (select)
			{
			case 1:
				assert (test_plate_locate() == 0);
				break;
			case 2:
				assert (test_plate_judge() == 0);
				break;
			case 3:
				assert (test_plate_detect() == 0);
				break;
			case 4:
				assert (test_chars_segment() == 0);
				break;
			case 5:
				assert (test_chars_identify() == 0);
				break;
			case 6:
				assert (test_chars_recognise() == 0);
				break;
			case 7:
				assert (test_plate_recognize() == 0);
				break;
			case 8:
				assert (test_plate_locate() == 0);
				assert (test_plate_judge() == 0);
				assert (test_plate_detect() == 0);

				assert (test_chars_segment() == 0);
				assert (test_chars_identify() == 0);
				assert (test_chars_recognise() == 0);

				assert (test_plate_recognize() == 0);
				break;
			case 9:
				isExit = true;
				break;
			default:
				cout << "�����������������:";
				isRepeat = true;
				break;
			}
		}
	}

	return 0;
}


int test_plate_locate()
{
	cout << "test_plate_locate" << endl;

	Mat src = imread(img_plate_locate);

	vector<Mat> resultVec;
	CPlateLocate plate;
	plate.setDebug(1);
	plate.setLifemode(true);

	vector<Mat> resultPlates;

	vector<CPlate> color_Plates;
	vector<CPlate> sobel_Plates;
	vector<CPlate> color_result_Plates;
	vector<CPlate> sobel_result_Plates;

	vector<CPlate> all_result_Plates;

	const int color_find_max = 4;

	plate.plateColorLocate(src, color_Plates);
	for (int i = 0; i<color_Plates.size(); ++i)
		color_result_Plates.push_back(color_Plates[i]);

	for (int i = 0; i< color_result_Plates.size(); i++)
	{
		CPlate plate = color_result_Plates[i];
		all_result_Plates.push_back(plate);
	}

	//��ɫ�ͱ߽�ղ���ͬʱ����
	{
		plate.plateSobelLocate(src, sobel_Plates);

		for (int i = 0; i<sobel_Plates.size(); ++i)
		{
			sobel_result_Plates.push_back(sobel_Plates[i]);
		}

		for (int i = 0; i< sobel_result_Plates.size(); i++)
		{
			CPlate plate = sobel_result_Plates[i];
			plate.bColored = false;

			all_result_Plates.push_back(plate);
		}
	}

	for (int i = 0; i < all_result_Plates.size(); i++)
	{
		// �ѽ�ȡ�ĳ���ͼ�����ηŵ����Ͻ�
		CPlate plate = all_result_Plates[i];
		Mat resultMat = plate.getPlateMat();
		imshow("plate_locate", resultMat);
		waitKey(0);
	}

	destroyWindow("plate_locate");

	/*int result = plate.plateLocate(src, resultVec);
	if (result == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			imshow("plate_locate", resultMat);
			waitKey(0);
		}
        destroyWindow("plate_locate");
	}*/

	return 0;
}

int test_plate_judge()
{
	cout << "test_plate_judge" << endl;

	Mat src = imread("image/plate_judge.jpg");

	//�����ǳ��Ƶ�ͼ�鼯��
	vector<Mat> matVec;

	//����SVM�жϺ�õ���ͼ�鼯��
	vector<Mat> resultVec;

	CPlateLocate lo;
	lo.setDebug(1);
	lo.setLifemode(true);

	int resultLo = lo.plateLocate(src, matVec);

	if (0 != resultLo)
		return -1;

	cout << "plate_locate_img" << endl;
	if (resultLo == 0)
	{
		int num = matVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = matVec[j];
			imshow("plate_judge", resultMat);
			waitKey(0);
		}
        destroyWindow("plate_judge");
	}

	CPlateJudge ju;
	int resultJu = ju.plateJudge(matVec, resultVec);

	if (0 != resultJu)
		return -1;

	cout << "plate_judge_img" << endl;
	if (resultJu == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			imshow("plate_judge", resultMat);
			waitKey(0);
		}
        destroyWindow("plate_judge");
	}

	return resultJu;
}

int test_chars_segment()
{
	cout << "test_chars_segment" << endl;

	Mat src = imread(img_chars_segment);

	vector<CPlate> resultVec;

	CPlateDetect pd;
	CCharsSegment cs;
	pd.setPDLifemode(true);

	int result = pd.plateDetectDeep(src, resultVec);
	if (result == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			CPlate plate = resultVec[j];

			imshow("plate_detect", plate.getPlateMat());
			waitKey(0);

			vector<Mat> resultChars;

			int result_cs = cs.charsSegment(plate.getPlateMat(), resultChars);
			if (result_cs == 0)
			{
				int num_cs = resultChars.size();
				for (int p = 0; p < num_cs; p++)
				{
					Mat resultChar = resultChars[p];
					imshow("chars_segment", resultChar);
					waitKey(0);
				}
				destroyWindow("chars_segment");
			}

		}
		destroyWindow("plate_detect");
	}

	return result;
}

int test_chars_identify()
{
	cout << "test_chars_identify" << endl;

	Mat src = imread("image/chars_identify.jpg");

	vector<Mat> resultVec;
	CCharsSegment cs;
	CCharsIdentify ci;

	string plateIdentify = "";

	int result = cs.charsSegment(src, resultVec);
	if (result == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			bool isChinses = false;
			bool isSpec = false;

			//Ĭ���׸��ַ����������ַ�
			if (j == 0)
				isChinses = true;

			if (j == 1)
				isSpec = true;

			string charcater = ci.charsIdentify(resultMat, isChinses, isSpec);
			plateIdentify = plateIdentify + charcater;
		}
	}

	const string plateLicense = "��E771H6";

	cout << "plateLicense: " << plateLicense << endl;
	cout << "plateIdentify: " << plateIdentify << endl;

	if (plateLicense != plateIdentify)
	{
		cout << "Identify Not Correct!" << endl;
		return -1;
	}
	cout << "Identify Correct!" << endl;

	return result;
}


int test_plate_detect()
{
	cout << "test_plate_detect" << endl;

	Mat src = imread(img_plate_detect);

	vector<CPlate> resultVec;
	CPlateDetect pd;
	pd.setPDLifemode(true);

	int result = pd.plateDetectDeep(src, resultVec);
	if (result == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			CPlate resultMat = resultVec[j];
			
			imshow("plate_detect", resultMat.getPlateMat());
			waitKey(0);
		}
		destroyWindow("plate_detect");
	}

	return result;
}


int test_chars_recognise()
{
	cout << "test_chars_recognise" << endl;

	Mat src = imread("image/chars_recognise.jpg");

	CCharsRecognise cr;
	string charsRecognise = "";

	int result = cr.charsRecognise(src, charsRecognise);
	if (result == 0)
	{
		cout << "charsRecognise: " << charsRecognise << endl;
	}

	return result;
}

int test_plate_recognize()
{
	cout << "test_plate_recognize" << endl;

	Mat src = imread(img_plate_recognize);

	CPlateRecognize pr;
	pr.LoadANN("model/ann.xml");
	pr.LoadSVM("model/svm.xml");

	pr.setLifemode(true);
	pr.setDebug(true);

	vector<string> plateVec;

	int result = pr.plateRecognize(src, plateVec);
	if (result == 0)
	{
		int num = plateVec.size();
		for (int j = 0; j < num; j++)
		{
			cout << "plateRecognize: " << plateVec[j] << endl;			
		}
	}

	if (result != 0)
		cout << "result:" << result << endl;

	return result;
}
