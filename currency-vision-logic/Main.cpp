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

#include "PatternMatcher.h";
#include "BoundaryDetector.h";
#include "ColorDetector.h";

using namespace std;
using namespace cv;

const char* keys = 
{
	"{i|input| |The source image}"
	"{o|outdir| |The output directory}"
};

void saveImage(IplImage *imageCurrencyHSV)
{
	int h = imageCurrencyHSV->height;				// Pixel height
	int w = imageCurrencyHSV->width;				// Pixel width

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
        //printf("%c, ", imageCurrencyHSV->imageData[i*w + j]);
        //}
    //}

    cvSaveImage("output/converted_image.png",imageCurrencyHSV);

}

int main(int argc, const char **argv)
{
	ofstream a_file;
	a_file.open("output/output.txt");
	String output = "";
	
	CommandLineParser parser(argc, argv, keys);
	string infile = parser.get<std::string>("input");
	string outdir = parser.get<std::string>("outdir");

	if(infile == "" && outdir == ""){
		infile = "resources/20_new_note_1 (Medium).jpg";
	}

	//char *strFileImage = new char[infile.size() + 1];
	//copy(infile.begin(), infile.end(), strFileImage);
	//strFileImage[infile.size()] = '\0'; // don't forget the terminating 0

	//char *strFileImage = "img4.jpg";	// default file

	output += "Note Location - " + infile + "|";

	//------------- EDGE DETECTION - BEGIN ---------------

	BoundaryDetector boundaryDetector;
	Mat croppedImage = boundaryDetector.detectBoundary(infile);

	//------------- EDGE DETECTION - END   ---------------

	
	//------------- COLOR DETECTION - BEGIN ---------------

	ColorDetector colorDetector;
	output += colorDetector.detectColor(croppedImage);
		
	//------------- COLOR DETECTION - END ---------------


	//------------- Flann Feature Matching - BEGIN---------------
		
	string templates[] = {"50template","20template","100template"};
	int good_matches = 0;
	string match_note = "";
	int size = sizeof(templates)/sizeof(string);
	PatternMatcher patternMatcherUtil;

	for(int i=0; i<size; i++){
		int matches_count = patternMatcherUtil.detectPattern(croppedImage, templates[i]);
		if(matches_count > good_matches){
			good_matches = matches_count;
			match_note = templates[i];
		}
	}
	a_file << match_note;
	a_file << "| Good matchess count - ";
	a_file << good_matches;
	output += match_note;
	output += "| Good matches counts - ";
	output += to_string(good_matches);

	 //   namedWindow("Good Matches");
	 //       //Show detected matches
	 //   imshow( "Good Matches", img_matches );

	//------------- Flann Feature Matching - END---------------

	//saveImage(imageCurrencyHSV);

	//This is the data which gets returned to the php server code
	cout << output;

	a_file.close();
	if(infile == "" || outdir == ""){
		//cvNamedWindow("Currency", 1);
		//cvShowImage("Currency", imageCurrencyHSV);
		cvWaitKey();
	}

	// Free resources.
	//delete[] strFileImage;
	/*cvReleaseImage(&imageCurrencyHSV);
    cvReleaseImage(&imageCurrency);
    cvReleaseImage(&imageIn);*/
	return 0;
}
