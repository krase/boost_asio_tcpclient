

class Connection
{
    const std::string m_host;
    const uint16_t m_port;

    const uint32_t m_size_field_len = 1;

    enum Mode {
        READ_SIZE,
        READ_DATA
    } m_mode;

public:
    Connection();

    void start();

    void stop();

    void handle_connected(const boost::system::error_code& error);

    void handle_disconnect();

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred, const char *data);

    void startSend();

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);

private:
    TCPClient m_tcpClient;
    bool      m_bConnected;
    bool      m_bWantReconnect;

    boost::array<char, (1 << 16)>  m_rx_buffer;
};
