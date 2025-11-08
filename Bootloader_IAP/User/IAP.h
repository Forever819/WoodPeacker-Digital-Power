


typedef union {
    struct {
        // u8 Major_Version;
        // u8 Minor_Version;
        // u8 Patch_Version;
        u32 OTA_Flag;
        //OTA_Flag
        //0:没有OTA请求
        //1:备份未完成
        //2:A区擦除未完成
        //3:固件接受未完成
        //4:固件校验未完成
        //5:回滚未完成
        // u32 Realse_Date;
        // u32 APP_Address;

        u8  bin_size_raw[4];
        u32 bin_size;
        u32 bin_size_rec;
    };

    u32 buf[64];
}IAP_INFO_u;

void IAP_Init (u32 baudrate);
void IAP_Loop(void);
u32 IAP_GET_Version(void);
u8 IAP_Get_Flash_IAP_INFO(void);
u8 IAP_Update_IAP_INFO (void);
void IAP2APP (void);