// 这个文件定义了EasyPR里所有plate判断的通用函数
// 所属命名空间为easypr
// 这个部分中的函数轻易不要改动

#include "easypr/core/core_func.h"
#include "easypr/core/plate.hpp"
#include "easypr/core/chars_identify.h"
#include "easypr/config.h"

using namespace cv;

namespace easypr {

//! 根据一幅图像与颜色模板获取对应的二值图
//! 输入RGB图像, 颜色模板（蓝色、黄色）
//! 输出灰度图（只有0和255两个值，255代表匹配，0代表不匹配）

Mat colorMatch(const Mat &src, Mat &match, const Color r,
               const bool adaptive_minsv) {

  // S和V的最小值由adaptive_minsv这个bool值判断
  // 如果为true，则最小值取决于H值，按比例衰减
  // 如果为false，则不再自适应，使用固定的最小值minabs_sv
  // 默认为false

  const float max_sv = 255;
  const float minref_sv = 64;

  const float minabs_sv = 95;

  // blue的H范围

  const int min_blue = 100;  // 100
  const int max_blue = 140;  // 140

  // yellow的H范围

  const int min_yellow = 15;  // 15
  const int max_yellow = 40;  // 40

  // white的H范围

  const int min_white = 0;   // 15
  const int max_white = 30;  // 40

  Mat src_hsv;

  // 转到HSV空间进行处理，颜色搜索主要使用的是H分量进行蓝色与黄色的匹配工作

  cvtColor(src, src_hsv, CV_BGR2HSV);

  std::vector<cv::Mat> hsvSplit;
  split(src_hsv, hsvSplit);
  equalizeHist(hsvSplit[2], hsvSplit[2]);
  merge(hsvSplit, src_hsv);

  //匹配模板基色,切换以查找想要的基色

  int min_h = 0;
  int max_h = 0;
  switch (r) {
    case BLUE:
      min_h = min_blue;
      max_h = max_blue;
      break;
    case YELLOW:
      min_h = min_yellow;
      max_h = max_yellow;
      break;
    case WHITE:
      min_h = min_white;
      max_h = max_white;
      break;
    default:
      // Color::UNKNOWN
      break;
  }

  float diff_h = float((max_h - min_h) / 2);
  float avg_h = min_h + diff_h;

  int channels = src_hsv.channels();
  int nRows = src_hsv.rows;

  //图像数据列需要考虑通道数的影响；

  int nCols = src_hsv.cols * channels;

  //连续存储的数据，按一行处理

  if (src_hsv.isContinuous()) {
    nCols *= nRows;
    nRows = 1;
  }

  int i, j;
  uchar* p;
  float s_all = 0;
  float v_all = 0;
  float count = 0;
  for (i = 0; i < nRows; ++i) {
    p = src_hsv.ptr<uchar>(i);
    for (j = 0; j < nCols; j += 3) {
      int H = int(p[j]);      // 0-180
      int S = int(p[j + 1]);  // 0-255
      int V = int(p[j + 2]);  // 0-255

      s_all += S;
      v_all += V;
      count++;

      bool colorMatched = false;

      if (H > min_h && H < max_h) {
        float Hdiff = 0;
        if (H > avg_h)
          Hdiff = H - avg_h;
        else
          Hdiff = avg_h - H;

        float Hdiff_p = float(Hdiff) / diff_h;

        // S和V的最小值由adaptive_minsv这个bool值判断
        // 如果为true，则最小值取决于H值，按比例衰减
        // 如果为false，则不再自适应，使用固定的最小值minabs_sv

        float min_sv = 0;
        if (true == adaptive_minsv)
          min_sv =
              minref_sv -
                  minref_sv / 2 *
                      (1
                          - Hdiff_p);  // inref_sv - minref_sv / 2 * (1 - Hdiff_p)
        else
          min_sv = minabs_sv;  // add

        if ((S > min_sv && S < max_sv) && (V > min_sv && V < max_sv))
          colorMatched = true;
      }

      if (colorMatched == true) {
        p[j] = 0;
        p[j + 1] = 0;
        p[j + 2] = 255;
      } else {
        p[j] = 0;
        p[j + 1] = 0;
        p[j + 2] = 0;
      }
    }
  }

  // cout << "avg_s:" << s_all / count << endl;
  // cout << "avg_v:" << v_all / count << endl;

  // 获取颜色匹配后的二值灰度图

  Mat src_grey;
  std::vector<cv::Mat> hsvSplit_done;
  split(src_hsv, hsvSplit_done);
  src_grey = hsvSplit_done[2];

  match = src_grey;

  return src_grey;
}

bool bFindLeftRightBound1(Mat &bound_threshold, int &posLeft, int &posRight) {

  //从两边寻找边界

  float span = bound_threshold.rows * 0.2f;

  //左边界检测

  for (int i = 0; i < bound_threshold.cols - span - 1; i += 3) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l < i + span; l++) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }
    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.15) {
      posLeft = i;
      break;
    }
  }
  span = bound_threshold.rows * 0.2f;

  //右边界检测

  for (int i = bound_threshold.cols - 1; i > span; i -= 2) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l > i - span; l--) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }

    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.06) {
      posRight = i;
      if (posRight + 5 < bound_threshold.cols) {
        posRight = posRight + 5;
      } else {
        posRight = bound_threshold.cols - 1;
      }

      break;
    }
  }

  if (posLeft < posRight) {
    return true;
  }
  return false;
}

bool bFindLeftRightBound(Mat &bound_threshold, int &posLeft, int &posRight) {

  //从两边寻找边界

  float span = bound_threshold.rows * 0.2f;

  //左边界检测

  for (int i = 0; i < bound_threshold.cols - span - 1; i += 2) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l < i + span; l++) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }
    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.36) {
      posLeft = i;
      break;
    }
  }
  span = bound_threshold.rows * 0.2f;

  //右边界检测

  for (int i = bound_threshold.cols - 1; i > span; i -= 2) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l > i - span; l--) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }

    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.26) {
      posRight = i;
      break;
    }
  }

  if (posLeft < posRight) {
    return true;
  }
  return false;
}

bool bFindLeftRightBound2(Mat &bound_threshold, int &posLeft, int &posRight) {

  //从两边寻找边界

  float span = bound_threshold.rows * 0.2f;

  //左边界检测

  for (int i = 0; i < bound_threshold.cols - span - 1; i += 3) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l < i + span; l++) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }
    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.32) {
      posLeft = i;
      break;
    }
  }
  span = bound_threshold.rows * 0.2f;

  //右边界检测

  for (int i = bound_threshold.cols - 1; i > span; i -= 3) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l > i - span; l--) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }

    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.22) {
      posRight = i;
      break;
    }
  }

  if (posLeft < posRight) {
    return true;
  }
  return false;
}

//! 判断一个车牌的颜色
//! 输入车牌mat与颜色模板
//! 返回true或fasle

bool plateColorJudge(const Mat &src, const Color r, const bool adaptive_minsv,
                     float &percent) {
  // 判断阈值

  const float thresh = 0.45f;

  Mat src_gray;
  colorMatch(src, src_gray, r, adaptive_minsv);

  percent =
      float(countNonZero(src_gray)) / float(src_gray.rows * src_gray.cols);
  // cout << "percent:" << percent << endl;

  if (percent > thresh)
    return true;
  else
    return false;
}

//判断车牌的类型

