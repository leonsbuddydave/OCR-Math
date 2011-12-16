#include <iostream>

#include "cv.h"
#include "cvblobs/BlobResult.h"
#include "cvblobs/blob.h"

#include "highgui.h"
#include "math.h"
#include <vector>

using namespace std;

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
		vector< vector<int> > ConvertImageTo2DArray(IplImage*);	
};
