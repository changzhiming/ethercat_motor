#include <comm/ethercat_communication.h>
#include <soem/soem.h>
#include <comm/log.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <ifaddrs.h>
#include <net/if.h>


#define EC_TIMEOUTMON 500


static int servo_setup(ecx_contextt * ctx, uint16 slave) {

    uint8_t u8val = 0;
    uint16_t u16val = 0;
    uint32_t u32val = 0;
    int wk = 0;
    //group
    u8val = 0;
    wk = ecx_SDOwrite(ctx, slave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    u8val = 0;
    wk = ecx_SDOwrite(ctx, slave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);


    u16val = 0x1600;
    wk = ecx_SDOwrite(ctx, slave, 0x1c12, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);
    u16val = 0x1A00;
    wk = ecx_SDOwrite(ctx, slave, 0x1c13, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);


    u8val = 1;
    wk = ecx_SDOwrite(ctx, slave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    u8val = 1;
    wk = ecx_SDOwrite(ctx, slave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);


    // RxPDO
    u8val = 0;
    wk = ecx_SDOwrite(ctx, slave, 0x1600, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    u32val = 0x60400010;
    wk = ecx_SDOwrite(ctx, slave, 0x1600, 0x01, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    u32val = 0x60FF0020;
    wk = ecx_SDOwrite(ctx, slave, 0x1600, 0x02, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    u32val = 0x60600008;
    wk = ecx_SDOwrite(ctx, slave, 0x1600, 0x03, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u8val = 3;
    wk = ecx_SDOwrite(ctx, slave, 0x1600, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

// //TPdo

    u8val = 0;
    wk = ecx_SDOwrite(ctx, slave, 0x1A00, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    u32val = 0x60410010;
    wk = ecx_SDOwrite(ctx, slave, 0x1A00, 0x01, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    u32val = 0x606C0020;
    wk = ecx_SDOwrite(ctx, slave, 0x1A00, 0x02, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);
    u32val = 0x60610008;
    wk = ecx_SDOwrite(ctx, slave, 0x1A00, 0x03, FALSE, sizeof(u32val), &u32val, EC_TIMEOUTRXM);

    u8val = 3;
    wk = ecx_SDOwrite(ctx, slave, 0x1A00, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);


    return 0;
}


EtherCatCommunication::EtherCatCommunication(const std::string if_name) : if_name_{if_name} {
}

EtherCatCommunication::~EtherCatCommunication(){ this->close();  }


ErrorCode EtherCatCommunication::open() {
    //防止重复open
    if(ethercat_thread_ || ethercat_error_thread_ || is_open) {
        return ErrorCode::OPEN_DEVICE_FAILED;
    }

    LOG_INFO("if_name:{}", if_name_);

    if(ecx_init(&ctx, if_name_.c_str()) <= 0) {
        return ErrorCode::DEVICE_NOT_INIT;
    }

    LOG_INFO("ecx_init:{}", if_name_);

    if (ecx_config_init(&ctx) <= 0)
    {
        ecx_close(&ctx);
        return ErrorCode::DEVICE_NOT_INIT;
    }

    LOG_INFO("ecx_config_init:{}", if_name_);

    for (int si = 1; si <= ctx.slavecount; si++)
    {
        ec_slavet *slave = &ctx.slavelist[si];
        slave->PO2SOconfig = servo_setup;
    }

    ecx_config_map_group(&ctx, IOMap, 0);

    for (int si = 1; si <= ctx.slavecount; si++)
    {
        ec_slavet *slave = &ctx.slavelist[si];
        LOG_INFO("Slave {}: {}", si, slave->name);
        LOG_INFO("State: {}", slave->state);
        LOG_INFO("Output size: {}", slave->Obytes);
        LOG_INFO("Input size: {}", slave->Ibytes);
        LOG_INFO("Configured address: {}", slave->configadr);
        LOG_INFO("Delay: {}", slave->pdelay);
    }

    uint16_t expectedWKC = ctx.grouplist[0].outputsWKC * 2 + ctx.grouplist[0].inputsWKC;

    ecx_configdc(&ctx);

    auto switch_device_state = [this](auto state_){
        int chk = 200;
        do
        {
            ctx.slavelist[0].state = state_;
            ecx_writestate(&ctx, 0);

            ecx_send_processdata(&ctx);
            ecx_receive_processdata(&ctx, EC_TIMEOUTRET);

            ecx_statecheck(&ctx, 0, state_, 50000);
            ecx_readstate(&ctx);

            LOG_INFO("wait for all slaves to reach {} state", state_);
        } while (chk-- && (ctx.slavelist[0].state != state_));
    };

    switch_device_state(EC_STATE_SAFE_OP);
    switch_device_state(EC_STATE_OPERATIONAL);

    if (ctx.slavelist[0].state != EC_STATE_OPERATIONAL) {
        LOG_INFO("DC wait fail");
    }
    //数据刷新线程
    ethercat_thread_ = std::make_shared<std::thread>([this, expectedWKC](){
        while(!quit_ethercat_thread)
        {
            osal_usleep(100);  //100us
            {
                std::unique_lock lock(ethercat_mutex_);  //写
                ecx_send_processdata(&ctx);
                int wkc = ecx_receive_processdata(&ctx, EC_TIMEOUTRET);

                if(wkc != expectedWKC) {
                    LOG_INFO( "wkc {} != expectedWKC {}", wkc, expectedWKC);
                    ethercat_has_error_ = true;
                }
            }
        }
    });
    //故障恢复线程
    ethercat_error_thread_ = std::make_shared<std::thread>([this](){
        while(!quit_ethercat_thread)
        {
            osal_usleep(100000);  //100ms
            if(ethercat_has_error_) {
                std::unique_lock lock(ethercat_mutex_);  //写
                ethercat_has_error_ = error_handle();
            }
        }
    });
    //高优先级
    pthread_t handle = ethercat_thread_->native_handle();
    sched_param sched_params;
    sched_params.sched_priority = 50;
    if (pthread_setschedparam(handle, SCHED_FIFO, &sched_params)) {
        LOG_INFO( "Failed to set thread priority");
    }

    is_open = true;

    LOG_INFO("Ethercat INIT SUCCESS");
    return ErrorCode::SUCCESS;
}

bool EtherCatCommunication::error_handle() {

    bool has_slave_error = false;
    int currentgroup = 0;
    /* one or more slaves are not responding */
    ecx_readstate(&ctx);
    for (int slaveix = 1; slaveix <= ctx.slavecount; slaveix++)
    {
        ec_slavet *slave = &ctx.slavelist[slaveix];

        if ((slave->group == currentgroup) && (slave->state != EC_STATE_OPERATIONAL))
        {
            has_slave_error = true;

            if (slave->state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
            {
                printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slaveix);
                slave->state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                ecx_writestate(&ctx, slaveix);
            }
            else if (slave->state == EC_STATE_SAFE_OP)
            {
                printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slaveix);
                slave->state = EC_STATE_OPERATIONAL;
                if (slave->mbxhandlerstate == ECT_MBXH_LOST) slave->mbxhandlerstate = ECT_MBXH_CYCLIC;
                ecx_writestate(&ctx, slaveix);
            }
            else if (slave->state > EC_STATE_NONE)
            {
                if (ecx_reconfig_slave(&ctx, slaveix, EC_TIMEOUTMON) >= EC_STATE_PRE_OP)
                {
                slave->islost = FALSE;
                printf("MESSAGE : slave %d reconfigured\n", slaveix);
                }
            }
            else if (!slave->islost)
            {
                /* re-check state */
                ecx_statecheck(&ctx, slaveix, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                if (slave->state == EC_STATE_NONE)
                {
                slave->islost = TRUE;
                slave->mbxhandlerstate = ECT_MBXH_LOST;
                /* zero input data for this slave */
                if (slave->Ibytes)
                {
                    memset(slave->inputs, 0x00, slave->Ibytes);
                }
                printf("ERROR : slave %d lost\n", slaveix);
                }
            }
        }
        if (slave->islost)
        {
            if (slave->state <= EC_STATE_INIT)
            {
                if (ecx_recover_slave(&ctx, slaveix, EC_TIMEOUTMON))
                {
                slave->islost = FALSE;
                printf("MESSAGE : slave %d recovered\n", slaveix);
                }
            }
            else
            {
                slave->islost = FALSE;
                printf("MESSAGE : slave %d found\n", slaveix);
            }
        }
    }

    return has_slave_error;
}

ErrorCode EtherCatCommunication::close() {
    if(!is_open) {
        return ErrorCode::ERROR_COMMUNICATION;
    }
    quit_ethercat_thread = true;  //线程退出

    if(ethercat_thread_ && ethercat_thread_->joinable()) {
        ethercat_thread_->join();
    }

    if(ethercat_error_thread_ && ethercat_error_thread_->joinable()) {
        ethercat_error_thread_->join();
    }

    ethercat_thread_.reset();
    ethercat_error_thread_.reset();

    ecx_close(&ctx);

    quit_ethercat_thread = false;

    is_open = false;

    return ErrorCode::SUCCESS;
}



ErrorCode EtherCatCommunication::SDOwrite(uint16 Slave, uint16 Index, uint8 SubIndex, int psize, const void *p) {

    if(Slave <= 0 || Slave > ctx.slavecount || ethercat_has_error_ || p == nullptr || psize == 0) {
        return ErrorCode::INVALID_DEVICE_ID;
    }
    std::unique_lock lock(ethercat_mutex_);  //写
    auto wkc = ecx_SDOwrite(&ctx, Slave, Index, SubIndex, FALSE, psize, (void*)p, EC_TIMEOUTTXM);

    if(wkc <= 0) {
        LOG_INFO( "SDO send fail: wkc: {}", wkc);
        return ErrorCode::INVALID_DEVICE_ID;
    }

    return ErrorCode::SUCCESS;
}

ErrorCode EtherCatCommunication::SDOread(uint16 Slave, uint16 Index, uint8 SubIndex, int psize, const void *p) {

    if(Slave <= 0 || Slave > ctx.slavecount || ethercat_has_error_ || p == nullptr || psize == 0) {
        return ErrorCode::INVALID_DEVICE_ID;
    }
    std::unique_lock lock(ethercat_mutex_);  //写
    auto wkc = ecx_SDOread(&ctx, Slave, Index, SubIndex, FALSE, &psize, (void*)p, EC_TIMEOUTTXM);

    if(wkc <= 0) {
        LOG_INFO( "SDO read fail: wkc: {}", wkc);
        return ErrorCode::INVALID_DEVICE_ID;
    }

    return ErrorCode::SUCCESS;
}

void EtherCatCommunication::PDOwrite(uint16 Slave, const TPdo_info_t &pdo) {
    if(Slave <= 0 || Slave > ctx.slavecount || ethercat_has_error_) {
        return;
    }
    std::unique_lock lock(ethercat_mutex_);

    TPdo_info_t *pdo_ptr = (TPdo_info_t *)(ctx.slavelist[Slave].outputs);
    *pdo_ptr = pdo;
}

void EtherCatCommunication::PDOread(uint16 Slave, RPdo_info_t &pdo) {
    if(Slave <= 0 || Slave > ctx.slavecount || ethercat_has_error_) {
        return;
    }
    std::shared_lock lock(ethercat_mutex_);  //读

    RPdo_info_t *pdo_ptr = (RPdo_info_t *)(ctx.slavelist[Slave].inputs);
    pdo = *pdo_ptr;
}

void EtherCatCommunication::init_motor(uint16 Slave) {
    if(Slave <= 0 || Slave > ctx.slavecount || ethercat_has_error_) {
        return;
    }

    // 初始化 PDO
    TPdo_info_t tpdo_info{.control_word = 6, .mode = 0x09, .speed = 8388608 * 3};
    PDOwrite(Slave, tpdo_info);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    tpdo_info.control_word = 7; // 使能
    PDOwrite(Slave, tpdo_info);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    tpdo_info.control_word = 15; // 使能
    PDOwrite(Slave, tpdo_info);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    RPdo_info_t rpdo_info;
    PDOread(Slave, rpdo_info);
    LOG_INFO("write: 15 state_word: {:#X}, mode: {:#X}, speed: {}", rpdo_info.state_word, rpdo_info.mode, rpdo_info.speed);
}