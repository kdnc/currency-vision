package com.currencyvision;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.view.Menu;
import java.util.Locale;

import android.app.Activity;
import android.content.DialogInterface;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class AndroidTextToSpeechActivity extends Activity implements TextToSpeech.OnInitListener{

	private TextToSpeech tts;
	String value = "";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_android_text_to_speech);
		
		Bundle extras = getIntent().getExtras();
		if (extras != null) {
			value = extras.getString("new_variable_name");
		}
		
		openAlert(value);
		
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
                speakOut(value);
            }
 
        } else {
            Log.e("TTS", "Initilization Failed!");
        }
 
    }
 
    private void speakOut(String text) {
 
        tts.speak(text, TextToSpeech.QUEUE_FLUSH, null);
    }
    
    private void openAlert(String result) {
		 AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(AndroidTextToSpeechActivity.this);
	     
		 alertDialogBuilder.setTitle(this.getTitle()+ " decision");
		 alertDialogBuilder.setMessage(result);
		 // set positive button: Yes message
		 alertDialogBuilder.setPositiveButton("Yes",new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog,int id) {
					// go to a new activity of the app
					AndroidTextToSpeechActivity.this.finish();
					dialog.cancel();
				}
			  });
		 // set negative button: No message
//		 alertDialogBuilder.setNegativeButton("No",new DialogInterface.OnClickListener() {
//				public void onClick(DialogInterface dialog,int id) {
//					// cancel the alert box and put a Toast to the user
//					dialog.cancel();
//					Toast.makeText(getApplicationContext(), "You chose a negative answer", 
//							Toast.LENGTH_LONG).show();
//				}
//			});
		 // set neutral button: Exit the app message
//		 alertDialogBuilder.setNeutralButton("Exit the app",new DialogInterface.OnClickListener() {
//				public void onClick(DialogInterface dialog,int id) {
//					// exit the app and go to the HOME
//					CameraDemoActivity.this.finish();
//				}
//			});
		 
		 AlertDialog alertDialog = alertDialogBuilder.create();
		 // show alert
		 alertDialog.show();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.android_text_to_speech, menu);
		return true;
	}

}
