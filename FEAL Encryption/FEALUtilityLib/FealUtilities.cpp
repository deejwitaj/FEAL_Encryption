#include "stdafx.h"
#include "FealUtilities.h"

#include <iostream>

static const int GATE_IMG_SIZE = 4;

//! Substitution box (S-box) function that performs a substitution commonly used in cryptography.
//! URL: https://en.wikipedia.org/wiki/S-box
unsigned char GetS(unsigned char i_A, unsigned char i_B, int i_Type)
{
	return ((i_A + i_B + i_Type) % 256) << 2;
}

namespace FealUtilities
{
	/*! F function used in the key schedule part of the FEAL algorithm.
	*/
	unsigned _int32 GetFk(unsigned __int32 i_A, unsigned __int32 i_B)
	{
		// Split i_A into 1 byte chunks
		unsigned __int32 a0 = i_A && 0xFF;
		unsigned __int32 a1 = (i_A && 0xFF00) >> 8;
		unsigned __int32 a2 = (i_A && 0xFF0000) >> 16;
		unsigned __int32 a3 = (i_A && 0xFF000000) >> 24;

		// Split i_B into 1 byte chunks
		unsigned __int32 b0 = i_B && 0xFF;
		unsigned __int32 b1 = (i_B && 0xFF00) >> 8;
		unsigned __int32 b2 = (i_B && 0xFF0000) >> 16;
		unsigned __int32 b3 = (i_B && 0xFF000000) >> 24;

		// Perform Fk algorithm
		unsigned __int32 a0a1xor = a0 ^ a1;
		unsigned __int32 a2a3xor = a2 ^ a3;

		unsigned __int32 out1 = GetS(a0a1xor, b0 ^ a2a3xor, 1);
		unsigned __int32 out0 = GetS(a0, b2 ^ out1, 0);
		unsigned __int32 out2 = GetS(a2a3xor, b1 ^ out1, 0);
		unsigned __int32 out3 = GetS(a3, b3 ^ out2, 1);

		// Combine output bytes into 32bit data structure
		_int32 out = 0;
		out |= out0;
		out |= out1 << 8;
		out |= out2 << 16;
		out |= out3 << 24;

		return out;
	}

  void Crypt(Mat i_image, Mat i_key, bool i_bDecryption)
  {
    Mat imgChunk(1, 8, CV_8UC1);
    auto size = imgChunk.size();
    for (int i = 0; i < size.height; i++)
    {
      int j = 0;
      while (j < size.width)
      {
        for (int k = 0; k < 8; k++)
        {
          imgChunk.at<unsigned char*>(i, k) = i_image.at<unsigned char*>(i, j);
          j++;
        }
        Mat encryptedChunk = RandomizeData(imgChunk, i_key, i_bDecryption);
        j -= 8;
        for (int k = 0; k < 8; k++)
        {
          i_image.at<unsigned char*>(i, j) = imgChunk.at<unsigned char*>(i, k);
          j++;
        }
      }
    }
  }

  /*
	The format of the partial image will be 2x2 bytes:
		AB
		CD
		with alpha 0 and 1 being A and B respectively
		while alpha 2 and 3 will be C and D, respectively

		@param i_partialImg 32-bit block of the image
		@param i_key 16-bit key block
	*/
	Mat GetF(Mat i_partialImg, Mat i_key)
	{
		using namespace std;

		auto const imgS = i_partialImg.size();
		auto const keyS = i_key.size();

		if ((imgS.width*imgS.height) > 4 || (keyS.width*keyS.height) > 2)
			cout << "Key or Image size is too large!" << std::endl;

		// Splitting out the "alpha" parameter into 4 bytes
		auto alpha0 = i_partialImg.at<unsigned char>(0, 0);
		auto alpha1 = i_partialImg.at<unsigned char>(0, 1);
		auto alpha2 = i_partialImg.at<unsigned char>(0, 2);
		auto alpha3 = i_partialImg.at<unsigned char>(0, 3);

		// Splitting out the "beta" parameter into 2 bytes
		auto beta0 = i_key.at<unsigned char>(0, 0);
		auto beta1 = i_key.at<unsigned char>(0, 1);

		// Represents the output of the four levels in the F function
		unsigned char lv0, lv1, lv2, lv3;

    lv0 = alpha0;
    lv3 = alpha3;
		lv1 = alpha1 ^ beta0;
		lv1 = lv1 ^ alpha0;
		lv2 = alpha2 ^ beta1;
		lv2 = lv2 ^ alpha3;

		// S-Box Computations
		lv1 = GetS(lv1, lv2, 1);
		lv2 = GetS(lv1, lv2, 0);
		lv3 = GetS(lv2, lv3, 1);
		lv0 = GetS(lv0, lv1, 0);

		// 2x2 matrix where each element is a unsigned char
		Mat out(1, 4, CV_8UC1);
		out.at<unsigned char>(0, 0) = lv0;
		out.at<unsigned char>(0, 1) = lv1;
		out.at<unsigned char>(0, 2) = lv2;
		out.at<unsigned char>(0, 3) = lv3;

		return out;
	}

