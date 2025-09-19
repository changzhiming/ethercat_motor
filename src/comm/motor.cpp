#include <iostream>
#include <comm/log.hpp>
#include <comm/ethercat_communication.h>
#include <zmq.hpp>
#include <thread>

int main(int argc, char **argv) {

    LOG_INIT("ethercat_motor");

    std::cout << "Hello, World!" << std::endl;

    auto ethercat_comm = std::make_shared<EtherCatCommunication>("enp6s0");
    auto code = ethercat_comm->open();
    if(code != ErrorCode::SUCCESS) {
        LOG_INFO("Failed to open ethercat_comm communication");
    }
    //初始化电机
    ethercat_comm->init_motor(1);
    ethercat_comm->init_motor(2);

    //zmq
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    socket.bind("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        socket.recv(request, zmq::recv_flags::none);

        std::vector<uint8_t> data(static_cast<uint8_t*>(request.data()), static_cast<uint8_t*>(request.data()) + request.size());

        LOG_INFO("buffer: {}", fmt::join(data, ", "));

        zmq::message_t reply("World", 5);
        socket.send(reply, zmq::send_flags::none);
    }

    LOG_FLUSH();
    return 0;
}