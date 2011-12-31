#include "Interpreter.h"

Interpreter::Interpreter()
{

}

Interpreter::~Interpreter()
{

}

void Interpreter::InterpretUsingAverageImages(string FilePath)
{
	/*
	* FilePath: The absolute path of the input file to be interpreted
	*/

	IplImage* InputImage = cvLoadImage( FilePath.c_str() );

	InputImage = PrepareImage(InputImage);

	InputImage = Threshold(InputImage);

	AllSymbols = ExtractAllSymbolBlobs(InputImage);

	path SymbolFilePath("./symbols/");

	map<string, IplImage* > AllReferenceSymbols;

	directory_iterator EndIt; // default constructor is an end iterator

	// Load all the reference symbols
	for (directory_iterator FileIterator(SymbolFilePath); FileIterator != EndIt; ++FileIterator)
	{
		string Key = FileIterator->path().stem().c_str();
		AllReferenceSymbols[ Key ] = cvLoadImage( FileIterator->path().c_str() );
	
		IplImage* Destination = cvCreateImage(cvSize(AllReferenceSymbols[ Key ]->width, AllReferenceSymbols[ Key ]->height), IPL_DEPTH_8U, 1);
		cvCvtColor(AllReferenceSymbols[ Key ], Destination, CV_RGB2GRAY);
		AllReferenceSymbols[ Key ] = Destination;

		cout << "Symbol \"" << FileIterator->path().stem().c_str() << "\" loaded." << endl;
	}

	// Compare
	for (vector<IplImage*>::iterator SymbolIter = AllSymbols.begin(); SymbolIter != AllSymbols.end(); ++SymbolIter)
	{
		uchar* PixelData = (uchar*)(*SymbolIter)->imageData;
		const int Width = (*SymbolIter)->width;
		const int Height = (*SymbolIter)->height;
		const int Step = (*SymbolIter)->widthStep;
		const int Depth = (*SymbolIter)->depth;
		const int Channels = (*SymbolIter)->nChannels;

		if (Width <= 10 || Height <= 10)
		{
			// If it's this small, just discard it as useless - this is a hack, we'll come up with a better way later
			// We may have to base this on the region size relative to the image size - so we don't discard decimals,
			// multiplication signs, etc
			continue;
		}

		map<string, double> ConfidenceMap;

		for (map<string, IplImage*>::iterator RefSymbolIter = AllReferenceSymbols.begin(); RefSymbolIter != AllReferenceSymbols.end(); ++RefSymbolIter)
		{
			IplImage* ResizedReference = cvCreateImage(cvSize(Width, Height), Depth, Channels);
			cvResize(RefSymbolIter->second, ResizedReference, CV_INTER_AREA);

			double TotalPixelsAnalyzed = 0.0;
			double MatchingPixels = 0.0;

			const uchar* ReferencePixelData = (uchar*)ResizedReference->imageData;

			for (int i = 0; i < Height; i++)
			{
				for (int j = 0; j < Width; j++)
				{
					if (PixelData[i * Step + j] == 255)
					{
						// Found a white pixel in the original image
						TotalPixelsAnalyzed += 1.0;

						// If the same pixel on the resized reference image is white
						if (ReferencePixelData[i * Step + j] == 255)
						{
							MatchingPixels += 1.0;
						}
					}
				}
			}
			
			ConfidenceMap[ RefSymbolIter->first ] = MatchingPixels / TotalPixelsAnalyzed * 100;
			cout << ConfidenceMap[ RefSymbolIter->first ] << "% chance." << endl;
			//cvRelease(&ResizedReference);
		}
	}
	SaveAllSymbols();
}

