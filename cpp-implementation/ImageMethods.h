#include <iostream>

#include "cv.h"
#include "cvblob.h"

#include "highgui.h"
#include "math.h"
#include <vector>

using namespace std;
using namespace cvb;

class ImageMethods
{
	public:
		ImageMethods();
		~ImageMethods();
	private:
	protected:
		static const int IMAGE_INITIAL_WIDTH_SCALE = 400;

		IplImage * PrepareImage(IplImage*);
		IplImage * Threshold(IplImage*);
		IplImage * CropLargestBlob(IplImage*);
		IplImage * CropOutBlob(IplImage*, CvBlob*);
		vector< vector<int> > ConvertImageTo2DArray(IplImage*);
};