Color getPlateType(const Mat &src, const bool adaptive_minsv) {
  float max_percent = 0;
  Color max_color = UNKNOWN;

  float blue_percent = 0;
  float yellow_percent = 0;
  float white_percent = 0;

  if (plateColorJudge(src, BLUE, adaptive_minsv, blue_percent) == true) {
    // cout << "BLUE" << endl;
    return BLUE;
  } else if (plateColorJudge(src, YELLOW, adaptive_minsv, yellow_percent) ==
      true) {
    // cout << "YELLOW" << endl;
    return YELLOW;
  } else if (plateColorJudge(src, WHITE, adaptive_minsv, white_percent) ==
      true) {
    // cout << "WHITE" << endl;
    return WHITE;
  } else {
    // cout << "OTHER" << endl;

    // 如果任意一者都不大于阈值，则取值最大者

    max_percent = blue_percent > yellow_percent ? blue_percent : yellow_percent;
    max_color = blue_percent > yellow_percent ? BLUE : YELLOW;

    max_color = max_percent > white_percent ? max_color : WHITE;
    return max_color;
  }
}

void clearLiuDingOnly(Mat &img) {
  const int x = 7;
  Mat jump = Mat::zeros(1, img.rows, CV_32F);
  for (int i = 0; i < img.rows; i++) {
    int jumpCount = 0;
    int whiteCount = 0;
    for (int j = 0; j < img.cols - 1; j++) {
      if (img.at<char>(i, j) != img.at<char>(i, j + 1)) jumpCount++;

      if (img.at<uchar>(i, j) == 255) {
        whiteCount++;
      }
    }

    jump.at<float>(i) = (float) jumpCount;
  }

  for (int i = 0; i < img.rows; i++) {
    if (jump.at<float>(i) <= x) {
      for (int j = 0; j < img.cols; j++) {
        img.at<char>(i, j) = 0;
      }
    }
  }
}

//去除车牌上方的钮钉
//计算每行元素的阶跃数，如果小于X认为是柳丁，将此行全部填0（涂黑）
// X的推荐值为，可根据实际调整

bool clearLiuDing(Mat &img) {
  std::vector<float> fJump;
  int whiteCount = 0;
  const int x = 7;
  Mat jump = Mat::zeros(1, img.rows, CV_32F);
  for (int i = 0; i < img.rows; i++) {
    int jumpCount = 0;

    for (int j = 0; j < img.cols - 1; j++) {
      if (img.at<char>(i, j) != img.at<char>(i, j + 1)) jumpCount++;

      if (img.at<uchar>(i, j) == 255) {
        whiteCount++;
      }
    }

    jump.at<float>(i) = (float) jumpCount;
  }

  int iCount = 0;
  for (int i = 0; i < img.rows; i++) {
    fJump.push_back(jump.at<float>(i));
    if (jump.at<float>(i) >= 16 && jump.at<float>(i) <= 45) {

      //车牌字符满足一定跳变条件

      iCount++;
    }
  }

  ////这样的不是车牌

  if (iCount * 1.0 / img.rows <= 0.40) {

    //满足条件的跳变的行数也要在一定的阈值内

    return false;
  }

  //不满足车牌的条件

  if (whiteCount * 1.0 / (img.rows * img.cols) < 0.15 ||
      whiteCount * 1.0 / (img.rows * img.cols) > 0.50) {
    return false;
  }

  for (int i = 0; i < img.rows; i++) {
    if (jump.at<float>(i) <= x) {
      for (int j = 0; j < img.cols; j++) {
        img.at<char>(i, j) = 0;
      }
    }
  }
  return true;
}

void clearLiuDing(Mat mask, int &top, int &bottom) {
  const int x = 7;

  for (int i = 0; i < mask.rows / 2; i++) {
    int whiteCount = 0;
    int jumpCount = 0;
    for (int j = 0; j < mask.cols - 1; j++) {
      if (mask.at<char>(i, j) != mask.at<char>(i, j + 1)) jumpCount++;

      if ((int) mask.at<uchar>(i, j) == 255) {
        whiteCount++;
      }
    }
    if ((jumpCount < x && whiteCount * 1.0 / mask.cols > 0.15) ||
        whiteCount < 4) {
      top = i;
    }
  }
  top -= 1;
  if (top < 0) {
    top = 0;
  }

  // ok,找到上下边界

  for (int i = mask.rows - 1; i >= mask.rows / 2; i--) {
    int jumpCount = 0;
    int whiteCount = 0;
    for (int j = 0; j < mask.cols - 1; j++) {
      if (mask.at<char>(i, j) != mask.at<char>(i, j + 1)) jumpCount++;
      if (mask.at<uchar>(i, j) == 255) {
        whiteCount++;
      }
    }
    if ((jumpCount < x && whiteCount * 1.0 / mask.cols > 0.15) ||
        whiteCount < 4) {
      bottom = i;
    }
  }
  bottom += 1;
  if (bottom >= mask.rows) {
    bottom = mask.rows - 1;
  }

  if (top >= bottom) {
    top = 0;
    bottom = mask.rows - 1;
  }
}

int ThresholdOtsu(Mat mat) {
  int height = mat.rows;
  int width = mat.cols;

  // histogram
  float histogram[256] = {0};
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      unsigned char p = (unsigned char) ((mat.data[i * mat.step[0] + j]));
      histogram[p]++;
    }
  }
  // normalize histogram
  int size = height * width;
  for (int i = 0; i < 256; i++) {
    histogram[i] = histogram[i] / size;
  }

  // average pixel value
  float avgValue = 0;
  for (int i = 0; i < 256; i++) {
    avgValue += i * histogram[i];
  }

  int thresholdV;
  float maxVariance = 0;
  float w = 0, u = 0;
  for (int i = 0; i < 256; i++) {
    w += histogram[i];
    u += i * histogram[i];

    float t = avgValue * w - u;
    float variance = t * t / (w * (1 - w));
    if (variance > maxVariance) {
      maxVariance = variance;
      thresholdV = i;
    }
  }

  return thresholdV;
}

//! 直方图均衡

Mat histeq(Mat in) {
  Mat out(in.size(), in.type());
  if (in.channels() == 3) {
    Mat hsv;
    std::vector<cv::Mat> hsvSplit;
    cvtColor(in, hsv, CV_BGR2HSV);
    split(hsv, hsvSplit);
    equalizeHist(hsvSplit[2], hsvSplit[2]);
    merge(hsvSplit, hsv);
    cvtColor(hsv, out, CV_HSV2BGR);
  } else if (in.channels() == 1) {
    equalizeHist(in, out);
  }
  return out;
}

#define HORIZONTAL 1
#define VERTICAL 0

Mat CutTheRect(Mat &in, Rect &rect) {
  int size = in.cols;  // (rect.width>rect.height)?rect.width:rect.height;
  Mat dstMat(size, size, CV_8UC1);
  dstMat.setTo(Scalar(0, 0, 0));

  int x = (int) floor((float) (size - rect.width) / 2.0f);
  int y = (int) floor((float) (size - rect.height) / 2.0f);

  //把rect中的数据 考取到dstMat的中间

  for (int i = 0; i < rect.height; ++i) {

    //宽

    for (int j = 0; j < rect.width; ++j) {
      dstMat.data[dstMat.step[0] * (i + y) + j + x] =
          in.data[in.step[0] * (i + rect.y) + j + rect.x];
    }
  }

  //
  return dstMat;
}

