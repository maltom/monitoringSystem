#pragma once
#include <iostream>
#include <string>
#include <array>
#include <cmath>
#include <random>
#include <limits>
#include <numeric>
#include <vector>
#include <utility>
#include <fftw3.h>
#include <exception>

class Packet;
template <typename T>
class TimeHistory;
template <typename T>
void Gauss(TimeHistory<T> &A, T mu, T sigma1);
template <typename T>
void Gauss(Packet *A, T mu, T sigma1);
template <typename T>
void calcFFT(Packet *A, unsigned int mode, std::vector<std::pair<double, double>> &output);
template <typename W>
double RMS(Packet *A);

class Packet
{
protected:
    std::string device;
    std::string description;
    long date; //seconds from beginning

    Packet(std::string dev = "default_dev", std::string desc = "default_descr", long date = 0) : device(dev), description(desc), date(date) {}

public:
    virtual int getLength() = 0;
    virtual std::string toString() const = 0;
    virtual std::string druczek() const = 0;
    virtual ~Packet() {}
    friend std::ostream &operator<<(std::ostream &out, Packet &A);
    virtual Packet &operator+(Packet &A) = 0;
    virtual Packet &operator=(Packet &A) = 0;
    virtual Packet &operator/(Packet &A) = 0;
};

template <typename T = int>
class Sequence : public Packet
{
protected:
    int channelNr;
    std::string unit;
    double resolution;
    int len = 2;
    std::vector<T> buffer;
    Sequence(int len = 2, std::string dev = "default_dev", std::string desc = "default_descr", long date = 0,
             int chan = 0, std::string unit = "none", double res = 1, std::vector<T> buf = {0, 0})
        : Packet(dev, desc, date), len(len), channelNr(chan), unit(unit), resolution(res), buffer(buf)
    {
        buffer.resize(len);
    }

public:
    int getLength() override { return len; }
    void fillBuffer(T word, int index)
    {
        buffer[index] = word;
    }
    virtual std::string toString() const = 0;
    virtual std::string druczek() const = 0;
    virtual Packet &operator+(Packet &A) = 0;
    virtual Packet &operator=(Packet &A) = 0;
    virtual Packet &operator/(Packet &A) = 0;
    virtual ~Sequence() {}
};

template <typename T = int>
class TimeHistory : public Sequence<T>
{
private:
    double sensitivity;

public:
    TimeHistory(int len = 2, std::string dev = "default_dev", std::string desc = "default_descr", long date = 0,
                int chan = 0, std::string unit = "none", double res = 1, double sens = 1, std::vector<T> buf = {0, 0})
        : Sequence<T>(len, dev, desc, date, chan, unit, res, buf), sensitivity(sens) {}

    std::string toString() const override
    {
        std::string s = "Device Name: " + Packet::device + "\nDescription: " + Packet::description + "\nDate: " + std::to_string(Packet::date) +
                        "\nChannel: " + std::to_string(Sequence<T>::channelNr) + "\nUnit: " + Sequence<T>::unit +
                        "\nSensitivity: " + std::to_string(sensitivity);
        return s;
    }
    int getLength() override { return Sequence<T>::len; }
    std::string druczek() const override
    {
        std::string s;
        for (int i = 0; i < Sequence<T>::len; ++i)
        {
            s += (std::to_string(Sequence<T>::buffer[i]) + " ");
        }
        return s;
    }
    double getSensitivity() { return sensitivity; }
    template <typename U, unsigned int len2>
    friend void Gauss(TimeHistory<U> A, T mu, T sigma1);
    template <typename Y>
    friend void Gauss(Packet *A, Y mu, Y sigma1);
    template <typename V>
    friend void calcFFT(Packet *A, unsigned int mode, std::vector<std::pair<double, double>> &output);
    template <typename W>
    friend double RMS(Packet *A);
    template <typename Z>
    friend double RMSinternet(Packet *A);
    friend std::ostream &operator<<(std::ostream &out, Packet &A);

