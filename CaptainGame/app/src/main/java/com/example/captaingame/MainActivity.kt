package com.example.captaingame

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.example.captaingame.ui.theme.CaptainGameTheme
import kotlin.random.Random

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            CaptainGameTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    CaptainGame()
                }
            }
        }
    }

    @Composable @Preview
    fun CaptainGame() {
        // 특정값이 컴포저에서 변하는 값을 UI 업데이트를 보기 위해서는
        // remember 내부 mutableStateOf로 변수를 선언한다.
        //val treasureFound = remember { mutableStateOf(0) }
        var treasureFound by remember { mutableStateOf(0)}

        val direction = remember { mutableStateOf("North") }
        val stormOrTreasure = remember { mutableStateOf("")}
        Column {
            Text("Tresure Found : ${treasureFound}")
            Text("Tresure Direction : ${direction.value}")
            Text("Storm : ${stormOrTreasure.value}")

            Button(onClick = {
                direction.value = "East"
                if (Random.nextBoolean()) {
                    treasureFound += 1
                }
            }) {
                // content
                Text("Shell East")
            }

            Button(onClick = {
                direction.value = "West"
                if (Random.nextBoolean()) {
                    treasureFound += 1
                }
            }) {
                // content
                Text("Shell West")
            }

            Button(onClick = {
                direction.value = "South"
                if (Random.nextBoolean()) {
                    treasureFound += 1
                }
            }) {
                // content
                Text("Shell South")
            }

            Button(onClick = {
                direction.value = "North"
                if (Random.nextBoolean()) {
                    treasureFound += 1
                }
            }) {
                // content
                Text("Shell North")
            }

            Button(onClick = {
                direction.value = "North"
                if (Random.nextBoolean()) {
                    treasureFound += 1
                    stormOrTreasure.value = "We Found a Treasure"
                } else {
                    stormOrTreasure.value = "We didnt found Treasure"
                }
            }) {
                // content
                Text("Tresure")
            }
        }
    }
}