Rect GetCenterRect(Mat &in) {
  Rect _rect;

  int top = 0;
  int bottom = in.rows - 1;

  //上下

  for (int i = 0; i < in.rows; ++i) {
    bool bFind = false;
    for (int j = 0; j < in.cols; ++j) {
      if (in.data[i * in.step[0] + j] > 20) {
        top = i;
        bFind = true;
        break;
      }
    }
    if (bFind) {
      break;
    }

    //统计这一行或一列中，非零元素的个数

  }
  for (int i = in.rows - 1;
  i >= 0;
  --i) {
    bool bFind = false;
    for (int j = 0; j < in.cols; ++j) {
      if (in.data[i * in.step[0] + j] > 20) {
        bottom = i;
        bFind = true;
        break;
      }
    }
    if (bFind) {
      break;
    }

    //统计这一行或一列中，非零元素的个数

  }

  //左右

  int left = 0;
  int right = in.cols - 1;
  for (int j = 0; j < in.cols; ++j) {
    bool bFind = false;
    for (int i = 0; i < in.rows; ++i) {
      if (in.data[i * in.step[0] + j] > 20) {
        left = j;
        bFind = true;
        break;
      }
    }
    if (bFind) {
      break;
    }

    //统计这一行或一列中，非零元素的个数

  }
  for (int j = in.cols - 1;
  j >= 0;
  --j) {
    bool bFind = false;
    for (int i = 0; i < in.rows; ++i) {
      if (in.data[i * in.step[0] + j] > 20) {
        right = j;
        bFind = true;

        break;
      }
    }
    if (bFind) {
      break;
    }

    //统计这一行或一列中，非零元素的个数

  }

  _rect.x = left;
  _rect.y = top;
  _rect.width = right - left + 1;
  _rect.height = bottom - top + 1;

  return _rect;
}

float countOfBigValue(Mat &mat, int iValue) {
  float iCount = 0.0;
  if (mat.rows > 1) {
    for (int i = 0; i < mat.rows; ++i) {
      if (mat.data[i * mat.step[0]] > iValue) {
        iCount += 1.0;
      }
    }
    return iCount;

  } else {
    for (int i = 0; i < mat.cols; ++i) {
      if (mat.data[i] > iValue) {
        iCount += 1.0;
      }
    }

    return iCount;
  }
}

// ！获取垂直和水平方向直方图

Mat ProjectedHistogram(Mat img, int t) {
  int sz = (t) ? img.rows : img.cols;
  Mat mhist = Mat::zeros(1, sz, CV_32F);

  for (int j = 0; j < sz; j++) {
    Mat data = (t) ? img.row(j) : img.col(j);

    //统计这一行或一列中，非零元素的个数，并保存到mhist中

    mhist.at<float>(j) = countOfBigValue(data, 20);
  }

  // Normalize histogram
  double min, max;
  minMaxLoc(mhist, &min, &max);

  //用mhist直方图中的最大值，归一化直方图

  if (max > 0)
    mhist.convertTo(mhist, -1, 1.0f / max, 0);

  return mhist;
}

Mat preprocessChar(Mat in, int char_size) {
  // Remap image
  int h = in.rows;
  int w = in.cols;

  //统一每个字符的大小

  int charSize = char_size;

  Mat transformMat = Mat::eye(2, 3, CV_32F);
  int m = max(w, h);
  transformMat.at<float>(0, 2) = float(m / 2 - w / 2);
  transformMat.at<float>(1, 2) = float(m / 2 - h / 2);

  Mat warpImage(m, m, in.type());
  warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR,
    BORDER_CONSTANT, Scalar(0));

  //！ 将所有的字符调整成统一的尺寸

  Mat out;
  resize(warpImage, out, Size(charSize, charSize));

  return out;
}

Rect GetChineseRect(const Rect rectSpe) {
  int height = rectSpe.height;
  float newwidth = rectSpe.width * 1.10f;
  int x = rectSpe.x;
  int y = rectSpe.y;

  int newx = x - int(newwidth * 1.10f);
  newx = newx > 0 ? newx : 0;

  Rect a(newx, y, int(newwidth), height);

  return a;
}

bool verifyCharSizes(Rect r) {
  // Char sizes 45x90
  float aspect = 45.0f / 90.0f;
  float charAspect = (float)r.width / (float)r.height;
  float error = 0.35f;
  float minHeight = 25.f;
  float maxHeight = 50.f;
  // We have a different aspect ratio for number 1, and it can be ~0.2
  float minAspect = 0.05f;
  float maxAspect = aspect + aspect * error;

  // bb area
  int bbArea = r.width * r.height;

  if (charAspect > minAspect && charAspect < maxAspect /*&&
                                                       r.rows >= minHeight && r.rows < maxHeight*/)
                                                       return true;
  else
    return false;
}

// 图像缩放
Mat scaleImage(const Mat& image, const Size& maxSize, double& scale_ratio) {
  Mat ret;

  if (image.cols > maxSize.width || image.rows > maxSize.height) {
    double widthRatio = image.cols / (double)maxSize.width;
    double heightRatio = image.rows / (double)maxSize.height;
    double m_real_to_scaled_ratio = max(widthRatio, heightRatio);

    int newWidth = int(image.cols / m_real_to_scaled_ratio);
    int newHeight = int(image.rows / m_real_to_scaled_ratio);

    resize(image, ret, Size(newWidth, newHeight), 0, 0);
    scale_ratio = m_real_to_scaled_ratio;
  }
  else {
    ret = image;
    scale_ratio = 1.0;
  }

  return ret;
}


// Scale back RotatedRect
RotatedRect scaleBackRRect(const RotatedRect& rr, const float scale_ratio) {
  float width = rr.size.width * scale_ratio;
  float height = rr.size.height * scale_ratio;
  float x = rr.center.x * scale_ratio;
  float y = rr.center.y * scale_ratio;
  RotatedRect mserRect(Point2f(x, y), Size2f(width, height), rr.angle);
  
  return mserRect;
}

bool verifyPlateSize(Rect mr) {
  float error = 0.6f;
  // Spain car plate size: 52x11 aspect 4,7272
  // China car plate size: 440mm*140mm，aspect 3.142857

  // Real car plate size: 136 * 32, aspect 4
  float aspect = 3.75;

  // Set a min and max area. All other patchs are discarded
  // int min= 1*aspect*1; // minimum area
  // int max= 2000*aspect*2000; // maximum area
  int min = 34 * 8 * 1;  // minimum area
  int max = 34 * 8 * 200;  // maximum area

  // Get only patchs that match to a respect ratio.
  float rmin = aspect - aspect * error;
  float rmax = aspect + aspect * error;

  float area = float(mr.height * mr.width);
  float r = (float)mr.width / (float)mr.height;
  if (r < 1) r = (float)mr.height / (float)mr.width;

  // cout << "area:" << area << endl;
  // cout << "r:" << r << endl;

  if ((area < min || area > max) || (r < rmin || r > rmax))
    return false;
  else
    return true;
}

