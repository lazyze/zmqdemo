#include <stdio.h>
#include <arpa/inet.h>
#include <string>

#define uint8_t unsigned char
#define uint32_t unsigned int

int main(int argc,char **argv)
{
    uint8_t ipv6[16]={0};
    char* IPdotdec = argv[1]; 
    uint8_t  prefix = std::stoi(std::string(argv[2]));
    inet_pton(AF_INET6, IPdotdec, (void *)&ipv6);
    printf("十六进制 : %s 掩码位： %d\n",IPdotdec, prefix);
    uint32_t shift_bits = 128 - prefix;
    uint32_t total_shift_byte,remain_shift_bits,j;
    uint8_t host_ipv6 = 0, host2_ipv6 = 0,host_max = 0xFF;
    uint8_t host_ipv6_str = 0;
    uint8_t reach_host_max = 0;
    total_shift_byte = shift_bits/8;  //6   
    remain_shift_bits = shift_bits%8; //6 


    for(j = 0; j < total_shift_byte+1; j++)
    {
        if(j < total_shift_byte){
        //最后的8的整数位置1
          ipv6[15-j] = 0xff;
        }
        else if(remain_shift_bits)
        {
          //最后的除8的余数位置1
          host_ipv6 = ipv6[15-j]>> remain_shift_bits;
          host_ipv6++;
          host_ipv6 = (host_ipv6<<remain_shift_bits)-1;
          ipv6[15-j]= host_ipv6 ;
          break;
        }
    }
    char ipv6_str1[46]={0};
    inet_ntop(AF_INET6,ipv6, ipv6_str1,INET6_ADDRSTRLEN);
    printf("ip最大值 %s, shift addr[%u]\n",  ipv6_str1, 15-j);

    for(j = 0; j < total_shift_byte+1; j++)
    {
        if(j < total_shift_byte){
        //最后的8的整数位置0
          ipv6[15-j] = 0x00;
        }
        else if(remain_shift_bits)
        {
          //最后的除8的余数位置0
          host_ipv6 = ipv6[15-j]>> remain_shift_bits;
          host_ipv6 = host_ipv6<<remain_shift_bits;
          ipv6[15-j]= host_ipv6 ;
          break;
        }

    }
    char ipv6_str2[46]={0};
    inet_ntop(AF_INET6,ipv6, ipv6_str2,INET6_ADDRSTRLEN);
    printf("ip最小值 %s, shift addr[%u]\n",  ipv6_str2, 15-j);

    return 0;
}