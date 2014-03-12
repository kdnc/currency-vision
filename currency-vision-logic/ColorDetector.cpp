#include "ColorDetector.h"
#include <cstdio>	// Used for "printf"
#include <string>	// Used for C++ strings
#include <iostream>	// Used for C++ cout print statements
#include <fstream>
//#include <cmath>	// Used to calculate square-root for statistics

// Include OpenCV libraries
#include <opencv\cv.h>
#include <opencv\cvaux.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
#include "opencv2/nonfree/features2d.hpp"

using namespace std;
using namespace cv;

// Various color types for detected currency note colors.
enum                             {cBLACK=0,cWHITE, cGREY, cRED, cORANGE, cYELLOW, cGREEN, cAQUA, cBLUE, cPURPLE, cPINK,  NUM_COLOR_TYPES};
char* sCTypes[NUM_COLOR_TYPES] = {"Black", "White","Grey","Red","Orange","Yellow","Green","Aqua","Blue","Purple","Pink"};
uchar cCTHue[NUM_COLOR_TYPES] =    {0,       0,      0,     0,     20,      30,      55,    85,   115,    138,     161};
uchar cCTSat[NUM_COLOR_TYPES] =    {0,       0,      0,    255,   255,     255,     255,   255,   255,    255,     255};
uchar cCTVal[NUM_COLOR_TYPES] =    {0,      255,    120,   255,   255,     255,     255,   255,   255,    255,     255};

// Determine what type of color the HSV pixel is. Returns the colorType between 0 and NUM_COLOR_TYPES.
int ColorDetector::getPixelColorType(int H, int S, int V)
{
	int color;
	if (V < 75)
		color = cBLACK;
	else if (V > 190 && S < 27)
		color = cWHITE;
	else if (S < 53 && V < 185)
		color = cGREY;
	else {	// Is a color
		if (H < 14)
			color = cRED;
		else if (H < 25)
			color = cORANGE;
		else if (H < 34)
			color = cYELLOW;
		else if (H < 73)
			color = cGREEN;
		else if (H < 102)
			color = cAQUA;
		else if (H < 127)
			color = cBLUE;
		else if (H < 149)
			color = cPURPLE;
		else if (H < 175)
			color = cPINK;
		else	// full circle 
			color = cRED;	// back to Red
	}
	return color;
}


string ColorDetector::detectColor(Mat croppedImage){
	string outputStr = "";

	//IplImage *imageIn = cvLoadImage(strFileImage, CV_LOAD_IMAGE_UNCHANGED);
	IplImage *imageIn = cvCloneImage(&(IplImage)croppedImage);

	IplImage *imageCurrency = cvCloneImage(imageIn);

	IplImage *imageCurrencyHSV = cvCreateImage(cvGetSize(imageCurrency), 8, 3);
	cvCvtColor(imageCurrency, imageCurrencyHSV, CV_BGR2HSV);	// (note that OpenCV stores RGB images in B,G,R order.

	int h = imageCurrencyHSV->height;				// Pixel height
	int w = imageCurrencyHSV->width;				// Pixel width
	int rowSize = imageCurrencyHSV->widthStep;		// Size of row in bytes, including extra padding
	char *imOfs = imageCurrencyHSV->imageData;	// Pointer to the start of the image HSV pixels.

	float initialConfidence = 1.0f;

	// Create an empty tally of pixel counts for each color type
	int tallyColors[NUM_COLOR_TYPES];
	for (int i=0; i<NUM_COLOR_TYPES; i++)
		tallyColors[i] = 0;
	// Scan the currency image to find the tally of pixel colors
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			// Get the HSV pixel components
			uchar H = *(uchar*)(imOfs + y*rowSize + x*3 + 0);	// Hue
			uchar S = *(uchar*)(imOfs + y*rowSize + x*3 + 1);	// Saturation
			uchar V = *(uchar*)(imOfs + y*rowSize + x*3 + 2);	// Value (Brightness)

			// Determine what type of color the HSV pixel is.
			int ctype = getPixelColorType(H, S, V);
			// Keep count of these colors.
			tallyColors[ctype]++;
		}
	}

	// Print a report about color types, and find the max tally
	//cout << "Number of pixels found using each color type (out of " << (w*h) << ":\n";
	int tallyMaxIndex = 0;
	int tallyMaxCount = -1;
	int pixels = w * h;
	for (int i=0; i<NUM_COLOR_TYPES; i++) {
		int v = tallyColors[i];
		//cout << sCTypes[i] << " " << (v*100/pixels) << "%, ";
		//a_file << sCTypes[i] << " " << (v*100/pixels) << "%, ";
		outputStr += sCTypes[i];
		outputStr += " ";
		outputStr += to_string(v*100/pixels);
		outputStr += "%, ";

		if (v > tallyMaxCount) {
			tallyMaxCount = tallyColors[i];
			tallyMaxIndex = i;
		}
	}
	cout << endl;
	
	int percentage = initialConfidence * (tallyMaxCount * 100 / pixels);
	//cout << "Color of currency note: " << sCTypes[tallyMaxIndex] << " (" << percentage << "% confidence)." << endl << endl;
	outputStr += "|Color of currency note: ";
	outputStr += sCTypes[tallyMaxIndex];
	outputStr += " (" + to_string(percentage);
	outputStr += "% confidence).";

	/*a_file << "Color of currency note: ";
	a_file << sCTypes[tallyMaxIndex];
	a_file << " (";
	a_file << percentage;
	a_file << "% confidence).";*/

	cvNamedWindow("Currency", 1);
	cvShowImage("Currency", imageCurrencyHSV);

	cvReleaseImage(&imageCurrencyHSV);
    cvReleaseImage(&imageCurrency);
    cvReleaseImage(&imageIn);

	return outputStr;
}