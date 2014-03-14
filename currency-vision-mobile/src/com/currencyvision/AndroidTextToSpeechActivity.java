package com.currencyvision;

import java.util.Locale;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.Menu;
import android.widget.TextView;

import com.currencyvision.model.ModelLocator;

public class AndroidTextToSpeechActivity extends Activity implements TextToSpeech.OnInitListener{

	private TextToSpeech tts;
	String value = "";
	ModelLocator model = ModelLocator.getInstance();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_android_text_to_speech);
		
		Bundle extras = getIntent().getExtras();
		if (extras != null) {
			value = extras.getString("new_variable_name");
		}
		
		model.getCurrencyCount().setCount(Integer.parseInt(value));
		
		openAlert(model.getCurrencyCount().getCount());
		
		tts = new TextToSpeech(this, this);
		 
//		speakOut(value);
	}
	
	@Override
    public void onDestroy() {
        // Don't forget to shutdown tts!
        if (tts != null) {
            tts.stop();
            tts.shutdown();
        }
        super.onDestroy();
    }
	
	@Override
    public void onInit(int status) {
 
        if (status == TextToSpeech.SUCCESS) {
 
            int result = tts.setLanguage(Locale.US);
 
            if (result == TextToSpeech.LANG_MISSING_DATA
                    || result == TextToSpeech.LANG_NOT_SUPPORTED) {
                Log.e("TTS", "This Language is not supported");
            } else {
        		if(model.getTaskMode() == ModelLocator.RECOGNIZE_MODE){
        			speakOut("This is a " + value + " rupee note.");
        		} else if(model.getTaskMode() == ModelLocator.COUNT_MODE){
        			speakOut("Total currency value is " + model.getCurrencyCount().getCount() + " rupees.");
        		}
            }
 
        } else {
            Log.e("TTS", "Initilization Failed!");
        }
 
    }
 
    private void speakOut(String text) {
 
        tts.speak(text, TextToSpeech.QUEUE_FLUSH, null);
    }
    
    private void openAlert(int result) {
		if(model.getTaskMode() == ModelLocator.RECOGNIZE_MODE){
			AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(AndroidTextToSpeechActivity.this);
		     
//			 alertDialogBuilder.setTitle(this.getTitle()+ " decision");
			 alertDialogBuilder.setMessage(Integer.toString(result));
			 // set positive button: Yes message
			 alertDialogBuilder.setPositiveButton("OK",new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog,int id) {
						// go to a new activity of the app
						AndroidTextToSpeechActivity.this.finish();
						dialog.cancel();
					}
				  });
			 
			 AlertDialog alertDialog = alertDialogBuilder.create();
			 // show alert
			 alertDialog.show();
			 TextView txtMsg = (TextView) alertDialog.findViewById(android.R.id.message);
			 txtMsg.setTextSize(TypedValue.COMPLEX_UNIT_SP, 80);
			 txtMsg.setGravity(Gravity.CENTER);
		} else if(model.getTaskMode() == ModelLocator.COUNT_MODE){
			AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(AndroidTextToSpeechActivity.this);
		     
//			 alertDialogBuilder.setTitle(this.getTitle()+ " decision");
			 alertDialogBuilder.setMessage(Integer.toString(result));
			 // set positive button: Next message
			 alertDialogBuilder.setPositiveButton("Count",new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog,int id) {
						AndroidTextToSpeechActivity.this.finish();
						Intent intent = new Intent(AndroidTextToSpeechActivity.this, CameraDemoActivity.class);
						startActivity(intent);
						dialog.cancel();
					}
				  });
			 // set negative button: Stop message
			 alertDialogBuilder.setNegativeButton("Stop",new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog,int id) {
						AndroidTextToSpeechActivity.this.finish();
						dialog.cancel();
					}
				});
			 
			 AlertDialog alertDialog = alertDialogBuilder.create();
			 // show alert
			 alertDialog.show();
			 TextView txtMsg = (TextView) alertDialog.findViewById(android.R.id.message);
			 txtMsg.setTextSize(TypedValue.COMPLEX_UNIT_SP, 80);
			 txtMsg.setGravity(Gravity.CENTER);
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.android_text_to_speech, menu);
		return true;
	}

}
