package com.example.myapplication

import android.app.Activity
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.TextView
import com.example.myapplication.databinding.ActivityMainBinding

// view binding 생성
class MainActivity : AppCompatActivity() {
    val binding by lazy { ActivityMainBinding.inflate(layoutInflater) }
    val TAG = "MainActivity"


    var intArray = intArrayOf(0,1,2,3,4,5,6,7,8)


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 1. binding
        setContentView(binding.root) // 액티비티에 바인딩 레이아웃을 넣는다.
        binding.btnSay.setOnClickListener {
            binding.textView.text = "dlgmlals3"
            // 2. log
            Log.d(TAG, "dlgmlals3")

            // 코틀린 문법
            // var 변수.
            var myName = "dlgmlals3"
            // val = 상수.
            val MYAGE = 19
            Log.d(TAG, "name ${myName}")
        }
        ShowIteration()
        // 클래스 사용
        var cls = classTest()
    }

    fun ShowIteration() {
        // IF 문
        // 비교 연산자
        var result = 1 > 2
        Log.d(TAG, "compare 1 > 2 : ${result}")

        // WHEN 문 if문의 확장자
        var month = 6
        when (month) {
            5 -> Log.d(TAG, "5월 입니다.")
        }

        for (index in 1..100) {
            Log.d(TAG, "index=${index}")
        }
        for (index in 1 until 100) {
            Log.d(TAG, "index=${index}")
        }

        Log.d(TAG, "in 100 downTo 1 step 2")
        for (index in 100 downTo 1 step 2) {
            Log.d(TAG, "index=${index}")
        }

        Log.d(TAG, " in 1 until 100 step 2")
        for (index in 1 until 100 step 2) {
            Log.d(TAG, "index=${index}")
        }

        val intarray = intArrayOf(1,2,3,4,5)
        for (value in intarray) {
            Log.d(TAG, "value = ${value}")
        }

        // while
        var out = 0
        while (out < 3) {
            Log.d(TAG, " current out : ${out}")
            out++
        }

        do {

        } while(out < 3)
    }

    fun ShowType() {
        //코틀린의 기본 타입
        // 실수형 ( 전부 더블형으로 인식 )
        var doubleValue:Double = 35.6
        var floatValue:Float = 23.4F

        // 정수형
        var intValue:Int = 123456
        var longValue:Long = 2147_483_647
        var shortValue:Short = 123
        var byteValue:Byte = 127

        // 문자형
        var charValue:Char = 'a'
        var stringValue:String = "asdfasdfasf"
        // 불린형
        var aaa:Boolean = true
        aaa = false

        // 상수형
        val VALVALUE : Boolean = true
        // valValue = false // error

        var myNumber = "1,2,3,4,5"
        var thisWeekNumbers = "5,6,7,8"

        if (myNumber == thisWeekNumbers) {
            binding.textView.text = "당첨되었습니다."
        }
        else {
            binding.textView.text = "미당첨"
        }
        for (index in 1..20){
            binding.textView.append("\n현재 숫자는 ${index} 입니다.")
        }

        var vari = "이희민4"
        var vari2:String
        vari2 = "dlgmlals3"
    }

    fun ShowCollection() {
        // 배열과 컬렉션
        var intArr :IntArray = IntArray(10)
        intArr[0] = 10
        intArr[1] = 20
        intArr[2] = 30
        Log.d(TAG, "9번 인덱스 = ${intArr[9]}")

        // 할당동시에 값을 할당하려면
        var intA = intArrayOf(0, 1, 2, 3, 4, 5)


        var weekArray = CharArray(7)
        weekArray[0] = '월'
        weekArray[1] = '화'

        // 컬렉션
        // 리스트, 뮤터블리스트
        var list = mutableListOf<Int>()
        list.add(3)
        list.add(10)
        list.add(20)
        Log.d(TAG, "컬렉션 : ${list[2]}")
        Log.d(TAG, "컬렉션 : ${list}")

        // 맵
        var mutableMap = mutableMapOf<String, String>()
        mutableMap.put("12", "345")

        Log.d(TAG, "컬렉션 : ${mutableMap["12"]}")
        Log.d(TAG, "컬렉션 : ${mutableMap.get("12")}")

        // Null Safety
        var myName : String = "예시"
        var number:Int ? = null // nullable
        var newVariable : Activity? = null

        // null로 설정된 객체는 세이프콜(?) 사용
        // 널인 경우 default로 51을 넣어라 의미
        var result = number?.plus(35)  ?: 51 // elvis express
        result.plus(35)
    }
}

open class parentTest {

}
class classTest : parentTest() {
    var variable:String = "" // 프로퍼티
    init {
        // 클래스 초기화 하면 호출된다.
    }
    fun function() { // 메서드

    }
}