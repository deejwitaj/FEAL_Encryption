#ifndef INC_21EA6DA865E54A3AA9B75B140EAC84BE
#define INC_21EA6DA865E54A3AA9B75B140EAC84BE

#include <opencv2\highgui\highgui.hpp>

using namespace cv;

namespace FealUtilities
{
  Mat GetF(Mat i_partialImg, Mat i_key);
  bool GetInputGateBranch(Mat i_image, Mat o_L, Mat o_R);
  bool GetOutputGateBranch(Mat io_L, Mat io_R);
  Mat RandomizeData(Mat i_image, Mat key, bool i_bDecrypting);
  void GetLR(Mat i_image, Mat o_L, Mat o_R, bool i_bDecrypting);
}

#endif //INC_21EA6DA865E54A3AA9B75B140EAC84BE
