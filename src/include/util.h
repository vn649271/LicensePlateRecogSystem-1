
#ifndef __UTIL_H__
#define __UTIL_H__

//C++�Ļ�ȡ�ļ��к���
void getFiles(string path, vector<string>& files);

//C++��spilt����
void SplitString(const string& s, vector<string>& v, const string& c);

/// test.cpp�з���
int testMain();

/// mc_data_prepare.cpp�з���
void getLearnData();
void Code2Province(const string& code, string& province);
void changeFileName();

/// learn_prepare.cpp�з���
void label_data();



#endif
/* endif __UTIL_H__ */