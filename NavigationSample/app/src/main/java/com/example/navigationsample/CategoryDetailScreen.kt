package com.example.navigationsample


import android.icu.util.ULocale
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.aspectRatio
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.semantics.Role.Companion.Image
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp

// category 안됨..... 다운받아야될듯?
@Composable
fun CategoryDetailScreen(category : ULocale.Category) {
    Column(modifier = Modifier.fillMaxSize()
        .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(text = category.toString(), textAlign = TextAlign.Center)
        Image(
            painter = rememberAsyncImagePainter(category.strCategoryThumb),
            contentDescription = "{category.strCategory} Thumbail",
            modifier = Modifier
                .wrapContentSize()
                .aspectRatio(1f)
        )

        Text(text = category.strCategoryDescription,
            textAlign = TextAlign.Justify,
            modifier = Modifier.verticalScroll(rememberScrollState())
    }
}