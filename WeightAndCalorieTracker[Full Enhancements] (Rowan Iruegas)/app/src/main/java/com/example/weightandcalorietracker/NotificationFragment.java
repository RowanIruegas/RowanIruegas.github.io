/**
 *Weight & Calories Tracking Android Application
 *Author:         Rowan Iruegas
 *Date:           April 20th, 2024
 *Version:        1.0

 *An android application that provides a user with the tools
 *to keep track of weights and calories on a daily basis.
 *
 * NotificationFragment: Contains the UI/UX code for notification
 * functionality including an editText for phone number input
 * and a button to request notification permission.
 */
package com.example.weightandcalorietracker;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.provider.ContactsContract;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;
import android.telephony.SmsManager;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link NotificationFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class NotificationFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";
    private static final int RC_NOTIFICATIONS = 1;

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public NotificationFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment NotificationFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static NotificationFragment newInstance(String param1, String param2) {
        NotificationFragment fragment = new NotificationFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        // Inflate the layout for this fragment
        View view  = inflater.inflate(R.layout.fragment_notification, container, false);

        // Initialize and find notifications button
        Button request_notifications = (Button) view.findViewById(R.id.request_notifications);

        // Initialize and find phone number text input
        EditText phone_input = (EditText) view.findViewById(R.id.phone_input);

        // Initialize database controller
        DatabaseController database = new DatabaseController(getActivity().getApplicationContext());

        // Requests notification permissions from the user and calls the makeNotifications function
        request_notifications.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if phone input is empty
                if (!phone_input.getText().toString().isEmpty()) {
                    // Checks build version
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        // Checks if SMS permissions are granted
                        if (getActivity().checkSelfPermission(android.Manifest.permission.SEND_SMS) == PackageManager.PERMISSION_GRANTED) {
                            database.makeNotification("Thanks for signing up for texts!",
                                    phone_input.getText().toString());
                        } else {
                            requestPermissions(new String[]{android.Manifest.permission.SEND_SMS}, 1);
                        }
                    }
                    // Adds phone number to database
                    database.addPhone(phone_input.getText().toString());
                }
            }
        });

        return view;
    }
}