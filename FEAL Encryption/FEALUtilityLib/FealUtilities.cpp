#include "stdafx.h"
#include "FealUtilities.h"

#include <iostream>

static const int GATE_IMG_SIZE = 64;

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
		auto alpha2 = i_partialImg.at<unsigned char>(1, 0);
		auto alpha3 = i_partialImg.at<unsigned char>(1, 1);

		// Splitting out the "beta" parameter into 2 bytes
		auto beta0 = i_key.at<unsigned char>(0, 0);
		auto beta1 = i_key.at<unsigned char>(0, 0);

		// Represents the output of the four levels in the F function
		unsigned char lv0, lv1, lv2, lv3;

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
		Mat out(2, 2, CV_8UC1);
		out.at<unsigned char>(0, 0) = lv0;
		out.at<unsigned char>(0, 1) = lv1;
		out.at<unsigned char>(1, 0) = lv2;
		out.at<unsigned char>(1, 1) = lv3;

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

  //Represents the DRE stage of the ciphertext
  Mat RandomizeData(Mat i_image, Mat i_key)
  {
    return GetF(i_image, i_key);
  }
}