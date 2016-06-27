#include "easypr/core/plate_locate.h"
#include "easypr/util/util.h"

using namespace std;

namespace easypr {

const float DEFAULT_ERROR = 0.9f;    // 0.6
const float DEFAULT_ASPECT = 3.75f;  // 3.75

CPlateLocate::CPlateLocate() {
  m_GaussianBlurSize = DEFAULT_GAUSSIANBLUR_SIZE;
  m_MorphSizeWidth = DEFAULT_MORPH_SIZE_WIDTH;
  m_MorphSizeHeight = DEFAULT_MORPH_SIZE_HEIGHT;

  m_error = DEFAULT_ERROR;
  m_aspect = DEFAULT_ASPECT;
  m_verifyMin = DEFAULT_VERIFY_MIN;
  m_verifyMax = DEFAULT_VERIFY_MAX;

  m_angle = DEFAULT_ANGLE;

  m_debug = DEFAULT_DEBUG;
}

//! 生活模式与工业模式切换
//!
//如果为真，则设置各项参数为定位生活场景照片（如百度图片）的参数，否则恢复默认值。

void CPlateLocate::setLifemode(bool param) {
  if (param) {
    setGaussianBlurSize(5);
    setMorphSizeWidth(10);
    setMorphSizeHeight(3);
    setVerifyError(0.75);
    setVerifyAspect(4.0);
    setVerifyMin(1);
    setVerifyMax(200);
  } else {
    setGaussianBlurSize(DEFAULT_GAUSSIANBLUR_SIZE);
    setMorphSizeWidth(DEFAULT_MORPH_SIZE_WIDTH);
    setMorphSizeHeight(DEFAULT_MORPH_SIZE_HEIGHT);
    setVerifyError(DEFAULT_ERROR);
    setVerifyAspect(DEFAULT_ASPECT);
    setVerifyMin(DEFAULT_VERIFY_MIN);
    setVerifyMax(DEFAULT_VERIFY_MAX);
  }
}

//! 对minAreaRect获得的最小外接矩形，用纵横比进行判断
bool CPlateLocate::verifySizes(RotatedRect mr) {
  float error = m_error;
  // Spain car plate size: 52x11 aspect 4,7272
  // China car plate size: 440mm*140mm，aspect 3.142857

  // Real car plate size: 136 * 32, aspect 4
  float aspect = m_aspect;

  // Set a min and max area. All other patchs are discarded
  // int min= 1*aspect*1; // minimum area
  // int max= 2000*aspect*2000; // maximum area
  int min = 34 * 8 * m_verifyMin;  // minimum area
  int max = 34 * 8 * m_verifyMax;  // maximum area

  // Get only patchs that match to a respect ratio.
  float rmin = aspect - aspect * error;
  float rmax = aspect + aspect * error;

  float area = mr.size.height * mr.size.width;
  float r = (float) mr.size.width / (float) mr.size.height;
  if (r < 1) r = (float) mr.size.height / (float) mr.size.width;

  // cout << "area:" << area << endl;
  // cout << "r:" << r << endl;

  if ((area < min || area > max) || (r < rmin || r > rmax))
    return false;
  else
    return true;
}

//! mser search method
int CPlateLocate::mserSearch(const Mat &src, const Color color, Mat &out,
  vector<CPlate>& out_plateVec, int img_index, bool showDebug) {
  Mat match_grey;

  vector<CPlate> plateVec;
  //mserMatch(src, match_grey, r, plateRects, charRects);
  mserCharMatch(src, match_grey, plateVec, color, img_index, showDebug);

  //calculet avg dist
  /*float sumdist = 0, avgdist = 0;
  for (auto plate : plateVec) {
    Vec2i disV = plate.getPlateDistVec();
    sumdist += (float)disV[0];
  }

  avgdist = sumdist / (float)plateVec.size();
  
  const int mser_morph_width = (int)avgdist;
  const int mser_morph_height = int(avgdist / 5);

  Mat src_threshold;
  threshold(match_grey, src_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
  
  Mat element = getStructuringElement(MORPH_RECT, Size(mser_morph_width, mser_morph_height));
    morphologyEx(src_threshold, src_threshold, MORPH_CLOSE, element);*/

  Mat src_threshold = match_grey.clone();

  vector<RotatedRect> mergeRects;
  vector<vector<Point>> contours;

  findContours(src_threshold,
      contours,               // a vector of contours
      CV_RETR_EXTERNAL,
      CV_CHAIN_APPROX_NONE);  // all pixels of each contours
  
    vector<vector<Point>>::iterator itc = contours.begin();
    while (itc != contours.end()) {
      RotatedRect mr = minAreaRect(Mat(*itc));
  
      // 需要进行大小尺寸判断
      if (!verifyRotatedPlateSizes(mr))
        itc = contours.erase(itc);
      else {
        ++itc;
        //contourRects.push_back(mr);
  
        float width = mr.size.width;
        float height = mr.size.height;     
  
        RotatedRect candRect(mr.center,
          Size2f(float(width * 1.15), float(height * 1.25)), mr.angle);
  
        //mergeRects.push_back(candRect);      
      }
    }

    for (auto plate : plateVec) {
      RotatedRect rrect = plate.getPlatePos();
      Rect_<float> rectSrc;
      calcSafeRect(rrect, src, rectSrc);
      double maxr = 0;
      RotatedRect similyRect;
      for (auto mergeRect : mergeRects) {
        Rect_<float> rectComp;
        calcSafeRect(mergeRect, src, rectComp);

        Rect rectInter = rectSrc & rectComp;
        Rect rectUnion = rectSrc | rectComp;
        double r = double(rectInter.area()) / double(rectUnion.area()); 
        if (r > maxr){
          maxr = r;
          similyRect = mergeRect;
        }
      }
      if (maxr > 0.5){
        plate.setPlatePos(similyRect);
      }
    }

    for (auto plate : plateVec){
      RotatedRect rrect = plate.getPlatePos();
      rotatedRectangle(match_grey, rrect, Scalar(255));
    }

    if (0) {
      imshow("match", match_grey);
      waitKey(0);
      destroyWindow("match");
    }
  
    out_plateVec = plateVec;
    out = match_grey;

  return 0;
}


// 
//int mserSearch2(const Mat &src, const Color color, Mat &out,
//  vector<RotatedRect> &outRects, int index, bool showDebug) {
//  Mat match_grey;
//  Mat result = src.clone();
//  cvtColor(result, result, COLOR_GRAY2BGR);
//
//  const int color_morph_width = 24;
//  const int color_morph_height = 4;
//
//  std::vector<RotatedRect> plateRects;
//  std::vector<Rect> charRects;
//  std::vector<RotatedRect> contourRects;
//
//  std::vector<Rect> mergedRects;
//
//  // 进行颜色查找
//
//  //mserMatch(src, match_grey, r, plateRects, charRects);
//  mserCharMatch(src, match_grey, plateRects, color, index, showDebug);
//
//
//  if (0) {
//    utils::imwrite("resources/image/tmp/match_grey.jpg", match_grey);
//  }
//
//  if (0) {
//    imshow("match_grey", match_grey);
//    waitKey(0);
//    destroyWindow("match_grey");
//  }
//
//  Mat src_threshold;
//  threshold(match_grey, src_threshold, 0, 255,
//    CV_THRESH_OTSU + CV_THRESH_BINARY);
//
//  Mat element = getStructuringElement(
//    MORPH_RECT, Size(color_morph_width, color_morph_height));
//  morphologyEx(src_threshold, src_threshold, MORPH_CLOSE, element);
//
//  if (0) {
//    utils::imwrite("resources/image/tmp/mser.jpg", src_threshold);
//  }
//
//
//  if (0) {
//    imshow("src_threshold", src_threshold);
//    waitKey(0);
//    destroyWindow("src_threshold");
//  }
//
//  src_threshold.copyTo(out);
//
//  // 查找轮廓
//
//  vector<vector<Point>> contours;
//
//  // 注意，findContours会改变src_threshold
//  // 因此要输出src_threshold必须在这之前使用copyTo方法
//
//  findContours(src_threshold,
//    contours,               // a vector of contours
//    CV_RETR_EXTERNAL,
//    CV_CHAIN_APPROX_NONE);  // all pixels of each contours
//
//  vector<vector<Point>>::iterator itc = contours.begin();
//  while (itc != contours.end()) {
//    RotatedRect mr = minAreaRect(Mat(*itc));
//
//    // 需要进行大小尺寸判断
//    if (!verifyRotatedPlateSizes(mr))
//      itc = contours.erase(itc);
//    else {
//      ++itc;
//      contourRects.push_back(mr);
//
//      float width = mr.size.width;
//      float height = mr.size.height;     
//
//      RotatedRect candRect(mr.center,
//        Size2f(float(width * 1.05), float(height * 1.1)), mr.angle);
//
//      outRects.push_back(candRect);
//
//      Rect_<float> outputRect;
//      calcSafeRect(candRect, src, outputRect);
//
//      cv::rectangle(result, outputRect, Scalar(0, 0, 255));
//      mergedRects.push_back(outputRect);
//      if (0) {
//        imshow("outputRect", src(outputRect));
//        waitKey(0);
//      }
//    }
//  }
//
//
//  for (auto mergeRect : mergedRects) {
//    std::vector<Point> points;
//    Vec4f line;
//
//    int maxarea = 0;
//    Rect maxrect;
//
//    for (auto charRect : charRects) {
//      Rect interRect = mergeRect & charRect;
//
//      if (interRect == charRect) {
//
//        Point center(charRect.tl().x + charRect.width / 2, charRect.tl().y + charRect.height / 2);
//        points.push_back(center);
//
//        cv::circle(result, center, 3, Scalar(0, 255, 0), 2);
//        if (charRect.area() - maxarea > 0.1f) {
//          maxrect = charRect;
//          maxarea = charRect.area();
//        }
//      }       
//    }
//
//    if (points.size() < 5)
//      continue;
//
//    int left = mergeRect.tl().x - maxrect.width / 8 * 9;
//    left = left > 0 ? left : 0;
//    int right = mergeRect.br().x + maxrect.width / 8;
//    right = right + maxrect.width < src_threshold.cols - 1 ? right : src_threshold.cols - (maxrect.width + 1);
//
//    fitLine(Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);
//    float k = line[1] / line[0];
//    float step = 100;
//    cv::line(result, Point2f(line[2] - step, line[3] - k*step), Point2f(line[2] + step, k*step + line[3]), Scalar(255, 255, 255));
//
//    Point2f leftPoint((float)left, k * (left - line[2]) + line[3]);
//    Point2f rightPoint((float)right, k * (right - line[2]) + line[3]);
//
//    Rect leftRect(Point2f(leftPoint.x, leftPoint.y - maxrect.height / 2), maxrect.size());
//    Rect rightRect(Point2f(rightPoint.x, rightPoint.y - maxrect.height / 2), maxrect.size());
//
//    cv::rectangle(result, leftRect, Scalar(255, 255, 0));
//    cv::rectangle(result, rightRect, Scalar(255, 255, 0));
//
//    /*vector<Mat> slideMat;
//
//    int steplength = maxrect.width;
//    for (int step = - steplength / 2; step < steplength / 2; step++) {
//      int rightx = right + step;
//      Point2f rightPoint((float)rightx, k * (rightx - line[2]) + line[3]);
//      Rect slideRightRect(Point2f(rightPoint.x, rightPoint.y - maxrect.height / 2), maxrect.size());
//
//      Mat region = grayImage(slideRightRect);
//      Mat binary_region;
//      threshold(region, binary_region, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
//
//      Mat charInput = preprocessChar(binary_region, 20);
//
//      slideMat.push_back(charInput);
//    }*/
//  }
//
//
//  for (auto prect : plateRects) {
//    outRects.push_back(prect);
//
//    Rect_<float> outputRect;
//    calcSafeRect(prect, src, outputRect);
//
//    cv::rectangle(result, outputRect, Scalar(0, 0, 255));
//
//    if (0) {
//      imshow("outputRect", src(outputRect));
//      waitKey(0);
//    }
//    //for (auto rrect : contourRects) {
//    //  Rect rect = prect.boundingRect();
//    //  Rect interRect = rect & rrect.boundingRect();
//    //  Rect unionRect = rect | rrect.boundingRect();
//    //  double ratio = (double)interRect.area() / (double)unionRect.area();
//    //  
//    //  if (unionRect == rect && ratio > 0.618) {
//    //    std::cout << "ratio:" << ratio << std::endl;
//    //    float x = (float)rect.tl().x;
//    //    float y = (float)rect.tl().y;
//    //    float width = (float)rect.width;
//    //    float height = (float)rect.height;
//    //    RotatedRect candRect(Point2f(float(x + width / 2), float(y + height / 2)),
//    //      Size2f(float(width), float(height)), 0);
//
//    //    Rect_<float> outputRect;
//    //    calcSafeRect(candRect, src, outputRect);
//
//    //    cv::rectangle(result, outputRect, Scalar(0, 255, 0));
//
//    //    if (0) {
//    //      imshow("outputRect", src(outputRect));
//    //      waitKey(0);
//    //    }
//
//    //    outRects.push_back(candRect);
//    //  }
//    //    
//    //}
//  }
//
//  if (0) {
//    imshow("result", result);
//    waitKey(0);
//  }
//
//
//  return 0;
//}


// !基于HSV空间的颜色搜索方法
int CPlateLocate::colorSearch(const Mat &src, const Color r, Mat &out,
                              vector<RotatedRect> &outRects, int index) {
  Mat match_grey;

  // width值对最终结果影响很大，可以考虑进行多次colorSerch，每次不同的值
  // 另一种解决方案就是在结果输出到SVM之前，进行线与角的再纠正

  const int color_morph_width = 10;
  const int color_morph_height = 2;

  // 进行颜色查找

  colorMatch(src, match_grey, r, false);

  if (m_debug) {
    utils::imwrite("resources/image/tmp/match_grey.jpg", match_grey);
  }

  Mat src_threshold;
  threshold(match_grey, src_threshold, 0, 255,
            CV_THRESH_OTSU + CV_THRESH_BINARY);

  Mat element = getStructuringElement(
      MORPH_RECT, Size(color_morph_width, color_morph_height));
  morphologyEx(src_threshold, src_threshold, MORPH_CLOSE, element);

  if (m_debug) {
    utils::imwrite("resources/image/tmp/color.jpg", src_threshold);
  }

  src_threshold.copyTo(out);

  // 查找轮廓

  vector<vector<Point>> contours;

  // 注意，findContours会改变src_threshold
  // 因此要输出src_threshold必须在这之前使用copyTo方法

  findContours(src_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point>>::iterator itc = contours.begin();
  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));

