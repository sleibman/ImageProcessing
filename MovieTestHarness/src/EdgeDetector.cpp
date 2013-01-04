/*
 * EdgeDetector.cpp
 *
 *  Created on: Jan 4, 2013
 *      Author: sleibman
 */

#include "opencv2/imgproc/imgproc.hpp"

#include "EdgeDetector.h"

int edgeThresh = 1;
int lowThreshold = 20;
int const max_lowThreshold = 100;
int ratio = 3;
int kernelSize = 3;

Mat EdgeDetector(Mat inputMatrix) {
	Mat gray;
	Mat outputMatrix, detectedEdges;

	// Note that we are relying on OpenCV reference counting to free
	// up memory at the appropriate time.
	// Current implementation incurs the cost of creating a new output
	// matrix with each call, which can be optimized out in the future.
	outputMatrix.create(inputMatrix.size(), inputMatrix.type());

	/// Convert the image to grayscale
	cvtColor(inputMatrix, gray, CV_BGR2GRAY);

	/// Reduce noise with a kernel 3x3
	blur(gray, detectedEdges, Size(3, 3));

	/// Canny detector
	Canny(detectedEdges, detectedEdges, lowThreshold, lowThreshold * ratio,
			kernelSize);

	/// Using Canny's output as a mask, we return our result
	outputMatrix = Scalar::all(0);

	inputMatrix.copyTo(outputMatrix, detectedEdges);
	return outputMatrix;
}

