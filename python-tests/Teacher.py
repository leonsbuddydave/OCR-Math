import Image
import ImageDraw
import os
import time
import argparse
import decimal
import math
import sys

# Global Variables
# 1 - thin outlines
# 8 - thicker outlines
BlackNeighborTolerance = 7

# Used as the width for input images
InitialImageWidth = 400

# Dataset Trimmming - keeps every nth point from the dataset
DatasetTrimThreshold = 10

# Average Image Resolution
# Images need to be uniform size to be properly average
# Since symbols can be either landscape or portrait, we
# average that to a square for simplicity's sake
AverageImageResolution = 300

# Decides how common a pixel has to be to be incorporated into the average: 1 - 100
PixelTolerance = 30


class Region():
	def __init__(self, x, y):
		self._pixels = [(x, y)]
		self._min_x = x
		self._max_x = x
		self._min_y = y
		self._max_y = y

	def add(self, x, y):
		self._pixels.append((x, y))
		self._min_x = min(self._min_x, x)
		self._max_x = max(self._max_x, x)
		self._min_y = min(self._min_y, y)
		self._max_y = max(self._max_y, y)

	def box(self):
		return [(self._min_x, self._min_y), (self._max_x, self._max_y)]


def FindLargestRegion(Regions):
	# Hack method for determining the real symbol amid possible garbage
	Largest = Regions[0];
	for r in Regions:
		if len(r._pixels) > len(Largest._pixels):
			Largest = r

	return r
			

def PrepareImage(Source):
	print("\tPreparing image for analysis...")
	Width,Height = Source.size
	Source = Source.resize((InitialImageWidth, Height/(Width / InitialImageWidth)), Image.ANTIALIAS)
	
	Source = Source.convert("L")
	return Source				


def GrowingRegions(Source):
	# TODO: Fix this - doesn't work for upward-facing concave shapes.
	print("\tFinding connected regions (growth method)...")
	Width, Height = Source.size
	AllPixels = Source.load()

	IDMapSource = Image.new("L", (Width, Height), 255)
	IDMap = IDMapSource.load()

	RowID = 0
	Regions = 0
	
	for x in xrange(Width):
		RowID = 0
		for y in xrange(Height):
			if AllPixels[x, y] == 0:
				if AllPixels[x, y - 1] == 255:
					# New ID in this row
					RowID += 1
					IDMap[x, y] = RowID
				else:
					IDMap[x, y] = RowID
		for y in xrange(Height):
			if AllPixels[x, y] == 0:
				if x > 0:
					if AllPixels[x - 1][y] > 0:
						continue
						
	IDMapSource.save("Map.png")
					

def FindRegions(Source):
	# Different solution - weirdly the same problem as the other method.
	print("\tFinding connected image regions...")
	Width, Height  = Source.size
	regions = {}
	pixel_region = [[0 for y in range(Height)] for x in range(Width)]
	equivalences = {}
	n_regions = 0

	AllPixels = Source.load()

	# First pass, find and assign regions.
	for x in xrange(Width):
		for y in xrange(Height):
			# Look for a black pixel (checks for both RGB and L modes)
			if AllPixels[x, y] == (0, 0, 0, 255) or AllPixels[x, y] == 0: #BLACK
				# Get the region number from north or west,
				# or create new region
				region_n = pixel_region[x-1][y] if x > 0 else 0
				region_w = pixel_region[x][y-1] if y > 0 else 0

				max_region = max(region_n, region_w)

				if max_region > 0:
					# A neighbour already has a region
					# New region is the smallest > 0
					new_region = min(filter(lambda i: i > 0, (region_n, region_w)))
					# Update equivalences
					if max_region > new_region:
						if max_region in equivalences:
							equivalences[max_region].add(new_region)
						else:
							equivalences[max_region] = set((new_region, ))
				else:
					n_regions += 1
					new_region = n_regions

				pixel_region[x][y] = new_region

	#Scan image again, assigning all equivalent regions the same region value.
	for x in xrange(Width):
		for y in xrange(Height):
				r = pixel_region[x][y]
				if r > 0:
					while r in equivalences:
						r = min(equivalences[r])

					if not r in regions:
						regions[r] = Region(x, y)
					else:
						regions[r].add(x, y)

	return list(regions.itervalues())

