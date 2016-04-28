#include "stdafx.h"
#include "FealUtilities.h"

#include <iostream>

Mat GetS(Mat i_partialImg, Mat i_key, int i_type)
{
  using namespace std;

  auto const imgS = i_partialImg.size();
  auto const keyS = i_key.size();

  if ((imgS.width*imgS.height) > 1 || (keyS.width*keyS.height) > 1)
    cout << "Key or Image size is too large!" << std::endl;

  Mat out;
  if (i_type == 1)
  {
    i_partialImg.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(0, 0) + 1;
    i_key.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 0) + 1;
  }

  cv::bitwise_or(i_partialImg, i_key, out);

  //TODO: Bitshift operation

  return out;
}

namespace FealUtilities
{
  /*
  The format of the partial image will be 2x2 bytes:
  AB
  CD
  with alpha 0 and 1 being AB and CD respectively
  while alpha 2 and 3 will be AC and BD, respectively
  */
  Mat GetF(Mat i_partialImg, Mat i_key, FType ft)
  {
    using namespace std;

    auto const imgS = i_partialImg.size();
    auto const keyS = i_key.size();

    if ((imgS.width*imgS.height) > 4 || (keyS.width*keyS.height) > 2)
      cout << "Key or Image size is too large!" << std::endl;

    Mat alpha0, alpha1, alpha2, alpha3;
    alpha0.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(0, 0);
    alpha1.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(0, 1);
    alpha2.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(1, 0);
    alpha3.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(1, 1);

    Mat key0, key1;
    key0.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 0);
    key1.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 1);

    Mat lv0, lv1, lv2, lv3; //Represent the output of the four levels in the F function
    cv::bitwise_xor(alpha1, key0, lv1);
    cv::bitwise_xor(lv1, alpha0, lv1);
    cv::bitwise_xor(alpha2, key1, lv2);
    cv::bitwise_xor(lv2, alpha3, lv2);
    lv1 = GetS(lv1, lv2, 1);
    lv2 = GetS(lv1, lv2, 0);
    lv3 = GetS(lv2, lv3, 1);
    lv0 = GetS(lv0, lv1, 0);

    Mat out;
    out.at<unsigned char*>(0, 0) = lv0.at<unsigned char*>(0, 0);
    out.at<unsigned char*>(0, 1) = lv1.at<unsigned char*>(0, 0);
    out.at<unsigned char*>(1, 0) = lv2.at<unsigned char*>(0, 0);
    out.at<unsigned char*>(1, 1) = lv3.at<unsigned char*>(0, 0);

    return out;
  }
}