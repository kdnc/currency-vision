#include "BoundaryDetector.h"
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

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

Mat BoundaryDetector::thresh_callback(int, void* )
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

Mat BoundaryDetector::detectBoundary(string infile){
	src = imread( infile, 1 );
	
	/// Convert image to gray and blur it
	cvtColor( src, src_gray, COLOR_BGR2GRAY );
	blur( src_gray, src_gray, Size(3,3) );

	/// Create Window
	const char* source_window = "Source";
	//namedWindow( source_window, WINDOW_AUTOSIZE );
	//imshow( source_window, src );

	//createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
	Mat croppedImage = thresh_callback( 0, 0 );
	
	/// Show in a window
	//namedWindow( "Contours", WINDOW_AUTOSIZE );
	//imshow( "Contours - ", src );
	namedWindow( "Cropped Image - ", WINDOW_AUTOSIZE );
	imshow( "Cropped Image - ", croppedImage );

	return croppedImage;
}