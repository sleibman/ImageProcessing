//============================================================================
// Name        : MovieTestHarness.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : MovieTestHarness imports a movie from a file, and simultaneously
//               plays both the original movie and a processed version in separate
//               windows. The "processed" version may have arbitrary manipulations
//               applied to the original in real time on a frame by frame basis.
//               The primary goal is to provide a mechanism for convenient interactive
//               testing of algorithms.
//============================================================================

#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "EdgeDetector.h"

using namespace std;
using namespace cv;

void printUsage() {
	cout << "Usage: MovieTestHarness <inputMovieFile> [outputMovieFile]"
			<< endl;
	cout << "       inputMovieFile is required; outputMovieFile is optional"
			<< endl;
}

int main(int argc, char** argv) {
	Mat image;
	cout << "A";
	cout << flush;
	string outputMovieFile;
	bool writeOutMovie = false;
	bool debug = true; // TODO: Ugh. Fix this.

	// TODO: Get rid of conditionals by creating a frameProcessor interface and a factory for
	// getting the right implementation of a frameProcessor, whose attributes can be used throughout
	// this code.
	bool splitScreen = true;

	string inputMovieFile;
	if (debug) {
		inputMovieFile = "/Users/sleibman/Documents/HandMovie.m4v";
		outputMovieFile = "/Users/sleibman/Documents/HandOut3.m4v";
		writeOutMovie = true;
	} else {
		if (argc < 2) {
			cout << "No input movie file specified." << endl;
			printUsage();
			return -1;
		}
		if (argc > 3) {
			cout << "Too many arguments." << endl;
			printUsage();
			return -1;
		}
		if (argc == 3) {
			outputMovieFile = argv[2];
			writeOutMovie = true;
		}
		inputMovieFile = argv[1];
	}

	char c;
	int delay = 33; // Default to 33 msec between frames
	int frameNum = -1;          // Frame counter

	VideoCapture inputMovie(inputMovieFile);

	cout << "Attempting to open " << inputMovieFile << " ... " << endl;
	cout << flush;

	if (!inputMovie.isOpened()) {
		cout << "Could not open movie file " << inputMovieFile << endl;
		return -1;
	}
	cout << "Opened " << inputMovieFile << endl;
	Size inputMovieSize = Size((int) inputMovie.get(CV_CAP_PROP_FRAME_WIDTH),
			(int) inputMovie.get(CV_CAP_PROP_FRAME_HEIGHT));
	Size outputMovieSize = inputMovieSize;
	if (splitScreen) {
		outputMovieSize = Size((int) inputMovie.get(CV_CAP_PROP_FRAME_WIDTH),
				(int) inputMovie.get(CV_CAP_PROP_FRAME_HEIGHT) * 2);
	}

	const char* ORIGINAL_MOVIE = "Original Movie";
	//const char* PROCESSED_MOVIE = "Processed Movie";
	namedWindow(ORIGINAL_MOVIE, CV_WINDOW_AUTOSIZE);
	//namedWindow(PROCESSED_MOVIE, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(ORIGINAL_MOVIE, 0, 0);
	//cvMoveWindow(PROCESSED_MOVIE, 0, inputMovieSize.height);

	cout << "Width=" << inputMovieSize.width << "  Height="
			<< inputMovieSize.height << "  Frame count="
			<< inputMovie.get(CV_CAP_PROP_FRAME_COUNT) << endl;
	cout << flush;

	Mat frame, processedFrame;
	VideoWriter outputMovie;

	if (writeOutMovie) {
		int codec = static_cast<int>(inputMovie.get(CV_CAP_PROP_FOURCC));
		outputMovie.open(outputMovieFile, codec,
				inputMovie.get(CV_CAP_PROP_FPS), outputMovieSize, true);
		if (!outputMovie.isOpened()) {
			cout << "Could not open the output video for write: "
					<< outputMovieFile << endl;
			return -1;
		}
		cout << "Opened " << outputMovieFile << " for writing." << endl;
	}

	for (;;) //Show the image captured in the window and repeat
			{
		inputMovie >> frame;
		if (frame.empty()) {
			cout << "You have reached the end of the movie" << endl;
			break;
		}
		++frameNum;
		// cout << "Frame " << frameNum << " | ";
		// cout << flush;
		processedFrame = EdgeDetector(frame);
		//imshow(ORIGINAL_MOVIE, frame);
		//imshow(PROCESSED_MOVIE, processedFrame);
		if (writeOutMovie) {
			if (splitScreen) {
				// Yikes. Concatenating 2 matrices in OpenCV is not anywhere near as simple as it should be.
				// The following code is just a vertcat. Matlab equivalent: mergedFrame = [frame ; processedFrame]
				// Caution: Note inconsistency of row,col ordering in Rect().
//				Mat mergedFrame(outputMovieSize.height, outputMovieSize.width, frame.depth());
//				Mat tmpFrame = mergedFrame(Rect(0, 0, frame.cols, frame.rows));
//				frame.copyTo(tmpFrame);
//				Mat tmpProcessedFrame = mergedFrame(Rect(0, frame.rows,processedFrame.cols, processedFrame.rows));
//				processedFrame.copyTo(tmpProcessedFrame);

				// Found undocumented vconcat and hconcat functions. Still curious why the above didn't quite work.
				Mat mergedFrame;
				vconcat(frame, processedFrame, mergedFrame);
				imshow(ORIGINAL_MOVIE, mergedFrame);
				outputMovie << mergedFrame;
			} else {
				outputMovie << processedFrame;
			}
		}
		c = (char) cvWaitKey(delay);
		if (c == 27) {
			break; // ASCII 27 = ESC
		}
	}
	return 0;
}

