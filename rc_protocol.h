#ifndef RC_PROTOCOL_H
#define RC_PROTOCOL_H
#define PC_PACKET_SIZE 8
#define PC_HEADER_VALUE 0xAA

#include <stdint.h>
#include <Arduino.h>
#include <WiFi.h>

#pragma pack(push,1)

typedef struct{
    uint8_t header;       //包头
    uint8_t safety_lock;  //安全锁
    uint16_t m1_pwm;      //一号电机pwm信号
    uint16_t m2_pwm;      //二号电机pwm信号
    uint8_t seq;          //序列号
    uint8_t checksum;     //校验和
}PC_Packet;

#pragma pack(pop)

IPAddress STA_init(const char *ssid, const char *password);
IPAddress AP_init(const char *ssid,const char *password);
int udp_init(WiFiUDP &udp,uint16_t listen_port);
int pac_send(WiFiUDP &udp,IPAddress tar_ip,const PC_Packet *pac);
int pac_receive(WiFiUDP &udp,PC_Packet *pac);

class unpacked{
private:
    PC_Packet raw;
public:
//    unpacked();//构造函数,默认
//    unpacked(PC_Packet ptr);//构造函数，浅拷贝
    unpacked(const uint8_t *raw_pac, int len);//构造函数，深拷贝
    ~unpacked();
    int pc_packet_verify();//校验包头和校验和
    uint8_t xor_checksum();//计算异或校验和
    uint16_t ntoh16(uint16_t data);//网络字节序转主机字节序
    int unpack();
    uint16_t get_m1();//接口函数
    uint16_t get_m2();//接口函数
};

#endif