	// Reads in a 64-bit key and returns 192 bits (12 intermediate keys of length 16 bits)
	// @returns A Mat object consisting of 8-byte chunks
	Mat PerformKeyScheduling(unsigned _int64 i_Key)
	{
		unsigned _int32 a0 = i_Key && 0xFFFFFFFF;
		unsigned _int32 b0 = i_Key >> 32;

		unsigned _int32 k0k1 = GetFk(a0, b0);

		unsigned _int32 a1 = b0;
		unsigned _int32 b1 = k0k1;

		unsigned _int32 k2k3 = GetFk(a1, a0 ^ b1);

		unsigned _int32 a2 = b1;
		unsigned _int32 b2 = k2k3;

		unsigned _int32 k4k5 = GetFk(a2, a1 ^ b2);

		unsigned _int32 a3 = b2;
		unsigned _int32 b3 = k4k5;

		unsigned _int32 k6k7 = GetFk(a3, a2 ^ b3);

		unsigned _int32 a4 = b3;
		unsigned _int32 b4 = k6k7;

		unsigned _int32 k8k9 = GetFk(a4, a3 ^ b4);

		unsigned _int32 a5 = b4;
		unsigned _int32 b5 = k8k9;

		unsigned _int32 k10k11 = GetFk(a5, a4 ^ b5);

		// Output containing the intermediate keys
		Mat out(1, 24, CV_8UC1);
		out.at<unsigned char>(0, 0) = k0k1 && 0xFF;
		out.at<unsigned char>(0, 1) = (k0k1 && 0xFF00) >> 8;
		out.at<unsigned char>(0, 2) = (k0k1 && 0xFF0000) >> 16;
		out.at<unsigned char>(0, 3) = (k0k1 && 0xFF000000) >> 24;
		out.at<unsigned char>(0, 4) = k2k3 && 0xFF;
		out.at<unsigned char>(0, 5) = (k2k3 && 0xFF00) >> 8;
		out.at<unsigned char>(0, 6) = (k2k3 && 0xFF0000) >> 16;
		out.at<unsigned char>(0, 7) = (k2k3 && 0xFF000000) >> 24;
		out.at<unsigned char>(0, 8) = k4k5 && 0xFF;
		out.at<unsigned char>(0, 9) = (k4k5 && 0xFF00) >> 8;
		out.at<unsigned char>(0, 10) = (k4k5 && 0xFF0000) >> 16;
		out.at<unsigned char>(0, 11) = (k4k5 && 0xFF000000) >> 24;
		out.at<unsigned char>(0, 12) = k6k7 && 0xFF;
		out.at<unsigned char>(0, 13) = (k6k7 && 0xFF00) >> 8;
		out.at<unsigned char>(0, 14) = (k6k7 && 0xFF0000) >> 16;
		out.at<unsigned char>(0, 15) = (k6k7 && 0xFF000000) >> 24;
		out.at<unsigned char>(0, 16) = k8k9 && 0xFF;
		out.at<unsigned char>(0, 17) = (k8k9 && 0xFF00) >> 8;
		out.at<unsigned char>(0, 18) = (k8k9 && 0xFF0000) >> 16;
		out.at<unsigned char>(0, 19) = (k8k9 && 0xFF000000) >> 24;
		out.at<unsigned char>(0, 20) = k10k11 && 0xFF;
		out.at<unsigned char>(0, 21) = (k10k11 && 0xFF00) >> 8;
		out.at<unsigned char>(0, 22) = (k10k11 && 0xFF0000) >> 16;
		out.at<unsigned char>(0, 23) = (k10k11 && 0xFF000000) >> 24;

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
          o_R.at<unsigned char*>(i, (j - halfW)) = i_image.at<unsigned char*>(i, j);
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
  /*
  Represents the Data Randomization stage of the ciphertext

  @param i_image the image chunk being encrypted/decrypted
  @param i_inputKey the four keys being used for the input of the data randomization stage
  @param i_outputKey the four keys being used for the output of the data randomization stage
  @param i_key1 the key used for DRE1 stage
  @param i_key2 the key used for DRE2 stage
  @param i_key3 the key used for DRE3 stage
  @param i_key4 the key used for DRE4 stage
  */
  Mat RandomizeData(Mat i_image, Mat i_key, bool i_bDecryption)
  {
    //Input
    InputImage(i_image, i_key);

    //Input Gate Branch
    Mat L(1, 4, CV_8UC1);
    Mat R(L);
    GetLR(i_image, L, R, i_bDecryption);
    cv::bitwise_xor(L, R, R);

    //DRE stage
    DoDRE(L, R, i_key, i_bDecryption);

    //Output Gate Branch: After this gate, the left side is now the right side of image
    //and the right side of image is now the left side
    cv::bitwise_xor(L, R, L);
    Mat cryptImg(1, 8, CV_8UC1);
    auto halfW = cryptImg.size().width / 2;
    for (int i = 0; i < halfW; i++)
    {
      cryptImg.at<unsigned char*>(0, i) = L.at<unsigned char*>(0, i);
      cryptImg.at<unsigned char*>(0, i + halfW) = R.at<unsigned char*>(0, i);
    }

    //Output
    OutputImage(cryptImg, i_key);

    return cryptImg;
  }

  void InputImage(Mat i_image, Mat i_key)
  {
    Mat inputKey(1, 8, CV_8UC1);
    inputKey.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 8);
    inputKey.at<unsigned char*>(0, 1) = i_key.at<unsigned char*>(0, 9);
    inputKey.at<unsigned char*>(0, 2) = i_key.at<unsigned char*>(0, 10);
    inputKey.at<unsigned char*>(0, 3) = i_key.at<unsigned char*>(0, 11);
    inputKey.at<unsigned char*>(0, 4) = i_key.at<unsigned char*>(0, 12);
    inputKey.at<unsigned char*>(0, 5) = i_key.at<unsigned char*>(0, 13);
    inputKey.at<unsigned char*>(0, 6) = i_key.at<unsigned char*>(0, 14);
    inputKey.at<unsigned char*>(0, 7) = i_key.at<unsigned char*>(0, 15);
    cv::bitwise_xor(i_image, inputKey, i_image);
  }

