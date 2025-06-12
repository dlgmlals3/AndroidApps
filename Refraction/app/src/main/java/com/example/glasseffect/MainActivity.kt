package com.example.glasseffect

import android.content.Context
import android.graphics.*
import android.graphics.drawable.BitmapDrawable
import android.os.Build
import android.os.Bundle
import android.util.AttributeSet
import android.view.Gravity
import android.view.MotionEvent
import android.view.View
import android.widget.FrameLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val rootLayout = FrameLayout(this).apply {
            background = ContextCompat.getDrawable(this@MainActivity, R.drawable.sample_image)
        }

        val glassView = GlassRefractionView(this).apply {
            layoutParams = FrameLayout.LayoutParams(1200, 450).apply { // 가로만 길게
                gravity = Gravity.CENTER
            }
        }

        rootLayout.addView(glassView)
        setContentView(rootLayout)

        // 레이아웃 완료 후 배경 복사
        rootLayout.post {
            glassView.copyBackgroundFrom(rootLayout)
        }

        // 드래그 기능
        var dX = 0f
        var dY = 0f

        glassView.setOnTouchListener { v, event ->
            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    dX = v.x - event.rawX
                    dY = v.y - event.rawY
                    true
                }
                MotionEvent.ACTION_MOVE -> {
                    val newX = event.rawX + dX
                    val newY = event.rawY + dY

                    v.animate()
                        .x(newX)
                        .y(newY)
                        .setDuration(0)
                        .start()

                    // 이동할 때마다 배경 다시 복사
                    glassView.updateBackgroundPosition(rootLayout, newX.toInt(), newY.toInt())
                    true
                }
                else -> false
            }
        }
    }
}

class GlassRefractionView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : FrameLayout(context, attrs, defStyleAttr) {

    private var currentShader: RuntimeShader? = null
    private var buttonText: TextView

    init {
        // 텍스트 추가
        buttonText = TextView(context).apply {
            text = "Glass Effect"
            textSize = 20f
            setTextColor(Color.WHITE)
            gravity = Gravity.CENTER
            setShadowLayer(3f, 1f, 1f, Color.BLACK) // 텍스트 그림자
            layoutParams = FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT
            )
        }
        addView(buttonText)
    }

    fun copyBackgroundFrom(parentView: View) {
        try {
            // 부모 뷰의 배경을 비트맵으로 캡처
            val parentBackground = parentView.background
            if (parentBackground != null) {

                // 현재 뷰 위치에 해당하는 배경 부분 잘라내기
                val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
                val canvas = Canvas(bitmap)

                // 부모 배경을 현재 뷰 크기만큼 그리기
                canvas.save()
                canvas.translate(-x, -y) // 현재 뷰 위치만큼 오프셋
                parentBackground.setBounds(0, 0, parentView.width, parentView.height)
                parentBackground.draw(canvas)
                canvas.restore()

                // 비트맵을 배경으로 설정
                background = BitmapDrawable(resources, bitmap)

                // 유리 굴절 효과 적용
                setupGlassEffect()
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun updateBackgroundPosition(parentView: View, newX: Int, newY: Int) {
        try {
            val parentBackground = parentView.background
            if (parentBackground != null) {

                val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
                val canvas = Canvas(bitmap)

                // 새 위치에 해당하는 배경 부분 그리기
                canvas.save()
                canvas.translate(-newX.toFloat(), -newY.toFloat())
                parentBackground.setBounds(0, 0, parentView.width, parentView.height)
                parentBackground.draw(canvas)
                canvas.restore()

                background = BitmapDrawable(resources, bitmap)
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    private fun setupGlassEffect() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            try {
                val glassShader = """
                    uniform shader background;
                    uniform float2 resolution;
                    
                    half4 main(float2 coord) {
                        float2 uv = coord / resolution;
                        float2 center = float2(0.5, 0.5);
                        float2 m2 = uv - center;
                        
                        // Rounded box calculation (참조 코드 방식)
                        float aspectRatio = resolution.x / resolution.y;
                        float roundedBox = pow(abs(m2.x * aspectRatio), 8.0) + pow(abs(m2.y), 8.0);
                        
                        // Glass area mask
                        float rb1 = clamp((1.0 - roundedBox * 5000.0) * 8.0, 0.0, 1.0);
                        float rb2 = clamp((0.95 - roundedBox * 4500.0) * 16.0, 0.0, 1.0) 
                                  - clamp(pow(0.9 - roundedBox * 4500.0, 1.0) * 16.0, 0.0, 1.0);
                        
                        if (rb1 + rb2 > 0.0) {
                            // 굴절 효과 (가장자리로 갈수록 강해짐)
                            float refractiveIndex = 1.8;
                            float2 distorted = (uv - 0.5) * (1.0 + (refractiveIndex - 1.0) * (1.0 - roundedBox * 2500.0)) + 0.5;
                            
                            // 색수차 효과
                            float chromaticAberration = 0.008;
                            float2 caOffset = chromaticAberration * m2;
                            
                            // 다중 샘플링으로 부드러운 효과
                            half4 color = half4(0.0);
                            float total = 0.0;
                            
                            for (float x = -2.0; x <= 2.0; x += 1.0) {
                                for (float y = -2.0; y <= 2.0; y += 1.0) {
                                    float2 offset = float2(x, y) * 0.3 / resolution;
                                    
                                    // RGB 분리 (색수차)
                                    half4 sample1 = background.eval((offset + distorted + caOffset) * resolution);
                                    half4 sample2 = background.eval((offset + distorted) * resolution);
                                    half4 sample3 = background.eval((offset + distorted - caOffset) * resolution);
                                    
                                    half4 combinedSample;
                                    combinedSample.r = sample1.r;
                                    combinedSample.g = sample2.g;
                                    combinedSample.b = sample3.b;
                                    combinedSample.a = sample2.a;
                                    
                                    color += combinedSample;
                                    total += 1.0;
                                }
                            }
                            color /= total;
                            
                            // 조명 효과
                            float gradient = clamp((clamp(m2.y, 0.0, 0.2) + 0.1) / 2.0, 0.0, 1.0) +
                                           clamp((clamp(-m2.y, -1.0, 0.2) + 0.1) / 2.0, 0.0, 1.0);
                            
                            color = clamp(color + half4(rb1 * gradient) + half4(rb2 * 0.3), 0.0, 1.0);
                            
                            return color;
                        } else {
                            // 유리 영역 외부는 원본
                            return background.eval(coord);
                        }
                    }
                """.trimIndent()

                currentShader = RuntimeShader(glassShader)
                currentShader!!.setFloatUniform("resolution", width.toFloat(), height.toFloat())

                val renderEffect = RenderEffect.createRuntimeShaderEffect(currentShader!!, "background")
                setRenderEffect(renderEffect)

            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)
        currentShader?.setFloatUniform("resolution", w.toFloat(), h.toFloat())
    }
}