package com.example.test2;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.Calendar;

public class MainActivity2 extends AppCompatActivity {

    private DatabaseReference mRef;
    private FirebaseAuth mAuth;
    private FirebaseUser currentUser;
    private TextView reservationInfo;
    private TextView userEmail; // Declare the user email TextView

    private String startDateTime; // To store the selected start date and time
    private String endDateTime; // To store the selected end date and time

    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);

        // Initialize Firebase
        mAuth = FirebaseAuth.getInstance();
        currentUser = mAuth.getCurrentUser();

        mRef = FirebaseDatabase.getInstance().getReference();

        // Bind UI elements
        userEmail = findViewById(R.id.user_email); // Bind the user email TextView
        TextView s1 = findViewById(R.id.slot1);
        TextView s2 = findViewById(R.id.slot2);
        TextView s3 = findViewById(R.id.slot3);
        TextView s4 = findViewById(R.id.slot4);
        TextView s5 = findViewById(R.id.slot5);
        TextView s6 = findViewById(R.id.slot6);
        TextView s7 = findViewById(R.id.rembal);  // This is the remaining balance
        Button manageUsersBtn = findViewById(R.id.manage_users_btn);
        reservationInfo = findViewById(R.id.reservation_info);
        Button reserveSlotBtn = findViewById(R.id.reserve_slot_btn);

        // Display the user's email in the TextView
        if (currentUser != null) {
            userEmail.setText("User Email: " + currentUser.getEmail());
        } else {
            userEmail.setText("User Email: Not logged in");
        }

        // Show the "Manage Users" button if the user is admin
        if (currentUser != null && currentUser.getEmail().equals("admin@gmail.com")) {
            manageUsersBtn.setVisibility(View.VISIBLE);

            manageUsersBtn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    // Redirect to AdminActivity for managing users
                    Intent intent = new Intent(MainActivity2.this, ManageUsersActivity.class);
                    startActivity(intent);
                }
            });
        } else {
            // Regular user: Do nothing, button remains hidden
            manageUsersBtn.setVisibility(View.GONE);
        }

        // Attach a listener to read the data at our reference
        mRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                // Retrieve data from Firebase
                String value1 = snapshot.child("Slot1").getValue(String.class);
                String value2 = snapshot.child("Slot2").getValue(String.class);
                String value3 = snapshot.child("Slot3").getValue(String.class);
                String value4 = snapshot.child("Slot4").getValue(String.class);
                String value5 = snapshot.child("Slot5").getValue(String.class);
                String value6 = snapshot.child("Slot6").getValue(String.class);
                String value7 = snapshot.child("Remaining Balance").getValue(String.class);

                // Set values to TextViews
                s1.setText("Slot 1: " + value1);
                s2.setText("Slot 2: " + value2);
                s3.setText("Slot 3: " + value3);
                s4.setText("Slot 4: " + value4);
                s5.setText("Slot 5: " + value5);
                s6.setText("Slot 6: " + value6);
                s7.setText("Remaining Balance: " + value7);
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                // Failed to read value
                Toast.makeText(MainActivity2.this, "Failed to read data.", Toast.LENGTH_SHORT).show();
            }
        });

        // Date and time picker for reserving a slot
        reserveSlotBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showDateTimePicker();
            }
        });
    }

    private void showDateTimePicker() {
        // Get current date and time
        Calendar calendar = Calendar.getInstance();
        int year = calendar.get(Calendar.YEAR);
        int month = calendar.get(Calendar.MONTH);
        int day = calendar.get(Calendar.DAY_OF_MONTH);
        int hour = calendar.get(Calendar.HOUR_OF_DAY);
        int minute = calendar.get(Calendar.MINUTE);

        // Show DatePickerDialog
        DatePickerDialog datePickerDialog = new DatePickerDialog(MainActivity2.this,
                (view, selectedYear, selectedMonth, selectedDay) -> {
                    // Show TimePickerDialog for start time after date is selected
                    TimePickerDialog timePickerDialog = new TimePickerDialog(MainActivity2.this,
                            (timePicker, selectedHour, selectedMinute) -> {
                                // Set the start date and time
                                startDateTime = selectedDay + "/" + (selectedMonth + 1) + "/" + selectedYear + " " +
                                        selectedHour + ":" + String.format("%02d", selectedMinute);
                                // Show the end time picker after selecting start time
                                showEndTimePicker(selectedYear, selectedMonth, selectedDay, selectedHour, selectedMinute);
                            }, hour, minute, true);
                    timePickerDialog.show();
                }, year, month, day);
        datePickerDialog.show();
    }

    private void showEndTimePicker(int year, int month, int day, int startHour, int startMinute) {
        // Show TimePickerDialog for end time
        TimePickerDialog endTimePickerDialog = new TimePickerDialog(MainActivity2.this,
                (timePicker, endHour, endMinute) -> {
                    // Set the end date and time
                    endDateTime = day + "/" + (month + 1) + "/" + year + " " +
                            endHour + ":" + String.format("%02d", endMinute);
                    // Display the reservation info
                    reservationInfo.setText("Reservation Info: Start - " + startDateTime + ", End - " + endDateTime);
                }, startHour, startMinute, true);
        endTimePickerDialog.show();
    }
}
