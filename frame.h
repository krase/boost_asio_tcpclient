#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include <vector>
#include <memory>
//#include <boost/shared_ptr.hpp>
#include <sstream>

class Frame
{
public:
    enum MsgType
    {
        TYPE_ACK = 0x01,
        TYPE_LOGIN = 0x02,
        TYPE_CAN_FRAME = 0x04,
        TYPE_CAN_AGGREGATE = 0x08,
        TYPE_ERROR = 0x10
    };

    Frame(const void *m_data, std::size_t size);

    //static std::shared_ptr<Frame> parse(const uint8_t *m_data);
    static std::shared_ptr<Frame> make_shared(const uint8_t *m_data, size_t size);

    std::size_t size() const { return m_data.size() - 1; }
    const uint8_t* data() const { return m_data.data() + 1; }
    MsgType type() const { return m_type; }


    operator std::string() {
        std::basic_stringstream<char> sb;
        std::string tmp;
        tmp.assign((const char*)data(), size() );

        std::string typ;
        switch(this->type()) {
        case TYPE_ACK:
            typ = "ACK";
            break;
        case TYPE_LOGIN:
            typ = "LOGIN";
            break;
        case TYPE_CAN_FRAME:
            typ = "CAN_FRAME";
            break;
        case TYPE_CAN_AGGREGATE:
            typ = "CAN_AGGREGATE";
            break;
        case TYPE_ERROR:
            typ = "ERROR";
            break;
        }

        sb << "Frame(" << typ << ": " << size() << ", " << tmp << ")";

        return sb.str();
    }
private:
    MsgType              m_type;
    std::vector<uint8_t> m_data;
};

/*
class TypedMessage
{
public:


    TypedMessage(std::shared_ptr<Frame> frame);

    //TypedMessage(MsgTypes type, uint8_t data, uint16_t size);

    std::shared_ptr<Frame> asFrame() { return m_frame; }

    std::size_t size() const { return m_frame->size() - 1; }
    const uint8_t* data() { return m_frame->data() + 1; }


    operator std::string() {
        std::basic_stringstream<char> sb;
        std::string tmp;
        tmp.assign((const char*)data(), size());

        std::string typ;
        switch(this->type()) {
        case TYPE_ACK:
            typ = "ACK";
            break;
        case TYPE_LOGIN:
            typ = "LOGIN";
            break;
        case TYPE_CAN_FRAME:
            typ = "CAN_FRAME";
            break;
        case TYPE_CAN_AGGREGATE:
            typ = "CAN_AGGREGATE";
            break;
        case TYPE_ERROR:
            typ = "ERROR";
            break;
        }

        sb << "TypedMessage(" << typ << ": " << size() << ", " << tmp << ")";
        return sb.str();
    }

private:
    MsgType               m_type;
    std::shared_ptr<Frame> m_frame;
};
*/

#endif // FRAME_H
