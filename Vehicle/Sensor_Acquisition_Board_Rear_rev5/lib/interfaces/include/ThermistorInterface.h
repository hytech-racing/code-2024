#ifndef __THERMISTOR_INTERFACE__
#define __THERMISTOR_INTERFACE__

#include <stdint.h>

#include "Filter_IIR.h"
#include "Thermistor.h"
#include "AnalogSensorsInterface.h"

template <int N>
struct TemperatureReport_s
{
    float temperatures[N]; 
};

template <int N>
struct ThermistorReadChannel_s
{
    int channel[N];
};

template <int N>
class ThermistorInterface
{
private:
// Data
    TemperatureReport_s<N> sabTemperatures_;
    Thermistors<N> sabThermistors_;
    Filter_IIR<uint16_t> sabThermFilters_[N];
    ThermistorReadChannel_s<N> channels_;
public:
// Constructors
    ThermistorInterface(const ThermistorReadChannel_s<N>& channels):
        channels_(channels),
        sabThermistors_(),
        sabThermFilters_() {};
    ThermistorInterface(const ThermistorReadChannel_s<N>& channels, const thermistor_params &params):
        channels_(channels),
        sabThermistors_(params),
        sabThermFilters_() {};
    ThermistorInterface(const ThermistorReadChannel_s<N>& channels, const float iirAlpha):
        channels_(channels),
        sabThermistors_()
    {
        for (int i = 0; i < N; i++)
        {
            sabThermFilters_[i].set_alpha(iirAlpha);
        }        
    }
    ThermistorInterface(const ThermistorReadChannel_s<N>& channels, const float* iirAlphas): 
        channels_(channels),
        sabThermistors_()
    {
        for (int i = 0; i < N; i++)
        {
            sabThermFilters_[i].set_alpha(iirAlphas[i]);
        }        
    }    
    ThermistorInterface(const ThermistorReadChannel_s<N>& channels, const thermistor_params &params, const float* iirAlphas):
        channels_(channels),
        sabThermistors_(params)
    {
        for (int i = 0; i < N; i++)
        {
            sabThermFilters_[i].set_alpha(iirAlphas[i]);
        }  
    }

    void tick(const AnalogConversionPacket_s<N> &themRaw);

    const TemperatureReport_s<N>& get()
    {
        return sabTemperatures_;
    }
};

#include "ThermistorInterface.tpp"

#endif  /* __THERMISTOR_INTERFACE__ */