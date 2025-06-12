package com.example.glasseffect

import android.content.Context
import android.graphics.*
import android.graphics.drawable.BitmapDrawable
import android.os.Build
import android.os.Bundle
import android.util.AttributeSet
import android.util.Log
import android.view.Gravity
import android.view.MotionEvent
import android.view.View
import android.widget.FrameLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import kotlin.math.sqrt

class MainActivity : AppCompatActivity() {
    private lateinit var rootLayout: FrameLayout
    private lateinit var glassView1: GlassRefractionView
    private lateinit var glassView2: GlassRefractionView
    private var bridgeView: BridgeView? = null
    private val TAG = "Glass"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        rootLayout = FrameLayout(this).apply {
            background = ContextCompat.getDrawable(this@MainActivity, R.drawable.sample_image)
        }

        // 첫 번째 글래스뷰
        glassView1 = GlassRefractionView(this, "Glass 1").apply {
            layoutParams = FrameLayout.LayoutParams(800, 400).apply {
                leftMargin = 200
                topMargin = 400
            }
        }

        // 두 번째 글래스뷰
        glassView2 = GlassRefractionView(this, "Glass 2").apply {
            layoutParams = FrameLayout.LayoutParams(800, 400).apply {
                leftMargin = 200
                topMargin = 600
            }
        }

        // 개별 뷰들만 먼저 추가
        rootLayout.addView(glassView1)
        rootLayout.addView(glassView2)
        setContentView(rootLayout)

        // 레이아웃 완료 후 배경 복사
        rootLayout.post {
            if (glassView1.width > 0 && glassView1.height > 0) {
                glassView1.copyBackgroundFrom(rootLayout)
            }
            if (glassView2.width > 0 && glassView2.height > 0) {
                glassView2.copyBackgroundFrom(rootLayout)
            }
        }

