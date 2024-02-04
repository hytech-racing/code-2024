#ifndef AUTOTIMEDETECTOR_COORDINATECONVERTER_HPP
#define AUTOTIMEDETECTOR_COORDINATECONVERTER_HPP

#include <utility>

class CoordinateConverter {

private:

    double reference_latitude = 0;
    double reference_longitude = 0;

public:

    std::pair<double, double> gps_to_len(double latitude, double longitude);

    void setReferenceLatitude(double referenceLatitude);

    void setReferenceLongitude(double referenceLongitude);

    double getReferenceLatitude() const;

    double getReferenceLongitude() const;
};


#endif //AUTOTIMEDETECTOR_COORDINATECONVERTER_HPP
