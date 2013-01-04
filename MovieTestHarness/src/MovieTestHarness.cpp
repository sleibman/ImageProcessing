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

int main(int argc, char** argv) {
	Mat image;

	if (argc != 2) {
		cout << "No movie file specified" << endl;
		return -1;
	}
	const string inputMovieFile = argv[1];

	char c;
	int delay = 33; // Default to 33 msec between frames
	int frameNum = -1;          // Frame counter

	VideoCapture inputMovie(inputMovieFile);

	cout << "Attempting to open " << inputMovieFile << " ... " << endl;
	if (!inputMovie.isOpened()) {
		cout << "Could not open movie file " << inputMovieFile << endl;
		return -1;
	}
	cout << "Opened " << inputMovieFile << endl;
	Size inputMovieSize = Size((int) inputMovie.get(CV_CAP_PROP_FRAME_WIDTH),
			(int) inputMovie.get(CV_CAP_PROP_FRAME_HEIGHT));

	const char* ORIGINAL_MOVIE = "Original Movie";
	const char* PROCESSED_MOVIE = "Processed Movie";
	namedWindow(ORIGINAL_MOVIE, CV_WINDOW_AUTOSIZE);
	namedWindow(PROCESSED_MOVIE, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(ORIGINAL_MOVIE, 0, 0);
	cvMoveWindow(PROCESSED_MOVIE, 0, inputMovieSize.height);

	cout << "Width=" << inputMovieSize.width << "  Height="
			<< inputMovieSize.height << "  Frame count="
			<< inputMovie.get(CV_CAP_PROP_FRAME_COUNT) << endl;

	Mat frame;

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
		imshow(ORIGINAL_MOVIE, frame);
		imshow(PROCESSED_MOVIE, EdgeDetector(frame));

		c = (char) cvWaitKey(delay);
		if (c == 27) {
			break; // ASCII 27 = ESC
		}
	}
	return 0;
}

