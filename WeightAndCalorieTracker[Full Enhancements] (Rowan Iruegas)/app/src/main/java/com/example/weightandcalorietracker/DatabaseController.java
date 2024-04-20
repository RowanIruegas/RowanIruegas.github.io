/**
 *Weight & Calories Tracking Android Application
 *Author:         Rowan Iruegas
 *Date:           April 20th, 2024
 *Version:        1.0

 *An android application that provides a user with the tools
 *to keep track of weights and calories on a daily basis.
 *
 * DatabaseController: Controller for all of the database code.
 * This includes the initialization of all necessary tables
 * as well as containing any query functions tied to application
 * functionality.
 */
package com.example.weightandcalorietracker;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.telephony.SmsManager;
import android.widget.Toast;

import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.Objects;

import com.jjoe64.graphview.series.DataPoint;

public class DatabaseController extends SQLiteOpenHelper {

    private static final int DATABASE_VERSION = 1;
    private static final String DATABASE_NAME = "users";
    private Context context;

    // Account table variables
    private static final String TABLE_ACCOUNT = "account";
    private static final String USERID = "userID";
    private static final String USERNAME = "username";
    private static final String PASSWORD = "password";

    // Phone table variables
    private static final String TABLE_PHONE = "phone";
    private static final String P_USERID = "pUserID";
    private static final String PHONE = "phone";

    // Goal table variables
    private static final String TABLE_GOAL = "goal";
    private static final String G_USERID = "gUserID";
    private static final String GOAL = "goal";

    // Weight table variables
    private static final String TABLE_WEIGHT = "weight";
    private static final String W_USERID = "wUserID";
    private static final String DAY = "day";
    private static final String WEIGHTS = "weights";

    // Calories table variables
    private static final String TABLE_CALORIE = "calorie";
    private static final String C_USERID = "wUserID";
    private static final String MEAL = "meal";
    private static final String CALORIES = "calories";

    // Initiate Global Variable
    GlobalId id = new GlobalId();

    public DatabaseController(@Nullable Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
        this.context = context;
    }

