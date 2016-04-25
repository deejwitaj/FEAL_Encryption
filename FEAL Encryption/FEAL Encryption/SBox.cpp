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

  if ((imgS.width*imgS.height) > 8 || (keyS.width*keyS.height) > 8)
    cout << "Key or Image size is too large!" << std::endl;

  Mat out;
  if (m_type == 1)
  {
    for (int i = 0; i < 8; i++)
    {
      partialImg.at<unsigned char*>(0, i) = partialImg.at<unsigned char*>(0, i) + 1;
      key.at<unsigned char*>(0, i) = key.at<unsigned char*>(0, i) + 1;
    }
  }

  cv::bitwise_or(partialImg, key, out);

  //TODO: Bitshif operation

  return out;
}


