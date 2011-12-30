#include "Interpreter.h"

Interpreter::Interpreter()
{

}

Interpreter::~Interpreter()
{

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
