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
	
	CommandLineParser parser(argc, argv, keys);
	string infile = parser.get<std::string>("input");
	string outdir = parser.get<std::string>("outdir");

	//------------- EDGE DETECTION - BEGIN ---------------

	if(infile == "" && outdir == ""){
		src = imread( "50_note_on_black_background_2_(Medium).jpg", 1 );
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
	//namedWindow( "Contours", WINDOW_AUTOSIZE );
	//imshow( "Contours - ", src );
	namedWindow( "Cropped Image - ", WINDOW_AUTOSIZE );
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

	Mat object = imread( "template.png", CV_LOAD_IMAGE_GRAYSCALE );

    if( !object.data )
    {
        std::cout<< "Error reading object " << std::endl;
        return -1;
    }

    //Detect the keypoints using SURF Detector
    int minHessian = 500;

    SurfFeatureDetector detector( minHessian );
    std::vector<KeyPoint> kp_object;

    detector.detect( object, kp_object );

    //Calculate descriptors (feature vectors)
    SurfDescriptorExtractor extractor;
    Mat des_object;

    extractor.compute( object, kp_object, des_object );

    FlannBasedMatcher matcher;

    VideoCapture cap(0);

    namedWindow("Good Matches");

    std::vector<Point2f> obj_corners(4);

    //Get the corners from the object
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint( object.cols, 0 );
    obj_corners[2] = cvPoint( object.cols, object.rows );
    obj_corners[3] = cvPoint( 0, object.rows );

    char key = 'a';
    int framecount = 0;
    //while (key != 27)
    //{
        //Mat frame = imread( "50.jpg", -1);;
		Mat frame = Mat(croppedImage);
        /*cap >> frame;

        if (framecount < 5)
        {
            framecount++;
            continue;
        }*/

        Mat des_image, img_matches;
        std::vector<KeyPoint> kp_image;
        std::vector<vector<DMatch > > matches;
        std::vector<DMatch > good_matches;
        std::vector<Point2f> obj;
        std::vector<Point2f> scene;
        std::vector<Point2f> scene_corners(4);
        Mat H;
        Mat image;

        cvtColor(frame, image, CV_RGB2GRAY);

        detector.detect( image, kp_image );
        extractor.compute( image, kp_image, des_image );

        matcher.knnMatch(des_object, des_image, matches, 2);

        for(int i = 0; i < min(des_image.rows-1,(int) matches.size()); i++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
        {
            if((matches[i][0].distance < 0.6*(matches[i][1].distance)) && ((int) matches[i].size()<=2 && (int) matches[i].size()>0))
            {
                good_matches.push_back(matches[i][0]);
            }
        }

        //Draw only "good" matches
        drawMatches( object, kp_object, image, kp_image, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

        if (good_matches.size() >= 4)
        {
            for( int i = 0; i < good_matches.size(); i++ )
            {
                //Get the keypoints from the good matches
                obj.push_back( kp_object[ good_matches[i].queryIdx ].pt );
                scene.push_back( kp_image[ good_matches[i].trainIdx ].pt );
            }

            H = findHomography( obj, scene, CV_RANSAC );

            perspectiveTransform( obj_corners, scene_corners, H);

            //Draw lines between the corners (the mapped object in the scene image )
            line( img_matches, scene_corners[0] + Point2f( object.cols, 0), scene_corners[1] + Point2f( object.cols, 0), Scalar(0, 255, 0), 4 );
            line( img_matches, scene_corners[1] + Point2f( object.cols, 0), scene_corners[2] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
            line( img_matches, scene_corners[2] + Point2f( object.cols, 0), scene_corners[3] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
            line( img_matches, scene_corners[3] + Point2f( object.cols, 0), scene_corners[0] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
        }

		a_file << "| Good matches count - ";
		a_file << good_matches.size();
        //Show detected matches
        imshow( "Good Matches", img_matches );


  //------------- Flann Feature Matching - END---------------

	//------- Saving the image
	//saveImage(imageShirtHSV);
	//------- Saving the image
	cout << "This is the data which gets returned to the php server code";

	//cvNamedWindow("Shirt", 1);
    //cvShowImage("Shirt", imageShirtHSV);
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
