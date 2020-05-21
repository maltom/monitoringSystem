#include <iostream>
#include "packet.h"
#include <memory>
#include <fstream>

int main()
{

    std::srand(time(NULL));

    double frequency = 400;
    double time = 10;
    int numberOfSamples = static_cast<int>(frequency * time);

    //i
    std::unique_ptr<Packet> channel1 = std::make_unique<TimeHistory<double>>(numberOfSamples, "Device1", "Registering device", numberOfSamples, 1, "V", 0.001, 0.001);
    std::unique_ptr<Packet> channel2 = std::make_unique<TimeHistory<double>>(numberOfSamples, "Device1", "Registering device", numberOfSamples, 2, "V", 0.001, 0.001);
    std::unique_ptr<Packet> channel3 = std::make_unique<TimeHistory<double>>(numberOfSamples, "Device1", "Registering device", numberOfSamples, 3, "V", 0.001, 0.001);
    std::unique_ptr<Packet> channel4 = std::make_unique<TimeHistory<double>>(numberOfSamples, "Device1", "Registering device", numberOfSamples, 4, "V", 0.001, 0.001);
    //Packet* test = new TimeHistory<int,100>;
    //ii
    try
    {
        Gauss<double>(channel1.get(), 3, 0.3);
        Gauss<double>(channel2.get(), 6, 0.3);
        Gauss<double>(channel3.get(), 8, 0.1);
        Gauss<double>(channel4.get(), 3, 0.3);
    }
    catch (std::bad_cast e)
    {
        std::cerr << e.what() << std::endl;
    }

    std::unique_ptr<Packet> bufor = std::make_unique<TimeHistory<double>>();

    //iii
    *bufor = *channel1;
    *channel1 = *channel2;
    *channel2 = *channel1;
    //iv
    *channel2 = *channel2 + *channel3;
    //v
    *channel1 = *channel1 / (*channel4);
    //vi
    std::vector<std::pair<double, double>> fourierComp1;
    std::vector<std::pair<double, double>> fourierComp2;
    std::vector<std::pair<double, double>> fourierComp3;
    std::vector<std::pair<double, double>> fourierComp4;
    std::vector<std::pair<double, double>> fourierAmp1;
    std::vector<std::pair<double, double>> fourierAmp2;
    std::vector<std::pair<double, double>> fourierAmp3;
    std::vector<std::pair<double, double>> fourierAmp4;
    calcFFT<double>(channel1.get(), 1, fourierComp1);
    calcFFT<double>(channel2.get(), 1, fourierComp2);
    calcFFT<double>(channel3.get(), 1, fourierComp3);
    calcFFT<double>(channel4.get(), 1, fourierComp4);
    calcFFT<double>(channel1.get(), 0, fourierAmp1);
    calcFFT<double>(channel2.get(), 0, fourierAmp2);
    calcFFT<double>(channel3.get(), 0, fourierAmp3);
    calcFFT<double>(channel4.get(), 0, fourierAmp4);
//vii
    std::ofstream outFile("output.txt", std::ios::app | std::ios::out);


    if (outFile.is_open())
    {
        outFile << fourierComp1;
        outFile << fourierComp2;
        outFile << fourierComp3;
        outFile << fourierComp4;

        outFile << fourierAmp1;
        outFile << fourierAmp2;
        outFile << fourierAmp3;
        outFile << fourierAmp4;
    }
    outFile.close();
    //viii
    std::cout<<RMS<double>(channel1.get())<<std::endl;
    std::cout<<RMS<double>(channel2.get())<<std::endl;
    std::cout<<RMS<double>(channel3.get())<<std::endl;
    std::cout<<RMS<double>(channel4.get())<<std::endl;
    std::cout<<RMSinternet<double>(channel1.get())<<std::endl; //w internecie był inny wzór
    std::cout<<RMSinternet<double>(channel2.get())<<std::endl;
    std::cout<<RMSinternet<double>(channel3.get())<<std::endl;
    std::cout<<RMSinternet<double>(channel4.get())<<std::endl;

    return 0;
}