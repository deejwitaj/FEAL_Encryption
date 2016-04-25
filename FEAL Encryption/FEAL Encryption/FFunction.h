#ifndef INC_83F1FA0B57D440D4BC79750690B00086
#define INC_83F1FA0B57D440D4BC79750690B00086

#include <opencv2\highgui\highgui.hpp>

#include "SBox.h"

using namespace cv;

class FFunc
{
public:
  FFunc(int type);

  Mat GetF(Mat partialImg, Mat key);

private:
  int m_type;
  SBox m_sBox0, m_sBox1;
};
#endif INC_83F1FA0B57D440D4BC79750690B00086
