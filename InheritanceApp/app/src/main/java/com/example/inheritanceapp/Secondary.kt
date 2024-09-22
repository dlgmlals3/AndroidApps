package com.example.inheritanceapp

// Derrived Class of BasicClass
// Inheirts Base class
class Secondary : BasicClass() {
    override fun role() {
        super.role()
        println("Knight of the House of BaseClass")
    }
}