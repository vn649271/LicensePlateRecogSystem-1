#include "easypr/core/plate_recognize.h"
#include "easypr/config.h"

namespace easypr {

CPlateRecognize::CPlateRecognize() {}

// !����ʶ��ģ��
int CPlateRecognize::plateRecognize(Mat src,
                                    std::vector<std::string> &licenseVec) {
  // ���Ʒ��鼯��
  std::vector<CPlate> plateVec;

  // ������ȶ�λ��ʹ����ɫ��Ϣ�����Sobel
  int resultPD = plateDetect(src, plateVec, kDebug, 0);

  if (resultPD == 0) {
    size_t num = plateVec.size();
    int index = 0;

    //����ʶ��ÿ�������ڵķ���
    for (size_t j = 0; j < num; j++) {
      CPlate item = plateVec[j];
      Mat plate = item.getPlateMat();

      //��ȡ������ɫ
      std::string plateType = getPlateColor(plate);

      //��ȡ���ƺ�
      std::string license = charsRecognise(plate);
      std::string full_license = plateType + ":" + license;
      licenseVec.push_back(full_license);
    }
    //����ʶ����̵��˽���

    //�����Debugģʽ������Ҫ����λ��ͼƬ��ʾ��ԭͼ���Ͻ�
    if (getPDDebug()) {
      Mat result;
      src.copyTo(result);

      for (size_t j = 0; j < num; j++) {
        CPlate item = plateVec[j];
        Mat plate = item.getPlateMat();

        int height = 36;
        int width = 136;
        if (height * index + height < result.rows) {
          Mat imageRoi = result(Rect(0, 0 + height * index, width, height));
          addWeighted(imageRoi, 0, plate, 1, 0, imageRoi);
        }
        index++;

        RotatedRect minRect = item.getPlatePos();
        Point2f rect_points[4];
        minRect.points(rect_points);

        Scalar lineColor = Scalar(255, 255, 255);

        if (item.getPlateLocateType() == SOBEL) lineColor = Scalar(255, 0, 0);

        if (item.getPlateLocateType() == COLOR) lineColor = Scalar(0, 255, 0);

        for (int j = 0; j < 4; j++)
          line(result, rect_points[j], rect_points[(j + 1) % 4], lineColor, 2,
               8);
      }

      //��ʾ��λ���ͼƬ
      showResult(result);
    }
  }

  return resultPD;
}
}
