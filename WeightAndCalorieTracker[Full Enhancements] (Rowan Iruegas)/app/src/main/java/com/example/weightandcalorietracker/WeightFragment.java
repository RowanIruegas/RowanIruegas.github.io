/**
 *Weight & Calories Tracking Android Application
 *Author:         Rowan Iruegas
 *Date:           April 20th, 2024
 *Version:        1.0

 *An android application that provides a user with the tools
 *to keep track of weights and calories on a daily basis.
 *
 * WeightFragment: Contains the UI/UX code for the necessary
 * weight functionalities. This includes graphView initialization,
 * display of data on a line graph, as well as all information inputs
 * as well as buttons for calling the necessary functions.
 */
package com.example.weightandcalorietracker;

import android.os.Build;
import android.os.Bundle;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentTransaction;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link WeightFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class WeightFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public WeightFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment WeightFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static WeightFragment newInstance(String param1, String param2) {
        WeightFragment fragment = new WeightFragment();
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
        View view  = inflater.inflate(R.layout.fragment_weight, container, false);

        // Initialize and find notifications button
        Button add_weight = (Button) view.findViewById(R.id.add_weight);
        Button delete_weight = (Button) view.findViewById(R.id.delete_weight);
        Button update_weight = (Button) view.findViewById(R.id.update_weight);
        Button add_goal = (Button) view.findViewById(R.id.add_goal);

        // Initialize and find text inputs
        EditText day_input = (EditText) view.findViewById(R.id.day_input);
        EditText weight_input = (EditText) view.findViewById(R.id.weight_input);
        EditText day_change = (EditText) view.findViewById(R.id.day_change);
        EditText new_weight = (EditText) view.findViewById(R.id.new_weight);
        EditText goal_input = (EditText) view.findViewById(R.id.goal_input);

        // Initialize and find text outputs
        TextView goal_display = (TextView) view.findViewById(R.id.goal_display);

        // Initialize database controller
        DatabaseController database = new DatabaseController(getActivity().getApplicationContext());

        // Initialize and set up GraphView
        GraphView weightLineGraph = (GraphView) view.findViewById(R.id.weightLineGraph);

        // Fills Datapoint's with user weight data
        DataPoint[] data = database.readWeight();
        // Initialize LineGraphSeries with a starter data point
        LineGraphSeries<DataPoint> series = new LineGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(0, 0)
        });

        // Checks if DataPoint array contains information from database
        if (data.length != 0) {
            // Appends data to the series
            for (int i = 0; i < data.length; i++) {
                series.appendData(data[i], true, 99);
            }

            // Displays information from graph starting from an X of 1
            weightLineGraph.addSeries(series);
            weightLineGraph.getViewport().setXAxisBoundsManual(true);
            weightLineGraph.getViewport().setMinX(1.0);
            weightLineGraph.getViewport().setMaxX(data.length);
        }

        // Checks if a goal has been added, then changes placeholder display
        goal_display.setText(database.readGoal() + "Lbs");

        // Add Button
        add_weight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if day and weight inputs are empty
                if(!day_input.getText().toString().isEmpty() && !weight_input.getText().toString().isEmpty()) {
                    database.addWeight(day_input.getText().toString(), weight_input.getText().toString());
                    // Checks if added weight has reached the goal, and sends a message if permissions are active
                    if (weight_input.getText().toString().equals(database.readGoal())){
                        database.makeNotification("CONGRATS YOU REACHED YOUR GOAL!",
                                database.readPhone());
                    }
                }
                else {
                    Toast.makeText(getActivity().getApplicationContext(), "Failed to add weight", Toast.LENGTH_SHORT).show();
                }
            }
        });

        // Delete Button
        delete_weight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if day and weight inputs are empty
                if(!day_input.getText().toString().isEmpty() && !weight_input.getText().toString().isEmpty()) {
                    database.deleteWeight(day_input.getText().toString(), weight_input.getText().toString());
                }
                else {
                    Toast.makeText(getActivity().getApplicationContext(), "Failed to delete weight", Toast.LENGTH_SHORT).show();
                }
            }
        });

        // Update Button
        update_weight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if day and weight inputs are empty
                if(!day_change.getText().toString().isEmpty() && !new_weight.getText().toString().isEmpty()) {
                    database.updateWeight(day_change.getText().toString(), new_weight.getText().toString());
                }
                else {
                    Toast.makeText(getActivity().getApplicationContext(), "Failed to update weight", Toast.LENGTH_SHORT).show();
                }
            }
        });

        // Goal Button
        add_goal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Checks if goal input is empty
                if(!goal_input.getText().toString().isEmpty()) {
                    database.addGoal(goal_input.getText().toString());
                }
                else {
                    Toast.makeText(getActivity().getApplicationContext(), "Failed to add goal", Toast.LENGTH_SHORT).show();
                }
            }
        });

        return view;
    }
}