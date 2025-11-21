#include <iostream>
#include <thread>
#include <future>
#include <memory>
//pakcaged_task的使用
//   pakcaged_task 是一个模板类，实例化的对象可以对一个函数进行二次封装，
//pakcaged_task可以通过get_future获取一个future对象，来获取封装的这个函数的异步执行结果

int Add(int num1, int num2) {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return num1 + num2;
}

int main()
{
    //std::packaged_task<int(int,int)> task(Add);
    //std::future<int> fu = task.get_future();

    //task(11, 22);  task可以当作一个可调用对象来调用执行任务
    //但是它又不能完全的当作一个函数来使用
    //std::async(std::launch::async, task, 11, 22);
    //std::thread thr(task, 11, 22);

    //但是我们可以把task定义成为一个指针，传递到线程中，然后进行解引用执行
    //但是如果单纯指针指向一个对象，存在生命周期的问题，很有可能出现风险
    //思想就是在堆上new对象，用智能指针管理它的生命周期
    auto ptask = std::make_shared<std::packaged_task<int(int,int)>>(Add);
    std::future<int> fu = ptask->get_future();
    std::thread thr([ptask](){
        (*ptask)(11, 22);
    });

    int sum = fu.get();
    std::cout << sum << std::endl;
    thr.join();
    return 0;
}