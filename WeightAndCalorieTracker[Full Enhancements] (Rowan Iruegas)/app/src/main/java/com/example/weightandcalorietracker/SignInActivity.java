/**
 *Weight & Calories Tracking Android Application
 *Author:         Rowan Iruegas
 *Date:           April 20th, 2024
 *Version:        1.0

 *An android application that provides a user with the tools
 *to keep track of weights and calories on a daily basis.
 *
 * SignInActivity: Separate activity for controlling the login
 * screen. Takes in username and password inputs, and sets up
 * the necessary checks for the buttons to add or login to
 * accounts
 */
package com.example.weightandcalorietracker;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class SignInActivity extends AppCompatActivity {

    // Buttons
    Button sign_up, sign_in;
    //EditTexts
    EditText username, password;

    // Initiate Database
    DatabaseController database = new DatabaseController(SignInActivity.this);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sign_in);

        // EditViews
        username = findViewById(R.id.username);
        password = findViewById(R.id.password);

        // Buttons
        sign_up = findViewById(R.id.sign_up);
        sign_in = findViewById(R.id.sign_in);

        // On Click listener checks if EditViews contain one of the username / passwords in the database
        sign_in.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean validate = database.checkLogin(username.getText().toString(), password.getText().toString());
                // Checks if validation is True
                if(validate) {
                    startActivity(new Intent(SignInActivity.this, MainActivity.class));
                } else {
                    Toast.makeText(SignInActivity.this, "Failed to Login", Toast.LENGTH_SHORT).show();
                }
            }
        });

        // On Click Listener adds username / password to the database to create an account
        sign_up.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if username and password inputs are empty
                if ((!username.getText().toString().isEmpty()) && (!password.getText().toString().isEmpty())) {
                    // Checks if an account already exists with the same username
                    if (database.checkUsername(username.getText().toString())) {
                        database.addAccount(username.getText().toString(), password.getText().toString());
                    } else {
                        Toast.makeText(SignInActivity.this, "Please Enter a Unique Username", Toast.LENGTH_SHORT).show();
                    }
                }
            }
        });
    }
}