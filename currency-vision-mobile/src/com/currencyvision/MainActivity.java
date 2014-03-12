package com.currencyvision;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;

import org.apache.http.client.HttpClient;
import org.apache.http.impl.client.DefaultHttpClient;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.provider.ContactsContract.CommonDataKinds.Note;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.Window;

public class MainActivity extends Activity {

	public final static String EXTRA_MESSAGE = "com.example.myfirstapp.MESSAGE";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
		setContentView(R.layout.activity_main);
		getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.window_title);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public void sendMessage(View view) {
	    Intent intent = new Intent(this, CameraDemoActivity.class);
	    startActivity(intent);
		
//		Intent intent = new Intent(this, AndroidTextToSpeechActivity.class);
//	    startActivity(intent);
	    
//		String noteName = "";
//		if(view.getId() == R.id.note_20_1){
//			noteName = "note_20_1";
//		}
		
//		String data = "";
//	    try{
//			// Set Request parameter
//		    data +="?" + URLEncoder.encode("data", "UTF-8") + "=" + noteName;
//		} catch (UnsupportedEncodingException e) {
//		    e.printStackTrace();
//		} 
//		String serverURL = "http://10.0.2.2/currency-vision/web-service/currency-detect.php"+data;
//        // Use AsyncTask execute Method To Prevent ANR Problem
//        new LongOperation().execute(serverURL);
	}
	
	private void openAlert(String result) {
		 AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(MainActivity.this);
	     
		 alertDialogBuilder.setTitle("Currency Note Result");
		 String[] results = result.split("\\|");
		 String resultTokenized = "";
		 for(int i=0; i<results.length; i++){
			 resultTokenized += results[i];
			 if(i+1 < results.length){
				 resultTokenized += "\n\n";
			 }
		 }
		 alertDialogBuilder.setMessage(resultTokenized);

		 alertDialogBuilder.setPositiveButton("OK",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {
				dialog.cancel();
			}
		 });
		 
		 AlertDialog alertDialog = alertDialogBuilder.create();
		 alertDialog.show();
	}
	
	private class LongOperation  extends AsyncTask<String, Void, Void> {
        
        // Required initialization
        private String content;
        private String error = null;
        private ProgressDialog Dialog = new ProgressDialog(MainActivity.this);
//        String data =""; 
         
        protected void onPreExecute() {
            // NOTE: You can call UI Element here.
            //Start Progress Dialog (Message)
            Dialog.setMessage("Please wait..");
            Dialog.show();
//            try{
//                // Set Request parameter
//                data +="&" + URLEncoder.encode("data", "UTF-8") + "=20rupee";
//            } catch (UnsupportedEncodingException e) {
//                e.printStackTrace();
//            } 
        }
  
        // Call after onPreExecute method
        protected Void doInBackground(String... urls) {
            // Make Post Call To Web Server
            BufferedReader reader=null;
            // Send data 
            try{ 
               // Defined URL  where to send data
               URL url = new URL(urls[0]);
                  
              // Send POST data request
              URLConnection conn = url.openConnection(); 
              conn.setDoOutput(true); 
              OutputStreamWriter wr = new OutputStreamWriter(conn.getOutputStream()); 
//              wr.write( data ); 
              wr.flush(); 
           
              // Get the server response 
              reader = new BufferedReader(new InputStreamReader(conn.getInputStream()));
              StringBuilder sb = new StringBuilder();
              String line = null;
             
	          // Read Server Response
	          while((line = reader.readLine()) != null) {
	             // Append server response in string
	             sb.append(line);
	          }
	          // Append Server Response To Content String 
              content = sb.toString();
            } catch(Exception ex) {
                error = ex.getMessage();
            } finally {
                try{
                    reader.close();
                } catch(Exception ex) {
                }
            }
            return null;
        }
          
        protected void onPostExecute(Void unused) {
            // NOTE: You can call UI Element here.
            // Close progress dialog
            Dialog.dismiss();
            
            if (error != null) {
            	Log.e("CV Error", "Error on the server call");
            } else {
                openAlert(content);
            }
        }
    }
}
