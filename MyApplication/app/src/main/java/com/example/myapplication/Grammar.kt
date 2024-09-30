package com.example.myapplication



class Grammar {
    /* 지연 초기화
     쓰지도 않는 메모리 미리 할당하지 않음
     기본형에는 사용 불가
    */
    var name:String = "1234"
    lateinit var name2: String
    lateinit var person:Person
    /*
    Lazy
    val 변수명  by lazy (변수에 들어갈 클래스 생성자)
    값을 바꿀수 없음. lateinit 과 차이
    */
    val age by lazy { Person() }
    val age2 by lazy { 12 }

    fun test() {
        person = Person()
    }
}
data class Person (
    var name : String ="",
    var age : String ="",
    var address : String =""
)

class SeoulPeople {
    var persons = mutableListOf<Person>()
    init {
        persons.add(Person("123", "123", "123"))
        persons.add(Person("456", "456", "456"))
        persons.add(Person("789", "789", "789"))
    }
}

class Scope {
    fun studyRun() {
        var phones = mutableListOf("010-343-2323", "010-422-4233")
        var names = mutableListOf("dlgmlals", "dltjtjtj3")
        var list = mutableListOf(1,2,3,4,5,6,8,9)

        var p1 = Person("123", )

        val seoulPeople = SeoulPeople()
        val result = seoulPeople.persons.run {
            add(Person("123", "123", "345"))
        }
        val result2 = seoulPeople.persons.let { persons->
            persons.add(Person("123", "123", "345"))
        }

    }
}