void Interpreter::Interpret(string FilePath)
{
	/*
	* FilePath: the absolute path of the input file to be interpreted
	*/

	IplImage* InputImage = cvLoadImage(FilePath.c_str());

	InputImage = PrepareImage(InputImage);

	InputImage = Threshold(InputImage);

	AllSymbols = ExtractAllSymbolBlobs(InputImage);

	path SymbolFilePath("./symbols");

	map<string, vector<Point> > AllPointLists;

	directory_iterator endIt; // default constructor is an end iterator

	// Iterate through all the symbol point cloud files
	for (directory_iterator FileIterator(SymbolFilePath); FileIterator != endIt; ++FileIterator)
	{
		// Load the point file
		ifstream PointFileStream(FileIterator->path().c_str());

		string SymbolName = FileIterator->path().stem().c_str();

		// Load in and break up the point file into a usable format
		stringstream Buffer;
		Buffer << PointFileStream.rdbuf();
		vector<string> PointPairs;
		vector<Point> Points;
		MethodsThatShouldBeStandard::StringExplode(Buffer.str(), "\n", &PointPairs);

		for (vector<string>::iterator PointIterator = PointPairs.begin(); PointIterator != PointPairs.end(); ++PointIterator)
		{
			string XValue = PointIterator->substr(0, PointIterator->find_first_of(","));
			string YValue = PointIterator->substr(PointIterator->find_first_of(",") + 1, PointIterator->length());
			//cout << "(" << XValue << "," << YValue << ")" << endl;

			Point point;
			point.x = atoi(XValue.c_str());
			point.y = atoi(YValue.c_str());
			Points.push_back(point);
		}
		AllPointLists[SymbolName] = Points;
		cout << "Symbol '" << SymbolName << "' loaded." << endl;
	}

	
	// Let's compare the stored symbol to the input region
	

	for (vector<IplImage*>::iterator SymbolIterator = AllSymbols.begin(); SymbolIterator != AllSymbols.end(); ++SymbolIterator)
	{
		// Let's interpret all these images
		uchar* PixelData = (uchar*)(*SymbolIterator)->imageData;
		const int Width = (*SymbolIterator)->width;
		const int Height = (*SymbolIterator)->height;
		const int Step = (*SymbolIterator)->widthStep;

		map<string, int> ConfidenceMap;

		for (map<string, vector<Point> >::iterator PointListIterator = AllPointLists.begin(); PointListIterator != AllPointLists.end(); ++PointListIterator)
		{
			double TotalPoints = PointListIterator->second.size();
			double PointsMatched = 0;

			for (vector<Point>::iterator PointIterator = PointListIterator->second.begin(); PointIterator != PointListIterator->second.end(); ++PointIterator)
			{	
				// I got so wrapped up in writing the outer iterators that I can't remember what the hell goes here
				// wait I figured it out
				// PointsMatched += (PixelData[PointIterator->x * Step + PointIterator->y] == 255 ? 1 : 0);
				// PixelData[PointIterator->x * Step + PointIterator->y] = (PixelData[PointIterator->x * Step + PointIterator->y] == 255 ? 100 : 0);
			}

			ConfidenceMap[PointListIterator->first] = static_cast<int>(PointsMatched / TotalPoints * 100);
			cout << ConfidenceMap[PointListIterator->first] << "% chance of " << PointListIterator->first << endl;
		}
	}
	

	SaveAllSymbols();
}

IplImage* Interpreter::PrepareImage(IplImage* Source)
{
	IplImage* Destination = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);
	cvCvtColor(Source, Destination, CV_RGB2GRAY);

	return Destination;
}

vector< IplImage* > Interpreter::ExtractAllSymbolBlobs(IplImage* Source)
{
	/*
	* Source: Image to be ripped from
	*/

	vector< IplImage* > AllBlobImages;

	IplImage* LabelImage = cvCreateImage(cvGetSize(Source), IPL_DEPTH_LABEL, 1);

	CvBlobs Blobs;
	unsigned int Result = cvLabel(Source, LabelImage, Blobs);


	// Crop out all the symbols, yeah
	for (CvBlobs::const_iterator it = Blobs.begin(); it != Blobs.end(); ++it)
	{
		// AllBlobImages.push_back( ConvertToSquare( CropOutBlob(Source, it->second), 300) );
		AllBlobImages.push_back( CropOutBlob(Source, it->second) );
	}

	// This next part is going to be tricky. We have to take out all _important_ blobs. There are a couple ways to do this:
	// 1. By assuming that all symbols are going to be above a certain size - this gets rid of "noise"
	// 2. By keeping all pulled regions, and later discarding any with really low confidence values - possibly slower
	// ...
	// I'll sleep on it.
	return AllBlobImages;
}

bool Interpreter::SaveAllSymbols()
{
	cout << "Saving " << AllSymbols.size() << " symbols." << endl;
	int i = 0;
	stringstream ss;
	for (vector< IplImage* >::iterator it = AllSymbols.begin(); it != AllSymbols.end(); ++it)
	{
		ss << "symbol";
		ss << i;
		cvSaveImage(string("./temp/" + ss.str() + ".jpg").c_str(), *it);
		ss.str("");
		i++;
	}
}
