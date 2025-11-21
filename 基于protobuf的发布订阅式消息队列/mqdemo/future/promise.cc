#include <iostream>
#include <thread>
#include <future>

//通过在线程中对promise对象设置数据，其他线程中通过future获取设置数据的方式实现获取异步任务执行结果的功能
void Add(int num1, int num2, std::promise<int> &prom) {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    prom.set_value(num1 + num2);
    return ;
}

int main()
{
    std::promise<int> prom;

    std::future<int> fu = prom.get_future();

    std::thread thr(Add, 11, 22, std::ref(prom));
    int res = fu.get();
    std::cout << "sum: " << res << std::endl;
    thr.join();
    return 0;
}