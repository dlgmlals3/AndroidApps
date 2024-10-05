package com.example.blurapp

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.BitmapFactory
import android.opengl.GLES20
import android.opengl.GLES31
import android.opengl.GLSurfaceView
import android.opengl.GLUtils
import android.opengl.Matrix
import android.util.Log
import android.view.LayoutInflater
import android.widget.SeekBar
import com.example.blurapp.databinding.ActivityMainBinding

import java.io.IOException
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MyGLRenderer(private val context: Context, private val binding : ActivityMainBinding) : GLSurfaceView.Renderer {
    companion object {
        private const val VERTEX_SHADER_NAME = "shaders/vertexShader.vert"
        private const val FRAGMENT_SHADER_NAME = "shaders/fragmentShader.frag"
        private const val COORDINATES_PER_VERTEX = 2
        private const val VERTEX_STRIDE: Int = 0;//COORDINATES_PER_VERTEX * 4

        private val TAG = this::class.java.simpleName

        private val squareCoords = floatArrayOf(
            -1f,  1f,  // top left
            -1f, -1f,  // bottom left
            1f,  1f,  // top right
            1f, -1f, // bottom right
        )
        private val textureCoords  = floatArrayOf(
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
        )
        private val indexOrder = intArrayOf(
            0, 1, 2,
            2, 1, 3)
    }

    private var program: Int = -1
    private val vPMatrix = FloatArray(16)
    private val projectionMatrix = FloatArray(16)
    private val viewMatrix = FloatArray(16)
    private val vaoIds = IntArray(1)


    // uniform handle
    private var viewProjectionMatrixHandle: Int = -1
    private var customHandle : Int = -1

    private var textureId : Int = -1
    private var textureHandle : Int = -1
    private var customValue : Float = 0f

    private val vertexBuffer = BufferUtils.allocateFloat(squareCoords.size)
    private val textureBuffer = BufferUtils.allocateFloat(textureCoords.size)
    private val indexBuffer = BufferUtils.allocateInt(indexOrder.size)


    init {
        Log.d(TAG, "INIT")
        vertexBuffer.put(squareCoords).position(0)
        textureBuffer.put(textureCoords).position(0)
        indexBuffer.put(indexOrder).position(0)

        val seekBarListener = object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(p0: SeekBar?, progress: Int, p2: Boolean) {
                customValue = progress.toFloat()
            }
            override fun onStartTrackingTouch(p0: SeekBar?) {}
            override fun onStopTrackingTouch(p0: SeekBar?) {}
        }
        binding.seekBar.setOnSeekBarChangeListener(seekBarListener)
    }

    private fun loadTexture(resourceId: Int): Int {
        Log.d(TAG, "loadTextures")
        val textureIds = IntArray(1)
        GLES31.glGenTextures(1, textureIds, 0)

        if (textureIds[0] != 0) {
            val bitmap = BitmapFactory.decodeResource(context.resources, resourceId)
            Log.d(TAG, "texture info : ${bitmap.width} ${bitmap.height} ${bitmap.byteCount}")

            GLES31.glBindTexture(GLES31.GL_TEXTURE_2D, textureIds[0])
            GLES31.glTexParameteri(GLES31.GL_TEXTURE_2D, GLES31.GL_TEXTURE_MIN_FILTER, GLES31.GL_LINEAR)
            GLES31.glTexParameteri(GLES31.GL_TEXTURE_2D, GLES31.GL_TEXTURE_MAG_FILTER, GLES31.GL_LINEAR)
            GLES31.glTexParameteri(GLES31.GL_TEXTURE_2D, GLES31.GL_TEXTURE_WRAP_S, GLES31.GL_CLAMP_TO_EDGE)
            GLES31.glTexParameteri(GLES31.GL_TEXTURE_2D, GLES31.GL_TEXTURE_WRAP_T, GLES31.GL_CLAMP_TO_EDGE)

            GLES31.glGenerateMipmap(GLES31.GL_TEXTURE_2D)
            GLUtils.texImage2D(GLES31.GL_TEXTURE_2D, 0, bitmap, 0)

            bitmap.recycle() // 비트맵 메모리 해제
            return textureIds[0]
        }
        return -1
    }

    override fun onSurfaceCreated(p0: GL10?, p1: EGLConfig?) {
        Log.d(TAG, "onSurfaceCreated")
        GLES31.glClearColor(0f, 0f, 0f, 1f)

        try {
            // Program Settings
            val vertexShader =
                ShaderUtil.loadGLShader(
                    TAG, context, GLES31.GL_VERTEX_SHADER,
                    VERTEX_SHADER_NAME
                )
            val fragmentShader =
                ShaderUtil.loadGLShader(
                    TAG, context, GLES31.GL_FRAGMENT_SHADER,
                    FRAGMENT_SHADER_NAME
                )

            program = GLES31.glCreateProgram()
            GLES31.glAttachShader(program, vertexShader)
            GLES31.glAttachShader(program, fragmentShader)
            GLES31.glLinkProgram(program)
            GLES31.glUseProgram(program)
            ShaderUtil.checkGLError(TAG, "Program created")

            // uniform get handle
            customHandle = GLES31.glGetUniformLocation(program, "kernelSize")
            //viewProjectionMatrixHandle = GLES31.glGetUniformLocation(program, "uVPMatrix")
            ShaderUtil.checkGLError(TAG, "Program parameters")

            // 텍스처 바인딩
            textureHandle = GLES31.glGetUniformLocation(program, "uTexture")
            textureId = loadTexture(R.drawable.img_1)


            val maxVertexAttribs = IntArray(1)
            GLES31.glGetIntegerv(GLES31.GL_MAX_VERTEX_ATTRIBS, maxVertexAttribs, 0)
            Log.d("OpenGL", "MAX_VERTEX_ATTRIBS: " + maxVertexAttribs[0])

        } catch (e: IOException) {
        }
    }

    override fun onSurfaceChanged(p0: GL10?, width: Int, height: Int) {
        Log.d(TAG, "onSurfaceChanged")
        GLES20.glViewport(0, 0, width, height)
        val ratio: Float = width.toFloat() / height.toFloat()
        Matrix.frustumM(projectionMatrix, 0, -ratio, ratio, -1f, 1f, 3f, 7f)
    }

    override fun onDrawFrame(p0: GL10?) {
        // Log.d(TAG, "onDrawFrame")
        GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT or GLES31.GL_DEPTH_BUFFER_BIT)
        Matrix.setLookAtM(viewMatrix, 0, 0f, 0f, 3f, 0f, 0f, 0f, 0f, 1.0f, 0.0f)
        Matrix.multiplyMM(vPMatrix, 0, projectionMatrix, 0, viewMatrix, 0)

        try {
            GLES31.glUseProgram(program)

            // Uniform binding
            GLES31.glUniform1f(customHandle, customValue)
            GLES31.glUniformMatrix4fv(viewProjectionMatrixHandle, 1, false, vPMatrix, 0)
            ShaderUtil.checkGLError(TAG, "After Uniform binding")

            // 정점 속성 가져오기
            val positionHandle = GLES31.glGetAttribLocation(program, "vPosition")
            val texCoordHandle = GLES31.glGetAttribLocation(program, "aTexCoordinate")


            // 정점 버퍼 바인딩
            if (positionHandle != -1) {
                //Log.d(TAG, "dlgmlals3 1")
                GLES31.glVertexAttribPointer(positionHandle, 2, GLES31.GL_FLOAT, false, 2 * 4, vertexBuffer)
                GLES31.glEnableVertexAttribArray(positionHandle)
            }
            // 텍스처 좌표 버퍼 바인딩
            if (texCoordHandle != -1) {
                GLES31.glVertexAttribPointer(texCoordHandle, 2, GLES31.GL_FLOAT, false, 2 * 4, textureBuffer)
                GLES31.glEnableVertexAttribArray(texCoordHandle)
            }

            GLES31.glActiveTexture(GLES31.GL_TEXTURE0)
            GLES31.glBindTexture(GLES31.GL_TEXTURE_2D, textureId)
            GLES31.glUniform1i(textureHandle, 0)
            ShaderUtil.checkGLError(TAG, "Texture Error")

            // 도형 그리기
            GLES31.glDrawElements(
                GLES31.GL_TRIANGLES,
                indexOrder.size,
                GLES31.GL_UNSIGNED_INT,
                indexBuffer)

            // 정점 속성 비활성화
            if (positionHandle != -1) GLES31.glDisableVertexAttribArray(positionHandle)
            if (texCoordHandle != -1) GLES31.glDisableVertexAttribArray(texCoordHandle)
            ShaderUtil.checkGLError(TAG, "After draw")
        } catch (t: Throwable) {
            // Avoid crashing the application due to unhandled exceptions.
        }
    }
}