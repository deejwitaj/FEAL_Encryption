#include "stdafx.h"
#include "FealUtilities.h"

#include <iostream>

static const int GATE_IMG_SIZE = 64;

Mat GetS(Mat i_partialImg, Mat i_key, int i_type)
{
  using namespace std;

  auto const imgS = i_partialImg.size();
  auto const keyS = i_key.size();

  if ((imgS.width*imgS.height) > 1 || (keyS.width*keyS.height) > 1)
    cout << "Key or Image size is too large!" << std::endl;

  Mat out(1, 1, CV_8UC1);
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
  with alpha 0 and 1 being A and B respectively
  while alpha 2 and 3 will be C and D, respectively
  */
  Mat GetF(Mat i_partialImg, Mat i_key, FType ft)
  {
    using namespace std;

    auto const imgS = i_partialImg.size();
    auto const keyS = i_key.size();

    if ((imgS.width*imgS.height) > 4 || (keyS.width*keyS.height) > 2)
      cout << "Key or Image size is too large!" << std::endl;

    Mat alpha0(1, 1, CV_8UC1);
    Mat alpha1(1, 1, CV_8UC1);
    Mat alpha2(1, 1, CV_8UC1);
    Mat alpha3(1, 1, CV_8UC1);
    alpha0.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(0, 0);
    alpha1.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(0, 1);
    alpha2.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(1, 0);
    alpha3.at<unsigned char*>(0, 0) = i_partialImg.at<unsigned char*>(1, 1);

    Mat key0(0, 1, CV_8UC1);
    Mat key1(0, 1, CV_8UC1);
    key0.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 0);
    key1.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 1);

    //Represent the output of the four levels in the F function
    Mat lv0(1, 1, CV_8UC1);
    Mat lv1(1, 1, CV_8UC1);
    Mat lv2(1, 1, CV_8UC1);
    Mat lv3(1, 1, CV_8UC1);
    cv::bitwise_xor(alpha1, key0, lv1);
    cv::bitwise_xor(lv1, alpha0, lv1);
    cv::bitwise_xor(alpha2, key1, lv2);
    cv::bitwise_xor(lv2, alpha3, lv2);
    lv1 = GetS(lv1, lv2, 1);
    lv2 = GetS(lv1, lv2, 0);
    lv3 = GetS(lv2, lv3, 1);
    lv0 = GetS(lv0, lv1, 0);

    Mat out(2, 2, CV_8UC1);
    out.at<unsigned char*>(0, 0) = lv0.at<unsigned char*>(0, 0);
    out.at<unsigned char*>(0, 1) = lv1.at<unsigned char*>(0, 0);
    out.at<unsigned char*>(1, 0) = lv2.at<unsigned char*>(0, 0);
    out.at<unsigned char*>(1, 1) = lv3.at<unsigned char*>(0, 0);

    return out;
  }

  /*
  This gate branch will treat the left half of the image as the L portion and
  the right half of the image as the R portion, and will return these two halves
  into the passed in L and R mats.
  The L and R inputs refer to the L and R during encryption and decryption in the
  Ciphertext image of page 8 of the FEAL.pdf
  */
  bool GetInputGateBranch(Mat i_image, Mat o_L, Mat o_R)
  {
    auto const imgS = i_image.size();

    if (imgS.width*imgS.height != GATE_IMG_SIZE)
    {
      std::cout << "Image size is not appropriate for Gate Branch" << std::endl;
      return false;
    }

    if (o_L.size() != o_R.size())
    {
      std::cout << "Image halves provided are not of equal size" << std::endl;
      return false;
    }

    auto const halfW = imgS.width / 2;
    for (int i = 0; i < imgS.height; i++)
    {
      for (int j = 0; j < imgS.width; j++)
      {
        if (j < halfW)
          o_L.at<unsigned char*>(i, j) = i_image.at<unsigned char*>(i, j);
        else
          o_L.at<unsigned char*>(i, (j - halfW)) = i_image.at<unsigned char*>(i, j);
      }
    }

    cv::bitwise_xor(o_L, o_R, o_R);

    return true;
  }

  /*
  The L and R inputs refer to the L and R during encryption and decryption in the
  Ciphertext image of page 8 of the FEAL.pdf
  */
  bool GetOutputGateBranch(Mat io_L, Mat io_R)
  {
    if (io_L.size() != io_R.size())
    {
      std::cout << "Image halves provided are not of equal size" << std::endl;
      return false;
    }

    cv::bitwise_xor(io_R, io_L, io_L);

    return true;
  }
}