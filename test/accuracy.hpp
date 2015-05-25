#ifndef EASYPR_ACCURACY_HPP
#define EASYPR_ACCURACY_HPP

#include <ctime>
#include <fstream>

namespace easypr {

namespace demo {

int accuracyTest(const char* test_path) {
  auto files = Utils::getFiles(test_path);

  CPlateRecognize pr;

  pr.LoadSVM("resources/model/svm.xml");
  pr.LoadANN("resources/model/ann.xml");
  pr.setLifemode(true);
  pr.setDebug(false);

  // ����Ҫ�����һ��ͼƬ������ж��ٳ���
  pr.setMaxPlates(4);

  size_t files_num = files.size();

  if (0 == files_num) {
    std::cout << "No File Found in " << test_path << "!" << std::endl;
    return 0;
  }

  std::cout << "Begin to test the easypr accuracy!" << std::endl;

  // �ܵĲ���ͼƬ����
  int count_all = 0;
  // �����ͼƬ����
  int count_err = 0;
  // δʶ���ͼƬ����
  int count_norecogin = 0;

  // �ܵ��ַ����
  float diff_all = 0;
  // ƽ���ַ����
  float diff_avg = 0;
  // ��ȫƥ���ʶ�����
  float match_count = 0;
  // ��ȫƥ���ʶ�������ռʶ��ͼƬ�еı���
  float match_rate = 0;

  // ��ʼ�ͽ���ʱ��
  time_t begin, end;
  time(&begin);

  for (int i = 0; i < files_num; i++) {
    std::string filepath = files[i];
    std::cout << "------------------" << std::endl;

    // ��ȡ��ʵ�ĳ���
    std::string plateLicense = Utils::getFileName(filepath);
    std::cout << "ԭ��:" << plateLicense << std::endl;

    // EasyPR��ʼ�жϳ���
    cv::Mat src = cv::imread(filepath);

    std::vector<std::string> plateVec;
    int result = pr.plateRecognize(src, plateVec);
    if (result == 0) {
      size_t num = plateVec.size();

      if (num == 0) {
        std::cout << "�޳���" << std::endl;
        if (plateLicense != "�޳���") count_norecogin++;
      } else if (num > 1) {
        // �೵��ʹ��diff��С���Ǹ���¼
        int mindiff = 10000;
        for (int j = 0; j < num; j++) {
          std::cout << plateVec[j] << " (" << j + 1 << ")" << std::endl;
          std::string colorplate = plateVec[j];

          // ����"����:��E7KU22"��ð�ź���ĳ��ƴ�С"
          auto spilt_plate = Utils::splitString(colorplate, ':');

          size_t size = spilt_plate.size();
          if (size == 2 && spilt_plate[1] != "") {
            int diff =
                    Utils::levenshtein_distance(plateLicense,
                                                spilt_plate[size - 1]);
            if (diff < mindiff) mindiff = diff;
          }
        }

        std::cout << "���:" << mindiff << "���ַ�" << std::endl;
        if (mindiff == 0) {
          // ��ȫƥ��
          match_count++;
        }
        diff_all = diff_all + mindiff;
      } else {
        // ������ֻ����һ��diff
        for (int j = 0; j < num; j++) {
          std::cout << plateVec[j] << std::endl;
          std::string colorplate = plateVec[j];

          // ����"����:��E7KU22"��ð�ź���ĳ��ƴ�С"
          std::vector<std::string> spilt_plate = Utils::splitString(colorplate,
                                                                    ':');

          size_t size = spilt_plate.size();
          if (size == 2 && spilt_plate[1] != "") {
            int diff =
                    Utils::levenshtein_distance(plateLicense,
                                                spilt_plate[size - 1]);
            std::cout << "���:" << diff << "���ַ�" << std::endl;

            if (diff == 0) {
              // ��ȫƥ��
              match_count++;
            }
            diff_all = diff_all + diff;
          }
        }
      }
    } else {
      std::cout << "������:" << result << std::endl;
      count_err++;
    }
    count_all++;
  }
  time(&end);

  std::cout << "------------------" << std::endl;
  std::cout << "Easypr accuracy test end!" << std::endl;
  std::cout << "------------------" << std::endl;
  std::cout << std::endl;
  std::cout << "ͳ�Ʋ���:" << std::endl;
  std::cout << "��ͼƬ��:" << count_all << "��,  ";
  std::cout << "δʶ��ͼƬ:" << count_norecogin << "��,  ";

  float count_recogin = count_all - (count_err + count_norecogin);
  float count_rate = count_recogin / count_all;

  std::cout << "��λ��:" << count_rate * 100 << "%  " << std::endl;

  diff_avg = diff_all / count_recogin;
  match_rate = match_count / count_recogin * 100;

  std::cout << "ƽ���ַ����:" << diff_avg << "��,  ";
  std::cout << "��ȫƥ����:" << match_count << "��,  ";
  std::cout << "��ȫƥ����:" << match_rate << "%  " << std::endl;

  double seconds = difftime(end, begin);
  double avgsec = seconds / double(count_all);

  std::cout << "��ʱ��:" << seconds << "��,  ";
  std::cout << "ƽ��ִ��ʱ��:" << avgsec << "��  " << std::endl;

  std::cout << std::endl;

  std::cout << "------------------" << std::endl;

  std::ofstream myfile("accuracy.txt", std::ios::app | std::ios::out);
  if (myfile.is_open()) {
    time_t t = time(0);  // get time now
    struct tm* now = localtime(&t);
    char buf[80];

    strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
    myfile << std::string(buf) << std::endl;

    myfile << "��ͼƬ��:" << count_all << "��,  ";
    myfile << "δʶ��ͼƬ:" << count_norecogin << "��,  ";
    myfile << "��λ��:" << count_rate * 100 << "%  " << std::endl;
    myfile << "ƽ���ַ����:" << diff_avg << "��,  ";
    myfile << "��ȫƥ����:" << match_count << "��,  ";
    myfile << "��ȫƥ����:" << match_rate << "%  " << std::endl;
    myfile << "��ʱ��:" << seconds << "��,  ";
    myfile << "ƽ��ִ��ʱ��:" << avgsec << "��" << std::endl;
    myfile.close();
  } else {
    std::cout << "Unable to open file";
  }
  return 0;
}

}

}

#endif //EASYPR_ACCURACY_HPP