bool verifyRotatedPlateSizes(RotatedRect mr, bool showDebug) {
  float error = 0.65f;
  // Spain car plate size: 52x11 aspect 4,7272
  // China car plate size: 440mm*140mm，aspect 3.142857

  // Real car plate size: 136 * 32, aspect 4
  float aspect = 3.75f;

  // Set a min and max area. All other patchs are discarded
  // int min= 1*aspect*1; // minimum area
  // int max= 2000*aspect*2000; // maximum area
  //int min = 34 * 8 * 1;  // minimum area
  //int max = 34 * 8 * 200;  // maximum area

  // Get only patchs that match to a respect ratio.
  float aspect_min = aspect - aspect * error;
  float aspect_max = aspect + aspect * error;

  float width_max = 600.f;
  float width_min = 30.f;

  float min = float(width_min * width_min / aspect_max);  // minimum area
  float max = float(width_max * width_max / aspect_min);  // maximum area

  float width = mr.size.width;
  float height = mr.size.height;
  float area = width * height;

  float ratio = width / height;
  float angle = mr.angle;
  if (ratio < 1) {
    swap(width, height);
    ratio = width / height;

    angle = 90.f + angle;
    //std::cout << "angle:" << angle << std::endl;
  }

  float angle_min = -60.f;
  float angle_max = 60.f;

  //std::cout << "aspect_min:" << aspect_min << std::endl;
  //std::cout << "aspect_max:" << aspect_max << std::endl;

  if (area < min || area > max) {
    if (0 && showDebug) {
      std::cout << "area < min || area > max: " << area << std::endl;
    }

    return false;
  }
  else if (ratio < aspect_min || ratio > aspect_max) {
    if (0 && showDebug) {
      std::cout << "ratio < aspect_min || ratio > aspect_max: " << ratio << std::endl;
    }
    
    return false;
  }
  else if (angle < angle_min || angle > angle_max) {
    if (0 && showDebug) {
      std::cout << "angle < angle_min || angle > angle_max: " << angle << std::endl;
    }
    
    return false;
  }
  else if (width < width_min || width > width_max) {
    if (0 && showDebug) {
      std::cout << "width < width_min || width > width_max: " << width << std::endl;
    }
    
    return false;  
  }
  else {
    return true;
  }

  return true;
}

//! 非极大值抑制
void NMStoCharacter(std::vector<CCharacter> &inVec, double overlap) {

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
}

// judge weather two CCharacter are nearly the same;
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

  // the character center in plate are not to near in the x-axis
  float x_1 = float(rect1.tl().x + rect1.width / 2);
  float x_2 = float(rect2.tl().x + rect2.width / 2);

  float x_diff = abs(x_1 - x_2);
  double x_diff_ratio = x_diff / min(height_1, height_2);

  if (x_diff_ratio < 0.25)
    return false;

  // the character in plate are near in the x-axis but not very near
  float x_margin_left = float(min(rect1.br().x, rect2.br().x));
  float x_margin_right = float(max(rect1.tl().x, rect2.tl().x));

  float x_margin_diff = abs(x_margin_left - x_margin_right);
  double x_margin_diff_ratio = x_margin_diff / min(height_1, height_2);

  if (x_margin_diff_ratio > 1.0)
    return false;

  return true;
}

//! merge chars to group, using the similarity
void mergeCharToGroup(std::vector<CCharacter> vecRect,
  std::vector<std::vector<CCharacter>>& charGroupVec) {

  std::vector<int> labels;

  int numbers = 0;
  if (vecRect.size() > 0)
    numbers = partition(vecRect, labels, &compareCharRect);

  for (size_t j = 0; j < size_t(numbers); j++) {
    std::vector<CCharacter> charGroup;

    for (size_t t = 0; t < vecRect.size(); t++) {
      int label = labels[t];

      if (label == j)
        charGroup.push_back(vecRect[t]);
    }

    if (charGroup.size() < 2)
      continue;

    charGroupVec.push_back(charGroup);
  }
}


void rotatedRectangle(InputOutputArray image, RotatedRect rrect, const Scalar& color, int thickness, int lineType, int shift) {
  Point2f rect_points[4];
  rrect.points(rect_points);
  for (int j = 0; j < 4; j++) {
    line(image, rect_points[j], rect_points[(j + 1) % 4], color, thickness, lineType, shift);
  }
}


void searchWeakSeed(const std::vector<CCharacter>& charVec, std::vector<CCharacter>& mserCharacter,
  const Vec4f& line, Point& boundaryPoint, const Rect& maxrect, Rect& plateResult, CharSearchDirection searchDirection) {

  float k = line[1] / line[0];
  float x_1 = line[2];
  float y_1 = line[3];

  std::vector<CCharacter> searchWeakSeedVec;

  for (auto weakSeed : charVec) {
    Rect weakRect = weakSeed.getCharacterPos();
    Point weakCenter(weakRect.tl().x + weakRect.width / 2, weakRect.tl().y + weakRect.height / 2);
    float x_2 = (float)weakCenter.x;

    if (searchDirection == CharSearchDirection::LEFT) {
      if (weakCenter.x + weakRect.width / 2 > boundaryPoint.x) {
        continue;
      }
    }
    else if (searchDirection == CharSearchDirection::RIGHT) {
      if (weakCenter.x - weakRect.width / 2 < boundaryPoint.x) {
        continue;
      }
    }

    float y_2l = k * (x_2 - x_1) + y_1;
    float y_2 = (float)weakCenter.y;

    float y_diff_ratio = abs(y_2l - y_2) / maxrect.height;

    if (y_diff_ratio < 0.1) {
      float width_1 = float(maxrect.width);
      float height_1 = float(maxrect.height);

      float width_2 = float(weakRect.width);
      float height_2 = float(weakRect.height);

      float height_diff = abs(height_1 - height_2);
      double height_diff_ratio = height_diff / min(height_1, height_2);

      if (height_diff_ratio < 0.15) {
        searchWeakSeedVec.push_back(weakSeed);
      }
    }
  }

  // form right to left to split
  if (searchWeakSeedVec.size() != 0) {
    if (searchDirection == CharSearchDirection::LEFT) {
      std::sort(searchWeakSeedVec.begin(), searchWeakSeedVec.end(),
        [](const CCharacter& r1, const CCharacter& r2) {
        return r1.getCharacterPos().tl().x > r2.getCharacterPos().tl().x;
      });
    }
    else if (searchDirection == CharSearchDirection::RIGHT) {
      std::sort(searchWeakSeedVec.begin(), searchWeakSeedVec.end(),
        [](const CCharacter& r1, const CCharacter& r2) {
        return r1.getCharacterPos().tl().x < r2.getCharacterPos().tl().x;
      });
    }

    CCharacter firstWeakSeed = searchWeakSeedVec.at(0);
    Rect firstWeakRect = firstWeakSeed.getCharacterPos();
    Point firstWeakCenter(firstWeakRect.tl().x + firstWeakRect.width / 2,
      firstWeakRect.tl().y + firstWeakRect.height / 2);

    float ratio = (float)abs(firstWeakCenter.x - boundaryPoint.x) / (float)maxrect.height;
    //std::cout << "ratio:" << ratio << std::endl;

    if (ratio > 2.f)
      return;

    mserCharacter.push_back(firstWeakSeed);
    plateResult |= firstWeakRect;
    boundaryPoint = firstWeakCenter;

    for (size_t weakSeedIndex = 0; weakSeedIndex + 1 < searchWeakSeedVec.size(); weakSeedIndex++) {
      CCharacter weakSeed = searchWeakSeedVec[weakSeedIndex];
      CCharacter weakSeedCompare = searchWeakSeedVec[weakSeedIndex + 1];

      Rect rect1 = weakSeed.getCharacterPos();
      Rect rect2 = weakSeedCompare.getCharacterPos();

      Rect weakRect = rect2;
      Point weakCenter(weakRect.tl().x + weakRect.width / 2, weakRect.tl().y + weakRect.height / 2);

      // the character in plate are similar height
      float width_1 = float(rect1.width);
      float height_1 = float(rect1.height);

      float width_2 = float(rect2.width);
      float height_2 = float(rect2.height);

      // the character in plate are near in the x-axis but not very near
      float x_margin_left = float(min(rect1.br().x, rect2.br().x));
      float x_margin_right = float(max(rect1.tl().x, rect2.tl().x));

      float x_margin_diff = abs(x_margin_left - x_margin_right);
      double x_margin_diff_ratio = x_margin_diff / min(height_1, height_2);

      if (x_margin_diff_ratio > 2.0) {
        if (0) {
          std::cout << "x_margin_diff_ratio:" << x_margin_diff_ratio << std::endl;
        }       
        break;
      }
      else {
        //::rectangle(result, weakRect, Scalar(255, 0, 0), 1);
        mserCharacter.push_back(weakSeedCompare);
        plateResult |= weakRect;
        if (searchDirection == CharSearchDirection::LEFT) {
          if (weakCenter.x < boundaryPoint.x) {
            boundaryPoint = weakCenter;
          }
        }
        else if (searchDirection == CharSearchDirection::RIGHT) {
          if (weakCenter.x > boundaryPoint.x) {
            boundaryPoint = weakCenter;
          }
        }      
      }
    }
  }
}

