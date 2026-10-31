#ifndef RC_PROTOCOL_H
#define RC_PROTOCOL_H
#define PC_PACKET_SIZE 8
#define PC_HEADER_VALUE 0xAA

#include <stdint.h>
#include <Arduino.h>
#include <WiFi.h>

#pragma pack(push,1)

struct PC_Packet{
    uint8_t header;       //包头
    uint8_t safety_lock;  //安全锁
    uint16_t m1_pwm;      //一号电机pwm信号
    uint16_t m2_pwm;      //二号电机pwm信号
    uint8_t seq;          //序列号
    uint8_t checksum;     //校验和
};

#pragma pack(pop)

IPAddress STA_init(const char *ssid, const char *password);
IPAddress AP_init(const char *ssid,const char *password);
int pac_send(WiFiUDP &udp,IPAddress tar_ip,const struct PC_Packet *pac);
int pac_receive(WiFiUDP &udp, struct PC_Packet *pac);

class unpacked{
private:
    struct PC_Packet *raw;
public:
    unpacked();
    unpacked(struct  PC_Packet *ptr);
    unpacked(const uint8_t *raw_pac, int len);
    ~unpacked();
    int pc_packet_verify(const struct PC_Packet *ptr);
    uint8_t xor_checksum(const struct PC_Packet *ptr);
    uint16_t ntoh16(uint16_t data);
    int unpack();
};

#endif
