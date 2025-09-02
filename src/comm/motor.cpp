#include <iostream>
#include <comm/log.hpp>
#include <comm/ethercat_communication.h>
#include <zmq.hpp>

int main(int argc, char **argv) {

    LOG_INIT("ethercat_motor");

    std::cout << "Hello, World!" << std::endl;

    auto ethercat_comm = std::make_shared<EtherCatCommunication>("enp6s0");
    auto code = ethercat_comm->open();
    if(code != ErrorCode::SUCCESS) {
        LOG_INFO("Failed to open ethercat_comm communication");
    }
//zmq
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    socket.bind("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        socket.recv(request, zmq::recv_flags::none);
        std::string msg(static_cast<char*>(request.data()), request.size());
        std::cout << "Received: " << msg << std::endl;

        zmq::message_t reply("World", 5);
        socket.send(reply, zmq::send_flags::none);
    }

    LOG_FLUSH();
    return 0;
}