    @Override
    public void onCreate(SQLiteDatabase db) {

        // Creates Account database
        String sqlAccount = "CREATE TABLE " + TABLE_ACCOUNT + "(" + USERID + " INTEGER PRIMARY KEY AUTOINCREMENT, "
                + USERNAME + " TEXT, "
                + PASSWORD + " TEXT " + ")";

        // Creates Phone database
        String sqlPhone = "CREATE TABLE " + TABLE_PHONE + "(" + PHONE + " TEXT, "
                + P_USERID + " INTEGER, "
                + " Foreign Key("+P_USERID+") references "+TABLE_ACCOUNT+"("+USERID+") " + ")";

        // Creates Goal database
        String sqlGoal = "CREATE TABLE " + TABLE_GOAL + "(" + GOAL + " TEXT, "
                + G_USERID + " INTEGER, "
                + " Foreign Key("+G_USERID+") references "+TABLE_ACCOUNT+"("+USERID+") " + ")";

        // Creates Weight database
        String sqlWeight = "CREATE TABLE " + TABLE_WEIGHT + "(" + DAY + " TEXT, "
                + WEIGHTS + " TEXT, "
                + W_USERID + " INTEGER, "
                + " Foreign Key("+W_USERID+") references "+TABLE_ACCOUNT+"("+USERID+") " + ")";

        // Creates Calorie database
        String sqlCalorie = "CREATE TABLE " + TABLE_CALORIE + "(" + MEAL + " TEXT, "
                + CALORIES + " TEXT, "
                + C_USERID + " INTEGER, "
                + " Foreign Key("+C_USERID+") references "+TABLE_ACCOUNT+"("+USERID+") " + ")";

        // Executes SQL Databases
        db.execSQL(sqlAccount);
        db.execSQL(sqlPhone);
        db.execSQL(sqlGoal);
        db.execSQL(sqlWeight);
        db.execSQL(sqlCalorie);

    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

        // Drop existing older tables
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_ACCOUNT);
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_PHONE);
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_GOAL);
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_WEIGHT);
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_CALORIE);

        // Create new tables
        onCreate(db);

    }

    /**
     * Checks the editView user input to validate
     * that the user account exists within the database
     * alongside the correct password.
     * NOTE: Assigns a globalId with the accounts primary key.
     *
     * @param user username string.
     * @param pass password string.
     */
    public boolean checkLogin(String user, String pass) {
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor c = db.rawQuery("SELECT * FROM " + TABLE_ACCOUNT + " WHERE " + USERNAME + "='" + user
                                + "' AND " + PASSWORD + "='" + pass + "'", null);

        if(c.getCount() == 0){
            c.close();
            db.close();
            return false;
        } else {
            if(c.moveToFirst()) {
                id.setGlobalId(c.getString(0));
            }
            c.close();
            db.close();
            return true;
        }
    }

    /**
     * Checks if the user's input username already exists
     *
     * @param username username string.
     */
    public boolean checkUsername(String username) {
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor c = db.rawQuery("SELECT * FROM " + TABLE_ACCOUNT + " WHERE " + USERNAME + "='"
                + username + "'", null);
        if(c.getCount() == 0) {
            c.close();
            db.close();
            return true;
        } else {
            c.close();
            db.close();
            return false;
        }
    }

    //==================================================ACCOUNT ADD===================================================
    /**
     * Inserts a new row to the account table containing
     * the primary key, username, and password information.
     *
     * @param username username string.
     * @param password password string.
     */
    void addAccount(String username, String password) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv  = new ContentValues();

        cv.put(USERNAME, username);
        cv.put(PASSWORD, password);

        long result = db.insert(TABLE_ACCOUNT, null, cv);
        if(result == -1){
            Toast.makeText(context, "Failed to Insert", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(context, "Account Insert Successful", Toast.LENGTH_SHORT).show();
        }
    }

    //===================================================PHONE ADD====================================================
    /**
     * Inserts a user phone number and foreign key matching
     * the currently logged in account into the phone
     * table.
     *
     * @param phone phone string.
     */
    void addPhone(String phone) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv  = new ContentValues();
        Cursor c = db.rawQuery("SELECT * FROM " + TABLE_PHONE + " WHERE " + P_USERID + "='"
                + id.getGlobalId() + "'", null);
        if (c.getCount() == 0) {
            cv.put(PHONE, phone);
            cv.put(P_USERID, id.getGlobalId());

            long result = db.insert(TABLE_PHONE, null, cv);
            if (result == -1) {
                Toast.makeText(context, "Failed to Insert", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(context, "Phone Number Insert Successful", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(context, "A Phone Number Already Exists", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Reads the phone information into a String from the PHONE Table
     *
     * @return String Returns the phone number if one exists within the database
     */
    String readPhone() {
        String phone = null;
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor c = db.rawQuery("SELECT * FROM " + TABLE_PHONE + " WHERE " + P_USERID + "='"
                + id.getGlobalId() + "'", null);

        if(c.moveToFirst()) {
            phone = c.getString(0);
        }

        return phone;
    }

    //===================================================GOAL ADD=====================================================
    /**
     * Inserts a user goal and foreign key matching
     * the currently logged in account into the goal
     * table.
     *
     * @param goal goal string.
     */
    void addGoal(String goal) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv  = new ContentValues();
        Cursor c = db.rawQuery("SELECT * FROM " + TABLE_GOAL + " WHERE " + G_USERID + "='"
                + id.getGlobalId() + "'", null);
        if (c.getCount() == 0) {
            cv.put(GOAL, goal);
            cv.put(G_USERID, id.getGlobalId());

            long result = db.insert(TABLE_GOAL, null, cv);
            if (result == -1) {
                Toast.makeText(context, "Failed to Insert", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(context, "Goal Insert Successful", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(context, "A goal already exists", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Reads the gaol information into a string from the GOAL table
     *
     * @return String Returns the goal from the database or a placeholder if
     * one does not already exist.
     */
    String readGoal() {
        String goal = null;
        String placeholder = "000";
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor c = db.rawQuery("SELECT * FROM " + TABLE_GOAL + " WHERE " + G_USERID + "='"
                + id.getGlobalId() + "'", null);

        if(c.getCount() == 0){
            c.close();
            return placeholder;
        } else {
            if(c.moveToFirst()) {
                goal = c.getString(0);
            }
            c.close();
            return goal;
        }
    }

    //==================================================WEIGHT CRUD===================================================

    /**
     * Inserts day / weight and foreign key matching
     * the currently logged in account into the goal
     * table.
     *
     * @param day day string.
     * @param weight weight string.
     */
    void addWeight(String day, String weight) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv  = new ContentValues();

        cv.put(DAY, day);
        cv.put(WEIGHTS, weight);
        cv.put(W_USERID, id.getGlobalId());

        long result = db.insert(TABLE_WEIGHT, null, cv);
        if(result == -1){
            Toast.makeText(context, "Failed to Insert", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(context, "Weight Insert Successful", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * deletes day / weight and foreign key matching
     * the currently logged in account into the goal
     * table.
     *
     * @param day day string.
     * @param weight weight string.
     */
    void deleteWeight(String day, String weight){
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor c = db.rawQuery("SELECT MAX(DAY) FROM " + TABLE_WEIGHT + " WHERE " + W_USERID + "='"
                + id.getGlobalId() + "'", null);

        if(c.moveToFirst()) {
            if (Objects.equals(c.getString(0), day)) {
                long result = db.delete(TABLE_WEIGHT, DAY + " = " + day + " AND "
                        + WEIGHTS + " = " + weight + " AND "
                        + W_USERID + " = " + id.getGlobalId(), null);
                if (result == -1) {
                    Toast.makeText(context, "Failed to Delete", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(context, "Weight Delete Successful", Toast.LENGTH_SHORT).show();
                }
            } else {
                Toast.makeText(context, "You can only delete the most recent entry", Toast.LENGTH_SHORT).show();
            }
        }
    }

    /**
     * updates day / weight and foreign key matching
     * the currently logged in account into the goal
     * table.
     *
     * @param day day string.
     * @param newWeight weight string.
     */
    void updateWeight(String day, String newWeight){
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv  = new ContentValues();

        cv.put(WEIGHTS, newWeight);

        long result = db.update(TABLE_WEIGHT, cv, DAY + " = " + day + " AND "
                + W_USERID + " = " + id.getGlobalId(), null);
        if(result == -1){
            Toast.makeText(context, "Failed to Update", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(context, "Weight Update Successful", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Reads the day and weight information into
     * the DataPoint array to display on the Line Graph
     *
     * @return data return DataPoint array
     */
    DataPoint[] readWeight() {
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor c = db.rawQuery("SELECT * FROM " + TABLE_WEIGHT + " WHERE " + W_USERID + "='"
                + id.getGlobalId() + "'", null);

        DataPoint[] data = new DataPoint[c.getCount()];
        int i = 0;

        while (c.moveToNext()) {
            DataPoint temp = new DataPoint(Integer.parseInt(c.getString(0)),
                    Integer.parseInt(c.getString(1)));
            data[i] = temp;
            i++;
        }
        c.close();
        return data;
    }

    //==================================================CALORIE CRUD==================================================

    /**
     * Inserts meal / calories and foreign key matching
     * the currently logged in account into the goal
     * table.
     *
     * @param meal meal string.
     * @param calories calorie string.
     */
    void addCalorie(String meal, String calories) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv  = new ContentValues();

        cv.put(MEAL, meal);
        cv.put(CALORIES, calories);
        cv.put(C_USERID, id.getGlobalId());

        long result = db.insert(TABLE_CALORIE, null, cv);
        if(result == -1){
            Toast.makeText(context, "Failed to Insert", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(context, "Calorie Insert Successful", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * deletes meal / calories and foreign key matching
     * the currently logged in account into the goal
     * table.
     *
     * @param meal meal string.
     * @param calories calorie string.
     */
    void deleteCalorie(String meal, String calories){
        SQLiteDatabase db = this.getWritableDatabase();

        long result = db.delete(TABLE_CALORIE, MEAL + " = " + meal + " AND "
                + CALORIES + " = " + calories + " AND "
                + C_USERID + " = " +  id.getGlobalId(), null);
        if(result == -1){
            Toast.makeText(context, "Failed to Delete", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(context, "Calorie Delete Successful", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * updates meal / calories and foreign key matching
     * the currently logged in account into the goal
     * table.
     *
     * @param meal meal string.
     * @param newCalories new calories string.
     */
    void updateCalorie(String meal, String newCalories){
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv  = new ContentValues();

        cv.put(CALORIES, newCalories);

        long result = db.update(TABLE_CALORIE, cv, MEAL + " = " + meal + " AND "
                + C_USERID + " = " + id.getGlobalId(), null);
        if(result == -1){
            Toast.makeText(context, "Failed to Update", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(context, "Calorie Update Successful", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Reads the meal and calorie information into
     * the DataPoint array to display on the Line Graph
     *
     * @return ArrayList Returns a list of strings
     */
    ArrayList<String> readCalorie() {
        ArrayList<String> list = new ArrayList<String>();
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor c = db.rawQuery("SELECT * FROM " + TABLE_CALORIE + " WHERE " + C_USERID + "='"
                + id.getGlobalId() + "'", null);
        String stuff = null;
        while(c.moveToNext()) {
            stuff = c.getString(0) + ":               " + c.getString(1) + " Kcal";
            list.add(stuff);
        }
        c.close();
        return list;
    }

    /**
     * Takes message in as a parameter, checks permissions,
     * and sends the user the message via SMS.
     *``
     * @param message message string.
     * @param number phone number string.
     */
    public void makeNotification(String message, String number) {
        try {
            SmsManager smsManager = SmsManager.getDefault();
            smsManager.sendTextMessage(number, null, message, null, null);
            Toast.makeText(context, "Notifications Sent", Toast.LENGTH_SHORT).show();
        } catch (Exception e) {
            e.printStackTrace();
            Toast.makeText(context, "Failed to Send", Toast.LENGTH_SHORT).show();
        }
    }
}
