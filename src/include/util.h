
#ifndef __UTIL_H__
#define __UTIL_H__

//C++�Ļ�ȡ�ļ��к���
void getFiles(string path, vector<string>& files);

//C++��spilt����
void SplitString(const string& s, vector<string>& v, const string& c);

//C++�Ĵ��ļ�·�����Ƶ��ļ����ƣ���������׺���ķ���
void getFileName(const string& filepath, string& name);

/// test.cpp�з���
int testMain();

/// mc_data_prepare.cpp�з���
void getLearnData();
void Code2Province(const string& code, string& province);
void changeFileName();
void getPlateLicense(const string& filepath, string& plateLicense);

/// learn_prepare.cpp�з���
void label_data();

/// accuracy_test.cpp�з���
int acurayTest();

#endif
/* endif __UTIL_H__ */