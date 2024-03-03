#ifndef __TCU_STATUS_H__
#define __TCU_STATUS_H__
#pragma once
#pragma pack(push, 1)

#include <string.h>
#include <stdint.h>

class TCU_status
{
public:
    TCU_status() = default;
    TCU_status(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8]) { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // getters
    inline uint16_t get_shutdown_status() const { return shutdown_status; }

    // setters
    inline void set_shutdown_status(const uint16_t status) { shutdown_status = status; }    

private:
    uint16_t shutdown_status;
};

#pragma pack(pop)
#endif  /* __TCU_STATUS_H__ */