#include "packet.h"


std::string TimeHistory<>::toString() 
{
    std::string s = "Device Name: " + Packet::device + "\nDescription: " + Packet::description + "\nDate: " + std::to_string(Packet::date) +
                    "\nChannel: " + std::to_string(Sequence<T, len>::channelNr) + "\nUnit: " + Sequence<T, len>::unit +
                    "\nSensitivity: " + std::to_string(sensitivity);
    return s;
}

template <typename T, unsigned int len>
std::string Spectrum<T, len>::toString() 
{
    std::string q = [=]() { return ((this->logarithmic) ? "logarithmic" : "linear"); };
    std::string s = "Device Name: " + Packet::device + "\nDescription: " + Packet::description + "\nDate: " + std::to_string(Packet::date) +
                    "\nChannel: " + std::to_string(Sequence<T, len>::channel) + "\nUnit: " + Sequence<T, len>::Unit + "\nBuffer: " + std::to_string(Sequence<T, len>::buffer) +
                    "\nScale: " + q;
    return s;
}

std::string Alarm::toString()
{

    std::string s = "Device Name: " + Packet::device + "\nDescription: " + Packet::description + "\nDate: " + std::to_string(Packet::date) +
                    "\nChannel: " + std::to_string(channelNr) + "\nThreshold: " + std::to_string(threshold) + "\nDirection: " + std::to_string(direction);

    return s;
}