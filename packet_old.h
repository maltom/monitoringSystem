#pragma once
#include <iostream>
#include <string>
#include <array>
class Packet
{
protected:
    std::string device;
    std::string description;
    long date; //seconds from beginning

    Packet(std::string dev = "default_dev", std::string desc = "default_descr", long date = 0) : device(dev), description(desc), date(date) {}

public:
    virtual std::string toString() const = 0;
    virtual ~Packet() {}
};

template <typename T = int, unsigned int len = 2>
class Sequence : public Packet
{
protected:
    int channelNr;
    std::string unit;
    double resolution;
    std::array<T, len> buffer;
    Sequence(std::string dev = "default_dev", std::string desc = "default_descr", long date = 0,
             int chan = 0, std::string unit = "none", double res = 1, std::array<T, len> buf = {0, 0})
        : Packet(dev, desc, date), channelNr(chan), unit(unit), resolution(res), buffer(buf) {}

public:
    virtual std::string toString() const = 0;

    virtual ~Sequence() {}
};

template <typename T = int, unsigned int len = 2>
class TimeHistory : public Sequence<T, len>
{
private:
    double sensitivity;

public:
    TimeHistory(std::string dev = "default_dev", std::string desc = "default_descr", long date = 0,
                int chan = 0, std::string unit = "none", double res = 1, std::array<T, len> buf = {0, 0}, double sens = 1)
        : Sequence<T, len>(dev, desc, date, chan, unit, res, buf), sensitivity(sens) {}

    std::string toString() const override
    {
        std::string s = "Device Name: " + Packet::device + "\nDescription: " + Packet::description + "\nDate: " + std::to_string(Packet::date) +
                        "\nChannel: " + std::to_string(Sequence<T, len>::channelNr) + "\nUnit: " + Sequence<T, len>::unit +
                        "\nSensitivity: " + std::to_string(sensitivity);
        return s;
    }

    ~TimeHistory() {}
};

template <typename T = int, unsigned int len = 2>
class Spectrum : public Sequence<T, len>
{
private:
    bool logarithmic;

public:
    Spectrum(std::string dev = "default_dev", std::string desc = "default_descr", long date = 0,
             int chan = 0, std::string unit = "none", double res = 1, std::array<T, len> buf = {0, 0}, bool logar = 1)
        : Sequence<T, len>(dev, desc, date, chan, unit, res, buf), logarithmic(logar) {}

    std::string toString() const override
    {
        std::string q = [l=logarithmic]() ->std::string { return (l ? "logarithmic" : "linear"); }();

        std::string s = "Device Name: " + Packet::device + "\nDescription: " + Packet::description + "\nDate: " + std::to_string(Packet::date) +
                        "\nChannel: " + std::to_string(Sequence<T, len>::channelNr) + "\nUnit: " + Sequence<T, len>::unit +
                        "\nScale: " + q;
        return s;
    }
    ~Spectrum() {}
};

class Alarm : public Packet
{
private:
    int channelNr;
    int threshold;
    int direction;

public:
    Alarm(std::string dev = "default_dev", std::string desc = "default_descr", long date = 0, int chan = 0, int thresh = 0, int dir = 0)
        : Packet(dev, desc, date), channelNr(chan), threshold(thresh), direction(dir) {}

    std::string toString() const override
    {
        std::string s = "Device Name: " + Packet::device + "\nDescription: " + Packet::description + "\nDate: " + std::to_string(Packet::date) +
                        "\nChannel: " + std::to_string(channelNr) + "\nThreshold: " + std::to_string(threshold) + "\nDirection: " + std::to_string(direction);

        return s;
    }

    ~Alarm() {}
};
