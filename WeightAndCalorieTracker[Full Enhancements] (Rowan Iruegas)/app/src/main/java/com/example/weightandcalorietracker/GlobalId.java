/**
 *Weight & Calories Tracking Android Application
 *Author:         Rowan Iruegas
 *Date:           April 20th, 2024
 *Version:        1.0

 *An android application that provides a user with the tools
 *to keep track of weights and calories on a daily basis.
 *
 * GlobalID: Contains the code for the global variable
 * that is used to store and check which user account
 * is being accessed.
 */
package com.example.weightandcalorietracker;

public class GlobalId {

    public static String globalId;

    public GlobalId() {
    }

    // Getters and Setters for GlobalID
    public static String getGlobalId() {
        return globalId;
    }

    public static void setGlobalId(String globalId) {
        GlobalId.globalId = globalId;
    }
}
