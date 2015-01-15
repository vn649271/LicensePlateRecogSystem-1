#include "../include/plate_recognize.h"

using namespace easypr;

/// test.cpp�з���
int testMain();

/// data_prepare.cpp�з���
void getLearnData();

const string option[] = 
	{
		"1. ����learndata;"	,
		"2. ����EasyPR;"		,  
		"3. �˳�;"			,  
	};

const int optionCount = 3;

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
				getLearnData();
				break;
			case 2:
				testMain();
				break;
			case 3:
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
