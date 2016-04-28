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
}

#endif //INC_21EA6DA865E54A3AA9B75B140EAC84BE
