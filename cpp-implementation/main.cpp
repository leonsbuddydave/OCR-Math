#include <iostream>
#include "Teacher.h"

using namespace std;

void BlobShit(IplImage*);

int main(int argc, char** argv)
{
	IplImage* pImg;

	Teacher teacher;
	teacher.Learn();

	return 0;
}

/*
void BlobShit(IplImage * Source)
{
	// Function for testing region differentiation
	CBlobResult blobs;
	blobs = CBlobResult(Source, NULL, 255);

	CBlob biggestBlob;
	blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );

	biggestBlob.FillBlob(Source, CV_RGB(100, 100, 100) );

	cvShowImage("Image", Source);

}
*/