    Packet &operator+(Packet &A) override
    {
        TimeHistory<T> &B = dynamic_cast<TimeHistory<T> &>(A);
        for (int i = 0; (i < this->buffer.size()) && (i < B.buffer.size()); ++i)
        {
            if (i >= this->buffer.size())
                this->buffer.push_back(B.buffer[i]);
            else if (i >= B.buffer.size())
            {
                Packet &C = dynamic_cast<Packet &>(*this);
                return C;
            }
            else
                this->buffer[i] += B.buffer[i];
        }
        Packet &C = dynamic_cast<Packet &>(*this);
        return C;
    }
    Packet &operator=(Packet &A) override
    {
        TimeHistory<T> &B = dynamic_cast<TimeHistory<T> &>(A);

        this->Packet::device = B.device;
        this->Packet::description = B.description;
        this->Packet::date = B.date; //seconds from beginning
        this->Sequence<T>::channelNr = B.channelNr;
        this->Sequence<T>::resolution = B.resolution;
        this->Sequence<T>::unit = B.unit;
        this->Sequence<T>::len = B.len;
        this->Sequence<T>::buffer = B.buffer;
        this->sensitivity = B.sensitivity;
        Packet &C = dynamic_cast<Packet &>(*this);
        return C;
    }
    Packet &operator/(Packet &A) override
    {
        TimeHistory<T> &B = dynamic_cast<TimeHistory<T> &>(A);
        for (int i = 0; (i < this->buffer.size()) && (i < B.buffer.size()); ++i)
        {
            if (i >= this->buffer.size())
                this->buffer.push_back(0);

            else if (i >= B.buffer.size())
            {
                Packet &C = dynamic_cast<Packet &>(*this);
                return C;
            }
            else if (B.buffer[i] != 0.0)
                this->buffer[i] /= B.buffer[i];
        }
        Packet &C = dynamic_cast<Packet &>(*this);
        return C;
    }
    ~TimeHistory() {}
};

template <typename T = int>
class Spectrum : public Sequence<T>
{
private:
    bool logarithmic;

public:
    Spectrum(int len = 2, std::string dev = "default_dev", std::string desc = "default_descr", long date = 0,
             int chan = 0, std::string unit = "none", double res = 1, std::vector<T> buf = {0, 0}, bool logar = 1)
        : Sequence<T>(len, dev, desc, date, chan, unit, res, buf), logarithmic(logar) {}

