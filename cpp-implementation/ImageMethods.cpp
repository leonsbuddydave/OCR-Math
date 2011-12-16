#include "ImageMethods.h"

ImageMethods::ImageMethods()
{

}

ImageMethods::~ImageMethods()
{

}

IplImage* ImageMethods::PrepareImage(IplImage* Source)
{
	int Width = Source->width;
	int Height = Source->height;

	// Create destination grayscale image and convert
	IplImage* Destination = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_8U, 1);
	cvCvtColor(Source, Destination, CV_RGB2GRAY);

	// Create scaled-down destination to copy to
	IplImage* NewSize = cvCreateImage(cvSize(IMAGE_INITIAL_WIDTH_SCALE, ceil((double)Height / ((double)Width / (double)IMAGE_INITIAL_WIDTH_SCALE))), IPL_DEPTH_8U, 1);

	cvResize(Destination, NewSize, CV_INTER_AREA);

	return NewSize;
}

IplImage* ImageMethods::Threshold(IplImage* Source)
{
	uchar* PixelData = (uchar *)Source->imageData;

	const int Width = Source->width;
	const int Height = Source->height;
	const int Step = Source->widthStep;


	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++)
		{
			PixelData[i * Step + j] = (PixelData[i * Step + j] < 80 ? 0 : 255);
		}
	}

	return Source;
}

vector< vector<int> > ImageMethods::ConvertImageTo2DArray(IplImage* Source)
{
	const uchar* PixelData = (uchar *)Source->imageData;

	const int Width = Source->width;
	const int Height = Source->height;
	const int Step = Source->widthStep;

	vector< vector<int> > ImageGrid;

	for (int i = 0; i < Height; i++)
	{
		vector<int> Row;
		for (int j = 0; j < Width; j++)
		{
			Row.push_back(PixelData[i * Step + j]);
		}
		ImageGrid.push_back(Row);
	}
}