def Threshold(Source):
	print("\tThreshold...")

	Width,Height = Source.size

	# Get a reference array to the pixels in the image
	AllPixels = Source.load()
	
	# Loop through the pixels in the image and perform the threshold operation
	# Brightness value of 80 or less: Black
	# Brightness value of 81 or more: White
	for x in range(0, Width):
		for y in range(0, Height):
			if AllPixels[x, y] < 80:
				AllPixels[x, y] = 0
			else:
				AllPixels[x, y] = 255
	return Source

def RemoveNoise(Source):
	# Discards pixels with few black neighbors.
	# Implementation sounds vaguely racist; consider altering.
	print("\tRemoving noise...")
	Width,Height = Source.size
	AllPixels = Source.load() 
	Result = Image.new("L", Source.size, 255)
	ResultPixels = Result.load()

	for x in range(1, Width - 1):
		for y in range(1, Height - 1):
			if AllPixels[x, y] == 0:
				BlackNeighbors = 0
				# Check surrounding pixels in a 3x3 grid with the current pixel as the center
				for theta in range(0, 360, 45):
					if AllPixels[x + round(math.cos(theta)), y + round(math.sin(theta))] == 0:
						BlackNeighbors += 1

				if BlackNeighbors == 0:
					ResultPixels[x, y] = 255
				else:
					ResultPixels[x, y] = AllPixels[x, y]
	return Result

def FindEdges(Source):
	print("\tFinding edges...")
	Width,Height = Source.size
	AllPixels = Source.load()

	Result = Image.new("L", Source.size, 255)

	for x in range(1, Width - 1):
		for y in range(1, Height - 1):
			BlackNeighbors = 0
			
			# Check surrounding pixels in a 3x3 grid with the current pixel as the center
			for theta in range(0, 360, 45):
				if AllPixels[x + math.cos(theta), y + math.sin(theta)] == 0:
					BlackNeighbors += 1
		
			# BlackNeighborTolerance determines how thick the outline will be
			if BlackNeighbors > BlackNeighborTolerance:
				Result.putpixel((x, y), 255)
			else:
				Result.putpixel((x, y), AllPixels[x, y])
	return Result

def CropToSymbol(Source):
	print("\tCropping image to symbol boundaries...")
	Width,Height = Source.size
	AllPixels = Source.load()

	LeftBound = -1
	TopBound = -1
	RightBound = -1
	BottomBound = -1

	for x in range(0, Width):
		for y in range(0, Height):
			if AllPixels[x, y] == 0:
				# print("Black pixel at " + str(x) + ", " + str(y))
				if LeftBound == -1:
					LeftBound = x

				if TopBound == -1 or y < TopBound:
					TopBound = y

				if y > BottomBound:
					BottomBound = y

				RightBound = x

	BoundingBox = (LeftBound, TopBound, RightBound, BottomBound)
	Result = Source.crop(BoundingBox)
	return Result	


def AssemblePointCloud(Source, Simplify):
	print("Assembling point cloud...")
	# Turn the simplified image into pure data
	Width,Height = Source.size
	AllPixels = Source.load()
	
	PointCloud = []
	for x in range(0, Width):
		for y in range(0, Height):
			if AllPixels[x, y] == 0:
				PointCloud.append((x, y))

	if Simplify == True:
		print("Simplifying point cloud...")
		NewPointCloud = []
		for point in range(0, len(PointCloud)):
			# Remove every Nth point - simplifying the data set
			if point % DatasetTrimThreshold == 0:
				NewPointCloud.append(PointCloud[point])
		return NewPointCloud
		
	else:
		return PointCloud

def BuildImageFromPointCloud(PointCloud):
	# Inverse operation to the AssemblePointCloud method - converts a list of points to a binary image
	print("Building image from point cloud...")
	Result = Image.new("L", (AverageImageResolution, AverageImageResolution), 255)
	AllPixels = Result.load()
	
	for point in PointCloud:
		AllPixels[point[0], point[1]] = 0

	return Result