    // 需要进行大小尺寸判断

    if (!verifySizes(mr))
      itc = contours.erase(itc);
    else {
      ++itc;
      outRects.push_back(mr);
    }
  }

  return 0;
}

//! Sobel第一次搜索
//! 不限制大小和形状，获取的BoundRect进入下一步

int CPlateLocate::sobelFrtSearch(const Mat &src,
                                 vector<Rect_<float>> &outRects) {
  Mat src_threshold;

  // soble操作，得到二值图像

  sobelOper(src, src_threshold, m_GaussianBlurSize, m_MorphSizeWidth,
            m_MorphSizeHeight);

  vector<vector<Point>> contours;
  findContours(src_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point>>::iterator itc = contours.begin();

  vector<RotatedRect> first_rects;

  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));

    // 需要进行大小尺寸判断

    if (verifySizes(mr)) {
      first_rects.push_back(mr);

      float area = mr.size.height * mr.size.width;
      float r = (float) mr.size.width / (float) mr.size.height;
      if (r < 1) r = (float) mr.size.height / (float) mr.size.width;
    }

    ++itc;
  }

  for (size_t i = 0; i < first_rects.size(); i++) {
    RotatedRect roi_rect = first_rects[i];

    Rect_<float> safeBoundRect;
    if (!calcSafeRect(roi_rect, src, safeBoundRect)) continue;

    outRects.push_back(safeBoundRect);
  }
  return 0;
}

