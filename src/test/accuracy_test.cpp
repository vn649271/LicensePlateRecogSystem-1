// ͨ����ȷ�ʲ����ļ�
// AcurayTest��Ӧ��main���������е�ѡ��2

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/features.h"

using namespace easypr;

int acurayTest()
{
	////��ȡ��·���µ������ļ�
	vector<string> files;
	getFiles("image/general_test", files);

	CPlateLocate lo;
	CPlateJudge ju;
	CPlateRecognize pr;

	pr.LoadANN("model/ann.xml");
	pr.LoadSVM("model/svm.xml");
	pr.setLifemode(true);

	int size = files.size();
	//int size = 200;

	if (0 == size)
		cout << "No File Found in general_test!" << endl;

	cout << "Begin to test the easypr accuracy!" << endl;

	for (int i = 0; i < size; i++)
	{
		string filepath = files[i].c_str();
		cout << "------------------" << endl;

		// ��ȡ��ʵ�ĳ���
		string plateLicense = "";
		getFileName(filepath, plateLicense);

		cout << "ԭ��:" << plateLicense << endl;

		// EasyPR��ʼ�жϳ���
		Mat src = imread(filepath);
		vector<string> plateVec;
		int result = pr.plateRecognize(src, plateVec);
		if (result == 0)
		{
			int num = plateVec.size();

			if (num == 0)
			{
				cout << ""<< "�޳���" <<endl;
			} 
			else if ( num > 1)
			{
				for (int j = 0; j < num; j++)
					cout << ""<< plateVec[j] << " (" << j+1 << ")"<<endl;			
			}
			else
			{
				for (int j = 0; j < num; j++)
					cout << ""<< plateVec[j] <<endl;			
			}
		} 
		else
		{
			cout << "������:" << result << endl;
		}

	}

	return 0;
}