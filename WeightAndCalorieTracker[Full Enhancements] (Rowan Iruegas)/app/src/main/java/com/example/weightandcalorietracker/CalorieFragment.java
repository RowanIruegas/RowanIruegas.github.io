/**
 *Weight & Calories Tracking Android Application
 *Author:         Rowan Iruegas
 *Date:           April 20th, 2024
 *Version:        1.0

 *An android application that provides a user with the tools
 *to keep track of weights and calories on a daily basis.
 *
 *CalorieFragment: Contains the UI/UX code for the Calorie
 * screen. This includes all buttons, editTexts, as well
 * as a ListView.
 */
package com.example.weightandcalorietracker;

import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link CalorieFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class CalorieFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public CalorieFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment CalorieFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static CalorieFragment newInstance(String param1, String param2) {
        CalorieFragment fragment = new CalorieFragment();
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
        View view  = inflater.inflate(R.layout.fragment_calorie, container, false);

        // Initialize and find notifications button
        Button add_calorie = (Button) view.findViewById(R.id.add_calorie);
        Button delete_calorie = (Button) view.findViewById(R.id.delete_calorie);
        Button update_calorie = (Button) view.findViewById(R.id.update_calorie);

        // Initialize and find text inputs
        EditText meal_input = (EditText) view.findViewById(R.id.meal_input);
        EditText calorie_input = (EditText) view.findViewById(R.id.calorie_input);
        EditText meal_change = (EditText) view.findViewById(R.id.meal_change);
        EditText new_calorie = (EditText) view.findViewById(R.id.new_calorie);

        // Initialize database controller
        DatabaseController database = new DatabaseController(getActivity().getApplicationContext());

        // Initialize ListView and fill with data from database if it exists
        ArrayList<String> calorieData = new ArrayList<String>();
        calorieData = database.readCalorie();
        ListView calorieList = (ListView) view.findViewById(R.id.listView);
        ArrayAdapter adapter = new ArrayAdapter(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, calorieData);
        calorieList.setAdapter(adapter);

        // Add Button
        add_calorie.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if meal and calorie inputs are empty
                if(!meal_input.getText().toString().isEmpty() && !calorie_input.getText().toString().isEmpty()) {
                    database.addCalorie(meal_input.getText().toString(), calorie_input.getText().toString());
                }
                else {
                    Toast.makeText(getActivity().getApplicationContext(), "Failed to add calories", Toast.LENGTH_SHORT).show();
                }
            }
        });

        // Delete Button
        delete_calorie.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if meal and calorie inputs are empty
                if(!meal_input.getText().toString().isEmpty() && !calorie_input.getText().toString().isEmpty()) {
                    database.deleteCalorie(meal_input.getText().toString(), calorie_input.getText().toString());
                }
                else {
                    Toast.makeText(getActivity().getApplicationContext(), "Failed to delete calories", Toast.LENGTH_SHORT).show();
                }
            }
        });

        // Update Button
        update_calorie.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if meal and calorie inputs are empty
                if(!meal_change.getText().toString().isEmpty() && !new_calorie.getText().toString().isEmpty()) {
                    database.updateCalorie(meal_change.getText().toString(), new_calorie.getText().toString());
                }
                else {
                    Toast.makeText(getActivity().getApplicationContext(), "Failed to update calories", Toast.LENGTH_SHORT).show();
                }
            }
        });

        return view;
    }
}