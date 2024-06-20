#ifndef __THERMISTOR_INTERFACE__
#define __THERMISTOR_INTERFACE__

#include <stdint.h>

#include "Filter_IIR.h"
#include "Thermistor.h"

template <int N>
struct TemperatureReport_s
{
    float temperatures_[N]; 
};

template <int N>
class ThermistorInterface
{
private:
// Data
    TemperatureReport_s<N> sabTemperatures_;
    Thermistors<N> sabThermistors_;
    FilterIIRMulti<uint16_t, N> sabThermFilters_;
public:
// Constructors
    ThermistorInterface():
        sabThermistors_(),
        sabThermFilters_() {};
    ThermistorInterface(const thermistor_params &params):
        sabThermistors_(params),
        sabThermFilters_() {};
    ThermistorInterface(const float[] iirAlphas): sabThermistors_()
    {
        for (int i = 0; i < N; i++)
        {
            sabThermFilters_.setAlphas(i, iirAlphas[i]);
        }        
    }    
    ThermistorInterface(const thermistor_params &params, const float[] iirAlphas): sabThermistors_(params)
    {
        for (int i = 0; i < N; i++)
        {
            sabThermFilters_.setAlphas(i, iirAlphas[i]);
        }  
    }

    void tick(AnalogConversionPacket_s<N> &themRaw);

    const TemperatureReport_s<N>& get()
    {
        return sabTemperatures_;
    }
};

#include "ThermistorInterface.tpp"

#endif  /* __THERMISTOR_INTERFACE__ */