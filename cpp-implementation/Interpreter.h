#include "ImageMethods.h"
#include "MethodsThatShouldBeStandard.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <stdlib.h>

struct Point
{
	int x;
	int y;
};

using namespace boost::filesystem;
using namespace std;

class Interpreter : ImageMethods
{
	public:
		Interpreter();
		~Interpreter();
		void Interpret(string);
		void InterpretUsingAverageImages(string);
	private:
		vector< IplImage* > AllSymbols;
		vector< IplImage* > ExtractAllSymbolBlobs(IplImage*);
		IplImage* PrepareImage(IplImage* Source);

		bool SaveAllSymbols();
	protected:
};
