#include "ThermistorInterface.h"

template <int N>
void ThermistorInterface<N>::tick(const AnalogConversionPacket_s<N> &thermRaw)
{
    for (int i = 0; i < N; i++)
    {
        uint16_t filteredRaw = sabThermFilters_[i].filtered_result(thermRaw.conversions[channels_.channel[i]].raw);

        sabTemperatures_.temperatures[i] = sabThermistors_.get(i).convert(filteredRaw);
    }    
}