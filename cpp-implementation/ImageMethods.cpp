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

IplImage* ImageMethods::CropLargestBlob(IplImage* Source)
{
	IplImage* LabelImage = cvCreateImage(cvGetSize(Source), IPL_DEPTH_LABEL, 1);
	CvBlobs Blobs;
	unsigned int Result = cvLabel(Source, LabelImage, Blobs);


	CvBlob* BiggestBlob = Blobs[cvGreaterBlob(Blobs)];

	return CropOutBlob(Source, BiggestBlob);
}

IplImage* ImageMethods::CropOutBlob(IplImage* Source, CvBlob* Blob)
{
	cout << "Largest region is " << Blob->maxx - Blob->minx << "x" << Blob->maxy - Blob->miny << endl;
	IplImage* CroppedBlob = cvCreateImage( cvSize( Blob->maxx - Blob->minx, Blob->maxy - Blob->miny ), IPL_DEPTH_8U, 1);

	CvRect ROI = cvRect( Blob->minx, Blob->miny, Blob->maxx - Blob->minx, Blob->maxy - Blob->miny );

	cvSetImageROI(Source, ROI);

	cvCopy(Source, CroppedBlob);
	cvResetImageROI(Source);

	return CroppedBlob;
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
			PixelData[i * Step + j] = (PixelData[i * Step + j] > 120 ? 0 : 255);
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
