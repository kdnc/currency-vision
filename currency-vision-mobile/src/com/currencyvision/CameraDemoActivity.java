package com.currencyvision;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v4.app.NavUtils;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

public class CameraDemoActivity extends Activity {

	int TAKE_PHOTO_CODE = 0;
	public static int count=0;
	private String file = "";
	Uri outputFileUri = null;
	
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.activity_camera_demo);

	    //here,we are making a folder named picFolder to store pics taken by the camera using this application
        final String dir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES) + "/picFolder/"; 
        File newdir = new File(dir); 
        newdir.mkdirs();

	    Button capture = (Button) findViewById(R.id.btnCapture);
	    capture.setOnClickListener(new View.OnClickListener() {
	        public void onClick(View v) {
	            // here,counter will be incremented each time,and the picture taken by camera will be stored as 1.jpg,2.jpg and likewise.
	            count++;
	            file = dir+count+".jpg";
	            File newfile = new File(file);
	            try {
	                newfile.createNewFile();
	            } catch (IOException e) {
	            	Log.d("Error", "Error");
	            }       
	            outputFileUri = Uri.fromFile(newfile);
	            Intent cameraIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE); 
	            cameraIntent.putExtra(MediaStore.EXTRA_OUTPUT, outputFileUri);
	            startActivityForResult(cameraIntent, TAKE_PHOTO_CODE);
	        }
	    });
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
	    super.onActivityResult(requestCode, resultCode, data);

	    if (requestCode == TAKE_PHOTO_CODE && resultCode == RESULT_OK) {
            sendCapturedImageToServer();
        } else if (requestCode == TAKE_PHOTO_CODE && resultCode == RESULT_OK) {
        	Log.d("CameraDemo", "Pic saved");
	    }
	}
	    
	    private void sendCapturedImageToServer() {
            try {
                // bimatp factory
                BitmapFactory.Options options = new BitmapFactory.Options();
                // downsizing image as it throws OutOfMemory Exception for larger images
                options.inSampleSize = 8;

                final Bitmap bitmap = BitmapFactory.decodeFile(outputFileUri.getPath(),
                        options);
                final String dir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES) + "/picFolder/"; 
    	        File newdir = new File(dir); 
    	        newdir.mkdirs();
                count++;
	            file = dir+count+".jpg";
                File file;
                file = new File(outputFileUri.getPath());
                try {
                    FileOutputStream out = new FileOutputStream(file);
                	bitmap.compress(Bitmap.CompressFormat.PNG, 90, out);
                	out.flush();
                	out.close();
                	
                } catch (Exception e) {
                    e.printStackTrace();
                }
                
	        	Thread trd = new Thread(new Runnable(){
	  	    	  @Override
	  	    	  public void run(){
	  	            String url = "http://10.200.124.41/currency-vision/web-service/test.php";
	
	      	        try {
	      	        	ByteArrayOutputStream stream = new ByteArrayOutputStream();
	                	bitmap.compress(Bitmap.CompressFormat.PNG, 90, stream);
	                	byte [] byte_arr = stream.toByteArray();
	                    String image_str = Base64.encodeBytes(byte_arr);
	                    ArrayList<NameValuePair> nameValuePairs = new  ArrayList<NameValuePair>();
	                    
	                    nameValuePairs.add(new BasicNameValuePair("image",image_str));
	                    
	      	            HttpClient httpclient = new DefaultHttpClient();
	      	            HttpPost httppost = new HttpPost(url);
//	      	          	httppost.setEntity(new UrlEncodedFormEntity(nameValuePairs));
	      	            HttpResponse response = httpclient.execute(httppost);
						String the_string_response = convertResponseToString(response);
						System.out.println("success");
	      	        }catch(Exception e){
	                    System.out.println("Error in http connection "+e.toString());
	      	        }
	  	    	  }
	        	});
	        	trd.start();
            } catch (NullPointerException e) {
                e.printStackTrace();
            }
        }
	    
	    public String convertResponseToString(HttpResponse response) throws IllegalStateException, IOException{
	    	 InputStream inputStream;
            String res = "";
            StringBuffer buffer = new StringBuffer();
            inputStream = response.getEntity().getContent();
            int contentLength = (int) response.getEntity().getContentLength(); //getting content length…..
            if (contentLength < 0){
            } else {
                   byte[] data = new byte[512];
                   int len = 0;
                   try {
                       while (-1 != (len = inputStream.read(data)) ){
                           buffer.append(new String(data, 0, len)); //converting to string and appending  to stringbuffer…..
                       }
                   } catch (IOException e) {
                       e.printStackTrace();
                   }
                   try {
                       inputStream.close(); // closing the stream…..
                   } catch (IOException e) {
                       e.printStackTrace();
                   }
                   res = buffer.toString();     // converting stringbuffer to string…..
            }
            return res;
       }

	/**
	 * Set up the {@link android.app.ActionBar}, if the API is available.
	 */
	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	private void setupActionBar() {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
			getActionBar().setDisplayHomeAsUpEnabled(true);
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.camera_demo, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			// This ID represents the Home or Up button. In the case of this
			// activity, the Up button is shown. Use NavUtils to allow users
			// to navigate up one level in the application structure. For
			// more details, see the Navigation pattern on Android Design:
			//
			// http://developer.android.com/design/patterns/navigation.html#up-vs-back
			//
			NavUtils.navigateUpFromSameTask(this);
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

}
