#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include <vector>
#include <boost/shared_ptr.hpp>

class Frame
{
public:
    Frame(uint8_t *data, uint16_t size);

    static boost::shared_ptr<Frame> parse(uint8_t *data);

    static uint8_t calc_crc(uint8_t *data, uint16_t size);
private:
    //uint16_t             size;
    std::vector<uint8_t> data;
};



class TypedMessage
{
public:
    enum MsgTypes
    {
        TYPE_ACK = 0x01,
        TYPE_LOGIN = 0x02,
        TYPE_CAN_FRAME = 0x04,
        TYPE_CAN_AGGREGATE = 0x08,
        TYPE_ERROR = 0x10
    };

    TypedMessage(Frame *frame);

    //TypedMessage(MsgTypes type, uint8_t data, uint16_t size);

    boost::shared_ptr<Frame> asFrame();
};

#endif // FRAME_H
