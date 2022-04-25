#include<iostream>

using namespace std;
class A{
    public:
    void print(){cout<<"A"<<endl;}
};
class B{
    public:
    void print2(){cout<<"B"<<endl;}
};

template<typename T>
void fuck(T a){
    a.print();
}


int main(){
    A a;
    B b;
    fuck(a);
    fuck(b);
}