#include "ThermistorInterface.h"

template <int N>
void ThermistorInterface<N>::tick(AnalogConversionPacket_s<N> &thermRaw)
{
    for (int i = 0; i < N; i++)
    {
        uint16_t filteredRaw = sabThermFilters_.filter_channels_[i].filtered_result(thermRaw.conversions[i]);

        sabTemperatures_.temperatures[i] = sabThermistors_.get(i).convert(filteredRaw);
    }    
}