def BuildAverageImage(PixelAppearanceTracker, TotalImagesProcessed):
	decimal.getcontext().prec = 2
	print("Building average image from a set of " + str(TotalImagesProcessed) + "...")
	AverageImage = Image.new("L", (AverageImageResolution, AverageImageResolution), 255)
	AllPixels = AverageImage.load()
	for x in range(0, AverageImageResolution):
		for y in range(0, AverageImageResolution):
			if ((decimal.Decimal(PixelAppearanceTracker[x][y]) / decimal.Decimal(TotalImagesProcessed)) * decimal.Decimal(100)) > PixelTolerance:
				# print((PixelAppearanceTracker[x][y] / TotalImagesProcessed) * 100) 
				AllPixels[x, y] = 0

			# DEBUG PURPOSES ONLY: Should generate a sort of grayscale heatmap
			# AllPixels[x, y] = (PixelAppearanceTracker[x][y] / TotalImagesProcessed) * 255
	return AverageImage

def AddToPixelAppearanceRate(PixelAppearanceTracker, Source):
	print("\tCalculating pixel appearance rate...")
	AllPixels = Source.load()
	for x in range(0, AverageImageResolution):
		for y in range(0, AverageImageResolution):
			if AllPixels[x, y] == 0:
				PixelAppearanceTracker[x][y] += 1
	
	return PixelAppearanceTracker

def CreatePointCloudFile(PointCloud, TargetDirectory, SymbolName):
	print("Creating point cloud file...")
	Handle = file(TargetDirectory + "/" + SymbolName + ".txt", "w")
	for point in PointCloud:
		Handle.write(str(point[0]) + " " + str(point[1]) + "\n")
	Handle.close()

def ConvertToSquare(Source):
	Source = Source.resize((AverageImageResolution, AverageImageResolution), Image.NEAREST)
	return Source

def main():

	# Argument handling
	
	if len(sys.argv) == 1:
		print("No arguments, running full analysis.")

	# End argument handling

	StartTime = time.time()
	FilesProcessed = 0
	# Label a directory for all converted output files
	TargetDirectory = "output/"

	# If that target directory doesn't exist, make it
	if not os.path.isdir(TargetDirectory):
		os.mkdir(TargetDirectory)

	ThisDirectory = os.listdir(".")

	# Find all the files in this directory
	for filename in ThisDirectory:

		# If we've found a new directory and it isn't the output one
		if os.path.isdir(filename) and filename != "output":
			print("Scanning directory " + filename)
			# Find all the images (well, all the files) in the directory
			TeachingImages = os.listdir(filename)
			# Assemble an output directory for these images
			TargetPath = TargetDirectory + filename + "_proc"
			
			# Set up the 2D array to track pixel occurrence rates for this symbol
			PixelAppearanceTracker = []
			for i in range(0, AverageImageResolution):
				PixelAppearanceTracker.append([])
				for ii in range(0, AverageImageResolution):
					PixelAppearanceTracker[i].append(0)
			
			ImagesProcessed = 0
			# Loop through all the images
			for image in TeachingImages:		

				# And finally, check whether this file is an actual image or not
				if os.path.splitext(image)[1] == ".jpg":

					# If our assembled output directory doesn't exist, create it
					if not os.path.isdir(TargetPath):
						os.mkdir(TargetPath)

					print("Loading image " + image)
					Input = Image.open(filename + "/" + image)

					# Aaaand, just put the image through the motions
					Output = PrepareImage(Input)
					
					Output = Threshold(Output)			

					Output = CropToSymbol(Output)
					
					Output = ConvertToSquare(Output)
					
					# Add to image averaging total
					PixelAppearanceTracker = AddToPixelAppearanceRate(PixelAppearanceTracker, Output)					

					# For testing purposes only, save each image after modification
					Output.save(TargetPath + "/" + image)
					
					# Tick up the total number of files worked with (overall)
					FilesProcessed += 1

					# Tick up the number of images processed in this directory
					ImagesProcessed += 1
					print("\tFinished analyzing image.")

			if ImagesProcessed > 0:
				AverageImage = BuildAverageImage(PixelAppearanceTracker, ImagesProcessed)
				AverageImage.save(TargetPath + "/Average.jpg")
				PointCloud = AssemblePointCloud(AverageImage, True)
				FinalImage = BuildImageFromPointCloud(PointCloud)
				FinalImage.save(TargetPath + "/AveragedSimplifiedPointCloud.jpg")
				CreatePointCloudFile(PointCloud, TargetPath, filename)
		else:
			print("Skipping " + filename)

	print "Done!"
	print(str(time.time() - StartTime) + " seconds elapsed.")
	print(str(FilesProcessed) + " files processed.")

main()