        // 드래그 기능 설정
        setupDragListener(glassView1)
        setupDragListener(glassView2)
    }

    private fun setupDragListener(glassView: GlassRefractionView) {
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

                    // 배경 업데이트
                    glassView.updateBackgroundPosition(rootLayout, newX.toInt(), newY.toInt())

                    // 실시간으로 브릿지 애니메이션 체크
                    updateBridgeAnimation()
                    true
                }

                MotionEvent.ACTION_UP -> {
                    updateBridgeAnimation()
                    true
                }

                else -> false
            }
        }
    }

    private fun updateBridgeAnimation() {
        if (rootLayout.width > 0 && rootLayout.height > 0) {
            val center1X = glassView1.x + glassView1.width / 2
            val center1Y = glassView1.y + glassView1.height / 2
            val center2X = glassView2.x + glassView2.width / 2
            val center2Y = glassView2.y + glassView2.height / 2

            val distance = sqrt((center1X - center2X) * (center1X - center2X) +
                    (center1Y - center2Y) * (center1Y - center2Y))

            android.util.Log.d("BridgeView", "dlgmlals3 Distance: $distance")

            // 브릿지 시작 거리
            val bridgeStartDistance = 400f
            val bridgeMaxDistance = 100f // 최대 브릿지 강도가 되는 거리

            if (distance < bridgeStartDistance) {
                // 브릿지 강도 계산 (거리 기반)
                val bridgeStrength = 1.0f - ((distance - bridgeMaxDistance) / (bridgeStartDistance - bridgeMaxDistance)).coerceIn(0.0f, 1.0f)

                android.util.Log.d("BridgeView", "dlgmlals3 Distance: $distance, Strength: $bridgeStrength")
                // 브릿지 생성/업데이트
                if (bridgeView == null) {
                    android.util.Log.d("BridgeView", "dlgmlals3 CREATING BRIDGE")

                    // 디버깅용으로 브릿지뷰를 크게 생성 (전체 영역 덮음)
                    val minX = minOf(glassView1.x, glassView2.x) - 100f
                    val minY = minOf(glassView1.y, glassView2.y) - 100f
                    val maxX = maxOf(glassView1.x + glassView1.width, glassView2.x + glassView2.width) + 100f
                    val maxY = maxOf(glassView1.y + glassView1.height, glassView2.y + glassView2.height) + 100f

                    val bridgeWidth = (maxX - minX).toInt()
                    val bridgeHeight = (maxY - minY).toInt()

                    android.util.Log.d("BridgeView", "BIG BRIDGE AREA - Width: $bridgeWidth, Height: $bridgeHeight")

                    // BridgeView 생성 (큰 영역으로)
                    bridgeView = BridgeView(this@MainActivity).apply {
                        layoutParams = FrameLayout.LayoutParams(bridgeWidth, bridgeHeight).apply {
                            leftMargin = minX.toInt()
                            topMargin = minY.toInt()
                        }
                    }

                    // 레이아웃에 추가 (개별 뷰들 위에)
                    rootLayout.addView(bridgeView)

                    // 즉시 설정 (배경 캡처 없이)
                    bridgeView!!.copyBackgroundFrom(rootLayout)

                    // 브릿지 뷰 기준으로 상대 위치 계산
                    val pos1X = (center1X - minX) / bridgeWidth
                    val pos1Y = (center1Y - minY) / bridgeHeight
                    val pos2X = (center2X - minX) / bridgeWidth
                    val pos2Y = (center2Y - minY) / bridgeHeight

                    val size1X = glassView1.width.toFloat() / bridgeWidth
                    val size1Y = glassView1.height.toFloat() / bridgeHeight
                    val size2X = glassView2.width.toFloat() / bridgeWidth
                    val size2Y = glassView2.height.toFloat() / bridgeHeight

                    // 브릿지 강도 계산 (거리 기반)
                    val bridgeStrength = 1.0f - ((distance - bridgeMaxDistance) / (bridgeStartDistance - bridgeMaxDistance)).coerceIn(0.0f, 1.0f)

                    android.util.Log.d("BridgeView", "BRIDGE CREATED - Strength: $bridgeStrength, Distance: $distance")

                    bridgeView!!.updateBridge(pos1X, pos1Y, pos2X, pos2Y, size1X, size1Y, size2X, size2Y, bridgeStrength)
                }
            } else {
                // 기존 브릿지 업데이트
                bridgeView?.let { bridge ->
                    val params = bridge.layoutParams as FrameLayout.LayoutParams
                    val minX = params.leftMargin.toFloat()
                    val minY = params.topMargin.toFloat()
                    val bridgeWidth = params.width.toFloat()
                    val bridgeHeight = params.height.toFloat()

                    val pos1X = (center1X - minX) / bridgeWidth
                    val pos1Y = (center1Y - minY) / bridgeHeight
                    val pos2X = (center2X - minX) / bridgeWidth
                    val pos2Y = (center2Y - minY) / bridgeHeight

                    val size1X = glassView1.width.toFloat() / bridgeWidth
                    val size1Y = glassView1.height.toFloat() / bridgeHeight
                    val size2X = glassView2.width.toFloat() / bridgeWidth
                    val size2Y = glassView2.height.toFloat() / bridgeHeight

                    // 브릿지 강도 계산
                    val bridgeStrength = 1.0f - ((distance - bridgeMaxDistance) / (bridgeStartDistance - bridgeMaxDistance)).coerceIn(0.0f, 1.0f)

                    android.util.Log.d("BridgeView", "dlgmlals3 BRIDGE UPDATE - Strength: $bridgeStrength, Distance: $distance")

                    bridge.updateBridge(pos1X, pos1Y, pos2X, pos2Y, size1X, size1Y, size2X, size2Y, bridgeStrength)

                }
            }
        } else {
            // 브릿지 제거
            bridgeView?.let { bridge ->
                android.util.Log.d("BridgeView", "REMOVING BRIDGE")

                rootLayout.removeView(bridge)
                bridgeView = null
            }
        }
    }
}

