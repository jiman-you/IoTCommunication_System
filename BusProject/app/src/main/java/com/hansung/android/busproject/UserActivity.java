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
        btnAdd = findViewById(R.id.callbtn);

        btnAdd.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {


                if(count == 10 ){
                    count = 0;
                    Toast toast = Toast.makeText(getApplicationContext(), "버스증편 요청이 완료되었습니다.",Toast.LENGTH_SHORT);
                    toast.show();
                }
                count++;
                tvCount.setText(count+"");
                intent.putExtra("countstate",count);
            }
        });

//        urlStr = "https://hj0w5goir5.execute-api.us-east-1.amazonaws.com/prod/devices/Project_ESP8286"
//        timer = new Timer();
//        timer.schedule(new TimerTask() {
//                           @Override
//                           public void run() {
//                               new UserGetThingShadow(UserActivity.this, urlStr).execute();
//                           }
//                       },
//                0, 2000);
//
//        tv_count = findViewById(R.id.countstate);

//        Button updateBtn1 = findViewById(R.id.callbtn);
//        updateBtn1.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//
//
//                JSONObject payload = new JSONObject();
//
//                try {
//                    JSONArray jsonArray = new JSONArray();
//                    String servo_input = "ON";
//                    if (servo_input != null && !servo_input.equals("")) {
//                        JSONObject tag1 = new JSONObject();
//                        tag1.put("tagName", "SERVO");
//                        tag1.put("tagValue", servo_input);
//
//                        jsonArray.put(tag1);
//                    }
//
//                    if (jsonArray.length() > 0)
//                        payload.put("tags", jsonArray);
//                } catch (JSONException e) {
//                    Log.e(TAG, "JSONEXception");
//                }
//                Log.i(TAG, "payload=" + payload);
//                if (payload.length() > 0)
//                    new UpdateShadow(UserActivity.this, urlStr).execute(payload);
//            }
//        });


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