  void OutputImage(Mat i_image, Mat i_key)
  {
    Mat outputKey(1, 8, CV_8UC1);
    outputKey.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 16);
    outputKey.at<unsigned char*>(0, 1) = i_key.at<unsigned char*>(0, 17);
    outputKey.at<unsigned char*>(0, 2) = i_key.at<unsigned char*>(0, 18);
    outputKey.at<unsigned char*>(0, 3) = i_key.at<unsigned char*>(0, 19);
    outputKey.at<unsigned char*>(0, 4) = i_key.at<unsigned char*>(0, 20);
    outputKey.at<unsigned char*>(0, 5) = i_key.at<unsigned char*>(0, 21);
    outputKey.at<unsigned char*>(0, 6) = i_key.at<unsigned char*>(0, 22);
    outputKey.at<unsigned char*>(0, 7) = i_key.at<unsigned char*>(0, 23);
    cv::bitwise_xor(i_image, outputKey, i_image);
  }

  void DoDRE(Mat L, Mat R, Mat i_key, bool i_bDecryption)
  {
    Mat k0(0, 2, CV_8UC1);
    Mat k1(0, 2, CV_8UC1);
    Mat k2(0, 2, CV_8UC1);
    Mat k3(0, 2, CV_8UC1);
    k0.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 0);
    k0.at<unsigned char*>(0, 1) = i_key.at<unsigned char*>(0, 1);
    k1.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 2);
    k1.at<unsigned char*>(0, 1) = i_key.at<unsigned char*>(0, 3);
    k2.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 4);
    k2.at<unsigned char*>(0, 1) = i_key.at<unsigned char*>(0, 5);
    k3.at<unsigned char*>(0, 0) = i_key.at<unsigned char*>(0, 6);
    k3.at<unsigned char*>(0, 1) = i_key.at<unsigned char*>(0, 7);
    if (!i_bDecryption)
    {
      cv::bitwise_xor(L, GetF(R, k0), L);
      cv::bitwise_xor(R, GetF(L, k1), R);
      cv::bitwise_xor(L, GetF(R, k2), L);
      cv::bitwise_xor(R, GetF(L, k3), R);
    }
    else
    {
      cv::bitwise_xor(L, GetF(R, k3), L);
      cv::bitwise_xor(R, GetF(L, k2), R);
      cv::bitwise_xor(L, GetF(R, k1), L);
      cv::bitwise_xor(R, GetF(L, k0), R);
    }
  }

  void GetLR(Mat i_image, Mat o_L, Mat o_R, bool i_bDecrypting)
  {
    Mat L(1, 4, CV_8UC1);
    Mat R(L);
    auto halfWidth = i_image.size().width / 2;
    for (int i = 0; i < halfWidth; i++)
    {
      L.at<unsigned char*>(0, i) = i_image.at<unsigned char*>(0, i);
      R.at<unsigned char*>(0, i) = i_image.at<unsigned char*>(0, i + halfWidth);
    }

    if (i_bDecrypting)
    {
      o_L = R;
      o_R = L;
    }
    else
    {
      o_L = L;
      o_R = R;
    }
  }
}