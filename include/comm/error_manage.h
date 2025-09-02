/**
 * @file error_manage.h
 * @author changzhiming (zhiming.chang@robotera.com)
 * @brief sdk error manage head
 * @version 0.1
 * @date 2025-08-18
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>

//错误码
enum class ErrorCode : uint32_t{
    SUCCESS                = 0,       // 无错误
    //IAP错误码
    IAPERROR_SIZE          = 1501007,      // 固件大小超范围
    IAPERROR_ERASE         = 1501008,      // 擦除Flash失败
    IAPERROR_WRITEFLASH    = 1501009,      // 写入Flash失败
    IAPERROR_INDEX         = 1501010,      // 索引错误
    IAPERROR_BUSY          = 1501011,      // 总线忙
    IAPERROR_FORM          = 1501012,      // 数据格式错误
    IAPERROR_VERIFY        = 1501013,      // 校验错误
    IAPERROR_OTHER         = 1501014,      // 其他错误
    IAPERROR_SHAR          = 1501015,
    IAPERROR_JUMP_CHECK    = 1501016,
    IAPERROR_SHA_CHECK     = 1501017,
    //通信错误
    ERROR_ID                 = 1501001,    // 通信ID错误码，与本地ID不匹配
    ERROR_CMD                = 1501002,    // 通信CMD错误，不存在的CMD
    ERROR_COMMUNICATION      = 1501003,    // 通信错误，协议异常或校验不通过
    ERROR_DATA_LEN           = 1501004,    // 数据段长度错误
    ERROR_NOFLASHPARAM       = 1501005,    // Flash参数读异常，不存在预设参数
    ERROR_COMMUNICATION_BUSY = 1501006,    // 上一帧数据未处理完，设备忙
    ERROR_BOOT_CMD           = 1501036,    // 运行在boot中CMD错误，不存在的CMD
    ERROR_CRC_ERROR          = 1501070,    // 通信数据CRC错误
    //关节板错误码
    ERROR_PARAM_INIT         = 1501026,     // flash参数初始化失败
    ERROR_TEMP_PROCTED       = 1501028,     // 关节板过温，超过安全温度限制
    ERROR_MOTION_MODE        = 1501029,     // 收到错误的运行模式
    ERROR_PARAM_OUTOF_RANGE  = 1501030,     // 控制参数超过允许范围
    ERROR_JOINT_NOFLASHPARAM = 1501031,     // Flash参数读异常，不存在预设参数
    ERROR_JOINT_COMMUNICATION= 1501032,     // 通信错误，协议异常或校验不通过
    ERROR_COMMUNICATION_CMD  = 1501033,     // 通信CMD错误，收到不存在的CMD
    ERROR_POSITION_RAW       = 1501034,     // 读取的位置信号原始电压不在正常范围内
    ERROR_CURRENT_PROCTED    = 1501035,     // 电流超过设定阈值，触发过流警告（超过500ms）
    JOINT_ERROR_CURRENT_ZERO = 1501080,
    JOINT_ERROR_FORCEMODE_NOT_SUPPORTED = 1501081,
    //指尖传感器错误码
    ERROR_READ_TOTAL_FORCE   = 1501018,     // 传感器读取合力失败
    ERROR_READ_FORCES        = 1501019,     // 传感器读取分布力失败
    ERROR_READ_TEMP          = 1501020,     // 传感器读取温度失败
    ERROR_CALIBRATE          = 1501021,     // 传感器校准失败
    ERROR_FIGURE_CMD         = 1501022,     // 通信CMD错误，不存在的CMD
    ERROR_SENSOR_NOFLASHPARAM       = 1501023,     // Flash参数读异常，不存在预设参数
    ERROR_FIGURE_COMMUNICATION      = 1501024,      // 通信错误，协议异常或校验不通过

    INVALID_DEVICE_ID                     = 1501037,  //无效设备id
    DEVICE_NOT_INIT                       = 1501038, //设备未初始化
    OPEN_DEVICE_FAILED                    = 1501039, //代开设备失败
    PDO_COMMUNICATION_ERROR               = 1501040,
    SDO_COMMUNICATION_ERROR               = 1501041,
    UPGRADE_STAGE_BEGIN_ERROR             = 1501042,
    UPGRADE_STAGE_TRANSMIT_ERROR          = 1501043,
    UPGRADE_STAGE_VERIFY_ERROR            = 1501044,
    INVALID_FIRMWARE_FILE                 = 1501045,

    CALIBRATE_OUT_OF_RANGE                = 1501046,  //数值超出范围
    CALIBRATE_NEAR_BOUNDARY               = 1501047,  //数值靠近边界
    CALIBRATE_SMALL_DIFFERENCE            = 1501048,  //差值过小
    CALIBRATE_SMALL_DIFFERENCE_SPECIAL    = 1501049,  //通道 3 差值过小

    SERIAL_COMMUNICATION_ERROR            = 1501050,

    FIRMWARE_VERSION_TOO_LOWER            = 1501051,  //固件版本太低
    CALIBRATION_ERROR_ANGLE_OUT_OF_RANGE  = 1501052,  //角度超出范围
    CALIBRATION_ERROR_ADC_OUT_OF_RANGE    = 1501053,  //ADC 超出范围
    CALIBRATION_ERROR_ADC_PRE_UPPER_ERROR = 1501054,  //ADC 预测上边界参数错误
    CALIBRATION_ERROR_ADC_PRE_LOWER_ERROR = 1501055,  //ADC 预测下边界参数错误
    CALIBRATION_SAVE_PARAM_ERROR          = 1501056,  //校准时保存参数错误
    CALIBRATION_INPUT_PARAM_ERROR         = 1501057,  //校准输入参数错误
    CALIBRATION_INVALID_TOOL              = 1501058,  //传入校准工装型号错误

    ACTION_GROUP_INVALID_ACTIONS          = 1501059,
    ACTION_GROUP_INVALID_ANGLE            = 1501060,
    ACTION_GROUP_INVALID_TIME             = 1501061,
    ACTION_GROUP_FLASH_ERROR              = 1501062,

    FIRMWARE_STATE_READ_ERROR             = 1501063,  //读取固件状态错误
    FIRMWARE_STATE_SWITCH_ERROR           = 1501064,  //固件状态切换错误

    JOINT_ACTION_COUNT_NO_RESULT_ERROR    = 1501065,  //获取动作次数计数无结果
    JOINT_CLEAR_ACTION_COUNT_FAIL_ERROR   = 1501066,  //清空动作次数计数返回错误
};


static const std::map<ErrorCode, std::string> error_detail = {
    {ErrorCode::SUCCESS, "成功"},
    {ErrorCode::ERROR_ID, "通信ID错误吗，与本地ID不匹配"},
    {ErrorCode::ERROR_CMD, "通信CMD错误，不存在的CMD"},
    {ErrorCode::ERROR_COMMUNICATION,"通信错误，协议异常或校验不通过"},
    {ErrorCode::ERROR_DATA_LEN,"数据段长度错误"},
    {ErrorCode::ERROR_NOFLASHPARAM, "Flash参数读异常，不存在预设参数"},
    {ErrorCode::ERROR_COMMUNICATION_BUSY,"上一帧数据未处理完，设备忙"},
    {ErrorCode::ERROR_BOOT_CMD,"运行在boot中CMD错误，不存在的CMD"},

    {ErrorCode::IAPERROR_SIZE, "固件大小超范围"},
    {ErrorCode::IAPERROR_ERASE,"擦除Flash失败"},
    {ErrorCode::IAPERROR_WRITEFLASH, "写入Flash失败"},
    {ErrorCode::IAPERROR_INDEX, "索引错误"},
    {ErrorCode::IAPERROR_BUSY, "总线忙"},
    {ErrorCode::IAPERROR_FORM,"数据格式错误"},
    {ErrorCode::IAPERROR_VERIFY, "完整性校验错误"},
    {ErrorCode::IAPERROR_SHAR, "SHAR计算错误"},
    {ErrorCode::IAPERROR_JUMP_CHECK, "stack 校验错误"},
    {ErrorCode::IAPERROR_SHA_CHECK, "sha 校验错误"},
    {ErrorCode::ERROR_READ_TOTAL_FORCE, "传感器读取合力失败"},
    {ErrorCode::ERROR_READ_FORCES,"传感器读取分布力失败"},
    {ErrorCode::ERROR_READ_TEMP,"传感器读取温度失败"},
    {ErrorCode::ERROR_CALIBRATE,"传感器校准失败"},
    {ErrorCode::ERROR_FIGURE_CMD,"通信CMD错误，不存在的CMD"},
    {ErrorCode::ERROR_SENSOR_NOFLASHPARAM, "Flash参数读异常，不存在预设参数"},
    {ErrorCode::ERROR_FIGURE_COMMUNICATION, "通信错误，协议异常或校验不通过"},
    {ErrorCode::ERROR_PARAM_INIT,"flash参数初始化失败"},
    {ErrorCode::ERROR_TEMP_PROCTED,"关节板过温，超过安全温度限制"},
    {ErrorCode::ERROR_MOTION_MODE, "收到错误的运行模式"},
    {ErrorCode::ERROR_PARAM_OUTOF_RANGE, "控制参数超过允许范围"},
    {ErrorCode::ERROR_JOINT_NOFLASHPARAM,"Flash参数读异常，不存在预设参数"},
    {ErrorCode::ERROR_JOINT_COMMUNICATION,"通信错误，协议异常或校验不通过"},
    {ErrorCode::ERROR_COMMUNICATION_CMD,"通信CMD错误，收到不存在的CMD"},
    {ErrorCode::ERROR_POSITION_RAW,"读取的位置信号原始电压不在正常范围内"},
    {ErrorCode::ERROR_CURRENT_PROCTED,"电流超过设定阈值，触发过流警告（超过500ms）"},
    {ErrorCode::JOINT_ERROR_CURRENT_ZERO,"老化测试时电流趋近0（正常不为0，可能为12V线断）"},
    {ErrorCode::JOINT_ERROR_FORCEMODE_NOT_SUPPORTED,"该硬件版本不支持力控模式"},

    {ErrorCode::INVALID_DEVICE_ID,"无效的设备ID"},
    {ErrorCode::DEVICE_NOT_INIT,"设备未初始化"},
    {ErrorCode::OPEN_DEVICE_FAILED,"打开设备失败"},
    {ErrorCode::SDO_COMMUNICATION_ERROR,"SDO通信错误"},
    {ErrorCode::UPGRADE_STAGE_BEGIN_ERROR,"升级开始阶段错误"},
    {ErrorCode::UPGRADE_STAGE_TRANSMIT_ERROR, "升级传输阶段错误"},
    {ErrorCode::UPGRADE_STAGE_VERIFY_ERROR, "升级确认阶段错误"},
    {ErrorCode::INVALID_FIRMWARE_FILE, "无效的固件文件"},

    {ErrorCode::SERIAL_COMMUNICATION_ERROR,"串口通信错误"},
    {ErrorCode::FIRMWARE_VERSION_TOO_LOWER,"固件版本太低"},
    {ErrorCode::CALIBRATION_ERROR_ANGLE_OUT_OF_RANGE,"角度超出范围"},
    {ErrorCode::CALIBRATION_ERROR_ADC_OUT_OF_RANGE,"ADC 超出范围"},
    {ErrorCode::CALIBRATION_ERROR_ADC_PRE_UPPER_ERROR, "ADC 预测上边界参数错误"},
    {ErrorCode::CALIBRATION_ERROR_ADC_PRE_LOWER_ERROR, "ADC 预测下边界参数错误"},
    {ErrorCode::CALIBRATION_SAVE_PARAM_ERROR,"校准时保存参数错误"},
    {ErrorCode::CALIBRATION_INPUT_PARAM_ERROR,"校准输入参数错误"},
    {ErrorCode::CALIBRATION_INVALID_TOOL,"校准工装型号错误"},

    {ErrorCode::ACTION_GROUP_INVALID_ACTIONS,"循环动作组动作为空"},
    {ErrorCode::ACTION_GROUP_INVALID_ANGLE, "循环动作组中的关节角度错误"},
    {ErrorCode::ACTION_GROUP_INVALID_TIME,"循环动作组插值时间错误"},
    {ErrorCode::ACTION_GROUP_FLASH_ERROR, "循环动作组flash写入错误"},
};

static inline std::string error_code_to_info(ErrorCode code) {
    return error_detail.at(code);
}
