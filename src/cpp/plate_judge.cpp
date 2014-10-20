#include "../include/plate_judge.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

CPlateJudge::CPlateJudge()
{
	//cout << "CPlateJudge" << endl;
	m_path = "model/svm.xml";
	LoadModel();
}

void CPlateJudge::LoadModel()
{
	svm.clear();
	svm.load(m_path.c_str(), "svm");
}

void CPlateJudge::LoadModel(string s)
{
	svm.clear();
	svm.load(s.c_str(), "svm");
}

//! ֱ��ͼ����
Mat CPlateJudge::histeq(Mat in)
{
	Mat out(in.size(), in.type());
	if(in.channels()==3)
	{
		Mat hsv;
		vector<Mat> hsvSplit;
		cvtColor(in, hsv, CV_BGR2HSV);
		split(hsv, hsvSplit);
		equalizeHist(hsvSplit[2], hsvSplit[2]);
		merge(hsvSplit, hsv);
		cvtColor(hsv, out, CV_HSV2BGR);
	}
	else if(in.channels()==1)
	{
		equalizeHist(in, out);
	}
	return out;
}


//! ʹ��ɫ��ͼ����SVM�ж�
int CPlateJudge::plateJudge(const vector<Mat>& inVec,
								  vector<Mat>& resultVec)
{
	int num = inVec.size();
	for (int j = 0; j < num; j++)
	{
		Mat inMat = inVec[j];

		//ͨ��ɫ��ֱ��ͼ����Ԥ��
		Mat p = histeq(inMat).reshape(1, 1);
		p.convertTo(p, CV_32FC1);
		int response = (int)svm.predict(p);
		if (response == 1)
		{
			resultVec.push_back(inMat);
		}
	}
	return 0;
}

}	/*! \namespace easypr*/
