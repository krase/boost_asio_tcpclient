#include "frame.h"
#include <cstring>
#include <iostream>

/*
std::shared_ptr<Frame> Frame::parse(const uint8_t *data)
{
    if (data == 0)
        return nullptr;

    uint8_t size = data[0]; // unpack size

    return std::shared_ptr<Frame>(new Frame(data, size));
}*/

std::shared_ptr<Frame> Frame::make_shared(const uint8_t *data, size_t size)
{
    if (data == 0)
        return nullptr;

    return std::shared_ptr<Frame>(new Frame(data, size));
}

Frame::Frame(const void *_data, std::size_t size)
{
    const uint8_t *pData = (const uint8_t*)_data;
    this->m_data.resize(size);
    std::copy(pData, pData+size, this->m_data.begin());
    //memcpy(this->data.data(), data, size);

    m_type = static_cast<MsgType>(m_data[0]);
}
