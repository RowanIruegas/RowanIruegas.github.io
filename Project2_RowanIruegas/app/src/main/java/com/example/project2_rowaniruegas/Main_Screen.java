package com.example.project2_rowaniruegas;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;

import android.view.View;
import android.widget.*;

public class Main_Screen extends AppCompatActivity {

    Button notifications;
    Button sign_out;
    Button add_data;
    Button edit_data;
    Button delete_data;
    Button add_goal;
    TextView display_goal;
    TextView user_info;
    EditText weight;
    EditText goal;
    GridView grid_display;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_screen);

        // Buttons
        notifications = findViewById(R.id.notifications);
        sign_out = findViewById(R.id.sign_out);
        add_data = findViewById(R.id.add_data);
        edit_data = findViewById(R.id.edit_data);
        delete_data = findViewById(R.id.delete_data);
        add_goal = findViewById(R.id.add_goal);

        // TextViews
        display_goal = findViewById(R.id.display_goal);
        user_info = findViewById(R.id.user_info);

        // EditTexts
        weight = findViewById(R.id.weight);
        goal = findViewById(R.id.goal);

        // GridView
        grid_display = findViewById(R.id.grid_display);

        // Initiate Database
        Database database = new Database(this);

        // Display information in GridView
        // FIXME: Pull weights from database and display on the grid

        // Initialize request class
        request_text_access request = new request_text_access();

        // On Click Listener moves to notifications permissions screen
        notifications.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(Main_Screen.this, request_text_access.class));
            }
        });

        // On Click Listener moves back to sign in screen
        sign_out.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(Main_Screen.this, MainActivity.class));
            }
        });

        // On Click Listener adds weight to database & sends notification if added weight
        // is equal to the goal
        add_data.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!weight.getText().toString().isEmpty()) {
                    database.addWeight(weight.getText().toString());
                    if (weight.getText().toString().equals(goal.getText().toString())) {
                        request.makeNotification("CONGRATULATIONS YOU REACHED YOUR GOAL!");
                    }
                }
            }
        });

        // On Click Listener edits weight in database
        edit_data.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!weight.getText().toString().isEmpty()) {
                    // FIXME: edit specified data
                }
            }
        });

        // On Click Listener deletes weight from database
        delete_data.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!weight.getText().toString().isEmpty()) {
                    // FIXME: delete specified data
                }
            }
        });

        // On Click Listener adds goal weight to database
        add_goal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!goal.getText().toString().isEmpty()) {
                    database.addGoal(goal.getText().toString());
                }
            }
        });
    }
}