package com.example.blurapp

import android.os.Bundle
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.example.blurapp.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private var renderer: MyGLRenderer? = null
    val binding by lazy { ActivityMainBinding.inflate(layoutInflater) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        with (binding) {
            setContentView(root)

            glSurfaceView.keepScreenOn = true // Keep screen awake till ARCore performs detection
            glSurfaceView.preserveEGLContextOnPause = true
            glSurfaceView.setEGLContextClientVersion(3)
            //glSurfaceView.setZOrderOnTop(true);
            glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0)
            //glSurfaceView.getHolder().setFormat(android.graphics.PixelFormat.RGBA_8888)
            renderer = MyGLRenderer(this@MainActivity, binding)
            glSurfaceView.setRenderer(renderer)
            glSurfaceView.renderMode = android.opengl.GLSurfaceView.RENDERMODE_CONTINUOUSLY
        }
    }
}