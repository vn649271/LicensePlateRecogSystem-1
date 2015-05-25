#include "easypr/plate_recognize.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

using namespace cv;
using namespace std;

CPlateRecognize::CPlateRecognize() {

}

int CPlateRecognize::plateRecognize(Mat src, vector<string>& licenseVec) {
  // ���Ʒ��鼯��
  vector<CPlate> plateVec;

  // ���������Debugģʽ����������ʾ���е�ͼƬ
  // int showDetectArea = getPDDebug();
  bool showDetectArea = false;
  // ������ȶ�λ��ʹ����ɫ��Ϣ�����Sobel
  int resultPD = plateDetectDeep(src, plateVec, showDetectArea, 0);

  Mat result;
  src.copyTo(result);

  if (resultPD == 0) {
    size_t num = plateVec.size();

    for (int j = 0; j < num; j++) {
      CPlate item = plateVec[j];

      Mat plate = item.getPlateMat();

      //��ȡ������ɫ
      string plateType = getPlateColor(plate);

      //��ȡ���ƺ�
      string plateIdentify = "";
      int resultCR = charsRecognise(plate, plateIdentify);
      if (resultCR == 0) {
        string license = plateType + ":" + plateIdentify;
        licenseVec.push_back(license);

        RotatedRect minRect = item.getPlatePos();
        Point2f rect_points[4];
        minRect.points(rect_points);

        if (item.bColored) {
          for (int k = 0; k < 4; k++) {
            line(result, rect_points[k], rect_points[(k + 1) % 4],
                 Scalar(255, 255, 0), 2, 8);
            //��ɫ��λ���ƣ���ɫ����
          }
        } else {
          for (int m = 0; m < 4; m++) {
            line(result, rect_points[m], rect_points[(m + 1) % 4],
                 Scalar(0, 0, 255), 2, 8);//sobel��λ���ƣ���ɫ����
          }
        }
      }
    }
  }
  showResult(result);

  return resultPD;
}

}  /*! \namespace easypr*/

