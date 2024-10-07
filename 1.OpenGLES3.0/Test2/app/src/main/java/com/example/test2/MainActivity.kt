package com.example.test2

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity


class MainActivity : ComponentActivity() {
    companion object {
        init {
            System.loadLibrary("jniCalculator")
        }
    }
    val TAG : String = "TEST"

    external fun getSum(a : Int, b : Int) : Int
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val sum = getSum(2, 3)
        Log.d(TAG, "dlgmlals3 ${sum}")
    }
}