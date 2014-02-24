package com.currencyvision;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.imgproc.Imgproc;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnTouchListener;

public class ColorBlobDetectionActivity extends Activity implements OnTouchListener, CvCameraViewListener2 {
    private static final String  TAG              = "OCVSample::Activity";

    private boolean              mIsColorSelected = false;
    private boolean              goneInsideFunction = false;
    private Mat                  mRgba;
    private Scalar               mBlobColorRgba;
    private Scalar               mBlobColorHsv;
    private ColorBlobDetector    mDetector;
    private Mat                  mSpectrum;
    private Size                 SPECTRUM_SIZE;
    private Scalar               CONTOUR_COLOR;
    
    private CameraBridgeViewBase mOpenCvCameraView;

    private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
//                    Log.i(TAG, "OpenCV loaded successfully");
                    mOpenCvCameraView.enableView();
                    mOpenCvCameraView.setOnTouchListener(ColorBlobDetectionActivity.this);
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };
    
    private String[] colors = {"cBLACK", "cWHITE", "cGREY", "cRED", "cORANGE", "cYELLOW", "cGREEN", "cAQUA", "cBLUE", "cPURPLE", "cPINK", "cRED"};


    private String getPixelColorType(int H, int S, int V)
    {
    	String color;
    	if (V < 75)
    		color = "cBLACK";
    	else if (V > 190 && S < 27)
    		color = "cWHITE";
    	else if (S < 53 && V < 185)
    		color = "cGREY";
    	else {	// Is a color
    		if (H < 14)
    			color = "cRED";
    		else if (H < 25)
    			color = "cORANGE";
    		else if (H < 34)
    			color = "cYELLOW";
    		else if (H < 73)
    			color = "cGREEN";
    		else if (H < 102)
    			color = "cAQUA";
    		else if (H < 127)
    			color = "cBLUE";
    		else if (H < 149)
    			color = "cPURPLE";
    		else if (H < 175)
    			color = "cPINK";
    		else	// full circle 
    			color = "cRED";	// back to Red
    	}
    	return color;
    }
    
    public ColorBlobDetectionActivity() {
//        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.color_blob_detection_surface_view);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.color_blob_detection_activity_surface_view);
        mOpenCvCameraView.setCvCameraViewListener(this);
    }

    @Override
    public void onPause()
    {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, mLoaderCallback);
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        mRgba = new Mat(height, width, CvType.CV_8UC4);
        mDetector = new ColorBlobDetector();
        mSpectrum = new Mat();
        mBlobColorRgba = new Scalar(255);
        mBlobColorHsv = new Scalar(255);
        SPECTRUM_SIZE = new Size(200, 64);
        CONTOUR_COLOR = new Scalar(255,0,0,255);
    }

    public void onCameraViewStopped() {
        mRgba.release();
    }

    public boolean onTouch(View v, MotionEvent event) {
        int cols = mRgba.cols();
        int rows = mRgba.rows();

        int xOffset = (mOpenCvCameraView.getWidth() - cols) / 2;
        int yOffset = (mOpenCvCameraView.getHeight() - rows) / 2;

        int x = (int)event.getX() - xOffset;
        int y = (int)event.getY() - yOffset;

//        Log.i(TAG, "Touch image coordinates: (" + x + ", " + y + ")");

        if ((x < 0) || (y < 0) || (x > cols) || (y > rows)) return false;

        Rect touchedRect = new Rect();

        touchedRect.x = (x>4) ? x-4 : 0;
        touchedRect.y = (y>4) ? y-4 : 0;

        touchedRect.width = (x+4 < cols) ? x + 4 - touchedRect.x : cols - touchedRect.x;
        touchedRect.height = (y+4 < rows) ? y + 4 - touchedRect.y : rows - touchedRect.y;

        Mat touchedRegionRgba = mRgba.submat(touchedRect);

        Mat touchedRegionHsv = new Mat();
        Imgproc.cvtColor(touchedRegionRgba, touchedRegionHsv, Imgproc.COLOR_RGB2HSV_FULL);

        // Calculate average color of touched region
        mBlobColorHsv = Core.sumElems(touchedRegionHsv);
        int pointCount = touchedRect.width*touchedRect.height;
        for (int i = 0; i < mBlobColorHsv.val.length; i++)
            mBlobColorHsv.val[i] /= pointCount;

        mBlobColorRgba = converScalarHsv2Rgba(mBlobColorHsv);

//        Log.i(TAG, "Touched rgba color: (" + mBlobColorRgba.val[0] + ", " + mBlobColorRgba.val[1] +
//                ", " + mBlobColorRgba.val[2] + ", " + mBlobColorRgba.val[3] + ")");

        mDetector.setHsvColor(mBlobColorHsv);

        Imgproc.resize(mDetector.getSpectrum(), mSpectrum, SPECTRUM_SIZE);

        mIsColorSelected = true;

        touchedRegionRgba.release();
        touchedRegionHsv.release();

        return false; // don't need subsequent touch events
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
        mRgba = inputFrame.rgba();

        if (mIsColorSelected && !goneInsideFunction) {
        	goneInsideFunction = true;
//            mDetector.process(mRgba);
//            List<MatOfPoint> contours = mDetector.getContours();
            
            Imgproc.cvtColor(mRgba, mRgba, Imgproc.COLOR_BGR2HSV);
            
//            Log.e(TAG, "Contours count: " + contours.size());
//            Imgproc.drawContours(mRgba, contours, -1, CONTOUR_COLOR);
            
            
			Size sz = new Size(100, 100);
            Imgproc.resize(mRgba, mRgba, sz, 0 , 0, Imgproc.INTER_CUBIC );
            
            
        	int h = mRgba.height();				// Pixel height
        	int w = mRgba.width();				// Pixel width
        	int rowSize = (int)mRgba.step1();		// Size of row in bytes, including extra padding
//        	char *imOfs = imageShirtHSV->imageData;	// Pointer to the start of the image HSV pixels.

        	float initialConfidence = 1.0f;

        	// Create an empty tally of pixel counts for each color type
//        	int tallyColors[NUM_COLOR_TYPES];
//        	for (int i=0; i<NUM_COLOR_TYPES; i++)
//        		tallyColors[i] = 0;
        	// Scan the shirt image to find the tally of pixel colors
        	Map<String, Integer> tallyColors = new HashMap<String, Integer>();
        	
        	byte[] pixelsTotal = new byte[h*rowSize];
        	mRgba.get(0,0,pixelsTotal);
        	
        	for (int y=0; y<h; y++) {
        		for (int x=0; x<w; x++) {
        			// Get the HSV pixel components
        			
        			double[] pix = mRgba.get(y, x);
        			int hVal = (int)pixelsTotal[(y*rowSize) + (x) + 0];	// Hue
        			int sVal = (int)pixelsTotal[(y*rowSize) + (x) + 1];	// Saturation
        			int vVal = (int)pixelsTotal[(y*rowSize) + (x) + 2];	// Value (Brightness)
//        			int hVal = (int)pix[0];	// Hue
//        			int sVal = (int)pix[1];	// Saturation
//        			int vVal = (int)pix[2];	// Value (Brightness)
        			
        			
        			// Determine what type of color the HSV pixel is.
        			String ctype = getPixelColorType(hVal, sVal, vVal);
        			// Keep count of these colors.
        			int totalNum = 0;
        			try{
        				totalNum = tallyColors.get(ctype);
        			} catch(Exception ex){
        				totalNum = 0;
        			}
        			totalNum++;
        			tallyColors.put(ctype, totalNum);
//        			tallyColors[ctype]++;
        		}
        	}
        	
        	// Print a report about color types, and find the max tally
        	//cout << "Number of pixels found using each color type (out of " << (w*h) << ":\n";
        	int tallyMaxIndex = 0;
        	int tallyMaxCount = -1;
        	int pixels = w * h;
        	for (int i=0; i<12; i++) {
        		String v = colors[i];
        		int pixCount;
        		try{
        			pixCount = tallyColors.get(v);
	       		} catch(Exception e){
	       			pixCount = 0;
	       		}
        		System.out.println(v + " - " + (pixCount*100/pixels) + "%, ");
        		if (pixCount > tallyMaxCount) {
        			tallyMaxCount = pixCount;
        			tallyMaxIndex = i;
        		}
        	}
        	float percentage = initialConfidence * (tallyMaxCount * 100 / pixels);
        	System.out.println("Color of currency note: " + colors[tallyMaxIndex] + " (" + percentage + "% confidence).");

        	

//            Mat colorLabel = mRgba.submat(4, 68, 4, 68);
//            colorLabel.setTo(mBlobColorRgba);
//
//            Mat spectrumLabel = mRgba.submat(4, 4 + mSpectrum.rows(), 70, 70 + mSpectrum.cols());
//            mSpectrum.copyTo(spectrumLabel);
        }

        return mRgba;
    }
    
    private Scalar converScalarHsv2Rgba(Scalar hsvColor) {
        Mat pointMatRgba = new Mat();
        Mat pointMatHsv = new Mat(1, 1, CvType.CV_8UC3, hsvColor);
        Imgproc.cvtColor(pointMatHsv, pointMatRgba, Imgproc.COLOR_HSV2RGB_FULL, 4);

        return new Scalar(pointMatRgba.get(0, 0));
    }
}
