package com.example.unitconvertor

import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material3.Button
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.unitconvertor.ui.theme.UnitConvertorTheme
import kotlin.math.roundToInt

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            UnitConvertorTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    UnitConverter()
                }
            }
        }
    }
}

@Composable
fun UnitConverter() {
    var inputValue by remember { mutableStateOf("")}
    var outputValue by remember { mutableStateOf("")}
    var inputUnit by remember { mutableStateOf("Meters")}
    var outputUnity by remember { mutableStateOf("Meters")}
    var iExpanded by remember { mutableStateOf(false)}
    var oExpanded by remember { mutableStateOf(false)}
    val conversionFactor = remember { mutableStateOf(1.0) }
    val oConversionFactor = remember { mutableStateOf(1.0) }

    fun convertUnits() {
        // ?: - elvis operator
        // 널이면 0.0 사용 하도록함.
        val inputValueDouble = inputValue.toDoubleOrNull() ?: 0.0
        val result = (inputValueDouble * conversionFactor.value * 100.0 / oConversionFactor.value).roundToInt() / 100.0
        outputValue = result.toString()
    }
    val customTextStyle = TextStyle (
        fontFamily = FontFamily.Default,
        fontSize = 30.sp,
        color = Color.LightGray
    )

    Column(
        modifier = Modifier.fillMaxSize(),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment =  Alignment.CenterHorizontally
    ) {

        Text("Unit Convertor",
            modifier = Modifier.padding(5.dp),
            style = MaterialTheme.typography.headlineMedium)

        Spacer(modifier = Modifier.height(16.dp))

        OutlinedTextField(
            value = inputValue,
            onValueChange = {
                inputValue = it
                convertUnits()
            },
            label = { Text("Enter Value")}
        )

        Spacer(modifier = Modifier.height(16.dp))

        Row {
            val context = LocalContext.current;
            // Input Box
            Row {
                // 세로로 정렬됨..
                Box {
                    // Input Button
                    Button(onClick = { iExpanded = true }) {
                        Text(inputUnit)
                        Icon(Icons.Default.ArrowDropDown,
                            contentDescription = "Arrow Down")
                    }
                    DropdownMenu(
                        expanded = iExpanded,
                        onDismissRequest = { iExpanded = false }) {
                        DropdownMenuItem(
                            text = { Text("Centimeter") },
                            onClick = {
                                iExpanded = false;
                                inputUnit = "Centimeter"
                                conversionFactor.value = 0.01
                                convertUnits()
                            }
                        )

                        DropdownMenuItem(
                            text = { Text("Meter")},
                            onClick = {
                                iExpanded = false;
                                inputUnit = "Meters"
                                conversionFactor.value = 1.0
                                convertUnits()
                            }
                        )
                        DropdownMenuItem(
                            text = { Text("Feet")},
                            onClick = {                                 iExpanded = false;
                                iExpanded = false;
                                inputUnit = "Feet"
                                conversionFactor.value = 0.3048
                                convertUnits()
                            }
                        )
                        DropdownMenuItem(
                            text = { Text("Milimeters")},
                            onClick = {
                                iExpanded = false;
                                inputUnit = "Milimeters"
                                conversionFactor.value = 0.001
                                convertUnits()
                            }
                        )
                    }
                }
                Spacer(modifier = Modifier.width(16.dp))
                Box {
                    Button(onClick = { oExpanded = true }) {
                        Text(outputUnity)
                        Icon(Icons.Default.ArrowDropDown,
                            contentDescription = "Arrow Down")
                    }

                    // onDismissRequest는 유저가 다른 곳을 클릭했을때 실행하는
                    DropdownMenu(
                        expanded = oExpanded,
                        onDismissRequest = { oExpanded = false }) {

                        DropdownMenuItem(
                            text = { Text("Centimeter") },
                            onClick = {
                                iExpanded = false;
                                outputUnity = "Centimeter"
                                oConversionFactor.value = 0.01
                                convertUnits()
                            }
                        )
                        DropdownMenuItem(
                            text = { Text("Meter")},
                            onClick = {
                                iExpanded = false;
                                outputUnity = "Meter"
                                oConversionFactor.value = 1.0
                                convertUnits()
                            }
                        )
                        DropdownMenuItem(
                            text = { Text("Feet")},
                            onClick = {
                                iExpanded = false;
                                outputUnity = "Feet"
                                oConversionFactor.value = 0.3048
                                convertUnits()
                            }
                        )
                        DropdownMenuItem(
                            text = { Text("Milimeters")},
                            onClick = {
                                iExpanded = false;
                                outputUnity = "Milimeters"
                                oConversionFactor.value = 0.001
                                convertUnits()
                            }
                        )
                    }
                }
            }
        }
        Spacer(modifier = Modifier.height(16.dp))
        // Units Result Text
        Text("Result : ${outputValue} ${outputUnity}",
            style = customTextStyle
        )
    }
}

@Preview(showBackground = true)
@Composable
fun UnitConverterPreview() {
    UnitConverter()
}
