#include "easypr/core/plate_recognize.h"
#include "easypr/config.h"
#include "third/erfilter.hpp"

namespace easypr {

  CPlateRecognize::CPlateRecognize() { }

  void groups_draw(Mat &src, std::vector<Rect> &groups)
  {
    for (int i = (int)groups.size() - 1; i >= 0; i--)
    {
      if (src.type() == CV_8UC3)
        rectangle(src, groups.at(i).tl(), groups.at(i).br(), Scalar(0, 255, 255), 3, 8);
      else
        rectangle(src, groups.at(i).tl(), groups.at(i).br(), Scalar(255), 3, 8);
    }
  }

  void er_show(std::vector<Mat> &channels, std::vector<std::vector<text::ERStat> > &regions)
  {
    for (int c = 0; c<(int)channels.size(); c++)
    {
      Mat dst = Mat::zeros(channels[0].rows + 2, channels[0].cols + 2, CV_8UC1);
      for (int r = 0; r<(int)regions[c].size(); r++)
      {
        text::ERStat er = regions[c][r];
        if (er.parent != NULL) // deprecate the root region
        {
          int newMaskVal = 255;
          int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
          floodFill(channels[c], dst, Point(er.pixel%channels[c].cols, er.pixel / channels[c].cols),
            Scalar(255), 0, Scalar(er.level), Scalar(0), flags);
        }
      }
      char buff[10]; char *buff_ptr = buff;
      sprintf(buff, "channel %d", c);
      imshow(buff_ptr, dst);
    }
    waitKey(-1);
  }

  int CPlateRecognize::plateRecognizeAsTextNM(Mat src, std::vector<CPlate> &licenseVec) {
    std::vector<Mat> channels;
    std::vector<int> flags;

    if (1) {
      Mat grayImage;
      cvtColor(src, grayImage, COLOR_BGR2GRAY);
      channels.push_back(grayImage);

      //Mat singleChannelImage;
      //extractChannel(src, singleChannelImage, 0);
      //channels.push_back(singleChannelImage);

      flags.push_back(0);
    }

    //text::computeNMChannels(src, channels);

    int cn = (int)channels.size();
    // Append negative channels to detect ER- (bright regions over dark background)
    for (int c = 0; c < cn; c++)
      channels.push_back(255 - channels[c]);

    // Create ERFilter objects with the 1st and 2nd stage default classifiers
    std::string pathMMmodel_1 = "resources/model/text/trained_classifierNM1.xml";
    std::string pathMMmodel_2 = "resources/model/text/trained_classifierNM2.xml";

    Ptr<text::ERFilter> er_filter1 = text::createERFilterNM1(text::loadClassifierNM1(pathMMmodel_1), 16, 0.00015f, 0.13f, 0.2f, true, 0.1f);
    Ptr<text::ERFilter> er_filter2 = text::createERFilterNM2(text::loadClassifierNM2(pathMMmodel_2), 0.5);

    std::vector<std::vector<text::ERStat> > regions(channels.size());
    // Apply the default cascade classifier to each independent channel (could be done in parallel)
    std::cout << "Extracting Class Specific Extremal Regions from " << (int)channels.size() << " channels ..." << std::endl;
    std::cout << "    (...) this may take a while (...)" << std::endl << std::endl;
    for (int c = 1; c < (int)channels.size(); c++)
    {
      er_filter1->run(channels[c], regions[c]);
      //er_filter2->run(channels[c], regions[c]);
    }

    // Detect character groups
    std::cout << "Grouping extracted ERs ... ";
    std::vector< std::vector<Vec2i> > region_groups;
    std::vector<Rect> groups_boxes;

    //er_show(channels, regions);

    //erGrouping(src, channels, regions, region_groups, groups_boxes, text::ERGROUPING_ORIENTATION_HORIZ);
    //erGrouping(src, channels, regions, region_groups, groups_boxes, ERGROUPING_ORIENTATION_ANY, "trained_classifier_erGrouping.xml", 0.5);

    //groups_draw(src, groups_boxes);
    //imshow("grouping", src);
    //waitKey(0);

    er_show(channels, regions);

    // draw groups

    // memory clean-up
    er_filter1.release();
    er_filter2.release();
    regions.clear();
    if (!groups_boxes.empty())
    {
      groups_boxes.clear();
    }

    return 0;
  }

 
  // compareRectIs
  bool compareCharRect(const CCharacter& character1, const CCharacter& character2)
  {
    Rect rect1 = character1.getCharacterPos();
    Rect rect2 = character2.getCharacterPos();

    // the character in plate are similar height
    float width_1 = float(rect1.width);
    float height_1 = float(rect1.height);

    float width_2 = float(rect2.width);
    float height_2 = float(rect2.height);

    float height_diff = abs(height_1 - height_2);
    double height_diff_ratio = height_diff / min(height_1, height_2);

    if (height_diff_ratio > 0.25)
      return false;

    // the character in plate are similar in the y-axis
    float y_1 = float(rect1.tl().y);
    float y_2 = float(rect2.tl().y);

    float y_diff = abs(y_1 - y_2);
    double y_diff_ratio = y_diff / min(height_1, height_2);

    if (y_diff_ratio > 0.5)
      return false;

    // the character in plate are near in the x-axis but not very near
    float x_margin_left = float(min(rect1.br().x, rect2.br().x));
    float x_margin_right = float(max(rect1.tl().x, rect2.tl().x));

    float x_margin_diff = abs(x_margin_left - x_margin_right);
    double x_margin_diff_ratio = x_margin_diff / max(width_1, width_2);

    if (x_margin_diff_ratio > 3)
      return false;

    return true;
  }

