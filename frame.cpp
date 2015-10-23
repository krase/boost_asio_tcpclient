#include "frame.h"
#include <cstring>

boost::shared_ptr<Frame> Frame::parse(uint8_t *data)
{
    if (data == 0)
        return nullptr;

    uint8_t size = data[0];
    uint8_t calced_crc = Frame::calc_crc(data, size);
    uint8_t crc = data[size-1];

    if (calced_crc != crc)
        return nullptr; //TODO: raise exception

    return boost::shared_ptr<Frame>(new Frame(data, size));
}

Frame::Frame(uint8_t *data, uint16_t size)
{
    this->data.resize(size);
    std::copy(data, data+size, this->data.begin());
    //memcpy(this->data.data(), data, size);
}

uint8_t Frame::calc_crc(uint8_t *data, uint16_t size)
{
    uint8_t crc = 0;
    for (uint32_t i = 0; i < size; ++i)
    {
        crc += data[i];
    }
    return crc;
}
