#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "FealUtilities.h"

using namespace cv;
using namespace std;
using namespace FealUtilities;

static const unsigned _int64 KEY = 0xcc78bd116fdbb635;

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
    return -1;
  }

  Mat unencryptedImg;
  unencryptedImg = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

  if (!unencryptedImg.data)                              // Check for invalid input
  {
    cout << "Could not open or find the image" << std::endl;
    return -1;
  }

  namedWindow("Original window", WINDOW_AUTOSIZE);// Create a window for display.
  imshow("Original window", unencryptedImg);                   // Show our image inside it.

  waitKey(0);                                          // Wait for a keystroke in the window

  Mat key = PerformKeyScheduling(KEY);
  FealUtilities::Crypt(unencryptedImg, key, false);
  namedWindow("Encrypted window", WINDOW_AUTOSIZE);
  imshow("Encrypted window", unencryptedImg);

  waitKey(0);

  FealUtilities::Crypt(unencryptedImg, key, true);
  namedWindow("Decrypted Window", WINDOW_AUTOSIZE);
  imshow("Decrypted Window", unencryptedImg);

  waitKey(0);
  return 0;
}