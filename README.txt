

///////////////////////////////
python-tests
	Teacher.py

	*** NOTE: Has a dependency on the Python Image Library (PIL) ***
	
	Collects source images from the folders in the root python-tests directory and averages them into a single image as well as a point-cloud representation. Averaged points will later be used for recognizing these characters in another context. Output images and point cloud text file are saved in "python-tests/output/<symbol-folder-name>_proc/". See source of Teacher.py for function documentation.

//////////////////////////////
cpp-implementation

	Work in progress of a port of the existing Python code to C++ using OpenCV. OpenCV must be installed for this to work. The CodeBlocks file has several dependencies currently (including OpenCV and CVBlob) and the port is not yet complete. While most of the methods have been ported, all it currently does is find all the images in the learning directories and displays the file paths without performing any operations.
	If those dependencies are available, it will compile (debug only) but there's currently no real point to it. 



/////////////////////////////

TODO:
 -- Finish C++ port of existing code.
 -- Add command line parameters to process single directories at a time to simplify debugging later.
 -- Refactor several image manipulation methods into a superclass - both the Teacher and the image interpreter classes will need to derive and use them. (partially done)
