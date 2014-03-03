package com.currencyvision;

import java.io.IOException;
import java.io.InputStream;

import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.Window;
import android.widget.EditText;

public class MainActivity extends Activity {

	public final static String EXTRA_MESSAGE = "com.example.myfirstapp.MESSAGE";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
		setContentView(R.layout.activity_main);
		getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.window_title);
//		final boolean customTitleSupported = requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
//		if ( customTitleSupported ) {
//	        getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.mytitle);
//	    }
//
//	    final TextView myTitleText = (TextView) findViewById(R.id.myTitle);
//	    if ( myTitleText != null ) {
//	        myTitleText.setText("========= NEW TITLE ==========");
//	        myTitleText.setBackgroundColor(Color.GREEN);
//	    }
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public void sendMessage(View view) {
	    Intent intent = new Intent(this, CameraDemoActivity.class);
//	    EditText editText = (EditText) findViewById(R.id.edit_message);
//	    String message = editText.getText().toString();
//	    intent.putExtra(EXTRA_MESSAGE, message);
	    startActivity(intent);
		
//		Intent intent = new Intent(this, AndroidTextToSpeechActivity.class);
//	    startActivity(intent);
	    
//	    sendCapturedImageToServer();
	}
	
	private void sendCapturedImageToServer() {
		Thread trd = new Thread(new Runnable(){
	    	  @Override
	    	  public void run(){
//	            String url = "http://10.200.124.41/currency-vision/web-service/test.php";
	            String url = "http://10.0.2.2/currency-vision/web-service/currency-detect.php";

    	        try {
//    	        	ByteArrayOutputStream stream = new ByteArrayOutputStream();
//    	        	byte [] byte_arr = stream.toByteArray();
//    	        	String image_str = Base64.encodeBytes(byte_arr);
//    	        	ArrayList<NameValuePair> nameValuePairs = new  ArrayList<NameValuePair>();
//                  
//    	        	nameValuePairs.add(new BasicNameValuePair("image",image_str));
//                  
    	            HttpClient httpclient = new DefaultHttpClient();
    	            HttpPost httppost = new HttpPost(url);
//    	          	httppost.setEntity(new UrlEncodedFormEntity(nameValuePairs));
    	            HttpResponse response = httpclient.execute(httppost);
					String the_string_response = convertResponseToString(response);
					System.out.println("success");
    	        }catch(Exception e){
                  System.out.println("Error in http connection "+e.toString());
    	        }
	    	  }
      	});
      	trd.start();
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
	
}
