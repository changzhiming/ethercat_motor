#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <string>
#include <soem/soem.h>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <shared_mutex>
#include <comm/error_manage.h>


#pragma pack(push, 1)
typedef struct {
    uint16_t control_word;
    int32_t target_position;  // -2147483648 to 2147483647
    int32_t target_velocity;  //-2147483648 to 2147483647
    int16_t target_torque;    // -4000 - 4000
    uint8_t mode_of_operation;
    uint16_t Probe_mode;
    uint32_t profile_velocity;  //最大转速
} TPdo_info_t;
#pragma pack(pop)
static_assert(sizeof(TPdo_info_t) == 19, "TPdo_info_t must be 1-byte packed");

#pragma pack(push, 1)
typedef struct {
    uint16_t error_code;
    uint16_t state_word;
    int32_t position;  // -2147483648 to 2147483647
    int16_t torque;    //转矩反馈 -4000 - 4000
    uint16_t operation; //模式下显示
    int32_t Probe_mode;//探针
    int32_t Probe_mode1;//探针
    int32_t Probe_mode2;//探针
    uint32_t DI;  //数字输入
} RPdo_info_t;
#pragma pack(pop)

static_assert(sizeof(RPdo_info_t) == 28, "RPdo_info_t must be 1-byte packed");

constexpr int IOMapSize = 4096;

class EtherCatCommunication {
public:
    EtherCatCommunication(const std::string if_name);
    virtual ~EtherCatCommunication();

    virtual ErrorCode open();
    virtual ErrorCode close();

    ErrorCode SDOwrite(uint16 Slave, uint16 Index, uint8 SubIndex, int psize, const void *p);
    ErrorCode SDOread(uint16 Slave, uint16 Index, uint8 SubIndex, int psize, const void *p);

    void PDOwrite(uint16 Slave, const TPdo_info_t &pdo);
    void PDOread(uint16 Slave, RPdo_info_t &pdo);
private:
    bool error_handle();

private:
    std::shared_ptr<std::thread> ethercat_thread_;
    std::shared_ptr<std::thread> ethercat_error_thread_;

    std::shared_mutex ethercat_mutex_;   //shared 读写锁
    std::string if_name_;

    ecx_contextt ctx;  //ethercat contex

    uint8_t IOMap[IOMapSize]{};

    std::atomic<bool> ethercat_has_error_{false};
    std::atomic<bool> quit_ethercat_thread{false}; //线程退出
    std::atomic<bool> is_open{false};              //
};