//代办：AP to STA和STA to AP走两条不同的UDP通道

#include "rc_protocol.h"


const uint16_t port = 1145;

IPAddress STA_init(const char *ssid, const char *password){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("[+]WiFi connected");
    Serial.println();
    Serial.print("[+]IP address: ");
    Serial.println(WiFi.localIP());
    return WiFi.localIP();
}//STA模式初始化

IPAddress AP_init(const char *ssid,const char *password){
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP(ssid,password)){
        Serial.println("[-]Unexpected error:Initialization faild!");
        return IPAddress(0,0,0,0);//初始化失败返回空IP
    }
    Serial.println("[+]WiFi AP mode initialized");
    Serial.print("[+]AP IP address: ");
    Serial.println(WiFi.softAPIP());
    return WiFi.softAPIP();
}//AP模式初始化

int pac_send(WiFiUDP &udp,IPAddress tar_ip,const struct PC_Packet *pac){
    if (tar_ip == IPAddress(0,0,0,0)){
        Serial.println("[-]Unexpected error:Initialization faild!");
        return -1;
    }
    udp.beginPacket(tar_ip,port);
    udp.write((const uint8_t *)pac,PC_PACKET_SIZE);//发送类型必须是字节指针
    udp.endPacket();
    return 0;
}

int pac_receive(WiFiUDP &udp, struct PC_Packet *pac){
    if (udp.parsePacket()){
        uint8_t buffer[PC_PACKET_SIZE];
        int len = udp.read(buffer,PC_PACKET_SIZE);
        if (len == PC_PACKET_SIZE){
            memcpy(pac, buffer, PC_PACKET_SIZE);
            return 0;
        }
    }
    return -1;
}


//构造函数
unpacked::unpacked() : raw(nullptr) {}
unpacked::unpacked(struct PC_Packet *ptr) : raw(ptr) {}

uint8_t unpacked::xor_checksum(const struct PC_Packet *ptr){
    uint8_t checksum = 0;
    checksum ^= ptr->header;
    checksum ^= ptr->safety_lock;
    checksum ^= (ptr->m1_pwm & 0xFF);
    checksum ^= ((ptr->m1_pwm >> 8) & 0xFF);
    checksum ^= (ptr->m2_pwm & 0xFF);
    checksum ^= ((ptr->m2_pwm >> 8) & 0xFF);
    checksum ^= ptr->seq;
    return checksum;
}//这里决定先使用异或校验和，检验通过后再使用ntoh16转换，保证数据的正确性,转换前&0xff取得数据对应的是转换后的高八位

int unpacked::pc_packet_verify(const PC_Packet *ptr){
    if (ptr == 0) {
        return 0;                 // 空指针
    }
    const struct PC_Packet *packet = ptr;
    if (packet->header != PC_HEADER_VALUE) {
        return 0;                 // 包头不对
    }
    if (xor_checksum(packet) != packet->checksum) {
        return 0;                 // 校验和不匹配
    }
    return 1;                     // 通过
}

uint16_t unpacked::ntoh16(uint16_t data){
    uint8_t byte1 = (data >> 8) & 0xFF;
    uint8_t byte2 = data & 0xFF;
    uint16_t data_ntoh16 = (byte2 << 8) | byte1;
    return data_ntoh16;
}//大小端ntoh16转换

