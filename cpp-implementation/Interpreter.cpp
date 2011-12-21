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

	InputImage = Threshold(InputImage);




	
}

vector< IplImage* > Interpreter::ExtractAllSymbolBlobs(IplImage* Source)
{
	/*
	* Source: Image to be ripped from
	*/

	IplImage* LabelImage = cvCreateImage(cvGetSize(Source), IPL_DEPTH_LABEL, 1);

	CvBlobs Blobs;
	unsigned int Result = cvLabel(Source, LabelImage, Blobs);

	// This next part is going to be tricky. We have to take out all _important_ blobs. There are a couple ways to do this:
	// 1. By assuming that all symbols are going to be above a certain size - this gets rid of "noise"
	// 2. By keeping all pulled regions, and later discarding any with really low confidence values - possibly slower
	// ...
	// I'll sleep on it.

}
