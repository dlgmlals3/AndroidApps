package com.example.mykids

import android.Manifest
import android.app.AlertDialog
import android.app.Dialog
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.provider.MediaStore
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.ImageButton
import android.widget.ImageView
import android.widget.Toast
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.core.view.get
import com.example.mykids.databinding.ActivityMainBinding
import com.example.mykids.databinding.DialogBrushSizeBinding

class MainActivity : AppCompatActivity() {
    //private var drawingView: DrawingView? = null
    private val binding by lazy { ActivityMainBinding.inflate(layoutInflater)}
    private val bindingBrush by lazy { DialogBrushSizeBinding.inflate(layoutInflater)}

    private val TAG: String = "MainActivity"

    private var mImageButtonCurrentPaint : ImageButton ? = null
    val openGalleryLauncher : ActivityResultLauncher<Intent> =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) {
            result ->
            if (result.resultCode == RESULT_OK && result.data != null) {
                val imageBackground: ImageView = binding.ivBackground
                imageBackground.setImageURI(result.data?.data)
            }
        }


    val requestPermission: ActivityResultLauncher<Array<String>> =
        registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) {
            permissions ->
            permissions.entries.forEach {
                val permissionName = it.key
                val isGranted = it.value
                if (isGranted) {
                    Toast.makeText(
                        this@MainActivity,
                        "Permission granted now you can read the storage files.",
                        Toast.LENGTH_LONG
                    ).show()

                    val pickIntent = Intent(Intent.ACTION_PICK,
                        MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
                    openGalleryLauncher.launch(pickIntent)

                } else {
                    if (permissionName == Manifest.permission.READ_EXTERNAL_STORAGE) {
                        Toast.makeText(
                            this@MainActivity,
                            "Oops you just denied the permission.",
                            Toast.LENGTH_LONG
                        ).show()
                    }
                }
            }
        }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        val linearLayoutPaintColor = binding.paintLinearLayout
        mImageButtonCurrentPaint = linearLayoutPaintColor[1] as ImageButton
        mImageButtonCurrentPaint?.setImageDrawable(
            ContextCompat.getDrawable(this, R.drawable.pallet_pressed)
        )


        with (binding) {
            drawingView.setSizeForBrush(10.toFloat())
            drawingView.setColor("#4DFFCC99")
            ibBrush.setOnClickListener {
                ShowBrushSizeChooserDialog()
            }

            ibGallery.setOnClickListener {
                requestStoragePermission()
            }

            ibUndo.setOnClickListener {
                drawingView.onClickUndo()
            }
        }

    }

    private fun ShowBrushSizeChooserDialog() {
        var brushDialog = Dialog(this)
        // 뷰는 오직 하나의 부모뷰만 가질수 있다.
        if (bindingBrush.root.parent != null) {
            //Log.d(TAG, "Brush : ${bindingBrush.root.parent} , ${brushDialog}")
            brushDialog.setTitle("Brush size :  ")
            ((bindingBrush.root.parent) as ViewGroup).removeView(bindingBrush.root)
        }
        brushDialog.setContentView(bindingBrush.root)
        brushDialog.setTitle("Brush size : " )

        bindingBrush.ibSmallBrush.setOnClickListener {
            binding.drawingView.setSizeForBrush(10.toFloat())
            brushDialog.dismiss()
        }
        bindingBrush.ibMediumBrush.setOnClickListener {
            binding.drawingView.setSizeForBrush(20.toFloat())
            brushDialog.dismiss()
        }
        bindingBrush.ibLargeBrush.setOnClickListener {
            binding.drawingView.setSizeForBrush(30.toFloat())
            brushDialog.dismiss()
        }
        brushDialog.show()
    }
    fun paintClicked(view: View) {
        if (view !== mImageButtonCurrentPaint) {
            val imageButton = view as ImageButton
            val colorTag = imageButton.tag.toString()
            binding.drawingView?.setColor(colorTag)
            imageButton.setImageDrawable(
                ContextCompat.getDrawable(this, R.drawable.pallet_pressed)
            )
            mImageButtonCurrentPaint?.setImageDrawable(
                ContextCompat.getDrawable(this, R.drawable.pallet_normal))
            mImageButtonCurrentPaint = view
        }
        // Toast.makeText(this, "clicked paint", Toast.LENGTH_LONG).show()
    }

    private fun requestStoragePermission() {
        if (ActivityCompat.shouldShowRequestPermissionRationale(
            this,
            Manifest.permission.READ_EXTERNAL_STORAGE)
        ) {
            showRationaleDialog("Kids Drawing App", "Kids Drawing App"
                    + "needs to Acceess your External Straoge")
        } else {
            requestPermission.launch(arrayOf(
                Manifest.permission.READ_EXTERNAL_STORAGE
                // TODO - add writing external storage permission
            ))
        }
    }

    private fun showRationaleDialog(
        title:String,
        message: String
    ) {
        val builder: AlertDialog.Builder = AlertDialog.Builder(this)
        builder.setTitle(title)
            .setMessage(message)
            .setPositiveButton("Cancel") { dialog, _->
                dialog.dismiss()
            }
        builder.create().show()
    }
}