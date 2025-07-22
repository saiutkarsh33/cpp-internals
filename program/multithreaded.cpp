#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;

void say_hello(int x) {
    std::lock_guard lock(mtx);
    std::cout << std::this_thread::get_id() << "with power " << x << " Has acquired mutex ";
    std::cout << "Hello from thread!" << std::this_thread::get_id()  << std::endl;
}


int main() {
    
    std::thread t2(say_hello,2);
    std::thread t1(say_hello,1);
    std::thread t3(say_hello,3);

    t1.detach(); // wont crash if u dont join before std::terminate is called!
    t2.join();
    t3.join();


   return 0;

}