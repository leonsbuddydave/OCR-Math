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
				}
			}
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
	cout << PixelAppearanceRate.size() << endl;	
}
