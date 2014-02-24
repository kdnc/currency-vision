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

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

void saveImage(IplImage *imageShirtHSV)
{
	int h = imageShirtHSV->height;				// Pixel height
	int w = imageShirtHSV->width;				// Pixel width

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

    cvSaveImage("foo3.png",imageShirtHSV);

}

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

Mat thresh_callback(int, void* )
{
  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using Threshold
  threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );

  /// Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  // Having CV_RETR_EXTERNAL instead of CV_RETR_TREE, will only return the outermost contours.
  //findContours( threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Approximate contours to polygons + get bounding rects and circles
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
  vector<Point2f>center( contours.size() );
  vector<float>radius( contours.size() );

  int largest_area=0;
  int largest_contour_index=0;
  Rect bounding_rect;

  for( size_t i = 0; i < contours.size(); i++ ) { 
	  double a=contourArea( contours[i],false);  //  Find the area of contour
      if(a>largest_area){
		largest_area=a;
		largest_contour_index=i;                //Store the index of largest contour
		bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
      }
   
	  approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
      boundRect[i] = boundingRect( Mat(contours_poly[i]) );
      minEnclosingCircle( contours_poly[i], center[i], radius[i] );
  }

  /// Draw polygonal contour + bonding rects + circles
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  for( size_t i = 0; i< contours.size(); i++ ) {
	 Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	 //drawContours( src, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
	 //rectangle( src, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
	 //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
  }

  Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
  //drawContours( src, contours,largest_contour_index, color, CV_FILLED, 8, hierarchy ); // Draw the largest contour using previously stored index.
  rectangle(src, bounding_rect,  Scalar(0,255,0),1, 8,0);  

  Mat croppedImage = src(bounding_rect);
  return croppedImage;
}

int main(int argc, const char **argv)
{
	ofstream a_file;
	a_file.open("output.txt");
	a_file << "test";
	
	CommandLineParser parser(argc, argv, keys);
	string infile = parser.get<std::string>("input");
	string outdir = parser.get<std::string>("outdir");

	//------------- EDGE DETECTION - BEGIN ---------------

	if(infile == "" && outdir == ""){
		src = imread( "50_note_on_black_background_1_(Medium).jpg", 1 );
	} else {
		src = imread( infile, 1 );
	}

	/// Convert image to gray and blur it
	cvtColor( src, src_gray, COLOR_BGR2GRAY );
	blur( src_gray, src_gray, Size(3,3) );

	/// Create Window
	const char* source_window = "Source";
	namedWindow( source_window, WINDOW_AUTOSIZE );
	imshow( source_window, src );

	//createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
	Mat croppedImage = thresh_callback( 0, 0 );
	
	/// Show in a window
	namedWindow( "Contours", WINDOW_AUTOSIZE );
	imshow( "Contours - ", src );
	imshow( "Cropped Image - ", croppedImage );

	//------------- EDGE DETECTION - END   ---------------

	
	//------------- COLOR DETECTION - BEGIN ---------------

	//char *strFileImage = new char[infile.size() + 1];
	//copy(infile.begin(), infile.end(), strFileImage);
	//strFileImage[infile.size()] = '\0'; // don't forget the terminating 0

	//char *strFileImage = "img4.jpg";	// default file
	//char *strFileImage = "20_new_note_1 (Medium)_cropped.jpg";

	//IplImage *imageIn = cvLoadImage(strFileImage, CV_LOAD_IMAGE_UNCHANGED);
	IplImage *imageIn = cvCloneImage(&(IplImage)croppedImage);

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
		a_file << sCTypes[i] << " " << (v*100/pixels) << "%, ";
		if (v > tallyMaxCount) {
			tallyMaxCount = tallyColors[i];
			tallyMaxIndex = i;
		}
	}
	cout << endl;
	
	int percentage = initialConfidence * (tallyMaxCount * 100 / pixels);
	cout << "Color of currency note: " << sCTypes[tallyMaxIndex] << " (" << percentage << "% confidence)." << endl << endl;
	a_file << "Color of currency note: ";
	a_file << sCTypes[tallyMaxIndex];
	a_file << " (";
	a_file << percentage;
	a_file << "% confidence).";
	
	//------------- COLOR DETECTION - END ---------------

	//------------- Flann Feature Matching - BEGIN---------------

	  //if( argc != 3 )
  //{ readme(); return -1; }
  Mat img_1 = imread( "template.png", IMREAD_GRAYSCALE );
  //Mat img_2 = imread( "50note1.jpg", IMREAD_GRAYSCALE );
  Mat img_2 = Mat(croppedImage);
  cvtColor( img_2, img_2, COLOR_BGR2GRAY );

  if( !img_1.data || !img_2.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 400;

  SurfFeatureDetector detector( minHessian );

  std::vector<KeyPoint> keypoints_1, keypoints_2;

  detector.detect( img_1, keypoints_1 );
  detector.detect( img_2, keypoints_2 );

  //-- Step 2: Calculate descriptors (feature vectors)
  SurfDescriptorExtractor extractor;

  Mat descriptors_1, descriptors_2;

  extractor.compute( img_1, keypoints_1, descriptors_1 );
  extractor.compute( img_2, keypoints_2, descriptors_2 );

  //-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  std::vector< DMatch > matches;
  matcher.match( descriptors_1, descriptors_2, matches );

  double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );

  //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
  //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
  //-- small)
  //-- PS.- radiusMatch can also be used here.
  std::vector< DMatch > good_matches;

  for( int i = 0; i < descriptors_1.rows; i++ )
  { if( matches[i].distance <= max(2*min_dist, 0.02) )
    { good_matches.push_back( matches[i]); }
  }

  //-- Draw only "good" matches
  Mat img_matches;
  drawMatches( img_1, keypoints_1, img_2, keypoints_2,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  //-- Show detected matches
  imshow( "Good Matches - ", img_matches );
  imwrite("img_matches.png", img_matches);
  
  for( int i = 0; i < (int)good_matches.size(); i++ )
  { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }


  //------------- Flann Feature Matching - END---------------

	//------- Saving the image
	saveImage(imageShirtHSV);
	//------- Saving the image
	cout << "This is the data which gets returned to the php server code";
	a_file << "Then Came here";
	cvNamedWindow("Shirt", 1);
    cvShowImage("Shirt", imageShirtHSV);
	a_file.close();
	if(infile == "" && outdir == ""){
		cvWaitKey();
	}

	// Free resources.
	//delete[] strFileImage;
	cvReleaseImage(&imageShirtHSV);
    cvReleaseImage(&imageShirt);
    cvReleaseImage(&imageIn);
	return 0;
}
