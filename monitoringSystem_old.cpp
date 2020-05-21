#include <iostream>
#include "packet_old.h"
#include <memory>

int main()
{


    std::unique_ptr<Packet> test(new TimeHistory<int,2>);


    std::cout<<test->toString()<<std::endl<<std::endl;

    std::unique_ptr<Packet> test2(new Spectrum<int,2>);

    std::cout<<test2->toString()<<std::endl<<std::endl;

    std::unique_ptr<Packet> test3(new Alarm);

    std::cout<<test3->toString()<<std::endl<<std::endl;

    return 0;
}
