package com.example.project2_rowaniruegas;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import androidx.annotation.Nullable;

import java.util.ArrayList;

public class Database extends SQLiteOpenHelper {

    private static final String NAME = "Customers";
    private static final int VERSION = 1;
    private static final String TABLE = "Customers";
    private static final String KEY_ID = "id";
    private static final String KEY_USERNAME = "username";
    private static final String KEY_PASSWORD = "password";
    private static final String KEY_WEIGHT = "weight";
    private static final String KEY_GOAL = "goal";
    public Database(@Nullable Context context) {
        super(context, NAME, null, VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase database) {

        database.execSQL("CREATE TABLE " + TABLE + "(" + KEY_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, "
                        + KEY_USERNAME + " TEXT, " + KEY_PASSWORD + " TEXT, " + KEY_WEIGHT + " TEXT, " + KEY_GOAL + " TEXT)");

    }

    @Override
    public void onUpgrade(SQLiteDatabase database, int i, int i1){

        database.execSQL("DROP TABLE IF EXISTS " + TABLE);
        onCreate(database);

    }

    public void addUser(String user, String pass) {
        SQLiteDatabase database = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_USERNAME, user);
        values.put(KEY_PASSWORD, pass);

        database.insert(TABLE, null, values);
    }

    public void addWeight(String weight) {
        SQLiteDatabase database = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_WEIGHT, weight);

        database.insert(TABLE, null, values);
    }

    public void addGoal(String goal) {
        SQLiteDatabase database = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_WEIGHT, goal);

        database.insert(TABLE, null, values);
    }

    public void updateWeight(String weight) {
        // FIXME: Create code to update the selected weight with a new value
    }

    public void deleteWeight(String weight) {
        // FIXME: Create code to delete specified weight
    }
}