//! Sobel第二次搜索,对断裂的部分进行再次的处理
//! 对大小和形状做限制，生成参考坐标

int CPlateLocate::sobelSecSearchPart(Mat &bound, Point2f refpoint,
                                     vector<RotatedRect> &outRects) {
  Mat bound_threshold;

  ////!
  ///第二次参数比一次精细，但针对的是得到的外接矩阵之后的图像，再sobel得到二值图像

  sobelOperT(bound, bound_threshold, 3, 6, 2);

  ////二值化去掉两边的边界

  // Mat mat_gray;
  // cvtColor(bound,mat_gray,CV_BGR2GRAY);

  // bound_threshold = mat_gray.clone();
  ////threshold(input_grey, img_threshold, 5, 255, CV_THRESH_OTSU +
  /// CV_THRESH_BINARY);
  // int w = mat_gray.cols;
  // int h = mat_gray.rows;
  // Mat tmp = mat_gray(Rect(w*0.15,h*0.2,w*0.6,h*0.6));
  // int threadHoldV = ThresholdOtsu(tmp);
  // threshold(mat_gray, bound_threshold,threadHoldV, 255, CV_THRESH_BINARY);

  Mat tempBoundThread = bound_threshold.clone();
  ////
  clearLiuDingOnly(tempBoundThread);

  int posLeft = 0, posRight = 0;
  if (bFindLeftRightBound(tempBoundThread, posLeft, posRight)) {

    //找到两个边界后进行连接修补处理

    if (posRight != 0 && posLeft != 0 && posLeft < posRight) {
      int posY = int(bound_threshold.rows * 0.5);
      for (int i = posLeft + (int) (bound_threshold.rows * 0.1);
           i < posRight - 4; i++) {
        bound_threshold.data[posY * bound_threshold.cols + i] = 255;
      }
    }

    utils::imwrite("resources/image/tmp/repaireimg1.jpg", bound_threshold);

    //两边的区域不要

    for (int i = 0; i < bound_threshold.rows; i++) {
      bound_threshold.data[i * bound_threshold.cols + posLeft] = 0;
      bound_threshold.data[i * bound_threshold.cols + posRight] = 0;
    }
    utils::imwrite("resources/image/tmp/repaireimg2.jpg", bound_threshold);
  }

  vector<vector<Point>> contours;
  findContours(bound_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point>>::iterator itc = contours.begin();

  vector<RotatedRect> second_rects;
  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));
    second_rects.push_back(mr);
    ++itc;
  }

  for (size_t i = 0; i < second_rects.size(); i++) {
    RotatedRect roi = second_rects[i];
    if (verifySizes(roi)) {
      Point2f refcenter = roi.center + refpoint;
      Size2f size = roi.size;
      float angle = roi.angle;

      RotatedRect refroi(refcenter, size, angle);
      outRects.push_back(refroi);
    }
  }

  return 0;
}

