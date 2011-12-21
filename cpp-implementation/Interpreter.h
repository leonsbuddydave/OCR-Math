#include "ImageMethods.h"
#include <boost/filesystem.hpp>
#include <fstream>

class Interpreter : ImageMethods
{
	public:
		Interpreter();
		~Interpreter();
		void Interpret(string);
	private:
		vector< IplImage* > ExtractAllSymbolBlobs(IplImage*);	
	protected:
};
