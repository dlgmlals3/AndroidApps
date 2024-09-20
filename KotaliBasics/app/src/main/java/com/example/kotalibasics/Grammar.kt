package com.example.kotalibasics

class Book (
        var title:String = "Unknow",
        var author:String= "Unknow",
        var yearPublished:Int = 2024) {
    fun init() {}
}

data class BookData (
    val title : String,
    val author : String,
    val yearPublished : Int) {
}

class BankAccount(var accountHolder:String, var balance:Double) {
    private val transactionHistory = mutableListOf<String>()
    private val test2 = mutableListOf<String>()
    val testMember = mutableListOf<Int>()

    fun deposit(amouunt : Double) {

    }
}
fun main() {
    // 1. Class
    var book = Book()
    println(book.title + " " + book.author + " " + book.yearPublished)
    var customBook = Book("1", "2", 1986)
    println("${customBook.title}")

    /*
    2. Data Class
    데이타 클래스 내부에 함수가 들어갈수 없다
    */
    println("2. Data Class")
    val data = BookData("123", "lee", 1984)
    println("${data.author} ${data.yearPublished}")

    // 3. Immutable list
    println("3. Immutable List")
    val shoppingList = listOf("Processor", "Ram", "Graphics Car", "SSD")

    //4. mutable list - you can add item later and modify
    println("4. Mutable List")
    var mutableList = mutableListOf("Processor", "Ram")
    // Adding items to lists
    mutableList.add("dl1"); mutableList.add("dl2");
    println(mutableList)
    mutableList.removeAt(1)
    println(mutableList)

    // Replace item
    mutableList.set(1, "Water Cooling")
    println(mutableList)
    mutableList[1] = "Water 2"
    println(mutableList)

    // Contains
    if (mutableList.contains("Water 2")) {
        println("found Water 2")
    } else {
        println("not found ")
    }

    // 5. For State
    println("5. For State")
    val slist = mutableListOf("cake", "apple", "stro")
    for (item in slist) {
        println(item)
        if (item == "cake") break;
    }
    slist.removeLast()
    println(slist)

    /*
     6. 0 until
     인덱스를 얻는다.
     == for(int i=0; i<size; i++)
     */
    println("0 until")
    for (index in 0 until slist.size)
    {
        println("item : ${slist[index]} is at index $index")
    }
    for (index in 0 until 4) {  }
    for (index in 0..4) { }
    
    /*
    class member 변수
     */
    val bank = BankAccount("dlgmlals", 34.5)
    bank.testMember.add(23)
    println(bank.testMember)
}