class GlassRefractionView @JvmOverloads constructor(
    context: Context,
    private val label: String = "Glass Effect",
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : FrameLayout(context, attrs, defStyleAttr) {

    private var currentShader: RuntimeShader? = null
    private var buttonText: TextView

    init {
        buttonText = TextView(context).apply {
            text = label
            textSize = 20f
            setTextColor(Color.WHITE)
            gravity = Gravity.CENTER
            setShadowLayer(3f, 1f, 1f, Color.BLACK)
            layoutParams = FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT
            )
        }
        addView(buttonText)
    }

    fun copyBackgroundFrom(parentView: View) {
        try {
            val parentBackground = parentView.background
            if (parentBackground != null) {
                val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
                val canvas = Canvas(bitmap)

                canvas.save()
                canvas.translate(-x, -y)
                parentBackground.setBounds(0, 0, parentView.width, parentView.height)
                parentBackground.draw(canvas)
                canvas.restore()

                background = BitmapDrawable(resources, bitmap)
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
        Log.d("Glass"," dlgmlals3 setupGlassEffect");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            try {
                val glassShader = """
                    uniform shader background;
                    uniform float2 resolution;
                    
                    half4 main(float2 coord) {
                        float2 uv = coord / resolution;
                        float2 center = float2(0.5, 0.5);
                        float2 m2 = uv - center;
                        
                        float aspectRatio = resolution.x / resolution.y;
                        float roundedBox = pow(abs(m2.x * aspectRatio), 8.0) + pow(abs(m2.y), 8.0);
                        
                        float rb1 = clamp((1.0 - roundedBox * 5000.0) * 8.0, 0.0, 1.0);
                        
                        if (rb1 > 0.0) {
                            float refractiveIndex = 1.8;
                            float2 distorted = (uv - 0.5) * (1.0 + (refractiveIndex - 1.0) * (1.0 - roundedBox * 2500.0)) + 0.5;
                            
                            half4 color = background.eval(distorted * resolution);
                            color = clamp(color + half4(rb1 * 0.3), 0.0, 1.0);
                            
                            return color;
                        } else {
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

class BridgeView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : FrameLayout(context, attrs, defStyleAttr) {

    private var currentShader: RuntimeShader? = null

    fun copyBackgroundFrom(parentView: View) {
        try {
            // 테스트를 위해 일단 배경 설정 안 함
            android.util.Log.d("BridgeView", "copyBackgroundFrom called")

            // 셰이더 설정도 일단 안 함
             post {
                 setupBridgeEffect()
             }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun updateBridge(pos1X: Float, pos1Y: Float, pos2X: Float, pos2Y: Float,
                     size1X: Float, size1Y: Float, size2X: Float, size2Y: Float, bridgeStrength: Float) {
        currentShader?.let { shader ->
            try {
                shader.setFloatUniform("pos1", pos1X, pos1Y)
                shader.setFloatUniform("pos2", pos2X, pos2Y)
                shader.setFloatUniform("size1", size1X, size1Y)
                shader.setFloatUniform("size2", size2X, size2Y)
                shader.setFloatUniform("bridgeStrength", bridgeStrength)
                invalidate()
            } catch (e: Exception) {
                android.util.Log.e("BridgeView", "Error updating bridge shader", e)
            }
        }
    }

    private fun setupBridgeEffect() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            try {
                android.util.Log.d("BridgeView", "Setting up bridge effect, size: ${width}x${height}")

                // 브릿지 전용 셰이더 - 다리 연결 애니메이션
                val bridgeShader = """
                    uniform shader background;
                    uniform float2 resolution;
                    uniform float2 pos1;
                    uniform float2 pos2;
                    uniform float2 size1;
                    uniform float2 size2;
                    uniform float bridgeStrength;
                    
                    // Superellipse SDF
                    float sdSuperellipse(float2 p, float2 size, float n) {
                        p = abs(p / size);
                        float m = pow(p.x, n) + pow(p.y, n);
                        return (pow(m, 1.0 / n) - 1.0) * min(size.x, size.y);
                    }
                    
                    // Smooth minimum - 브릿지의 핵심!
                    float smin(float a, float b, float k) {
                        float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
                        return mix(b, a, h) - k * h * (1.0 - h);
                    }
                    
                    // 브릿지 SDF - 두 점 사이의 캡슐 모양
                    float sdCapsule(float2 p, float2 a, float2 b, float r) {
                        float2 pa = p - a;
                        float2 ba = b - a;
                        float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
                        return length(pa - ba * h) - r;
                    }
                    
                    half4 main(float2 coord) {
                        float2 uv = (coord - 0.5 * resolution) / resolution.y;
                        
                        // 두 유리의 위치 (UV 좌표계)
                        float2 glassPos1 = (pos1 - 0.5) * float2(resolution.x / resolution.y, 1.0);
                        float2 glassPos2 = (pos2 - 0.5) * float2(resolution.x / resolution.y, 1.0);
                        
                        // 크기 변환
                        float2 glassSize1 = size1 * float2(resolution.x / resolution.y, 1.0) * 0.5;
                        float2 glassSize2 = size2 * float2(resolution.x / resolution.y, 1.0) * 0.5;
                        
                        float n = 4.0;
                        
                        // 두 유리의 SDF
                        float d1 = sdSuperellipse(uv - glassPos1, glassSize1, n);
                        float d2 = sdSuperellipse(uv - glassPos2, glassSize2, n);
                        
                        // 브릿지 SDF - 두 중심을 연결하는 캡슐
                        float bridgeRadius = bridgeStrength * 0.12; // 더 두꺼운 브릿지
                        float bridgeD = sdCapsule(uv, glassPos1, glassPos2, bridgeRadius);
                        
                        // 동적 블렌딩 반지름 - bridgeStrength에 따라 변화
                        float blendRadius = 0.01 + bridgeStrength * 0.08; // 더 부드러운 블렌딩
                        
                        // 브릿지와 두 유리를 모두 블렌딩
                        float d = smin(smin(d1, d2, blendRadius), bridgeD, blendRadius * 0.5);
                        
                        // 그림자 효과
                        float2 shadowOffset = float2(0.0, 0.008);
                        float shadow1 = sdSuperellipse(uv - glassPos1 - shadowOffset, glassSize1, n);
                        float shadow2 = sdSuperellipse(uv - glassPos2 - shadowOffset, glassSize2, n);
                        float shadowBridge = sdCapsule(uv - shadowOffset, glassPos1, glassPos2, bridgeRadius);
                        float shadowSDF = smin(smin(shadow1, shadow2, blendRadius), shadowBridge, blendRadius * 0.5);
                        float shadowMask = 1.0 - smoothstep(0.0, 0.04, shadowSDF);
                        
                        // 기본 배경 - 반투명 디버깅 색상으로!
                        half4 baseColor = half4(1.0, 0.0, 0.0, 0.2 * bridgeStrength); // 빨간색 반투명 (디버깅)
                        
                        // 브릿지 영역에서만 효과 적용 (bridgeStrength > 0일 때만)
                        if (d < 0.0 && bridgeStrength > 0.01) {
                            // 중심점 계산 (가중 평균)
                            float w1 = exp(-d1 * d1 * 8.0);
                            float w2 = exp(-d2 * d2 * 8.0);
                            float wBridge = exp(-bridgeD * bridgeD * 12.0) * bridgeStrength;
                            float totalWeight = w1 + w2 + wBridge + 1e-6;
                            
                            float2 center = (glassPos1 * w1 + glassPos2 * w2 + (glassPos1 + glassPos2) * 0.5 * wBridge) / totalWeight;
                            
                            // 유리 효과 적용
                            float2 offset = uv - center;
                            float distFromCenter = length(offset);
                            
                            float depthInShape = abs(d);
                            float radius = (min(glassSize1.x, glassSize1.y) + min(glassSize2.x, glassSize2.y)) * 0.5;
                            float normalizedDepth = clamp(depthInShape / (radius * 0.8), 0.0, 1.0);
                            
                            float edgeFactor = 1.0 - normalizedDepth;
                            float exponentialDistortion = exp(edgeFactor * 3.0) - 1.0;
                            
                            float baseMagnification = 0.75;
                            float lensStrength = 0.4 + bridgeStrength * 0.2; // 브릿지 강도에 따른 효과 증가
                            float distortionAmount = exponentialDistortion * lensStrength;
                            
                            float baseDistortion = baseMagnification + distortionAmount * distFromCenter;
                            float redDistortion = baseDistortion * 0.9;
                            float greenDistortion = baseDistortion * 1.0;
                            float blueDistortion = baseDistortion * 1.1;
                            
                            float2 redUV = center + offset * redDistortion;
                            float2 greenUV = center + offset * greenDistortion;
                            float2 blueUV = center + offset * blueDistortion;
                            
                            // 브릿지 영역에서만 효과 적용
                            if (bridgeD < bridgeRadius * 1.5) { // 브릿지 근처에서만
                                // 브릿지 유리 색상 생성
                                half4 bridgeColor = half4(0.7, 0.85, 1.0, 0.4 + bridgeStrength * 0.6);
                                return bridgeColor;
                            } else {
                                return half4(0.0, 0.0, 0.0, 0.0); // 투명
                            }
                        } else {
                            return half4(0.0, 0.0, 0.0, 0.0); // 완전 투명
                        }
                    }
                """.trimIndent()

                currentShader = RuntimeShader(bridgeShader)
                currentShader!!.setFloatUniform("resolution", width.toFloat(), height.toFloat())
                currentShader!!.setFloatUniform("pos1", 0.3f, 0.4f)
                currentShader!!.setFloatUniform("pos2", 0.7f, 0.6f)
                currentShader!!.setFloatUniform("size1", 0.3f, 0.2f)
                currentShader!!.setFloatUniform("size2", 0.3f, 0.2f)
                currentShader!!.setFloatUniform("bridgeStrength", 0.0f)

                val renderEffect = RenderEffect.createRuntimeShaderEffect(currentShader!!, "background")
                setRenderEffect(renderEffect)

                android.util.Log.d("BridgeView", "Bridge shader applied successfully")

            } catch (e: Exception) {
                android.util.Log.e("BridgeView", "Error setting up bridge shader", e)
                e.printStackTrace()
            }
        }
    }

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)
        currentShader?.setFloatUniform("resolution", w.toFloat(), h.toFloat())
    }
}