void slideWindowSearch(const Mat &image, std::vector<CCharacter>& slideCharacter, const Vec4f& line, 
  Point& fromPoint, const Vec2i& dist, double ostu_level, const Rect& maxrect, Rect& plateResult, 
  CharSearchDirection searchDirection, bool isChinese, Mat& result = Mat()) {
  float k = line[1] / line[0];
  float x_1 = line[2];
  float y_1 = line[3];

  int slideLength = int(0.5 * maxrect.width);
  int slideStep = 1;
  int fromX = 0;
  if (searchDirection == CharSearchDirection::LEFT) {
    fromX = fromPoint.x - dist[0];
  }
  else if (searchDirection == CharSearchDirection::RIGHT) {
    fromX = fromPoint.x + dist[0];
  }
  
  std::vector<CCharacter> charCandidateVec;
  for (int slideX = -slideLength; slideX < slideLength; slideX += slideStep) {
    float x_slide = 0; 

    if (searchDirection == CharSearchDirection::LEFT) {
      x_slide = float(fromX - slideX);
    }
    else if (searchDirection == CharSearchDirection::RIGHT) {
      x_slide = float(fromX + slideX);
    }

    float y_slide = k * (x_slide - x_1) + y_1;
    Point2f p_slide(x_slide, y_slide);
    cv::circle(result, p_slide, 2, Scalar(255, 255, 255), 1);

    int chineseWidth = int(maxrect.width * 1.05);
    int chineseHeight = int(maxrect.height * 1.05);

    Rect rect(Point2f(x_slide - chineseWidth / 2, y_slide - chineseHeight / 2), Size(chineseWidth, chineseHeight));

    if (rect.tl().x < 0 || rect.tl().y < 0 || rect.br().x >= image.cols || rect.br().y >= image.rows)
      continue;

    Mat region = image(rect);
    Mat binary_region;

    threshold(region, binary_region, ostu_level, 255, CV_THRESH_BINARY);
    //double ostu_level = threshold(region, binary_region, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //std::cout << "ostu_level:" << ostu_level << std::endl;*/

    Mat charInput = preprocessChar(binary_region, 20);

    if (0) {
      imshow("charInput", charInput);
      waitKey(0);
      destroyWindow("charInput");
    }

    CCharacter charCandidate;
    charCandidate.setCharacterPos(rect);
    charCandidate.setCharacterMat(charInput);
    charCandidate.setIsChinese(isChinese);
    charCandidateVec.push_back(charCandidate);
  }

  CharsIdentify::instance()->classify(charCandidateVec);
  double overlapThresh = 0.1;
  NMStoCharacter(charCandidateVec, overlapThresh);

  for (auto character : charCandidateVec) {
    Rect rect = character.getCharacterPos();
    Point center(rect.tl().x + rect.width / 2, rect.tl().y + rect.height / 2);

    if (character.getCharacterScore() > 0.8 && character.getCharacterStr() != "1") {
      //cv::rectangle(result, rect, Scalar(255, 255, 255), 1);
      plateResult |= rect;
      slideCharacter.push_back(character);
      fromPoint = center;

      if (0) {
        std::cout << "label:" << character.getCharacterStr();
        std::cout << "__score:" << character.getCharacterScore() << std::endl;
      }
    }
  }
}


