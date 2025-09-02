# 目录
- [目录](#目录)
- [开始](#开始)
  - [编译](#编译)
  - [测试](#测试)
- [接口](#接口)
  - [c++](#c)
  - [python](#python)
- [用例](#用例)
- [联系](#联系)
- [重要声明](#重要声明)


# 开始
## 编译

```
依赖

sudo apt install libspdlog-dev libgtest-dev  libgflags-dev pybind11-dev libboost-stacktrace-dev

编译
bash build.sh编译

install文件夹是动态库和测试文件
pybind_control/dist目录下是whl pip安装文件
source install/setup.sh
```
## 测试

ctest / ctest -V 全部测试

ctest -R Serial    分别运行
ctest -R ETHERCAT

# 接口

## c++

```
/**
 * @brief 获取SDK版本
 * @return std::string SDK版本 例如 0.1.1
 */

std::string sdk_version()
```

## python

> 数据结构
```
关节命令结构体
JointCommand_User
{
    int kp;          /* Subindex2 - kp */
    int ki;          /* Subindex3 - ki */
    int kd;          /* Subindex4 - kd */
    float position;    /* Subindex5 - position */
    int current_max; /* Subindex6 - current_max */
    int mode;        /* Subindex7 - mode */
    int res0;        /* Subindex8 - res0 */
    int res1;        /* Subindex9 - res1 */
    int res2;        /* Subindex10 - res2 */
    int res3;        /* Subindex11 - res3 */
}
返回关节状态结构体
JointState_User
{
  float current_pos; /* Subindex2 - current_pos */
  float iq;          /* Subindex3 - Iq */
  float id;          /* Subindex4 - Id */
  int default1;    /* Subindex5 - default1 */
  int default2;    /* Subindex6 - default2 */
  int default3;    /* Subindex7 - default3 */
  int default4;    /* Subindex8 - default4 */
  int default5;    /* Subindex9 - default5 */
  int default6;    /* Subindex10 - default6 */
  int default7;    /* Subindex11 - default7 */
}
返回传感器状态结构体
Sensor
{
  uint8_t  sensor_id;         /* Subindex34 - sensord */
  uint32_t channelData[11];   /* Subindex35 - channelData */
  uint32_t first_ydds_nf;     /* Subindex36 - first_ydds_nf */
  uint32_t first_ydds_tf;     /* Subindex37 - first_ydds_tf */
  uint16_t first_ydds_tfDir;  /* Subindex38 - first_ydds_tfDir */
  uint32_t second_ydds_nf;    /* Subindex39 - second_ydds_nf */
  uint32_t second_ydds_tf;    /* Subindex40 - second_ydds_tf */
  uint16_t second_ydds_tfDir; /* Subindex41 - second_ydds_tfDir */
  uint32_t sProxData;         /* Subindex42 - sProxData */
}

Sensor_2
{
  uint16_t sensor_id;         /* Subindex34 - sensord */
  uint32_t channelData[13];   /* Subindex35 - channelData */
  uint32_t first_ydds_nf;     /* Subindex36 - first_ydds_nf */
  uint32_t first_ydds_tf;     /* Subindex37 - first_ydds_tf */
  uint16_t first_ydds_tfDir;  /* Subindex38 - first_ydds_tfDir */
  uint32_t second_ydds_nf;    /* Subindex39 - second_ydds_nf */
  uint32_t second_ydds_tf;    /* Subindex40 - second_ydds_tf */
  uint16_t second_ydds_tfDir; /* Subindex41 - second_ydds_tfDir */
  uint32_t sProxData[2];         /* Subindex42 - sProxData */
} Sensor_2;

```

```
/**
 * @brief 获取SDK版本
 * @return SDK版本 例如 0.1.1
 */
sdk_version()
```
**SerialCommunication**

| 函数 | 参数 | 返回值 |

| 函数  | 参数 | 返回值   |
| :----- | :---- | :------ |
| 构造  | serialname(/dev/ttyUSB0), baud(3000 * 1000), timeout(ms)| 无 |
| open  | 无   | tuple(int(ErrorCode), str, data) |
| close  | 无   | tuple(int(ErrorCode), str, data) |
| enumerate_devices  | 无   | tuple(int(ErrorCode), str, list) |

**EtherCatCommunication**

| 函数 | 参数 | 返回值 |

| 函数  | 参数 | 返回值   |
| :----- | :---- | :------ |
| 构造  | if_name(eth0) | 无|
| open  | 无   | tuple(int(ErrorCode), str, data) |
| close  | 无   | tuple(int(ErrorCode), str, data) |
| enumerate_devices  | 无  | tuple(int(ErrorCode), str, list) |

**BaseHand**

| 函数 | 参数 | 返回值 |

| 函数  | 参数 | 返回值   |
| :----- | :---- | :------ |
|构造|hand_id(0), comm(EtherCatCommunication/SerialCommunication)| 无|
| get_software_version  | 无   | tuple(int(ErrorCode), str, version) |
| list_hand_ids  | 无   | tuple(int(ErrorCode), str, list) |
| get_hand_type  | 无   | tuple(int(ErrorCode), str, "R"/"L") |
| get_sn  | 无   | tuple(int(ErrorCode), str, sn) |
| get_soft_version  | id   | tuple(int(ErrorCode), str, version) |
| set_hand_id  | id   | tuple(int(ErrorCode), str, version) |
| get_hand_id  | 无  | tuple(int(ErrorCode), str, version) |

**GreenHand**

| 函数 | 参数 | 返回值 |

| 函数  | 参数 | 返回值   |
| :----- | :---- | :------ |
|构造|hand_id(0), comm(EtherCatCommunication/SerialCommunication)| 无|
| get_software_version  | 无   | tuple(int(ErrorCode), str, version) |
| list_hand_ids  | 无   | tuple(int(ErrorCode), str, list) |
| get_hand_type  | 无   | tuple(int(ErrorCode), str, "R"/"L") |
| set_joint_cmd  | joint_id(0-max), joint_data(JointCommand_User) | tuple(int(ErrorCode), str, "") |
| get_joint_state  | joint_id(0-max), force_flush(true/false)| tuple(int(ErrorCode), str, JointState_User) |
| get_joint_state_list  | force_flush(true/false)| tuple(int(ErrorCode), str, JointState_User(list)) |
| get_sensor_state  | sensor_id(0-max) force_flush(true/false)| tuple(int(ErrorCode), str, Sensor) |
| get_sensor_state2  | sensor_id(30) force_flush(true/false)| tuple(int(ErrorCode), str, Sensor_2) |

# 用例

```
from hand_controller import hand_control

code, msg, data = hand_control.serialCommunication.enumerate_devices()
communication = hand_control.serialCommunication(data[0], 3000000, 1000)
communication.open()

hand = hand_control.BaseHand(0, communication);
code, msg, ids = hand.list_hand_ids();
code, msg, data = hand.get_sn();

hand = hand_control.GreenHand(0, communication);
code, msg, data = hand.get_joint_state(1)

```


# 联系
[邮箱](www.robotera.com)


# 重要声明
用户使用本产品即视为已充分阅读、理解并同意接受用户协议的全部条款约束。在使用过程中，用户应严格遵守该协议约定，合理合法地使用本产品。用户协议详见【<a target="_blank" href="./用户协议.md">**用户协议**</a>】。