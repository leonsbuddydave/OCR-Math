#include "Teacher.h"

Teacher::Teacher()
{

}

Teacher::~Teacher()
{

}

void Teacher::Learn()
{
	// Bring me the horizon
	path LearningPath("./sourceimages");

	if (!exists("./output"))
	{
		create_directory("./output");
	}

	directory_iterator end_it; // default constructor is an end iterator

	// Loop through all the folders in the learning directory - each folder is its own symbol
	for (directory_iterator it(LearningPath); it != end_it; ++it)
	{
		if (is_directory(it->status()))
		{
			// Iterate through all the files in the learning directories
			string SymbolDirectory =  it->path().leaf().c_str();

			path OutputDirectory("./output/" + SymbolDirectory + "_proc");


			int TotalImagesInDirectory = 0;
			vector< vector<int> > PixelAppearanceRate;
			for (int i = 0; i < 300; i++)
			{
				vector<int> Row;
				for (int j = 0; j < 300; j++)
				{
					Row.push_back(0);
			}
				PixelAppearanceRate.push_back(Row);
			}


			directory_iterator inner_end_it;
			for (directory_iterator innerIt(it->path()); innerIt != inner_end_it; ++innerIt)
			{
				if (innerIt->path().extension() == ".jpg" || innerIt->path().extension() == ".JPG")
				{
					if (!exists(OutputDirectory))
					{
						create_directory(OutputDirectory);
					}


					cout << "Image found: " << innerIt->path() << endl;


					IplImage* SourceImage = cvLoadImage(innerIt->path().c_str());


					cvNamedWindow("Image", 1);

					SourceImage = PrepareImage(SourceImage);

					SourceImage = Threshold(SourceImage);

					SourceImage = CropLargestBlob(SourceImage);

					SourceImage = ConvertToSquare(SourceImage, 300);

					AddImageToPixelCount(SourceImage, PixelAppearanceRate);

					cvSaveImage((OutputDirectory / innerIt->path().filename()).c_str(), SourceImage);

					TotalImagesInDirectory++;
				}
			}

			BuildPointCloudFile(PixelAppearanceRate, (SymbolDirectory).c_str(), TotalImagesInDirectory);
			BuildAverageImage(PixelAppearanceRate, (OutputDirectory / "Average.png").c_str(), TotalImagesInDirectory);
		}
	}
}

void Teacher::AddImageToPixelCount(IplImage* Source, vector< vector<int> >& PixelAppearanceRate)
{
	const uchar* PixelData = (uchar*)Source->imageData;

	const int Width = Source->width;
	const int Height = Source->height;
	const int Step = Source->widthStep;

	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++)
		{
			PixelAppearanceRate[i][j] += (PixelData[i * Step + j] == 255 ? 1 : 0);
		}
	}
}

void Teacher::BuildAverageImage(vector< vector<int> >& PixelAppearanceRate, string FilePath, int TotalImagesProcessed)
{

	IplImage* Destination = cvCreateImage(cvSize(PixelAppearanceRate.size(), PixelAppearanceRate[0].size()), IPL_DEPTH_8U, 1);

	cvSet(Destination, CV_RGB(0, 0, 0));

	uchar* PixelData = (uchar*)Destination->imageData;
	int Step = Destination->widthStep;

	for (int i = 0; i < PixelAppearanceRate.size(); i++)
	{
		for (int j = 0; j < PixelAppearanceRate[i].size(); j++)
		{
			if( (i * j) % DatasetTrimThreshold == 0 ) // Only using every Nth point
			{
				if ((double)(PixelAppearanceRate[i][j] / (double)TotalImagesProcessed * 100.0) > PixelTolerance) // Pixel has to be common
				{
					PixelData[i * Step + j] = 255;
				}
				else
				{
					PixelData[i * Step + j] = 0;
				}
			}
		}
	}

	cout << "Average is saved at " << FilePath.c_str() << endl;
	cvSaveImage(FilePath.c_str(), Destination);
	cvReleaseImage(&Destination);
}

void Teacher::BuildPointCloudFile(vector< vector<int> >& PixelAppearanceRate, string FilePath, int TotalImagesProcessed)
{
	ofstream PointCloudFile;
	PointCloudFile.open(("./symbols/" + FilePath + ".txt").c_str());
	cout << "Saving point cloud file to " << FilePath << endl;
	for (int i = 0; i < PixelAppearanceRate.size(); i++)
	{
		for (int j = 0; j < PixelAppearanceRate[i].size(); j++)
		{
			if( (i * j) % DatasetTrimThreshold == 0 ) // Only using every Nth point
			{
				if ((double)(PixelAppearanceRate[i][j] / (double)TotalImagesProcessed * 100.0) > PixelTolerance) // Pixel has to be common
				{
					PointCloudFile << i << "," << j << endl;
				}
			}
		}
	}
	PointCloudFile.close();
}