//! Sobel第二次搜索
//! 对大小和形状做限制，生成参考坐标

int CPlateLocate::sobelSecSearch(Mat &bound, Point2f refpoint,
                                 vector<RotatedRect> &outRects) {
  Mat bound_threshold;

  //!
  //第二次参数比一次精细，但针对的是得到的外接矩阵之后的图像，再sobel得到二值图像

  sobelOper(bound, bound_threshold, 3, 10, 3);

  utils::imwrite("resources/image/tmp/sobelSecSearch.jpg", bound_threshold);

  vector<vector<Point>> contours;
  findContours(bound_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point>>::iterator itc = contours.begin();

  vector<RotatedRect> second_rects;
  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));
    second_rects.push_back(mr);
    ++itc;
  }

  for (size_t i = 0; i < second_rects.size(); i++) {
    RotatedRect roi = second_rects[i];
    if (verifySizes(roi)) {
      Point2f refcenter = roi.center + refpoint;
      Size2f size = roi.size;
      float angle = roi.angle;

      RotatedRect refroi(refcenter, size, angle);
      outRects.push_back(refroi);
    }
  }

  return 0;
}

//! Sobel运算//对图像分割，腐蚀和膨胀的操作
//! 输入彩色图像，输出二值化图像

int CPlateLocate::sobelOper(const Mat &in, Mat &out, int blurSize, int morphW,
                            int morphH) {
  Mat mat_blur;
  mat_blur = in.clone();
  GaussianBlur(in, mat_blur, Size(blurSize, blurSize), 0, 0, BORDER_DEFAULT);

  Mat mat_gray;
  if (mat_blur.channels() == 3)
    cvtColor(mat_blur, mat_gray, CV_RGB2GRAY);
  else
    mat_gray = mat_blur;

  int scale = SOBEL_SCALE;
  int delta = SOBEL_DELTA;
  int ddepth = SOBEL_DDEPTH;

  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  // 对X  soble

  Sobel(mat_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_x, abs_grad_x);

  // 对Y  soble
  // Sobel(mat_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
  // convertScaleAbs(grad_y, abs_grad_y);
  // 在两个权值组合
  // 因为Y方向的权重是0，因此在此就不再计算Y方向的sobel了

  Mat grad;
  addWeighted(abs_grad_x, SOBEL_X_WEIGHT, 0, 0, 0, grad);

  // 分割

  Mat mat_threshold;
  double otsu_thresh_val =
      threshold(grad, mat_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

  // 腐蚀和膨胀

  Mat element = getStructuringElement(MORPH_RECT, Size(morphW, morphH));
  morphologyEx(mat_threshold, mat_threshold, MORPH_CLOSE, element);

  out = mat_threshold;

  return 0;
}

void DeleteNotArea(Mat &inmat) {
  Mat input_grey;
  cvtColor(inmat, input_grey, CV_BGR2GRAY);

  int w = inmat.cols;
  int h = inmat.rows;

  Mat tmpMat = inmat(Rect_<double>(w * 0.15, h * 0.1, w * 0.7, h * 0.7));

  //判断车牌颜色以此确认threshold方法

  Color plateType = getPlateType(tmpMat, true);
  Mat img_threshold;
  if (BLUE == plateType) {
    img_threshold = input_grey.clone();
    Mat tmp = input_grey(Rect_<double>(w * 0.15, h * 0.15, w * 0.7, h * 0.7));
    int threadHoldV = ThresholdOtsu(tmp);

    threshold(input_grey, img_threshold, threadHoldV, 255, CV_THRESH_BINARY);
    // threshold(input_grey, img_threshold, 5, 255, CV_THRESH_OTSU +
    // CV_THRESH_BINARY);

    utils::imwrite("resources/image/tmp/inputgray2.jpg", img_threshold);

  } else if (YELLOW == plateType) {
    img_threshold = input_grey.clone();
    Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
    int threadHoldV = ThresholdOtsu(tmp);

    threshold(input_grey, img_threshold, threadHoldV, 255,
              CV_THRESH_BINARY_INV);

    utils::imwrite("resources/image/tmp/inputgray2.jpg", img_threshold);

    // threshold(input_grey, img_threshold, 10, 255, CV_THRESH_OTSU +
    // CV_THRESH_BINARY_INV);
  } else
    threshold(input_grey, img_threshold, 10, 255,
              CV_THRESH_OTSU + CV_THRESH_BINARY);

  int posLeft = 0;
  int posRight = 0;

  int top = 0;
  int bottom = img_threshold.rows - 1;
  clearLiuDing(img_threshold, top, bottom);
  if (bFindLeftRightBound1(img_threshold, posLeft, posRight)) {
    inmat = inmat(Rect(posLeft, top, w - posLeft, bottom - top));
  }
}

//! 抗扭斜处理

int CPlateLocate::deskew(const Mat &src, const Mat &src_b,
                         vector<RotatedRect> &inRects,
                         vector<CPlate> &outPlates) {
  Mat mat_debug;
  src.copyTo(mat_debug);

  for (size_t i = 0; i < inRects.size(); i++) {
    RotatedRect roi_rect = inRects[i];

    float r = (float) roi_rect.size.width / (float) roi_rect.size.height;
    float roi_angle = roi_rect.angle;

    Size roi_rect_size = roi_rect.size;
    if (r < 1) {
      roi_angle = 90 + roi_angle;
      swap(roi_rect_size.width, roi_rect_size.height);
    }

    if (m_debug) {
      Point2f rect_points[4];
      roi_rect.points(rect_points);
      for (int j = 0; j < 4; j++)
        line(mat_debug, rect_points[j], rect_points[(j + 1) % 4],
             Scalar(0, 255, 255), 1, 8);
    }

    // changed
    // rotation = 90 - abs(roi_angle);
    // rotation < m_angel;

    // m_angle=60
    if (roi_angle - m_angle < 0 && roi_angle + m_angle > 0) {
      Rect_<float> safeBoundRect;
      bool isFormRect = calcSafeRect(roi_rect, src, safeBoundRect);
      if (!isFormRect) continue;

      Mat bound_mat = src(safeBoundRect);
      Mat bound_mat_b = src_b(safeBoundRect);

      Point2f roi_ref_center = roi_rect.center - safeBoundRect.tl();

      Mat deskew_mat;
      if ((roi_angle - 3 < 0 && roi_angle + 3 > 0) || 90.0 == roi_angle ||
          -90.0 == roi_angle) {
        deskew_mat = bound_mat;
      } else {

        // 角度在5到60度之间的，首先需要旋转 rotation

        Mat rotated_mat;
        Mat rotated_mat_b;

        if (!rotation(bound_mat, rotated_mat, roi_rect_size, roi_ref_center,
                      roi_angle))
          continue;

        if (!rotation(bound_mat_b, rotated_mat_b, roi_rect_size, roi_ref_center,
                      roi_angle))
          continue;

        // 如果图片偏斜，还需要视角转换 affine

        double roi_slope = 0;
        // imshow("1roated_mat",rotated_mat);
        // imshow("rotated_mat_b",rotated_mat_b);
        if (isdeflection(rotated_mat_b, roi_angle, roi_slope)) {
          affine(rotated_mat, deskew_mat, roi_slope);
        } else
          deskew_mat = rotated_mat;
      }

      Mat plate_mat;
      plate_mat.create(HEIGHT, WIDTH, TYPE);

      // haitungaga添加，删除非区域，这个函数影响了25%的完整定位率
      // DeleteNotArea(deskew_mat);

      // 这里对deskew_mat进行了一个筛选
      // 使用了经验数值：2.3和6

      if (deskew_mat.cols * 1.0 / deskew_mat.rows > 2.3 &&
          deskew_mat.cols * 1.0 / deskew_mat.rows < 6) {

        //如果图像大于我们所要求的图像，对图像进行一个大小变更

        if (deskew_mat.cols >= WIDTH || deskew_mat.rows >= HEIGHT)
          resize(deskew_mat, plate_mat, plate_mat.size(), 0, 0, INTER_AREA);
        else
          resize(deskew_mat, plate_mat, plate_mat.size(), 0, 0, INTER_CUBIC);

        CPlate plate;
        plate.setPlatePos(roi_rect);
        plate.setPlateMat(plate_mat);

        if (1) {
          imshow("plate_mat", plate_mat);
          waitKey(0);
          destroyWindow("plate_mat");
        }

        outPlates.push_back(plate);
      }
    }
  }

  return 0;
}

//! 旋转操作

bool CPlateLocate::rotation(Mat &in, Mat &out, const Size rect_size,
                            const Point2f center, const double angle) {
  Mat in_large;
  in_large.create(int(in.rows * 1.5), int(in.cols * 1.5), in.type());

  float x = in_large.cols / 2 - center.x > 0 ? in_large.cols / 2 - center.x : 0;
  float y = in_large.rows / 2 - center.y > 0 ? in_large.rows / 2 - center.y : 0;

  float width = x + in.cols < in_large.cols ? in.cols : in_large.cols - x;
  float height = y + in.rows < in_large.rows ? in.rows : in_large.rows - y;

  /*assert(width == in.cols);
  assert(height == in.rows);*/

  if (width != in.cols || height != in.rows) return false;

  Mat imageRoi = in_large(Rect_<float>(x, y, width, height));
  addWeighted(imageRoi, 0, in, 1, 0, imageRoi);

  Point2f center_diff(in.cols / 2.f, in.rows / 2.f);
  Point2f new_center(in_large.cols / 2.f, in_large.rows / 2.f);

  Mat rot_mat = getRotationMatrix2D(new_center, angle, 1);

  /*imshow("in_copy", in_large);
  waitKey(0);*/

  Mat mat_rotated;
  warpAffine(in_large, mat_rotated, rot_mat, Size(in_large.cols, in_large.rows),
             CV_INTER_CUBIC);

  /*imshow("mat_rotated", mat_rotated);
  waitKey(0);*/

  Mat img_crop;
  getRectSubPix(mat_rotated, Size(rect_size.width, rect_size.height),
                new_center, img_crop);

  out = img_crop;

  /*imshow("img_crop", img_crop);
  waitKey(0);*/

  return true;
}

//! 是否偏斜
//! 输入二值化图像，输出判断结果

bool CPlateLocate::isdeflection(const Mat &in, const double angle,
                                double &slope) { /*imshow("in",in);
                                                waitKey(0);*/
  int nRows = in.rows;
  int nCols = in.cols;

  assert(in.channels() == 1);

  int comp_index[3];
  int len[3];

  comp_index[0] = nRows / 4;
  comp_index[1] = nRows / 4 * 2;
  comp_index[2] = nRows / 4 * 3;

  const uchar* p;

  for (int i = 0; i < 3; i++) {
    int index = comp_index[i];
    p = in.ptr<uchar>(index);

    int j = 0;
    int value = 0;
    while (0 == value && j < nCols) value = int(p[j++]);

    len[i] = j;
  }

  // cout << "len[0]:" << len[0] << endl;
  // cout << "len[1]:" << len[1] << endl;
  // cout << "len[2]:" << len[2] << endl;

  // len[0]/len[1]/len[2]这三个应该是取车牌边线的值，来计算车牌边线的斜率

  double maxlen = max(len[2], len[0]);
  double minlen = min(len[2], len[0]);
  double difflen = abs(len[2] - len[0]);

  double PI = 3.14159265;

  // angle是根据水平那根直线的斜率转换过来的角度

  double g = tan(angle * PI / 180.0);

  if (maxlen - len[1] > nCols / 32 || len[1] - minlen > nCols / 32) {

    // 如果斜率为正，则底部在下，反之在上

    //求直线的斜率

    double slope_can_1 =
        double(len[2] - len[0]) / double(comp_index[1]);
    double slope_can_2 = double(len[1] - len[0]) / double(comp_index[0]);
    double slope_can_3 = double(len[2] - len[1]) / double(comp_index[0]);
    // cout<<"angle:"<<angle<<endl;
    // cout<<"g:"<<g<<endl;
    // cout << "slope_can_1:" << slope_can_1 << endl;
    // cout << "slope_can_2:" << slope_can_2 << endl;
    // cout << "slope_can_3:" << slope_can_3 << endl;
    // if(g>=0)
    slope = abs(slope_can_1 - g) <= abs(slope_can_2 - g) ? slope_can_1
                                                         : slope_can_2;
    // cout << "slope:" << slope << endl;
    return true;
  } else {
    slope = 0;
  }

  return false;
}

//! 扭变操作//通过opencv的仿射变换

void CPlateLocate::affine(const Mat &in, Mat &out, const double slope) {
  // imshow("in", in);
  // waitKey(0);

  //这里的slope是通过判断是否倾斜得出来的倾斜率

  Point2f dstTri[3];
  Point2f plTri[3];

  float height = (float) in.rows;
  float width = (float) in.cols;
  float xiff = (float) abs(slope) * height;

  if (slope > 0) {

    //右偏型，新起点坐标系在xiff/2位置

    plTri[0] = Point2f(0, 0);
    plTri[1] = Point2f(width - xiff - 1, 0);
    plTri[2] = Point2f(0 + xiff, height - 1);

    dstTri[0] = Point2f(xiff / 2, 0);
    dstTri[1] = Point2f(width - 1 - xiff / 2, 0);
    dstTri[2] = Point2f(xiff / 2, height - 1);
  } else {

    //左偏型，新起点坐标系在 -xiff/2位置

    plTri[0] = Point2f(0 + xiff, 0);
    plTri[1] = Point2f(width - 1, 0);
    plTri[2] = Point2f(0, height - 1);

    dstTri[0] = Point2f(xiff / 2, 0);
    dstTri[1] = Point2f(width - 1 - xiff + xiff / 2, 0);
    dstTri[2] = Point2f(xiff / 2, height - 1);
  }

  Mat warp_mat = getAffineTransform(plTri, dstTri);

  Mat affine_mat;
  affine_mat.create((int) height, (int) width, TYPE);

  if (in.rows > HEIGHT || in.cols > WIDTH)

    //仿射变换

    warpAffine(in, affine_mat, warp_mat, affine_mat.size(),
               CV_INTER_AREA);
  else
    warpAffine(in, affine_mat, warp_mat, affine_mat.size(), CV_INTER_CUBIC);

  out = affine_mat;
}

//! 计算一个安全的Rect
//! 如果不存在，返回false

// !基于颜色信息的车牌定位

int CPlateLocate::plateColorLocate(Mat src, vector<CPlate> &candPlates,
                                   int index) {
  vector<RotatedRect> rects_color_blue;
  vector<RotatedRect> rects_color_yellow;
  vector<CPlate> plates;
  Mat src_b;

  // 查找蓝色车牌
  // 查找颜色匹配车牌

  colorSearch(src, BLUE, src_b, rects_color_blue, index);

  // 进行抗扭斜处理

  deskew(src, src_b, rects_color_blue, plates);

  // 查找黄色车牌

  colorSearch(src, YELLOW, src_b, rects_color_yellow, index);
  deskew(src, src_b, rects_color_yellow, plates);

  for (size_t i = 0; i < plates.size(); i++) {
    candPlates.push_back(plates[i]);
  }
  return 0;
}


//! MSER plate locate

int CPlateLocate::plateMserLocate(Mat src, vector<CPlate> &candPlates, int img_index) {
  std::vector<Mat> channelImages;
  std::vector<Color> flags;

  // only conside blue plate
  if (1) {
    Mat grayImage;
    cvtColor(src, grayImage, COLOR_BGR2GRAY);
    channelImages.push_back(grayImage);
    flags.push_back(BLUE);

    //Mat singleChannelImage;
    //extractChannel(src, singleChannelImage, 2);
    //channelImages.push_back(singleChannelImage);
    //flags.push_back(BLUE);

    channelImages.push_back(255 - grayImage);
    flags.push_back(YELLOW);
  }

  int scale_size = 1024;
  double scale_ratio = 1;

  for (size_t i = 0; i < channelImages.size(); ++i) {
    vector<RotatedRect> rects_mser;
    vector<CPlate> plates;
    Mat src_b;

    Mat channelImage = channelImages.at(i);
    Color color = flags.at(i);
    Mat image = scaleImage(channelImage, Size(scale_size, scale_size), scale_ratio);

    // vector<RotatedRect> rects;
    mserSearch(image, color, src_b, plates, img_index);

    // deskew for rotation and slope image
    /*for (auto plate : plates) {
      RotatedRect rrect = plate.getPlatePos();
      rects_mser.push_back(rrect);
    }
    deskew(image, src_b, rects_mser, candPlates);*/

    // no deskew
    for (size_t j = 0; j < plates.size(); ++j) {
      CPlate plate = plates.at(j);
      RotatedRect rrect = plate.getPlatePos();
      RotatedRect scaleRect = scaleBackRRect(rrect, (float)scale_ratio);
      plate.setPlatePos(scaleRect);

      Rect_<float> outputRect;
      calcSafeRect(scaleRect, src, outputRect);

      Mat plate_mat;
      plate_mat.create(HEIGHT, WIDTH, TYPE);

      resize(src(outputRect), plate_mat, plate_mat.size(), 0, 0, INTER_AREA);

      plate.setPlateMat(plate_mat);

      candPlates.push_back(plate);
    }
  }

  //for (size_t i = 0; i < rects_mser.size(); ++i) {
  //  Rect_<float> outputRect;
  //  calcSafeRect(rects_mser[i], src, outputRect);

  //  if (0) {
  //    std::stringstream ss(std::stringstream::in | std::stringstream::out);
  //    ss << "resources/image/tmp/plate_" << i << ".jpg";
  //    imwrite(ss.str(), src(outputRect));
  //  }

  //  CPlate plate;
  //  plate.setPlateLocateType(CMSER);
  //  plate.setPlateMat(src(outputRect));
  //  plate.setPlatePos(rects_mser[i]);
  //  candPlates.push_back(plate);
  //}

  //deskew(src, src_b, rects_mser_blue, plates);

  //for (size_t i = 0; i < plates.size(); i++) {
  //  candPlates.push_back(plates[i]);
  //}

  return 0;
}

//! Sobel运算
//! 输入彩色图像，输出二值化图像

int CPlateLocate::sobelOperT(const Mat &in, Mat &out, int blurSize, int morphW,
                             int morphH) {
  Mat mat_blur;
  mat_blur = in.clone();
  GaussianBlur(in, mat_blur, Size(blurSize, blurSize), 0, 0, BORDER_DEFAULT);

  Mat mat_gray;
  if (mat_blur.channels() == 3)
    cvtColor(mat_blur, mat_gray, CV_BGR2GRAY);
  else
    mat_gray = mat_blur;

  utils::imwrite("resources/image/tmp/grayblure.jpg", mat_gray);

  // equalizeHist(mat_gray, mat_gray);

  int scale = SOBEL_SCALE;
  int delta = SOBEL_DELTA;
  int ddepth = SOBEL_DDEPTH;

  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  Sobel(mat_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_x, abs_grad_x);

  //因为Y方向的权重是0，因此在此就不再计算Y方向的sobel了

  Mat grad;
  addWeighted(abs_grad_x, 1, 0, 0, 0, grad);

  utils::imwrite("resources/image/tmp/graygrad.jpg", grad);

  Mat mat_threshold;
  double otsu_thresh_val =
      threshold(grad, mat_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

  utils::imwrite("resources/image/tmp/grayBINARY.jpg", mat_threshold);

  Mat element = getStructuringElement(MORPH_RECT, Size(morphW, morphH));
  morphologyEx(mat_threshold, mat_threshold, MORPH_CLOSE, element);

  utils::imwrite("resources/image/tmp/phologyEx.jpg", mat_threshold);

  out = mat_threshold;

  return 0;
}

// !基于垂直线条的车牌定位

int CPlateLocate::plateSobelLocate(Mat src, vector<CPlate> &candPlates,
                                   int index) {
  vector<RotatedRect> rects_sobel;
  vector<RotatedRect> rects_sobel_sel;
  vector<CPlate> plates;

  vector<Rect_<float>> bound_rects;

  // Sobel第一次粗略搜索

  sobelFrtSearch(src, bound_rects);

  vector<Rect_<float>> bound_rects_part;

  //对不符合要求的区域进行扩展

  for (size_t i = 0; i < bound_rects.size(); i++) {
    float fRatio = bound_rects[i].width * 1.0f / bound_rects[i].height;
    if (fRatio < 3.0 && fRatio > 1.0 && bound_rects[i].height < 120) {
      Rect_<float> itemRect = bound_rects[i];

      //宽度过小，进行扩展

      itemRect.x = itemRect.x - itemRect.height * (4 - fRatio);
      if (itemRect.x < 0) {
        itemRect.x = 0;
      }
      itemRect.width = itemRect.width + itemRect.height * 2 * (4 - fRatio);
      if (itemRect.width + itemRect.x >= src.cols) {
        itemRect.width = src.cols - itemRect.x;
      }

      itemRect.y = itemRect.y - itemRect.height * 0.08f;
      itemRect.height = itemRect.height * 1.16f;

      bound_rects_part.push_back(itemRect);
    }
  }

  //对断裂的部分进行二次处理

  for (size_t i = 0; i < bound_rects_part.size(); i++) {
    Rect_<float> bound_rect = bound_rects_part[i];
    Point2f refpoint(bound_rect.x, bound_rect.y);

    float x = bound_rect.x > 0 ? bound_rect.x : 0;
    float y = bound_rect.y > 0 ? bound_rect.y : 0;

    float width =
        x + bound_rect.width < src.cols ? bound_rect.width : src.cols - x;
    float height =
        y + bound_rect.height < src.rows ? bound_rect.height : src.rows - y;

    Rect_<float> safe_bound_rect(x, y, width, height);
    Mat bound_mat = src(safe_bound_rect);

    // Sobel第二次精细搜索(部分)

    sobelSecSearchPart(bound_mat, refpoint, rects_sobel);
  }

  for (size_t i = 0; i < bound_rects.size(); i++) {
    Rect_<float> bound_rect = bound_rects[i];
    Point2f refpoint(bound_rect.x, bound_rect.y);

    float x = bound_rect.x > 0 ? bound_rect.x : 0;
    float y = bound_rect.y > 0 ? bound_rect.y : 0;

    float width =
        x + bound_rect.width < src.cols ? bound_rect.width : src.cols - x;
    float height =
        y + bound_rect.height < src.rows ? bound_rect.height : src.rows - y;

    Rect_<float> safe_bound_rect(x, y, width, height);
    Mat bound_mat = src(safe_bound_rect);

    // Sobel第二次精细搜索

    sobelSecSearch(bound_mat, refpoint, rects_sobel);
    // sobelSecSearchPart(bound_mat, refpoint, rects_sobel);
  }

  Mat src_b;
  sobelOper(src, src_b, 3, 10, 3);

  // 进行抗扭斜处理

  deskew(src, src_b, rects_sobel, plates);

  for (size_t i = 0; i < plates.size(); i++) candPlates.push_back(plates[i]);

  return 0;
}

// !车牌定位
// !把颜色与Sobel定位的车牌全部输出

int CPlateLocate::plateLocate(Mat src, vector<Mat> &resultVec, int index) {
  vector<CPlate> all_result_Plates;

  plateColorLocate(src, all_result_Plates, index);
  plateSobelLocate(src, all_result_Plates, index);

  for (size_t i = 0; i < all_result_Plates.size(); i++) {
    CPlate plate = all_result_Plates[i];
    resultVec.push_back(plate.getPlateMat());
  }

  return 0;
}

}