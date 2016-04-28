#ifndef INC_21EA6DA865E54A3AA9B75B140EAC84BE
#define INC_21EA6DA865E54A3AA9B75B140EAC84BE

#include <opencv2\highgui\highgui.hpp>

using namespace cv;

namespace FealUtilities
{
  enum FType
  {
    K,
    NON_K
  };

  Mat GetF(Mat partialImg, Mat key, FType ft);
  bool GetInputGateBranch(Mat i_image, Mat o_L, Mat o_R);
  bool GetOutputGateBranch(Mat io_L, Mat io_R);
  Mat RandomizeData(Mat i_image, Mat i_key);
}

#endif //INC_21EA6DA865E54A3AA9B75B140EAC84BE
