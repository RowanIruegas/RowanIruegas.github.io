package com.example.project2_rowaniruegas;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;

import android.view.View;
import android.widget.*;

public class MainActivity extends AppCompatActivity {

    EditText user;
    EditText pass;
    Button sign_up;
    Button sign_in;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // EditViews
        user = findViewById(R.id.user);
        pass = findViewById(R.id.pass);

        // Buttons
        sign_up = findViewById(R.id.sign_up);
        sign_in = findViewById(R.id.sign_in);

        // Initiate Database
        Database database = new Database(this);

        // On Click listener checks if EditViews contain one of the username / passwords in the database
        sign_in.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (user.getText().toString().equals("username") && pass.getText().toString().equals("password")) {
                    startActivity(new Intent(MainActivity.this, Main_Screen.class));
                    // FIXME: Set username and password checks from database
                }
            }
        });

        // On Click Listener adds username / password to the database to create an account
        sign_up.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //if ((!user.getText().toString().isEmpty()) && (pass.getText().toString().isEmpty())) {
                    database.addUser(user.getText().toString(), pass.getText().toString());
                //}
            }
        });
    }
}