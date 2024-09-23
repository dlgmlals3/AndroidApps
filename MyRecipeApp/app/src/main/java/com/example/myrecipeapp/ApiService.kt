package com.example.myrecipeapp

import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.http.GET

private val retrofit = Retrofit.Builder()
    .baseUrl("https://www.themealdb.com/api/json/v1/1/")
    .addConverterFactory(GsonConverterFactory.create())
    .build()
var recipeService = retrofit.create(ApiService::class.java)

interface ApiService {
    // suspend fun : 백그라운드 작업 (웹에서 로딩 데이터)
    @GET("categories.php") // it comes from themealdb.com/api.php
    suspend fun getCategories() : CategoriesResponse

}