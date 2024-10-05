package com.example.fireworks;

import android.annotation.SuppressLint
import android.graphics.PixelFormat
import android.opengl.GLES20
import android.opengl.GLES31
import android.opengl.GLSurfaceView
import android.opengl.Matrix
import android.os.Bundle
import android.os.SystemClock
import android.util.Log
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.example.fireworks.databinding.ActivityShapeBinding
import java.io.IOException
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import java.nio.ShortBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

/**
 * This class uses for render shape using OpenGLES
 */
class ShapeActivity : AppCompatActivity(), GLSurfaceView.Renderer {
    companion object {
        private const val VERTEX_SHADER_NAME = "shaders/vertexShader.vert"
        private const val FRAGMENT_SHADER_NAME = "shaders/fragmentShader.frag"
        private const val COORDINATES_PER_VERTEX = 2
        private const val VERTEX_STRIDE: Int = COORDINATES_PER_VERTEX * 4

        private val TAG = this::class.java.simpleName

        private val QUADRANT_COORDINATES = floatArrayOf(
            //x,    y
            -1.0f, 1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f, 1.0f
        )

        private val DRAW_ORDER = shortArrayOf(0, 1, 2, 0, 2, 3)
    }

    private val vPMatrix = FloatArray(16)
    private val projectionMatrix = FloatArray(16)
    private val viewMatrix = FloatArray(16)
    private var quadPositionHandle = -1
    private var viewProjectionMatrixHandle: Int = -1
    private var program: Int = -1
    private var timeHandle: Int =-1
    private var explosionHandler: Int = -1
    private var particleHandler: Int = -1

    private var particleNum: Float = 95f
    private var explostionNum: Float = 5f


    val binding by lazy { ActivityShapeBinding.inflate(layoutInflater) }

    /**
     * Convert float array to float buffer
     */
    private val quadrantCoordinatesBuffer: FloatBuffer = ByteBuffer.allocateDirect(QUADRANT_COORDINATES.size * 4).run {
        order(ByteOrder.nativeOrder())
        asFloatBuffer().apply {
            put(QUADRANT_COORDINATES)
            position(0)
        }
    }

