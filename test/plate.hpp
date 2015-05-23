#ifndef EASYPR_PLATE_HPP
#define EASYPR_PLATE_HPP

namespace easypr {

namespace demo {

int test_plate_locate() {
  cout << "test_plate_locate" << endl;

  Mat src = imread("resources/image/test.jpg");

  vector <Mat> resultVec;
  CPlateLocate plate;
  plate.setDebug(1);
  plate.setLifemode(true);

  int result = plate.plateLocate(src, resultVec);
  if (result == 0) {
    size_t num = resultVec.size();
    for (int j = 0; j < num; j++) {
      Mat resultMat = resultVec[j];
      imshow("plate_locate", resultMat);
      waitKey(0);
    }
    destroyWindow("plate_locate");
  }

  return result;
}

int test_plate_judge() {
  cout << "test_plate_judge" << endl;

  Mat src = imread("resources/image/plate_judge.jpg");

  //可能是车牌的图块集合
  vector <Mat> matVec;

  //经过SVM判断后得到的图块集合
  vector <Mat> resultVec;

  CPlateLocate lo;
  lo.setDebug(1);
  lo.setLifemode(true);

  int resultLo = lo.plateLocate(src, matVec);

  if (0 != resultLo)
    return -1;

  cout << "plate_locate_img" << endl;
  if (resultLo == 0) {
    size_t num = matVec.size();
    for (int j = 0; j < num; j++) {
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
  if (resultJu == 0) {
    size_t num = resultVec.size();
    for (int j = 0; j < num; j++) {
      Mat resultMat = resultVec[j];
      imshow("plate_judge", resultMat);
      waitKey(0);
    }
    destroyWindow("plate_judge");
  }

  return resultJu;
}

int test_plate_detect() {
  cout << "test_plate_detect" << endl;

  Mat src = imread("resources/image/plate_detect.jpg");

  vector <CPlate> resultVec;
  CPlateDetect pd;
  pd.setPDLifemode(true);

  int result = pd.plateDetectDeep(src, resultVec);
  if (result == 0) {
    size_t num = resultVec.size();
    for (int j = 0; j < num; j++) {
      CPlate resultMat = resultVec[j];

      imshow("plate_detect", resultMat.getPlateMat());
      waitKey(0);
    }
    destroyWindow("plate_detect");
  }

  return result;
}

int test_plate_recognize() {
  cout << "test_plate_recognize" << endl;

  Mat src = imread("resources/image/test.jpg");

  CPlateRecognize pr;
  pr.LoadANN("resources/model/ann.xml");
  pr.LoadSVM("resources/model/svm.xml");

  pr.setLifemode(true);
  pr.setDebug(true);

  vector <string> plateVec;

  int result = pr.plateRecognize(src, plateVec);
  if (result == 0) {
    size_t num = plateVec.size();
    for (size_t j = 0; j < num; j++) {
      cout << "plateRecognize: " << plateVec[j] << endl;
    }
  }

  if (result != 0)
    cout << "result:" << result << endl;

  return result;
}

}

}

#endif //EASYPR_PLATE_HPP
