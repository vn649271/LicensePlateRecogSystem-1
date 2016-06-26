#include "easypr/core/plate_judge.h"
#include "easypr/config.h"

namespace easypr {

  PlateJudge* PlateJudge::instance_ = nullptr;

  PlateJudge* PlateJudge::instance() {
    if (!instance_) {
      instance_ = new PlateJudge;
    }
    return instance_;
  }

  PlateJudge::PlateJudge() { 
    svm_ = ml::SVM::load<ml::SVM>(kDefaultSvmPath); 
    extractFeature = getLBPFeatures;
  }

  void PlateJudge::LoadModel(std::string path) {
    svm_->clear();
    svm_->ml::SVM::load<ml::SVM>(path);
  }

  //! �Ե���ͼ�����SVM�ж�

  int PlateJudge::plateJudge(const Mat &inMat, int &result) {
    Mat features;
    extractFeature(inMat, features);

    float response = svm_->predict(features);
    result = (int)response;

    return 0;
  }

  //! �Զ��ͼ�����SVM�ж�

  int PlateJudge::plateJudge(const std::vector<Mat> &inVec,
    std::vector<Mat> &resultVec) {
    int num = inVec.size();
    for (int j = 0; j < num; j++) {
      Mat inMat = inVec[j];

      int response = -1;
      plateJudge(inMat, response);

      if (response == 1) resultVec.push_back(inMat);
    }
    return 0;
  }

  //! ���ó���ͼ������Ŷ�
  //! ����ֵ��0�����ǳ��ƣ�����ֵ������
  int PlateJudge::plateSetScore(CPlate& plate) {
    Mat features;
    extractFeature(plate.getPlateMat(), features);

    float score = svm_->predict(features, noArray(), cv::ml::StatModel::Flags::RAW_OUTPUT);

    // scoreֵ������margin�ľ��룬С��0�����ǳ��ƣ�����0�����ǳ���
    // ��С��0ʱ��ֵԽС�����ǳ��Ƶĸ���Խ��
    plate.setPlateScore(score);

    if (score < 0)
      return 0;
    else
      return -1;
  }

  ////! �ȽϺ���
  //struct PlateScoreCompaer {
  //  bool operator() (const CPlate& i, const CPlate& j) { return (i.getPlateScore() < j.getPlateScore()); }
  //} plateScoreCompaerObject;


  //! �Ǽ���ֵ����
  void NMS(std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, double overlap) {

    std::sort(inVec.begin(), inVec.end());

    std::vector<CPlate>::iterator it = inVec.begin();
    for (; it != inVec.end(); ++it) {
      CPlate plateSrc = *it;
      //std::cout << "plateScore:" << plateSrc.getPlateScore() << std::endl;
      Rect rectSrc = plateSrc.getPlatePos().boundingRect();

      std::vector<CPlate>::iterator itc = it + 1;

      for (; itc != inVec.end();) {
        CPlate plateComp = *itc;
        Rect rectComp = plateComp.getPlatePos().boundingRect();
        Rect rectInter = rectSrc & rectComp;
        Rect rectUnion = rectSrc | rectComp;
        double r = double(rectInter.area()) / double(rectUnion.area());
        if (r > overlap) {
          itc = inVec.erase(itc);
        }
        else {
          ++itc;
        }
      }
    }

    resultVec = inVec;
  }

  //! ʹ�÷Ǽ���ֵ���Ƶĳ����ж�
  int PlateJudge::plateJudgeUsingNMS(const std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, int maxPlates) {
    std::vector<CPlate> plateVec;
    int num = inVec.size();
    bool outputResult = false;

    for (int j = 0; j < num; j++) {
      CPlate plate = inVec[j];
      Mat inMat = plate.getPlateMat();

      if (0) {
        imshow("inMat", inMat);
        waitKey(0);
        destroyWindow("inMat");
      }

      int result = plateSetScore(plate);

      if (result == 0) {
          int w = inMat.cols;
          int h = inMat.rows;

          Mat tmpmat = inMat(Rect_<double>(w * 0.05, h * 0.1, w * 0.9, h * 0.8));
          Mat tmpDes = inMat.clone();
          resize(tmpmat, tmpDes, Size(inMat.size()));

          plate.setPlateMat(tmpDes);

          int resultCascade = plateSetScore(plate);

          if (resultCascade == 0) {
            if (0) {
              imshow("inMat", inMat);
              waitKey(0);
              destroyWindow("inMat");
            }
            plateVec.push_back(plate);      
          }
      }

      //if (result == 0) {
      //  plateVec.push_back(plate);
      //  if (outputResult) {
      //    std::stringstream ss(std::stringstream::in | std::stringstream::out);
      //    ss << "resources/image/tmp/plate/has" << "/" << plate.getPlatePos().center << "_"
      //      << plate.getPlatePos().size << "_" << plate.getPlatePos().angle << "_"
      //      << plate.getPlateScore() << ".jpg";
      //    imwrite(ss.str(), inMat);
      //  }
      //}
      //else {
      //  int w = inMat.cols;
      //  int h = inMat.rows;

      //  //��ȡ�м䲿���ж�һ��

      //  Mat tmpmat = inMat(Rect_<double>(w * 0.05, h * 0.1, w * 0.9, h * 0.8));
      //  Mat tmpDes = inMat.clone();
      //  resize(tmpmat, tmpDes, Size(inMat.size()));

      //  plate.setPlateMat(tmpDes);

      //  int resultCascade = plateSetScore(plate);

      //  if (resultCascade == 0) {
      //    plateVec.push_back(plate);
      //    if (outputResult) {
      //      std::stringstream ss(std::stringstream::in | std::stringstream::out);
      //      ss << "resources/image/tmp/plate/has" << "/" << plate.getPlatePos().center << "_" 
      //        << plate.getPlatePos().size << "_" << plate.getPlatePos().angle << "_" 
      //        << plate.getPlateScore() << ".jpg";
      //      imwrite(ss.str(), tmpDes);
      //    }
      //  }
      //  else {
      //    if (outputResult) {
      //      std::stringstream ss(std::stringstream::in | std::stringstream::out);
      //      ss << "resources/image/tmp/plate/no" << "/" << plate.getPlatePos().center << "_"
      //        << plate.getPlatePos().size << "_" << plate.getPlatePos().angle << "_"
      //        << plate.getPlateScore() << ".jpg";
      //      imwrite(ss.str(), tmpDes);
      //    }
      //  }
      //}

    }

    std::vector<CPlate> reDupPlateVec;

    // ʹ�÷Ǽ���ֵ������ȥ����Щ�ص��ĳ���
    // overlap��ֵ����Ϊ0.5
    double overlap = 0.5;
    NMS(plateVec, reDupPlateVec, overlap);
  
    std::vector<CPlate>::iterator it = reDupPlateVec.begin();
    int count = 0;
    for (; it != reDupPlateVec.end(); ++it) {
      resultVec.push_back(*it);

      if (0) {
        imshow("plateMat", it->getPlateMat());
        waitKey(0);
        destroyWindow("plateMat");
      }

      count++;
      if (count >= maxPlates)
        break;
    }


    return 0;
  }


  //! �Զ�����ƽ���SVM�ж�
  int PlateJudge::plateJudge(const std::vector<CPlate> &inVec,
    std::vector<CPlate> &resultVec) {
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
