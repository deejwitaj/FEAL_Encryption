#ifndef INC_21EA6DA865E54A3AA9B75B140EAC84BE
#define INC_21EA6DA865E54A3AA9B75B140EAC84BE

#include <opencv2\highgui\highgui.hpp>

using namespace cv;

namespace FealUtilities
{
  void Crypt(Mat i_image, Mat i_key, bool i_bDecryption);
  Mat GetF(Mat i_partialImg, Mat i_key);
  bool GetInputGateBranch(Mat i_image, Mat o_L, Mat o_R);
  bool GetOutputGateBranch(Mat io_L, Mat io_R);
  Mat RandomizeData(Mat i_image, Mat key, bool i_bDecrypting);
  void InputImage(Mat i_image, Mat i_key);
  void OutputImage(Mat i_image, Mat i_key);
  void DoDRE(Mat L, Mat R, Mat i_key, bool i_bDecryption);
  void GetLR(Mat i_image, Mat o_L, Mat o_R, bool i_bDecrypting);
	unsigned _int32 GetFk(unsigned __int32 i_A, unsigned __int32 i_B);
}

#endif //INC_21EA6DA865E54A3AA9B75B140EAC84BE
