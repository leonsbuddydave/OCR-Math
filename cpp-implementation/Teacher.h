#include "ImageMethods.h"
#include <boost/filesystem.hpp>
#include <fstream>

using namespace boost::filesystem;
using namespace std;

class Teacher : public ImageMethods
{
	public:
		Teacher();
		~Teacher();
		void Learn();
	private:
		static const int PixelTolerance = 20; // Decides how common a pixel has to be to be incorporated into the average
		static const int DatasetTrimThreshold = 1; // Decides how many reference pixels to keep for a domain of > 0 where 1 is all pixels.

		void AddImageToPixelCount(IplImage*, vector< vector<int> >&);
		void BuildAverageImage(vector< vector<int> >&, string, int);
		void BuildPointCloudFile(vector< vector<int> >&, string, int);
	protected:

};
