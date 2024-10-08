package com.example.customview

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.RectF
import android.util.Log
import android.view.View

class CustomView(context: Context?) : View (context) {
    val TAG : String = "asfsadf"

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        val paint = Paint()
        paint.color = Color.BLACK
        paint.textSize = 100f
        Log.d(TAG, "dlgmlals3 onDRaw")
        canvas.run {
            drawText("안녕하세요", 30f, 100f, paint)
            customDrawCircle(canvas)
            customDrawRect(canvas)
        }
    }

    fun customDrawCircle(canvas : Canvas) {
        val paint = Paint()
        paint.style = Paint.Style.FILL
        paint.color = Color.BLUE

        canvas.drawCircle(150f, 300f, 100f, paint)
    }
    fun customDrawRect(canvas : Canvas) {
        val paint = Paint()
        paint.style = Paint.Style.STROKE
        paint.strokeWidth = 20f
        paint.color = Color.RED
        val rect = RectF(50f, 450f, 250f, 650f)
        canvas.drawRect(rect, paint)
    }
}

/*
*/
