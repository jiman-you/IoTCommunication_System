package com.hansung.android.busproject;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.hansung.android.busproject.UI.UserGetThingShadow;
import com.hansung.android.busproject.UI.UpdateShadow;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Timer;
import java.util.TimerTask;

public class UserActivity extends AppCompatActivity {

    private TextView tv_count,tv_call,tv_boo;
    String urlStr;
    Timer timer;
    final static String TAG = "AndroidAPITest";
    private Button btnAdd;
    private TextView tvCount;
    private int count = 0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user);

        Intent intent = getIntent();
        tvCount = findViewById(R.id.countstate);
        count = intent.getIntExtra("countstate",0);
        tvCount.setText(count+"");



        urlStr = "https://hj0w5goir5.execute-api.us-east-1.amazonaws.com/prod/devices/Project_ESP8286"
        timer = new Timer();
        timer.schedule(new TimerTask() {
                           @Override
                           public void run() {
                               new UserGetThingShadow(UserActivity.this, urlStr).execute();
                           }
                       },
                0, 2000);

        tv_count = findViewById(R.id.countstate);

        Button updateBtn1 = findViewById(R.id.callbtn);
        updateBtn1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {


                 count++;
                 tvCount.setText(count+"");
                 intent.putExtra("countstate",count);

                JSONObject payload = new JSONObject();

                try {
                    JSONArray jsonArray = new JSONArray();
                    String SR_COUNT_input = "10";
                    if (SR_COUNT_input != null && !SR_COUNT_input.equals(count)) {
                        JSONObject tag1 = new JSONObject();
                        tag1.put("tagName", "SR_COUNT");
                        count = 0;
                        tvCount.setText(count+"");
                        Toast toast = Toast.makeText(getApplicationContext(), "???????????? ????????? ?????????????????????.",Toast.LENGTH_SHORT);
                        toast.show();
                    }

                        jsonArray.put(tag1);
                    }

                    if (jsonArray.length() > 0)
                        payload.put("tags", jsonArray);
                } catch (JSONException e) {
                    Log.e(TAG, "JSONEXception");
                }
                Log.i(TAG, "payload=" + payload);
                if (payload.length() > 0)
                    new UpdateShadow(UserActivity.this, urlStr).execute(payload);
            }
        });


        Button Control = findViewById(R.id.alarm);
        Control.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(), ControlActivity.class);
                intent.putExtra("countstate",count);
                startActivity(intent);
                finish();

            }
        });
    }

}