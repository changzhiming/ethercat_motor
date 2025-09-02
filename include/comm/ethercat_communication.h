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

constexpr int IOMapSize = 4096;

class EtherCatCommunication {
public:
    EtherCatCommunication(const std::string if_name);
    virtual ~EtherCatCommunication();

    virtual ErrorCode open();
    virtual ErrorCode close();

    virtual size_t sdo_read(uint8_t device_id , uint8_t *data, size_t len);
    virtual ErrorCode sdo_write(uint8_t device_id , uint8_t *data, size_t len);
    virtual size_t pdo_read(uint8_t device_id , uint8_t *data, size_t len);
    virtual ErrorCode pdo_write(uint8_t device_id , uint8_t *data, size_t len);

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