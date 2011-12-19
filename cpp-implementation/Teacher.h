#include "ImageMethods.h"


#include <boost/filesystem.hpp>
#include <iostream>

using namespace boost::filesystem;
using namespace std;

class Teacher : public ImageMethods
{
	public:
		Teacher();
		~Teacher();
		void Learn();
	private:
		void AddImageToPixelCount(IplImage*, vector< vector<int> >&);
	protected:

};
