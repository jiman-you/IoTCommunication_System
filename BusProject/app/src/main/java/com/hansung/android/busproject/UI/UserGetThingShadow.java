package com.hansung.android.busproject.UI;


import android.app.Activity;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.hansung.android.busproject.R;
import com.hansung.android.busproject.httpconnection.GetRequest;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

public class UserGetThingShadow extends GetRequest {
    final static String TAG = "AndroidAPITest";
    String urlStr;

    public UserGetThingShadow(Activity activity, String urlStr) {
        super(activity);
        this.urlStr = urlStr;
    }

    @Override
    protected void onPreExecute() {
        try {
            Log.e(TAG, urlStr);
            url = new URL(urlStr);

        } catch (MalformedURLException e) {
            e.printStackTrace();
            Toast.makeText(activity,"URL is invalid:"+urlStr, Toast.LENGTH_SHORT).show();
            activity.finish();
        }
    }

    @Override
    protected void onPostExecute(String jsonString) {
        if (jsonString == null)
            return;
        Map<String, String> state = getStateFromJSONString(jsonString);
        TextView countstate = activity.findViewById(R.id.countstate);
        countstate.setText(state.get("countstate"));

    }

    protected Map<String, String> getStateFromJSONString(String jsonString) {
        Map<String, String> output = new HashMap<>();
        try {
            // 처음 double-quote와 마지막 double-quote 제거
            jsonString = jsonString.substring(1,jsonString.length()-1);
            // \\\" 를 \"로 치환
            jsonString = jsonString.replace("\\\"","\"");
            Log.i(TAG, "jsonString="+jsonString);
            JSONObject root = new JSONObject(jsonString);
            JSONObject state = root.getJSONObject("state");
            JSONObject reported = state.getJSONObject("reported");
            String countvalue = reported.getString("SR_COUNT");
            output.put("countstate", countvalue);

            JSONObject desired = state.getJSONObject("desired");
            String desired_count = desired.getString("countstate");
            output.put("desired_servo", desired_count);


        } catch (JSONException e) {
            Log.e(TAG, "Exception in processing JSONString.", e);
            e.printStackTrace();
        }
        return output;
    }
}
