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

					cvShowImage("Image", SourceImage);
					cvWaitKey(0);

					SourceImage = CropLargestBlob(SourceImage);

					cvShowImage("Image", SourceImage);
					cvWaitKey(0);

					cvSaveImage((OutputDirectory / innerIt->path().filename()).c_str(), SourceImage);
				}
			}


		}
	}
}