  //! �Ǽ���ֵ����
  void NMStoCharacter(std::vector<CCharacter> &inVec, std::vector<CCharacter> &resultVec, double overlap) {

    std::sort(inVec.begin(), inVec.end());

    std::vector<CCharacter>::iterator it = inVec.begin();
    for (; it != inVec.end(); ++it) {
      CCharacter charSrc = *it;
      //std::cout << "plateScore:" << plateSrc.getPlateScore() << std::endl;
      Rect rectSrc = charSrc.getCharacterPos();

      std::vector<CCharacter>::iterator itc = it + 1;

      for (; itc != inVec.end();) {
        CCharacter charComp = *itc;
        Rect rectComp = charComp.getCharacterPos();
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


  void MergeCharToPlate(std::vector<CCharacter>& vecRect, std::vector<std::vector<CCharacter>>& vecRectPlate){
     std::vector<int> labels;

     std::vector<CCharacter> charVec;

     double overlap = 0.5;
     NMStoCharacter(vecRect, charVec, overlap);

     int numbers = partition(charVec, labels, &compareCharRect);
     for (size_t j = 0; j < size_t(numbers); j++) {
       std::vector<CCharacter> charRects;

       for (size_t t = 0; t < charVec.size(); t++) {
          int label = labels[t];

          if (label == j)
            charRects.push_back(charVec[t]);
        }

        if (charRects.size() == 0 || charRects.size() < 5)
          continue;

        vecRectPlate.push_back(charRects);       
     }
  }


  int CPlateRecognize::plateRecognizeAsText(Mat src, std::vector<CPlate> &licenseVec) 
  {
    // ���Ʒ��鼯��
    std::vector<CPlate> plateVec;

    std::vector<Mat> channelImages;
    std::vector<int> flags;


    // only conside blue plate
    if (1) {
      Mat grayImage;
      cvtColor(src, grayImage, COLOR_BGR2GRAY);
      channelImages.push_back(grayImage);
      flags.push_back(0);

      //Mat singleChannelImage;
      //extractChannel(src, singleChannelImage, 2);
      //channelImages.push_back(singleChannelImage);
      //flags.push_back(0);
    }

    std::vector<std::vector<std::vector<Point>>> all_contours;
    std::vector<std::vector<Rect>> all_boxes;

    all_contours.resize(channelImages.size());
    all_boxes.resize(channelImages.size());

    for (int i = 0; i < (int)channelImages.size(); ++i)
    {
      Ptr<MSER> mser;
      std::vector<CCharacter> charVec;
      Mat channelImage = channelImages[i];

      // ��ͼ�����ͳһ���ţ�ȷ��ͼ��Ҫ����
      // ���ٶ�������������
      int scale_size = 1200;
      double scale_ratio = 1;
      Mat image = scaleImage(channelImage, Size(scale_size, scale_size), scale_ratio);

      Mat result = image;
      cvtColor(result, result, COLOR_GRAY2BGR);

      int imageArea = image.rows * image.cols;

      if (3 == flags[i]) {
        mser = MSER::create(3, 30, int(0.05 * imageArea));
      }
      else {
        mser = MSER::create(1, 30, int(0.05 * imageArea));
      }
      mser->detectRegions(image, all_contours[i], all_boxes[i]);

      size_t size = all_contours[i].size();

      for (size_t index = 0; index < size; index++) {
        Rect rect = all_boxes[i][index];
    
        if (verifyCharSizes(rect)) {
          float aspect = float(rect.width) / float(rect.height);
          //if (aspect < 0.2) {
          //  cv::rectangle(result, rect, Scalar(0, 0, 255));
          //}

          Mat region = image(rect);
          Mat binary_region;
          threshold(region, binary_region, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

          Mat charInput = preprocessChar(binary_region, 20);
          std::string label = "";
          float maxVal = -2.f;
          bool isCharacter = CharsIdentify::instance()->isCharacter(charInput, label, maxVal);

          if (isCharacter) {
            cv::rectangle(result, rect, Scalar(255, 0, 0));

            CCharacter character;
            character.setCharacterPos(rect);
            character.setCharacterMat(binary_region);
            character.setCharacterStr(label);
            character.setCharacterScore(maxVal);
            charVec.push_back(character);
          }    
        }
      }

      std::vector<std::vector<CCharacter>> vecRectPlate;
      MergeCharToPlate(charVec, vecRectPlate);

      for (size_t plate_i = 0; plate_i < vecRectPlate.size(); plate_i++) {
        std::vector<CCharacter> chars = vecRectPlate[plate_i];

        std::sort(chars.begin(), chars.end(),
          [](const CCharacter& r1, const CCharacter& r2) { return r1.getCharacterPos().x < r2.getCharacterPos().x; });

        Rect specificRect = chars[0].getCharacterPos();
        Rect chineseRect = GetChineseRect(specificRect);

        cv::rectangle(result, chineseRect, Scalar(0, 255, 0));

        Mat region = image(chineseRect);
        Mat binary_region;
        threshold(region, binary_region, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

        Mat charInput = preprocessChar(binary_region, 20);
        std::string label = "";
        float maxVal = -2.f;
        bool isCharacter = CharsIdentify::instance()->isCharacter(charInput, label, maxVal, true);

        if (isCharacter) {
          CCharacter character;
          character.setCharacterPos(chineseRect);
          character.setCharacterMat(binary_region);
          character.setCharacterStr(label);
          character.setCharacterScore(maxVal);
          chars.push_back(character);
        }

        int leftmax = image.rows;
        int rightmax = 0;
        int topmax = image.cols;
        int bottommax = 0;

        std::string plateLicense = "";

        for (size_t p = 0; p < chars.size(); p++) {
          Rect rect = chars[p].getCharacterPos();
          int left = rect.tl().x;
          int right = rect.br().x;

          int top = rect.tl().y;
          int bottom = rect.br().y;

          if (left < leftmax) leftmax = left;
          if (right > rightmax) rightmax = right;
          if (top < topmax) topmax = top;
          if (bottom > bottommax) bottommax = bottom;

          if (p != chars.size() - 1)
            plateLicense.append(chars[p].getCharacterStr());
        }

        Rect grouprect(leftmax, topmax, rightmax - leftmax + 1, bottommax - topmax + 1); 
        Point2f center(float(leftmax + grouprect.width / 2), float(topmax + grouprect.height / 2));

        plateLicense = label + plateLicense;

          if (verifyPlateSize(grouprect)) {
            
            cv::rectangle(result, grouprect, Scalar(0, 0, 255));

            CPlate plate;
            plate.setPlateLocateType(LocateType::CMSER);
            plate.setPlatePos(RotatedRect(center, Size2f(float(grouprect.width), float(grouprect.height)), 0));
            plate.setPlateMat(image(grouprect));
            plate.setPlateStr(plateLicense);
            licenseVec.push_back(plate);
          }
      }

      //RotatedRect testrr(Point2f(200, 200), Size2f(375, 100), 1.f);
      //Point2f rect_points[4]; 
      //testrr.points(rect_points);
      //for (int j = 0; j < 4; j++)
      //  line(result, rect_points[j], rect_points[(j + 1) % 4], Scalar(0,0,255), 2, 8);


      if (1) {
        imshow("result", result);
        waitKey(0);
      }
    }

    
    //showResult(result);

    return 0;
  }
 

  // !����ʶ��ģ��
  int CPlateRecognize::plateRecognize(Mat src,
    std::vector<CPlate> &licenseVec) {

    // ���Ƽ���
    std::vector<CPlate> plateVec;

    // ��ͼ�����ͳһ���ţ�ȷ��ͼ��Ҫ����
    // ���ٶ�������������
    //int scale_size = 1600;
    //double scale_ratio = 1;
    //Mat ret = scaleImage(src, Size(scale_size, scale_size), scale_ratio);
    //src = ret;

    // ������ȶ�λ��ʹ����ɫ��Ϣ�����Sobel
    int resultPD = plateDetect(src, plateVec);

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

        std::string plateIdentify = "";
        int resultCR = charsRecognise(plate, plateIdentify);
        if (resultCR == 0) {
          std::string license = plateType + ":" + plateIdentify;
          item.setPlateStr(license);
          licenseVec.push_back(item);
        }
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

          if (item.getPlateLocateType() == CMSER) lineColor = Scalar(0, 0, 255);

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


  int CPlateRecognize::plateRecognize(Mat src,
    std::vector<std::string> &licenseVec) {

    // ���Ʒ��鼯��

    std::vector<CPlate> plateVec;

    // ������ȶ�λ��ʹ����ɫ��Ϣ�����Sobel

    int resultPD = plateDetect(src, plateVec, 0, kDebug, 0);

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

        std::string plateIdentify = "";
        int resultCR = charsRecognise(plate, plateIdentify);
        if (resultCR == 0) {
          std::string license = plateType + ":" + plateIdentify;
          licenseVec.push_back(license);
        }
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
