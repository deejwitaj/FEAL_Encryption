#include "SBox.h"

#include <iostream>

SBox::SBox(int type)
  : m_type(type)
{
}

Mat SBox::GetS(Mat partialImg, Mat key)
{
  using namespace std;

  auto const imgS = partialImg.size();
  auto const keyS = key.size();

  if ((imgS.width*imgS.height) > 1 || (keyS.width*keyS.height) > 1)
    cout << "Key or Image size is too large!" << std::endl;

  Mat out;
  if (m_type == 1)
  {
    partialImg.at<unsigned char*>(0, 0) = partialImg.at<unsigned char*>(0, 0) + 1;
    key.at<unsigned char*>(0, 0) = key.at<unsigned char*>(0, 0) + 1;
  }

  cv::bitwise_or(partialImg, key, out);

  //TODO: Bitshif operation

  return out;
}


