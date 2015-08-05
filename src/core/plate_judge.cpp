#include "easypr/core/plate_judge.h"
#include "easypr/config.h"

namespace easypr {

  PlateJudge* PlateJudge::instance_ = nullptr;

  PlateJudge* PlateJudge::instance(){
    if (!instance_){
      instance_ = new PlateJudge;
    }
    return instance_;
  }

  PlateJudge::PlateJudge() {
    svm_ = ml::SVM::load<ml::SVM>(kDefaultSvmPath);
  }

  //! ֱ��ͼ����
  Mat PlateJudge::histeq(Mat in) {
    Mat out(in.size(), in.type());
    if (in.channels() == 3) {
      Mat hsv;
      std::vector<Mat> hsvSplit;
      cvtColor(in, hsv, CV_BGR2HSV);
      split(hsv, hsvSplit);
      equalizeHist(hsvSplit[2], hsvSplit[2]);
      merge(hsvSplit, hsv);
      cvtColor(hsv, out, CV_HSV2BGR);
    }
    else if (in.channels() == 1) {
      equalizeHist(in, out);
    }
    return out;
  }

  //! �Ե���ͼ�����SVM�ж�
  int PlateJudge::plateJudge(const Mat& inMat, int& result) {
    Mat features;
    getHistogramFeatures(inMat, features);

    //ͨ��ֱ��ͼ���⻯��Ĳ�ɫͼ����Ԥ��
    Mat p = features.reshape(1, 1);
    p.convertTo(p, CV_32FC1);

    float response = svm_->predict(p);
    result = (int)response;

    return 0;
  }

  //! �Զ��ͼ�����SVM�ж�
  int PlateJudge::plateJudge(const std::vector<Mat>& inVec, std::vector<Mat>& resultVec) {
    int num = inVec.size();
    for (int j = 0; j < num; j++) {
      Mat inMat = inVec[j];

      int response = -1;
      plateJudge(inMat, response);

      if (response == 1) resultVec.push_back(inMat);
    }
    return 0;
  }

  //! �Զ�����ƽ���SVM�ж�
  int PlateJudge::plateJudge(const std::vector<CPlate>& inVec,
    std::vector<CPlate>& resultVec) {
    int num = inVec.size();
    for (int j = 0; j < num; j++) {
      CPlate inPlate = inVec[j];
      Mat inMat = inPlate.getPlateMat();

      int response = -1;
      plateJudge(inMat, response);

      if (response == 1)
        resultVec.push_back(inPlate);
      else {
        int w = inMat.cols;
        int h = inMat.rows;
        //��ȡ�м䲿���ж�һ��
        Mat tmpmat = inMat(Rect_<double>(w * 0.05, h * 0.1, w * 0.9, h * 0.8));
        Mat tmpDes = inMat.clone();
        resize(tmpmat, tmpDes, Size(inMat.size()));

        plateJudge(tmpDes, response);

        if (response == 1) resultVec.push_back(inPlate);
      }
    }
    return 0;
  }

}
