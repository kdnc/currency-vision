#include <cstdio>	// Used for "printf"
#include <string>	// Used for C++ strings
#include <iostream>	// Used for C++ cout print statements
//#include <cmath>	// Used to calculate square-root for statistics

// Include OpenCV libraries
#include <opencv\cv.h>
#include <opencv\cvaux.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>


using namespace std;
using namespace cv;

const char* keys = 
{
	"{i|input| |The source image}"
	"{o|outdir| |The output directory}"
};

// Various color types for detected shirt colors.
enum                             {cBLACK=0,cWHITE, cGREY, cRED, cORANGE, cYELLOW, cGREEN, cAQUA, cBLUE, cPURPLE, cPINK,  NUM_COLOR_TYPES};
char* sCTypes[NUM_COLOR_TYPES] = {"Black", "White","Grey","Red","Orange","Yellow","Green","Aqua","Blue","Purple","Pink"};
uchar cCTHue[NUM_COLOR_TYPES] =    {0,       0,      0,     0,     20,      30,      55,    85,   115,    138,     161};
uchar cCTSat[NUM_COLOR_TYPES] =    {0,       0,      0,    255,   255,     255,     255,   255,   255,    255,     255};
uchar cCTVal[NUM_COLOR_TYPES] =    {0,      255,    120,   255,   255,     255,     255,   255,   255,    255,     255};


// Determine what type of color the HSV pixel is. Returns the colorType between 0 and NUM_COLOR_TYPES.
int getPixelColorType(int H, int S, int V)
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

int main(int argc, const char **argv)
{
	CommandLineParser parser(argc, argv, keys);
	string infile = parser.get<std::string>("input");
	string outdir = parser.get<std::string>("outdir");

	char * strFileImage = new char[infile.size() + 1];
	copy(infile.begin(), infile.end(), strFileImage);
	strFileImage[infile.size()] = '\0'; // don't forget the terminating 0

	//char *strFileImage = "20_new_note_1 (Medium)_cropped.jpg";	// default file
	//char *strFileImage = "20_new_note_1 (Medium)_cropped.jpg";

	IplImage* imageIn = cvLoadImage(strFileImage, CV_LOAD_IMAGE_UNCHANGED);

	IplImage *imageShirt = cvCloneImage(imageIn);

	IplImage *imageShirtHSV = cvCreateImage(cvGetSize(imageShirt), 8, 3);
	cvCvtColor(imageShirt, imageShirtHSV, CV_BGR2HSV);	// (note that OpenCV stores RGB images in B,G,R order.

	int h = imageShirtHSV->height;				// Pixel height
	int w = imageShirtHSV->width;				// Pixel width
	int rowSize = imageShirtHSV->widthStep;		// Size of row in bytes, including extra padding
	char *imOfs = imageShirtHSV->imageData;	// Pointer to the start of the image HSV pixels.

	float initialConfidence = 1.0f;

	// Create an empty tally of pixel counts for each color type
	int tallyColors[NUM_COLOR_TYPES];
	for (int i=0; i<NUM_COLOR_TYPES; i++)
		tallyColors[i] = 0;
	// Scan the shirt image to find the tally of pixel colors
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
		cout << sCTypes[i] << " " << (v*100/pixels) << "%, ";
		if (v > tallyMaxCount) {
			tallyMaxCount = tallyColors[i];
			tallyMaxIndex = i;
		}
	}
	cout << endl;
	int percentage = initialConfidence * (tallyMaxCount * 100 / pixels);
	cout << "Color of currency note: " << sCTypes[tallyMaxIndex] << " (" << percentage << "% confidence)." << endl << endl;


	//------- Saving the image
	CvSize size;
    //IplImage *rgb_img;
    int i = 0;
	int j = 0;

    size.height = h;
    size.width = w;

    //rgb_img = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);
    //rgb_img->imageData = my_device.ColorBuffer;

    // You should NOT have the line below or OpenCV will try to deallocate your data
    //rgb_img->imageDataOrigin = rgb_img->imageData;

    //for (i = 0; i < size.height; i++)
    //{
        //for (j = 0;j < size.width; j++)
        //{
        // confirming all values print correctly
        //printf("%c, ", imageShirtHSV->imageData[i*w + j]);
        //}
    //}

    cvSaveImage("foo2.png",imageShirtHSV);
	//------- Saving the image



	cvNamedWindow("Shirt", 1);
    cvShowImage("Shirt", imageShirtHSV);
	cvWaitKey();
	// Free resources.
	delete[] strFileImage;
	cvReleaseImage(&imageShirtHSV);
    cvReleaseImage(&imageShirt);
    cvReleaseImage(&imageIn);
	return 0;
}
