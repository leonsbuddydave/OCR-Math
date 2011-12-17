

///////////////////////////////
python-tests
	Teacher.py

	*** NOTE: Has a dependency on the Python Image Library (PIL) ***
	
	Collects source images from the folders in the root python-tests directory and averages them into a single image as well as a point-cloud representation. Averaged points will later be used for recognizing these characters in another context. Output images and point cloud text file are saved in "python-tests/output/<symbol-folder-name>_proc/". See source of Teacher.py for function documentation.

//////////////////////////////
cpp-implementation

	Work in progress of a port of the existing Python code to C++ using OpenCV. OpenCV must be installed for this to work. The CodeBlocks file has several dependencies currently (including OpenCV and CVBlob) and the port is not yet complete. 
	Most of the methods have been ported from the Python version. It currently scans all the directories where learning images will be and processes/renames/moves them into the appropriate output directory based on the input directory filename. Output image will be an inverted binary version of the original, cropped to the largest contiguous region. 


/////////////////////////////

TODO:
 -- Finish C++ port of existing code.
 -- Add command line parameters to process single directories at a time to simplify debugging later.
 -- Analysis and point-cloud conversion in teaching class. 
