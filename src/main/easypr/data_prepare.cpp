// data_prepare.cpp : 
// ����Ԥ����ĳ�����Ҫ������������
// 1.��ȡԭ������ rawdata�����������������
// 2.���/ѡ���Ե�ѡȡ�������ݳ�Ϊlearndata��������ݲ������ã�һ������Ϊ1000��10000������1����

#include <iostream>
#include <cstdlib>
#include <io.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

using namespace std;
using namespace cv;

const int LEARANDATA_COUNT = 1100;

void getFiles( string path, vector<string>& files );

//��ȥ�ϲ���ײ����ŵ�ϸ��
Mat cutBottom(Mat img)
{
	int width = img.size().width;
	int height = img.size().height;
	Rect rect(0, 0.00, width*1, height*0.98);
	Mat dst = img(rect);
	return dst;
}

//C++��spilt����
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while(std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2-pos1));
 
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if(pos1 != s.length())
		v.push_back(s.substr(pos1));
}

// filepathʾ����F:\data\easypr-data\learndata\20150110132005-210028-S18-H3952K.jpg
bool isNotNight(const string& filepath)
{
	vector<string> spilt_path;
	SplitString(filepath, spilt_path, "\\");

	int spiltsize = spilt_path.size();
	string filename = "";
	if (spiltsize != 0)
	{
		filename = spilt_path[spiltsize-1];
		if (filename != "")
		{
			vector<string> spilt_name;
			SplitString(filename, spilt_name, "-");

			int name_size = spilt_name.size();
			string datestr = "";
			if (name_size != 0)
			{
				datestr = spilt_name[0];
				if (datestr != "")
				{
					//"20150110132005", ʱ���ڵ�6��7��λ��
					//������10�㵽����2���ͼƬ��ȡ����
					string hourstr = datestr.substr(8, 2);
					if ( hourstr <= "14" && hourstr >= "10")
						return true;	
				}	
			}
		}
	}

	return false;
}

//! ��rawdata���ļ�����·����learndata��
bool getNewPath(const string& filepath, string& newfilepath)
{
	string writePath = "F:/data/easypr-data/learndata/";
	vector<string> spilt_path;
	SplitString(filepath, spilt_path, "\\");

	int spiltsize = spilt_path.size();
	string filename = "";
	if (spiltsize != 0)
	{
		filename = spilt_path[spiltsize-1];
		newfilepath = writePath + filename;
		return true;
	}
	return false;
}

//! ��rawdata��ȡ�������ݵ�learndata��
void getLearnData()
{
	char * filePath = "F:/data/easypr-data/rawdata";

	////��ȡ��·���µ������ļ�
	vector<string> files;
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in rawdata!" << endl;

	////�������rawdata
	srand(unsigned(time(NULL)));
	random_shuffle(files.begin(), files.end());

	////ѡȡǰLEARANDATA_COUNT��rawdata������Ϊlearndata
	int boundry = LEARANDATA_COUNT;
	int count = 0;
	cout << "Save learndata!" << endl;
	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		string filepath = files[i].c_str();

		//ֻ������������
		if (isNotNight(filepath)!=true)
			continue;

		//��ȡ���ݣ�����ͼƬ����Ԥ����
		Mat img = imread(filepath);
		img = cutBottom(img);

		//�洢·���滻Ϊ�µ�
		string newfilepath = "";
		getNewPath(filepath, newfilepath);

		if(1)
		{ 
			stringstream ss(stringstream::in | stringstream::out);
			ss << newfilepath;
			imwrite(ss.str(), img);
		}

		count++;
		if (count >= boundry)
			break;
	}
}

void testGetLearnData()
{
	cout << "testGetLearnData!" << endl;
}
//int main()
//{
//	getLearnData();
//}