    /**
     * Convert short array to short buffer
     */
    private val drawOrderBuffer: ShortBuffer = ByteBuffer.allocateDirect(DRAW_ORDER.size * 2).run {
        order(ByteOrder.nativeOrder())
        asShortBuffer().apply {
            put(DRAW_ORDER)
            position(0)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        setConfiguration()


        val seekBarListener = object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(p0: SeekBar?, progress: Int, p2: Boolean) {
                particleNum = progress.toFloat()
            }
            override fun onStartTrackingTouch(p0: SeekBar?) {}
            override fun onStopTrackingTouch(p0: SeekBar?) {}
        }

        val explosionListener = object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(p0: SeekBar?, progress: Int, p2: Boolean) {
                explostionNum = progress.toFloat()
            }
            override fun onStartTrackingTouch(p0: SeekBar?) {}
            override fun onStopTrackingTouch(p0: SeekBar?) {}
        }
        binding.particleSeekBar.setOnSeekBarChangeListener(seekBarListener)
        binding.explosionSeekBar.setOnSeekBarChangeListener(explosionListener)
    }

    /**
     * Uses for set configuration of GlSurfaceView
     */
    @SuppressLint("ClickableViewAccessibility")
    fun setConfiguration() {
        //this.keepScreenOn = true // Keep screen awake till ARCore performs detection
        //this.preserveEGLContextOnPause = true
        Log.d(TAG, "SetConfiguration")
        with (binding) {
            glSurfaceView.setEGLContextClientVersion(2)
            glSurfaceView.setZOrderOnTop(true);
            glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0)
            glSurfaceView.getHolder().setFormat(PixelFormat.RGBA_8888)
            glSurfaceView.setRenderer(this@ShapeActivity)
            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
        }
    }

    /**
     * Called when surface is created or recreated
     */
    override fun onSurfaceCreated(p0: GL10?, p1: EGLConfig?) {
        // Set GL clear color to black.
        Log.d(TAG, " onSurfaceCreated")
        GLES31.glClearColor(0f, 0f, 0f, 0f) // 투명

        // Prepare the rendering objects. This involves reading shaders, so may throw an IOException.
        try {
            val vertexShader =
                ShaderUtil.loadGLShader(
                    TAG, this, GLES20.GL_VERTEX_SHADER,
                    VERTEX_SHADER_NAME
                )
            val fragmentShader =
                ShaderUtil.loadGLShader(
                    TAG, this, GLES20.GL_FRAGMENT_SHADER,
                    FRAGMENT_SHADER_NAME
                )

            program = GLES31.glCreateProgram()
            GLES31.glAttachShader(program, vertexShader)
            GLES31.glAttachShader(program, fragmentShader)
            GLES31.glLinkProgram(program)
            GLES31.glUseProgram(program)

            ShaderUtil.checkGLError(TAG, "Program created")

            //Quadrant position handler
            quadPositionHandle = GLES31.glGetAttribLocation(program, "a_Position")

            explosionHandler = GLES31.glGetUniformLocation(program, "num_explosion")
            particleHandler = GLES31.glGetUniformLocation(program, "num_particle")


            timeHandle = GLES31.glGetUniformLocation(program, "u_time")
            viewProjectionMatrixHandle = GLES31.glGetUniformLocation(program, "uVPMatrix")

            ShaderUtil.checkGLError(TAG, "Program parameters")
        } catch (e: IOException) {
        }
    }

    /**
     * Called after the surface is crated and whenever surface size changes
     */
    override fun onSurfaceChanged(p0: GL10?, width: Int, height: Int) {
        GLES20.glViewport(0, 0, width, height)

        val ratio: Float = width.toFloat() / height.toFloat()

        // this projection matrix is applied to object coordinates
        // in the onDrawFrame() method
        Matrix.frustumM(projectionMatrix, 0, -ratio, ratio, -1f, 1f, 3f, 7f)
    }

    /**
     * Uses for draw the current frame
     */
    override fun onDrawFrame(p0: GL10?) {
        // Use the cGL clear color specified in onSurfaceCreated() to erase the GL surface.
        //GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT or GLES31.GL_DEPTH_BUFFER_BIT)
        GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT)

        // Set the camera position (View matrix)
        Matrix.setLookAtM(viewMatrix, 0, 0f, 0f, 3f, 0f, 0f, 0f, 0f, 1.0f, 0.0f)
        // Calculate the projection and view transformation
        Matrix.multiplyMM(vPMatrix, 0, projectionMatrix, 0, viewMatrix, 0)

        GLES31.glUniform1f(timeHandle, (SystemClock.elapsedRealtime() * 0.001f) % 1000)
        GLES31.glUniform1f(explosionHandler, explostionNum)
        GLES31.glUniform1f(particleHandler, particleNum)

        //Log.d(TAG, "uniform : ${explostionNum} , ${particleNum}")
        try {
            GLES31.glUseProgram(program)

            // Pass the projection and view transformation to the shader
            GLES31.glUniformMatrix4fv(viewProjectionMatrixHandle, 1, false, vPMatrix, 0)

            //Pass quadrant position to shader
            GLES31.glVertexAttribPointer(
                quadPositionHandle,
                COORDINATES_PER_VERTEX,
                GLES31.GL_FLOAT,
                false,
                VERTEX_STRIDE,
                quadrantCoordinatesBuffer
            )

            // Enable vertex arrays
            GLES31.glEnableVertexAttribArray(quadPositionHandle)

            GLES31.glDrawElements(
                GLES31.GL_TRIANGLES,
                DRAW_ORDER.size,
                GLES31.GL_UNSIGNED_SHORT,
                drawOrderBuffer
            )

            // Disable vertex arrays
            GLES31.glDisableVertexAttribArray(quadPositionHandle)

            ShaderUtil.checkGLError(TAG, "After draw")
        } catch (t: Throwable) {
            // Avoid crashing the application due to unhandled exceptions.
        }
    }
}