package com.example.basiclayout

import android.os.Bundle
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.basiclayout.databinding.ActivityMainBinding
import com.example.basiclayout.databinding.ItemRecyclerBinding
import java.text.SimpleDateFormat


class MainActivity : AppCompatActivity() {
    val binding by lazy { ActivityMainBinding.inflate(layoutInflater) }
    val TAG = "MainActivity"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        // 1. 데이터를 불러온다
        val data = loadData()
        // 2. 아답터를 생성한다.
        val customAdapter = CustomAdapter(data)
        // 3. 화면의 recyclerView와 연결
        binding.recycleView.adapter = customAdapter
        // 4. 레이아웃 매니저 설정
        binding.recycleView.layoutManager = LinearLayoutManager(this)
    }

    fun loadData() : MutableList<Memo> {
        val memoList = mutableListOf<Memo>()
        for (no in 1 .. 100) {
            val title = "android is $no"
            val date = System.currentTimeMillis()
            val memo = Memo(no, title, date)
            memoList.add(memo)
        }
        return memoList
    }
}

class CustomAdapter(val listData : MutableList<Memo>) : RecyclerView.Adapter<CustomAdapter.Holder>() {
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): Holder {
        val binding = ItemRecyclerBinding.inflate(LayoutInflater.from(parent.context), parent, false)
        return Holder(binding)
    }

    override fun getItemCount(): Int = listData.size

    override fun onBindViewHolder(holder: Holder, position: Int) {
        // 1. 사용할 데이터를 꺼내고
        val memo = listData.get(position)
        // 2. 홀더에 데이터를 전달한다.
        holder.setMemo(memo)
    }

    class Holder(val binding:ItemRecyclerBinding) : RecyclerView.ViewHolder(binding.root) {
        lateinit var currentMemo:Memo
        init {// 클릭처리는 init 에서만 해야 한다.
            binding.root.setOnClickListener {
                val title=  binding.textTitle.text
                Toast.makeText(binding.root.context,
                     "클릭된 아이템 ${currentMemo.title}", Toast.LENGTH_SHORT).show()
            }
        }
        // 3. 받은 데이터를 화면에 출력한다.
        fun setMemo(memo: Memo) {
            currentMemo = memo
            with(binding) {
                textNo.text = "${memo.no}"
                textTitle.text = "${memo.title}"
                val sdf = SimpleDateFormat("yyyy-mm-dd")
                val formattedDate = sdf.format(memo.timestamp)
                textDate.text = formattedDate

            }
        }

    }
}