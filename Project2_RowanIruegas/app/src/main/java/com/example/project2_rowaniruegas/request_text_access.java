package com.example.project2_rowaniruegas;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;

import android.telephony.SmsManager;
import android.view.View;
import android.widget.*;

public class request_text_access extends AppCompatActivity {

    private static final int RC_NOTIFICATIONS = 1;
    Button go_back;
    Button enable_notif;
    EditText phone;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_request_text_access);

        // Buttons
        go_back = findViewById(R.id.go_back);
        enable_notif = findViewById(R.id.enable_notif);

        // EditText
        phone = findViewById(R.id.phone);

        // Button to send the user back to the main screen
        go_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(request_text_access.this, Main_Screen.class));
            }
        });

        // Button to request message permissions
        enable_notif.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!phone.getText().toString().isEmpty()) {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        if (checkSelfPermission(android.Manifest.permission.SEND_SMS) == PackageManager.PERMISSION_GRANTED) {
                            makeNotification("Thanks for signing up for texts!");
                        } else {
                            requestPermissions(new String[]{android.Manifest.permission.SEND_SMS}, 1);
                        }
                    }
                }
            }
        });
    }

    // Creates the notification and sends it when called
    public void makeNotification(String message) {
        String number = phone.getText().toString().trim();

        try {
            SmsManager smsManager = SmsManager.getDefault();
            smsManager.sendTextMessage(number, null, message, null, null);
            Toast.makeText(this, "Goal Reached!", Toast.LENGTH_SHORT).show();
        } catch (Exception e) {
            e.printStackTrace();
            Toast.makeText(this, "Failed to Send", Toast.LENGTH_SHORT).show();
        }
    }
}