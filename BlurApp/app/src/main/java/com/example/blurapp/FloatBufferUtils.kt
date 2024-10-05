package com.example.blurapp

import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import java.nio.IntBuffer

object BufferUtils {
    fun allocateFloat(size: Int): FloatBuffer {
        val buffer = ByteBuffer.allocateDirect(size * 4)
        buffer.order(ByteOrder.nativeOrder())
        return buffer.asFloatBuffer()
    }
    fun allocateInt(size: Int): IntBuffer {
        val buffer = ByteBuffer.allocateDirect(size * 4)
        buffer.order(ByteOrder.nativeOrder())
        return buffer.asIntBuffer()
    }
}