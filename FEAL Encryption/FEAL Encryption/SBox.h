#ifndef INC_3384F7182C484F4F98F40433A1521B78
#define INC_3384F7182C484F4F98F40433A1521B78

#include <opencv2\highgui\highgui.hpp>

using namespace cv;

class SBox
{
public:
  SBox(int type); //Type determines if a 1 is added to the ORing of bytes (0 for no, 1 for yes)

  Mat GetS(Mat partialImg, Mat key);

private:
  int m_type;
};
#endif //INC_3384F7182C484F4F98F40433A1521B78