bool judegMDOratio2(const Mat& image, const Rect& rect, std::vector<Point>& contour, Mat& result){

  Mat mser = image(rect);
  Mat mser_mat;
  threshold(mser, mser_mat, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

  Rect normalRect = adaptive_charrect_from_rect(rect, image.cols, image.rows);
  Mat region = image(normalRect);
  Mat thresh_mat;
  threshold(region, thresh_mat, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

  // count mser diff ratio
  int countdiff = countNonZero(thresh_mat) - countNonZero(mser_mat);

  float MserDiffOstuRatio = float(countdiff) / float(rect.area());

  if (MserDiffOstuRatio > 1) {
    /*std::cout << "MserDiffOstuRatio:" << MserDiffOstuRatio << std::endl;
    imshow("tmpMat", mser_mat);
    waitKey(0);
    imshow("tmpMat", thresh_mat);
    waitKey(0);*/

    cv::rectangle(result, rect, Scalar(0, 0, 0), 2);
    return false;
  }

  return true;
}

bool judegMDOratio(const Mat& image, const Rect& rect, std::vector<Point>& contour, Mat& result){
  
  Rect normalRect = adaptive_charrect_from_rect(rect, image.cols, image.rows);
  //cv::rectangle(result, normalRect, Scalar(0, 0, 255), 1);

  Mat mser_mat = adaptive_image_from_points(contour, normalRect, normalRect.size());

  Mat region = image(normalRect);

  Mat thresh_mat;
  threshold(region, thresh_mat, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

  // count mser diff ratio
  Mat differMat;
  absdiff(thresh_mat, mser_mat, differMat);

  int countdiff = countNonZero(differMat);
  float MserDiffOstuRatio = float(countdiff) / float(rect.area());

  if (MserDiffOstuRatio > 1) {
    std::cout << "MserDiffOstuRatio:" << MserDiffOstuRatio << std::endl;
    imshow("tmpMat", mser_mat);
    waitKey(0);
    imshow("tmpMat", thresh_mat);
    waitKey(0);
    imshow("tmpMat", differMat);
    waitKey(0);
    
    cv::rectangle(result, rect, Scalar(0, 0, 0), 2);
    return false;
  }

  return true;
}



//! use verify size to first generate char candidates
Mat mserCharMatch(const Mat &src, Mat &match, std::vector<CPlate>& out_plateVec, Color color, int img_index, 
  bool showDebug) {
  Mat image = src;

  std::vector<std::vector<Point>> all_contours;
  std::vector<Rect> all_boxes;

  Ptr<MSER> mser;
  std::vector<CCharacter> charVec;

  match = Mat::zeros(image.rows, image.cols, image.type());

  Mat result = image.clone();
  cvtColor(result, result, COLOR_GRAY2BGR);

  const int imageArea = image.rows * image.cols;
  const int delta = 1;
  const int minArea = 30;
  const double maxAreaRatio = 0.05;

  mser = MSER::create(delta, minArea, int(maxAreaRatio * imageArea));

  mser->setPass2Only(true);
  mser->detectRegions(image, all_contours, all_boxes);

  size_t size = all_contours.size();

  int char_index = 0;
  int char_size = 20;

  // verify char size and output to rects;
  for (size_t index = 0; index < size; index++) {
    Rect rect = all_boxes[index];
    std::vector<Point> contour = all_contours[index];

    if (verifyCharSizes(rect)) {
      Mat mserMat = adaptive_image_from_points(contour, rect, Size(char_size, char_size));
      Mat charInput = preprocessChar(mserMat, char_size);
      Rect charRect = rect;
      
      Mat tmpMat;
      double ostu_level = threshold(image(charRect), tmpMat, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

      // remove the small lines in character like "zh-cuan"
      if (judegMDOratio2(image, rect, contour, result)) {
        CCharacter charCandidate;
        charCandidate.setCharacterPos(charRect);
        charCandidate.setCharacterMat(charInput);
        charCandidate.setOstuLevel(ostu_level);
        charCandidate.setIsChinese(false);
        charVec.push_back(charCandidate);
      }     
    }
  }

  CharsIdentify::instance()->classify(charVec);

  double overlapThresh = 0.5;
  NMStoCharacter(charVec, overlapThresh);

  std::vector<CCharacter> strongSeedVec;
  std::vector<CCharacter> weakSeedVec;
  std::vector<CCharacter> littleSeedVec;

  //size_t charCan_size = charVec.size();
  for (auto charCandidate : charVec) {
    //CCharacter& charCandidate = charVec[char_index];
    Rect rect = charCandidate.getCharacterPos();
    double score = charCandidate.getCharacterScore();
    if (charCandidate.getIsStrong()) {
      strongSeedVec.push_back(charCandidate);
    }
    else if (charCandidate.getIsWeak()) {
      weakSeedVec.push_back(charCandidate);
    }
    else if (charCandidate.getIsLittle()) {
      littleSeedVec.push_back(charCandidate);
    }
  }

  std::vector<CCharacter> searchCandidate = charVec;

  //nms
  overlapThresh = 0.3;
  NMStoCharacter(strongSeedVec, overlapThresh);

  //merge chars to group
  std::vector<std::vector<CCharacter>> charGroupVec;
  mergeCharToGroup(strongSeedVec, charGroupVec);

  //genenrate the line of the group
  std::vector<CPlate> plateVec;
  for (auto charGroup : charGroupVec) {
    Rect plateResult = charGroup[0].getCharacterPos();
    std::vector<Point> points;
    Vec4f line;
    int maxarea = 0;
    Rect maxrect;
    double ostu_level_sum = 0;
    
    int leftx = image.cols;
    Point leftPoint(leftx, 0);
    int rightx = 0;
    Point rightPoint(rightx, 0);

    std::vector<CCharacter> mserCharVec;
    for (auto character : charGroup) {
      Rect charRect = character.getCharacterPos();
      cv::rectangle(result, charRect, Scalar(0, 255, 0), 1);
      plateResult |= charRect;

      Point center(charRect.tl().x + charRect.width / 2, charRect.tl().y + charRect.height / 2);
      points.push_back(center);
      mserCharVec.push_back(character);
      cv::circle(result, center, 3, Scalar(0, 255, 0), 2);

      ostu_level_sum += character.getOstuLevel();

      if (charRect.area() > maxarea) {
        maxrect = charRect;
        maxarea = charRect.area();
      }
      if (center.x < leftPoint.x) {
        leftPoint = center;
      }
      if (center.x > rightPoint.x) {
        rightPoint = center;
      }
    }

    double ostu_level_avg = ostu_level_sum / (double)charGroup.size();
    if (1 && showDebug) {
      std::cout << "ostu_level_avg:" << ostu_level_avg << std::endl;
    }  
    float ratio_maxrect = (float)maxrect.width / (float)maxrect.height;

    if (points.size() >= 2 && ratio_maxrect >= 0.3) {
      fitLine(Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);

      float k = line[1] / line[0];
      //std::cout << "k:" << k << std::endl;

      std::sort(mserCharVec.begin(), mserCharVec.end(),
        [](const CCharacter& r1, const CCharacter& r2) {
        return r1.getCharacterPos().tl().x < r2.getCharacterPos().tl().x;
      });

      CCharacter midChar = mserCharVec.at(int(mserCharVec.size() / 2.f));
      Rect midRect = midChar.getCharacterPos();
      Point midCenter(midRect.tl().x + midRect.width / 2, midRect.tl().y + midRect.height / 2);

      int mindist = 7 * maxrect.width;
      std::vector<Vec2i> distVecVec;
      Vec2i mindistVec;
      Vec2i avgdistVec;

      for (size_t mser_i = 0; mser_i + 1 < mserCharVec.size(); mser_i++) {
        Rect charRect = mserCharVec.at(mser_i).getCharacterPos();
        Point center(charRect.tl().x + charRect.width / 2, charRect.tl().y + charRect.height / 2);

        Rect charRectCompare = mserCharVec.at(mser_i + 1).getCharacterPos();
        Point centerCompare(charRectCompare.tl().x + charRectCompare.width / 2,
          charRectCompare.tl().y + charRectCompare.height / 2);

        int dist = charRectCompare.x - charRect.x;
        Vec2i distVec(charRectCompare.x - charRect.x, charRectCompare.y - charRect.y);
        distVecVec.push_back(distVec);

        //if (dist < mindist) {
        //  mindist = dist;
        //  mindistVec = distVec;
        //}
      }

      std::sort(distVecVec.begin(), distVecVec.end(),
        [](const Vec2i& r1, const Vec2i& r2) {
        return r1[0] < r2[0];
      });

      avgdistVec = distVecVec.at(int((distVecVec.size()-1) / 2.f));

      //float step = 10.f * (float)maxrect.width;
      //float step = (float)mindistVec[0];
      float step = (float)avgdistVec[0];

      //cv::line(result, Point2f(line[2] - step, line[3] - k*step), Point2f(line[2] + step, k*step + line[3]), Scalar(255, 255, 255));
      cv::line(result, Point2f(midCenter.x - step, midCenter.y - k*step), Point2f(midCenter.x + step, k*step + midCenter.y), Scalar(255, 255, 255));
      //cv::circle(result, leftPoint, 3, Scalar(0, 0, 255), 2);

      CPlate plate;
      plate.setPlateLeftPoint(leftPoint);
      plate.setPlateRightPoint(rightPoint);

      plate.setPlateLine(line);
      plate.setPlatDistVec(avgdistVec);
      plate.setOstuLevel(ostu_level_avg);

      plate.setPlateMergeCharRect(plateResult);
      plate.setPlateMaxCharRect(maxrect);
      plate.setMserCharacter(mserCharVec);
      plateVec.push_back(plate);
    }
  }

  for (auto plate : plateVec) {
    Vec4f line = plate.getPlateLine();
    Point leftPoint = plate.getPlateLeftPoint();
    Point rightPoint = plate.getPlateRightPoint();

    Rect plateResult = plate.getPlateMergeCharRect();
    Rect maxrect = plate.getPlateMaxCharRect();
    Vec2i dist = plate.getPlateDistVec();
    double ostu_level = plate.getOstuLevel();

    std::vector<CCharacter> mserCharacter = plate.getCopyOfMserCharacters();

    float k = line[1] / line[0];
    float x_1 = line[2];
    float y_1 = line[3];

    std::vector<CCharacter> searchWeakSeedVec;

    std::vector<CCharacter> searchRightWeakSeed;
    std::vector<CCharacter> searchLeftWeakSeed;

    std::vector<CCharacter> slideRightWindow;
    std::vector<CCharacter> slideLeftWindow;
    
    //draw weak seed and little seed from line;
    //search for mser rect
    if (1 && showDebug) {
      std::cout << "search for mser rect:" << std::endl;
    }
    
    if (0 && showDebug) {
      std::stringstream ss(std::stringstream::in | std::stringstream::out);
      ss << "resources/image/tmp/" << img_index << "_1_" << "searcgMserRect.jpg";
      imwrite(ss.str(), result);
    }
    if (1 && showDebug) {
      std::cout << "mserCharacter:" << mserCharacter.size() << std::endl;
    }
     
    if (mserCharacter.size() < 7) {
      searchWeakSeed(searchCandidate, searchRightWeakSeed, line, rightPoint, maxrect, plateResult, CharSearchDirection::RIGHT);     
      if (1 && showDebug) {
        std::cout << "searchRightWeakSeed:" << searchRightWeakSeed.size() << std::endl;
      }
      for (auto seed : searchRightWeakSeed) {
        cv::rectangle(result, seed.getCharacterPos(), Scalar(255, 0, 0), 1);
        mserCharacter.push_back(seed);
      }

      searchWeakSeed(searchCandidate, searchLeftWeakSeed, line, leftPoint, maxrect, plateResult, CharSearchDirection::LEFT);     
      if (1 && showDebug) {
        std::cout << "searchLeftWeakSeed:" << searchLeftWeakSeed.size() << std::endl;
      }
      for (auto seed : searchLeftWeakSeed) {
        cv::rectangle(result, seed.getCharacterPos(), Scalar(255, 0, 0), 1);
        mserCharacter.push_back(seed);
      }
    }
      
    if (mserCharacter.size() < 7) {
      slideWindowSearch(image, slideRightWindow, line, rightPoint, dist, ostu_level, maxrect, plateResult, CharSearchDirection::RIGHT, false, result);      
      if (1 && showDebug) {
        std::cout << "slideRightWindow:" << slideRightWindow.size() << std::endl;
      }
      for (auto seed : slideRightWindow) {
        cv::rectangle(result, seed.getCharacterPos(), Scalar(0, 0, 255), 1);
        mserCharacter.push_back(seed);
      }

      if (1 && showDebug) {
        std::cout << "search chinese:" << std::endl;
        std::cout << "judege the left is chinese:" << std::endl;
      }
      bool leftIsChinese = false;
      if (1) {
        std::sort(mserCharacter.begin(), mserCharacter.end(),
          [](const CCharacter& r1, const CCharacter& r2) {
          return r1.getCharacterPos().tl().x < r2.getCharacterPos().tl().x;
        });

        CCharacter leftChar = mserCharacter[0];

        //Rect theRect = adaptive_charrect_from_rect(leftChar.getCharacterPos(), image.cols, image.rows);
        Rect theRect = leftChar.getCharacterPos();
        //cv::rectangle(result, theRect, Scalar(255, 0, 0), 1);

        Mat region = image(theRect);
        Mat binary_region;

        ostu_level = threshold(region, binary_region, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
        if (1 && showDebug) {
          std::cout << "left : ostu_level:" << ostu_level << std::endl;
        }
        
        //plate.setOstuLevel(ostu_level);

        Mat charInput = preprocessChar(binary_region, 20);
        if (0 && showDebug) {
          imshow("charInput", charInput);
          waitKey(0);
          destroyWindow("charInput");
        }

        std::string label = "";
        float maxVal = -2.f;
        leftIsChinese = CharsIdentify::instance()->isCharacter(charInput, label, maxVal, true);
        if (1 && showDebug) {
          std::cout << "isChinese:" << leftIsChinese << std::endl;
          std::cout << "chinese:" << label;
          std::cout << "__score:" << maxVal << std::endl;
        }
      }

      //search for sliding window
      if (!leftIsChinese) {
        slideWindowSearch(image, slideLeftWindow, line, leftPoint, dist, ostu_level, maxrect, plateResult, CharSearchDirection::LEFT, true, result);      
        if (1 && showDebug) {
          std::cout << "slideLeftWindow:" << slideLeftWindow.size() << std::endl;
        }
        for (auto seed : slideLeftWindow) {
          cv::rectangle(result, seed.getCharacterPos(), Scalar(0, 0, 255), 1);
          mserCharacter.push_back(seed);
        }
      }
    }
   
    float angle = atan(k) * 180 / (float)CV_PI;
    if (1 && showDebug) {
      std::cout << "k:" << k << std::endl;
      std::cout << "angle:" << angle << std::endl;
    }

    RotatedRect platePos(Point2f((float)plateResult.x + plateResult.width / 2.f, (float)plateResult.y + plateResult.height / 2.f),
      Size2f(plateResult.width * 1.15f, maxrect.height * 1.25f), angle);

    if (verifyRotatedPlateSizes(platePos)) {
      rotatedRectangle(result, platePos, Scalar(0, 0, 255), 1);

      plate.setPlatePos(platePos);
      
      out_plateVec.push_back(plate);
    }

    //cv::rectangle(result, plateResult, Scalar(0, 0, 255), 1);
    //match(plateResult) = 255;
    if (1) {
      for (auto mserChar : mserCharacter) {
        Rect rect = mserChar.getCharacterPos();
        match(rect) = 255;
      }
      cv::line(match, rightPoint, leftPoint, Scalar(255));
    }
    
  }

  if (0 && showDebug) {
    imshow("result", result);
    waitKey(0);
    destroyWindow("result");
  }

  if (0) {
    imshow("match", match);
    waitKey(0);
    destroyWindow("match");
  }

  if (1) {
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << "resources/image/tmp/plateDetect/plate_" << img_index << "_" << color << ".jpg";
    imwrite(ss.str(), result);
  }

  return match;
}

//! use verify size to first generate candidates
Mat mserMatch(const Mat &src, Mat &match, const Color r,
  std::vector<RotatedRect>& out_plateRect, std::vector<Rect>& out_charRect) {
  Mat image = src;
 
  std::vector<RotatedRect> plateRects;
  std::vector<Rect> charRects;

  std::vector<std::vector<Point>> all_contours;
  std::vector<Rect> all_boxes;

    Ptr<MSER> mser;
    std::vector<CCharacter> charVec;
  
    match = Mat::zeros(image.rows, image.cols, image.type());

    Mat result = image.clone();
    cvtColor(result, result, COLOR_GRAY2BGR);

    int imageArea = image.rows * image.cols;
    mser = MSER::create(1, 30, int(0.05 * imageArea));
    mser->detectRegions(image, all_contours, all_boxes);

    size_t size = all_contours.size();

    int char_index = 0;

    for (size_t index = 0; index < size; index++) {
      Rect rect = all_boxes[index];
      std::vector<Point> contour = all_contours[index];
      RotatedRect rrect = minAreaRect(Mat(contour));

      if (verifyRotatedPlateSizes(rrect)) {
        //cv::rectangle(result, rrect.boundingRect(), Scalar(0, 255, 0));

        Point2f rect_points[4];
        rrect.points(rect_points);
        for (int j = 0; j < 4; j++)
          line(result, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0), 1, 8);

        plateRects.push_back(rrect);
      }

      if (verifyCharSizes(rect)) {
        //if (0)
        //{
        //  std::stringstream ss(std::stringstream::in | std::stringstream::out);
        //  ss << "resources/image/tmp/character" << char_index++ << ".jpg";
        //  imwrite(ss.str(), mserMat);
        //}

        if (1) {
          //match(rect) = min(max(0, int(maxVal * 255)),255);
          match(rect) = 255;

          cv::rectangle(result, rect, Scalar(255, 0, 0));
          Point center(rect.tl().x + rect.width / 2, rect.tl().y + rect.height / 2);

          //cv::circle(result, center, 3, Scalar(0, 255, 0), 2);

          charRects.push_back(rect);
          out_charRect.push_back(rect);
          //CCharacter character;
          //character.setCharacterPos(rect);
          //character.setCharacterMat(binary_region);
          //character.setCharacterStr(label);
          //character.setCharacterScore(maxVal);
          //charVec.push_back(character);
        }
      }
    }
 
    for (auto prect : plateRects) {
      float areasum = 0.f;
      int count = 0;

      Rect boundingrect = prect.boundingRect();
      for (auto crect : charRects) {
        
        Rect interRect = boundingrect & crect;
        if (interRect == crect) {
 
          Mat region = image(crect);
          Mat binary_region;
          threshold(region, binary_region, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

          Mat charInput = preprocessChar(binary_region, 20);

          	if(0)
          	{ 
          		std::stringstream ss(std::stringstream::in | std::stringstream::out);
              ss << "resources/image/tmp/character" << char_index++ << ".jpg";
              imwrite(ss.str(), charInput);
          	}

          std::string label = "";
          float maxVal = -2.f;
          bool isCharacter = CharsIdentify::instance()->isCharacter(charInput, label, maxVal);
          if (isCharacter) {
            areasum += crect.area();
            count++;
          }
        }
      }

      float ratio = areasum / (float)boundingrect.area();
      if (ratio - 0.5f > 0 && count >= 5) {
        std::cout << "ratio:" << ratio << std::endl;
        std::cout << "count:" << count << std::endl;
        //cv::rectangle(result, prect.boundingRect(), Scalar(0, 255, 0));
        out_plateRect.push_back(prect);
      }
    }

    if (1) {
      imshow("result", result);
      waitKey(0);
    }

 
  return match;
}


bool mat_valid_position(const Mat& mat, int row, int col) {
  return row >= 0 && col >= 0 && row < mat.rows && col < mat.cols;
}


template<class T>
static void mat_set_invoke(Mat& mat, int row, int col, const Scalar& value) {
  if (1 == mat.channels()) {
    mat.at<T>(row, col) = (T)value.val[0];
  }
  else if (3 == mat.channels()) {
    T* ptr_src = mat.ptr<T>(row, col);
    *ptr_src++ = (T)value.val[0];
    *ptr_src++ = (T)value.val[1];
    *ptr_src = (T)value.val[2];
  }
  else if (4 == mat.channels()) {
    T* ptr_src = mat.ptr<T>(row, col);
    *ptr_src++ = (T)value.val[0];
    *ptr_src++ = (T)value.val[1];
    *ptr_src++ = (T)value.val[2];
    *ptr_src = (T)value.val[3];
  }
}

void setPoint(Mat& mat, int row, int col, const Scalar& value) {
  if (CV_8U == mat.depth()) {
    mat_set_invoke<uchar>(mat, row, col, value);
  }
  else if (CV_8S == mat.depth()) {
    mat_set_invoke<char>(mat, row, col, value);
  }
  else if (CV_16U == mat.depth()) {
    mat_set_invoke<ushort>(mat, row, col, value);
  }
  else if (CV_16S == mat.depth()) {
    mat_set_invoke<short>(mat, row, col, value);
  }
  else if (CV_32S == mat.depth()) {
    mat_set_invoke<int>(mat, row, col, value);
  }
  else if (CV_32F == mat.depth()) {
    mat_set_invoke<float>(mat, row, col, value);
  }
  else if (CV_64F == mat.depth()) {
    mat_set_invoke<double>(mat, row, col, value);
  }
}

Rect adaptive_charrect_from_rect(const Rect& rect, int maxwidth, int maxheight) {
  int expendWidth = 0;

  if (rect.height > 3 * rect.width) {
    expendWidth = (rect.height / 2 - rect.width) / 2;
  }

  //Rect resultRect(rect.tl().x - expendWidth, rect.tl().y, 
  //  rect.width + expendWidth * 2, rect.height);

  int tlx = rect.tl().x - expendWidth > 0 ? rect.tl().x - expendWidth : 0;
  int tly = rect.tl().y;

  int brx = rect.br().x + expendWidth < maxwidth ? rect.br().x + expendWidth : maxwidth;
  int bry = rect.br().y;

  Rect resultRect(tlx, tly, brx - tlx, bry - tly);
  return resultRect;
}


Mat adaptive_image_from_points(const std::vector<Point>& points,
  const Rect& rect, const Size& size, const Scalar& backgroundColor /* = ml_color_white */, 
  const Scalar& forgroundColor /* = ml_color_black */, bool gray /* = true */) {
  int expendHeight = 0;
  int expendWidth = 0;

  if (rect.width > rect.height) {
    expendHeight = (rect.width - rect.height) / 2;
  }
  else if (rect.height > rect.width) {
    expendWidth = (rect.height - rect.width) / 2;
  }

  Mat image(rect.height + expendHeight * 2, rect.width + expendWidth * 2, gray ? CV_8UC1 : CV_8UC3, backgroundColor);

  for (int i = 0; i < (int)points.size(); ++i) {
    Point point = points[i];
    Point currentPt(point.x - rect.tl().x + expendWidth, point.y - rect.tl().y + expendHeight);
    if (mat_valid_position(image, currentPt.y, currentPt.x)) {
      setPoint(image, currentPt.y, currentPt.x, forgroundColor);
    }
  }

  Mat result;
  resize(image, result, size, 0, 0, INTER_NEAREST);

  return result;
}

// shift an image
Mat translateImg(Mat img, int offsetx, int offsety){
  Mat dst;
  Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
  warpAffine(img, dst, trans_mat, img.size());
  return dst;
}

// rotate an image
Mat rotateImg(Mat source, float angle){
  Point2f src_center(source.cols / 2.0F, source.rows / 2.0F);
  Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
  Mat dst;
  warpAffine(source, dst, rot_mat, source.size());
  return dst;
}

//! 计算一个安全的Rect
//! 如果不存在，返回false

bool calcSafeRect(const RotatedRect &roi_rect, const Mat &src,
  Rect_<float> &safeBoundRect) {
  Rect_<float> boudRect = roi_rect.boundingRect();

  // boudRect的左上的x和y有可能小于0

  float tl_x = boudRect.x > 0 ? boudRect.x : 0;
  float tl_y = boudRect.y > 0 ? boudRect.y : 0;

  // boudRect的右下的x和y有可能大于src的范围

  float br_x = boudRect.x + boudRect.width < src.cols
    ? boudRect.x + boudRect.width - 1
    : src.cols - 1;
  float br_y = boudRect.y + boudRect.height < src.rows
    ? boudRect.y + boudRect.height - 1
    : src.rows - 1;

  float roi_width = br_x - tl_x;
  float roi_height = br_y - tl_y;

  if (roi_width <= 0 || roi_height <= 0) return false;

  // 新建一个mat，确保地址不越界，以防mat定位roi时抛异常

  safeBoundRect = Rect_<float>(tl_x, tl_y, roi_width, roi_height);

  return true;
}

}