#include "main.h"

using namespace std;

int main(int argc, char** argv)
{
	// Main variables
	ServerModes Mode = TEACH;
	char* InterpretFilepath = "";

	IplImage* pImg;
	if (argc > 1)
	{
		cout << "We have arguments!" << endl;
		for (int i = 0; i < argc; i++)
		{
			cout << "Argv " << i << " is '" << argv[i] << "'" << endl;
		}
		if (string(argv[1]) == "-i")
		{
			Mode = INTERPRET;
			InterpretFilepath = argv[2];
		}
	}

	if (Mode == INTERPRET && string(InterpretFilepath) != "")
	{
		cout << "Interpreting input image." << endl;
		Interpreter interpreter;
		interpreter.InterpretUsingAverageImages(InterpretFilepath);
	}
	else
	{
		cout << "Learning from source images." << endl;
		Teacher teacher;
		teacher.Learn();
	}

	return 0;
}
