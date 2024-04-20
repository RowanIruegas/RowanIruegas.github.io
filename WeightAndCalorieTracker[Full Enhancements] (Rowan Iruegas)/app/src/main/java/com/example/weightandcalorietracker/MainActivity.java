/**
 *Weight & Calories Tracking Android Application
 *Author:         Rowan Iruegas
 *Date:           April 20th, 2024
 *Version:        1.0

 *An android application that provides a user with the tools
 *to keep track of weights and calories on a daily basis.
 *
 * MainActivity: Contains the code for the navigation bar
 * activity that controls fragment navigation.
 */
package com.example.weightandcalorietracker;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

import android.os.Bundle;

import com.example.weightandcalorietracker.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        changeFragment(new WeightFragment());

        binding.bottomNavigationView4.setOnItemSelectedListener(item -> {

            int itemId = item.getItemId();

            if (itemId == R.id.weight) {
                changeFragment(new WeightFragment());
            }
            else if (itemId == R.id.calorie) {
                changeFragment(new CalorieFragment());
            }
            else if (itemId == R.id.notification) {
                changeFragment(new NotificationFragment());
            }
            else if (itemId == R.id.exit) {
                changeFragment(new ExitFragment());
            }

            return true;

        });
    }

    private void changeFragment(Fragment fragment){
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.replace(R.id.frame_layout, fragment);
        fragmentTransaction.commit();
    }
}