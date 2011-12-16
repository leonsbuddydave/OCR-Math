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

	directory_iterator end_it; // default constructor is an end iterator

	// Loop through all the folders in the learning directory - each folder is its own symbol
	for (directory_iterator it(LearningPath); it != end_it; ++it)
	{
		if (is_directory(it->status()))
		{
			// Iterate through all the files in the learning directories

			directory_iterator inner_end_it;
			for (directory_iterator innerIt(it->path()); innerIt != inner_end_it; ++innerIt)
			{
				if (innerIt->path().extension() == ".jpg" || innerIt->path().extension() == ".JPG")
				{
					cout << "Image found: " << innerIt->path() << endl;
				}
			}


		}
	}
}