    std::string toString() const override
    {
        std::string q = [l = logarithmic]() -> std::string { return (l ? "logarithmic" : "linear"); }();

        std::string s = "Device Name: " + Packet::device + "\nDescription: " + Packet::description + "\nDate: " + std::to_string(Packet::date) +
                        "\nChannel: " + std::to_string(Sequence<T>::channelNr) + "\nUnit: " + Sequence<T>::unit +
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

template <typename T, unsigned int len = 2>
void Gauss(TimeHistory<T> &A, T mu, T sigma1)
{
    /*for (int i = 0; i < len; ++i)
    {
        std::srand(time(NULL));
        const double epsilon = std::numeric_limits<double>::min();

        T z1;
        T u1, u2;
        do
        {
            u1 = rand() * (1.0 / RAND_MAX);
            u2 = rand() * (1.0 / RAND_MAX);
        } while (u1 <= epsilon);

        double z0;
        z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2 * M_PI * u2);
        A.fillBuffer(z0, i);
    }*/

    std::default_random_engine generator(time(NULL));
    std::normal_distribution<T> distribution(mu, sigma1);
    for (int i = 0; i < len; ++i)
    {
        A.fillBuffer(distribution(generator), i);
    }
}
template <typename T>
void Gauss(Packet *A, T mu, T sigma1)
{
    TimeHistory<T> *B = dynamic_cast<TimeHistory<T> *>(A);
    /*for (int i = 0; i < B->getLength(); ++i)
    {
        const double epsilon = std::numeric_limits<double>::min();
        T z1;
        T u1, u2;
        do
        {
            u1 = rand() * (1.0 / RAND_MAX);
            u2 = rand() * (1.0 / RAND_MAX);
        } while (u1 <= epsilon);

        double z0;
        z0 = (std::sqrt(-2.0 * std::log(u1)) * std::cos(2 * M_PI * u2)) * sigma1 + mu;
        z0 = std::round(z0 * (1 / B->getSensitivity())) / (1 / B->getSensitivity());
        B->fillBuffer(z0, i);
    }*/

    std::default_random_engine generator(time(NULL));
    std::normal_distribution<T> distribution(mu, sigma1);
    for (int i = 0; i < B->getLength(); ++i)
    {
        B->fillBuffer(distribution(generator), i);
    }
}
template <typename T>
double RMS(Packet *A)
{
    TimeHistory<T> *B = dynamic_cast<TimeHistory<T> *>(A);
    double mu = std::accumulate(B->buffer.begin(), B->buffer.end(), 0) / B->buffer.size();

    double sum;
    for (auto in : B->buffer)
    {
        sum += std::pow((in-mu),2);
    }
    double sigma = std::sqrt(1.0 / (B->getLength() - 1.0) * sum);
    return sigma;
}
template <typename T>
double RMSinternet(Packet *A)
{
    TimeHistory<T> *B = dynamic_cast<TimeHistory<T> *>(A);
    double mu = std::accumulate(B->buffer.begin(), B->buffer.end(), 0) / B->buffer.size();

    double sum;
    for (auto in : B->buffer)
    {
        sum += std::pow((in),2);
    }
    double sigma = std::sqrt(1.0 / (B->getLength() - 1.0) * sum);
    return sigma;
}
//FFT function, 0 mode for amplitude, 1 for complex
template <typename T>
void calcFFT(Packet *A, unsigned int mode, std::vector<std::pair<double, double>> &output)
{
    TimeHistory<T> *B = dynamic_cast<TimeHistory<T> *>(A);
    output.clear();

    fftw_complex *in = new fftw_complex[B->getLength()];
    fftw_complex *out = new fftw_complex[B->getLength()];

    for (int i = 0; i < B->getLength(); ++i)
        in[i][0] = B->buffer[i];

    fftw_plan p = fftw_plan_dft_1d(B->getLength(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    if (mode)
    {
        std::pair<T, T> firstWord = {-888.888, 888.888}; //for printing to distinguish between amplitude and real/imaginary spectrum
        output.push_back(firstWord);
        for (int i = 0; i < B->getLength(); ++i)
        {
            std::pair<T, T> word;
            word.first = out[i][0];
            word.second = out[i][1];
            output.push_back(word);
        }
    }
    else
    {
        std::pair<T, T> firstWord;
        firstWord.first = -999.999;
        firstWord.second = -999.999;
        output.push_back(firstWord);
        for (int i = 0; i < B->getLength(); ++i)
        {
            std::pair<T, T> word;
            word.first = std::sqrt(std::pow(out[i][0], 2) + std::pow(out[i][1], 2));
            word.second = 0;
            output.push_back(word);
        }
    }
    fftw_destroy_plan(p);
    delete in;
    delete out;
    in = nullptr;
    out = nullptr;
}

std::ostream &operator<<(std::ostream &out, Packet &A)
{
    TimeHistory<double> th = dynamic_cast<TimeHistory<double> &>(A);
    out << th.device << "\n"
        << th.description << "\n"
        << th.channelNr << "\n";
    for (auto in : th.buffer)
        out << in << std::endl;
    return out;
}
std::ostream &operator<<(std::ostream &out, std::vector<std::pair<double, double>> A)
{
    if (A.empty())
        throw std::length_error("Invalid size");

    if ((A[0].first == -999.999) && (A[0].second == -999.999))
    {
        A.erase(A.begin());
        out << "Amplitude" << std::endl;
        for (auto in : A)
            out << in.first << "\n";
        return out;
    }
    else
    {
        A.erase(A.begin());
        out << "Real\t\tImaginary" << std::endl;
        for (auto in : A)
            out << in.first << "\t\t" << in.second << "\n";
        return out;
    }
}
/*template <typename T>
std::ostream &operator<<(std::ostream& out, TimeHistory<T> &th)
{
    out << th.device << std::endl
        << th.description << std::endl
        << th.channelNr << std::endl;
    for (auto in : th.buffer)
        out << in << std::endl;
    return out;
}*/
