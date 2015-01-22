#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/features.h"

using namespace easypr;

int svmMain();

namespace easypr {

	int svmTrain(bool dividePrepared = true, bool trainPrepared = true,
	svmCallback getFeatures = getHistogramFeatures);

}


////////////////////////////////////////////////////////////
// EasyPR ѵ��������

const string option[] = 
	{
		"1. ����;"	,
		"2. ��������(δ����);"		,
		"3. SVMѵ��;"		,
		"4. ANNѵ��(δ����);"		,
		"5. �����Ŷ�;"		,
		"6. ��л����;"		,
		"7. �˳�;"			,  
	};

const int optionCount = 7;

int main()
{
	bool isExit = false;
	while (isExit != true)
	{
		stringstream selectOption(stringstream::in | stringstream::out);
		selectOption << "EasyPR Option:"<< endl;
		for(int i = 0; i < optionCount; i++)
		{
			selectOption << option[i] << endl;
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
				testMain();
				break;
			case 2:
				// TODO
				break;
			case 3:
				svmMain();
				break;
			case 4:
				// TODO
				break;
			case 5:
				// TODO;
				break;
			case 6:
				// ��л����
				break;
			case 7:
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
// /EasyPR ѵ�������� ����
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// SVM ѵ�������� 

const string svm_option[] = 
	{
		"1. ����learndata;"	,
		"2. ��ǩlearndata;"		,
		"3. ���Ƽ��(not divide and train);"		,
		"4. ���Ƽ��(not train);"		,
		"5. ���Ƽ��(not divide);"		,
		"6. ���Ƽ��;"		,
		"7. ����;"			,  
	};

const int svm_optionCount = 7;

int svmMain()
{
	bool isExit = false;
	while (isExit != true)
	{
		stringstream selectOption(stringstream::in | stringstream::out);
		selectOption << "svmMain Option:"<< endl;
		for(int i = 0; i < svm_optionCount; i++)
		{
			selectOption << svm_option[i] << endl;
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
				getLearnData();
				break;
			case 2:
				label_data();
				break;
			case 3:
				svmTrain(false, false);
				break;
			case 4:
				svmTrain(true, false);
				break;
			case 5:
				svmTrain(false, true);
				break;
			case 6:
				svmTrain();
				break;
			case 7:
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

// SVM ѵ�������� 
////////////////////////////////////////////////////////////