package com.example.basiclayout

import android.os.Bundle
import android.util.Log
import android.widget.CompoundButton
import android.widget.CompoundButton.OnCheckedChangeListener
import androidx.appcompat.app.AppCompatActivity
import com.example.basiclayout.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    val binding by lazy { ActivityMainBinding.inflate(layoutInflater) }
    val TAG = "MainActivity"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        with(binding){
            checkApple.setOnCheckedChangeListener {button, isChecked ->
                if (isChecked) {
                    Log.d(TAG, "apple selection")
                } else {
                    Log.d(TAG, "apple is not selected")
                }
            }
        }

        val checkBoxListener2 = object : CompoundButton.OnCheckedChangeListener {
            override fun onCheckedChanged(checkBox: CompoundButton?, isChecked: Boolean) {
                when (checkBox?.id) {
                    R.id.checkApple -> {
                        Log.d(TAG, "apple ${isChecked}")
                    }
                    R.id.checkBanana -> {
                        Log.d(TAG, "banana  ${isChecked}")
                    }
                    R.id.checkKiwi -> {
                        Log.d(TAG, "kiwi  ${isChecked}")
                    }
                }
            }
        }

        val checkBoxListener = CompoundButton.OnCheckedChangeListener { checkBox, isChecked ->
            when (checkBox.id) {
                R.id.checkApple -> {
                    Log.d(TAG, "apple ${isChecked}")
                }
                R.id.checkBanana -> {
                    Log.d(TAG, "banana  ${isChecked}")
                }
                R.id.checkKiwi -> {
                    Log.d(TAG, "kiwi  ${isChecked}")
                }
            }
        }
        with (binding){
            checkApple.setOnCheckedChangeListener(checkBoxListener2)
            checkBanana.setOnCheckedChangeListener(checkBoxListener2)
            checkKiwi.setOnCheckedChangeListener(checkBoxListener2)
        }

    }
}