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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //
    TPdo_info_t tpdo_info{};
    tpdo_info.control_word = 6; // 使能
    tpdo_info.mode_of_operation = 0x09; // 速度模式
    tpdo_info.profile_velocity = 2147483600; // 最大转速
    tpdo_info.target_velocity = 89898482; // 目标速度
    ethercat_comm->PDOwrite(1, tpdo_info);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    RPdo_info_t rpdo_info{};
    ethercat_comm->PDOread(1, rpdo_info);
    LOG_INFO("state_word: {:#b}", rpdo_info.state_word);

    //

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //
    tpdo_info.control_word = 7; // 使能
    ethercat_comm->PDOwrite(1, tpdo_info);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    ethercat_comm->PDOread(1, rpdo_info);
    LOG_INFO("state_word: {:#b}", rpdo_info.state_word);

     //
    tpdo_info.control_word = 15; // 使能
    ethercat_comm->PDOwrite(1, tpdo_info);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    ethercat_comm->PDOread(1, rpdo_info);
    LOG_INFO("state_word: {:#b}", rpdo_info.state_word);



    // //init motor
    // uint8_t mode_t = 0x9;
    // ethercat_comm->SDOwrite(1, 0x6060, 0x00, sizeof(mode_t), (void *)&mode_t); //准备
    // LOG_INFO("0x6060:{:#X}", mode_t);
    // // command = 0x0;
    // // ethercat_comm->SDOwrite(1, 0x6040, 0x00, sizeof(command), (void *)&command); //准备
    // uint16_t command = 0x6;
    // ethercat_comm->SDOread(1, 0x6041, 0x00, sizeof(command), (void *)&command);
    // LOG_INFO("1. 0x6041:{:#b}", command);

    // command = 0x6;
    // ethercat_comm->SDOwrite(1, 0x6040, 0x00, sizeof(command), (void *)&command); //准备

    // ethercat_comm->SDOread(1, 0x6041, 0x00, sizeof(command), (void *)&command);
    // LOG_INFO("0x6041:{:#X}", command);

    // command = 0x7;
    // ethercat_comm->SDOwrite(1, 0x6040, 0x00, sizeof(command), (void *)&command);

    // ethercat_comm->SDOread(1, 0x6041, 0x00, sizeof(command), (void *)&command);
    // LOG_INFO("0x6041:{:#X}", command);

    // int32_t speed = 89898482;
    // ethercat_comm->SDOwrite(1, 0x60FF, 0x00, sizeof(speed), (void *)&speed);

    // ethercat_comm->SDOread(1, 0x60FF, 0x00, sizeof(speed), (void *)&speed);
    // LOG_INFO("1. 0x60FF:{}", speed);

    // command = 0xF;
    // ethercat_comm->SDOwrite(1, 0x6040, 0x00, sizeof(command), (void *)&command);

    // ethercat_comm->SDOread(1, 0x6041, 0x00, sizeof(command), (void *)&command);
    // LOG_INFO("0x6041:{:#X}", command);



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