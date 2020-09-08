//###########################################################################
//
// 101，104规约加密解密
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  09.10| 19 may 2018 | S.H.Q. |
//###################################+########################################
#include "protocol1.h"
#include "protocol3.h"
#include "Encryption.h"
#include "queue.h"
#include "spi.h"
#include "flash.h"
#include "ds1302.h"
#include "global.h"
#include "string.h"
//Super_Sign从0到11分别表示12个优先级，标号越小，优先级越高
//且标号的值不一样表示处在这个优先级的某个阶段
//Super_Sign[0]=0,表示这个优先级的事情没有发生，Super_Sign[0]=1表示这个优先级的事情发生了，且在初始阶段
//Super_Sign[0]=2,表示该优先级事情处在执行阶段，Super_Sign[0]=3表示处于结束阶段
//每个优先级的阶段不一样，阶段数量也不一样
//如果低优先级的事情发生了，那么标志位会置位，但是后面高优先级又发生了，那么高优先级会置位
//每次都会判断这个数组，有高优先级的就先执行高优先级的，不管低优先级的有没有执行
/*-----------------------非平衡优先级----------------------------*/
/*---------------------一级数据优先级-----------------------------*/
//   优先级                应用类型
//      1                 初始化结束                  Super_Sign[0]
//      2                总召（初始化）               Super_Sign[1]
//      3                    遥控                     Super_Sign[2]
//      4             事件报告（状态量变化TCOS）      Super_Sign[3]
//      5               总召（非初始化）              Super_Sign[4]
/*---------------------二级数据优先级-----------------------------*/
//   优先级                应用类型
//      1                  时钟同步                   Super_Sign[5]
//      2                  测试命令                   Super_Sign[6]
//      3               模拟量超越死区                Super_Sign[7]
//      4                  复位进程                   Super_Sign[8]
//      5                  文件召唤                   Super_Sign[9]
//      6                  文件传输                   Super_Sign[10]
//      7                  电能召唤                   Super_Sign[11]
//      8                  参数读写                   Super_Sign[12]
//      自添加						 传输干扰故障               Super_Sign[13]



/*-----------------------平衡优先级----------------------------*/
//   优先级                应用类型																						主动上传
//      1                 初始化结束                  Super_Sign[0]
//      2                总召（初始化）               Super_Sign[1]
//      3                    遥控                     Super_Sign[2]
//      4             事件报告（状态量变化TCOS）      Super_Sign[3]						遥信变位
//      5               总召（非初始化）              Super_Sign[4]
//      6                  故障事件                   Super_Sign[5]						故障事件
//      7                  时钟同步                   Super_Sign[6]
//      8                  变化遥测                		Super_Sign[7]						遥测变化
//      9                  复位进程                   Super_Sign[8]
//      10                 文件召唤                   Super_Sign[9]
//      11                 文件传输                   Super_Sign[10]
//      12                 电能召唤                   Super_Sign[11]
//      13                 参数读写                   Super_Sign[12]
//      14                 链路测试                   Super_Sign[14]
//      自添加						 传输干扰故障               Super_Sign[13]

#define Runpara_Num         21
#define Fixpara_Num         48

struct BACKSTAGE_ENCRYPTION_TYPE1 Decryption;
union Control_Type control_jm;	
FLOAT_BYTE YC_Float;
struct YC_data yc_send;
struct RUN_data run_send;
struct FIX_data fix_send;

u8 Index_1[8]={'C','O','M','T','R','A','D','E'};
u8 Index_2[11]={'H','I','S','T','O','R','Y','/','S','O','E'};
u8 Index_3[10]={'H','I','S','T','O','R','Y','/','C','O'};
u8 Index_4[11]={'H','I','S','T','O','R','Y','/','E','X','V'};
u8 Index_5[13]={'H','I','S','T','O','R','Y','/','F','I','X','P','T'};
u8 Index_6[11]={'H','I','S','T','O','R','Y','/','F','R','Z'};
u8 Index_7[15]={'H','I','S','T','O','R','Y','/','F','L','O','W','R','E','V'};
u8 Index_8[12]={'H','I','S','T','O','R','Y','/','U','L','O','G'};
u8 cpu_style[]={'S','T','M','3','2','F','4','0','7','Z','E','T','6'};
u8 frequence[]={'1','6','8','M','H','z'};
u8 communcation_modle[]={'U','S','R','-','T','C','P','2','3','2','-','D'};
u8 zz_ok=0,wg_ok=0,yw_ok=0;	             //主站、网关、运维认证标志位
u8 Wg_ca1[6]={0x80,0x16,0x00,0x80,0x00,0x08};				//网关验证芯片读取码1
u8 Wg_ca2[6]={0x80,0x18,0x00,0x05,0x00,0x40};				//网关验证芯片读取码2
u8 ID_Spi[8]={0x00,0xb0,0x99,0x05,0x00,0x02,0x00,0x08};
u8 Get_Ter_Random[6]={0x00,0x84,0x00,0x08,0x00,0x00};
u8 Sec_ca1[6]={0x80,0x1A,0x00,0x00,0x00,0x00};
u8 Sec_ca2[6]={0x00,0x84,0x00,0x08,0x00,0x00};	
u8 Recv_JM[4]={0x80,0x2C,0x60,0x01};
u8 Send_JM[4]={0x80,0x26,0x60,0x01};
u8 Read_Cer_len[8]={0x00,0xb0,0x81,0x00,0x00,0x02,0x00,0x02};
u8 decrypt[16]={0};              //升级文件摘要值
u8 ucaFlashBuf[1025]={0};
u8 Super_Sign[15]={0};
u8 Callend_Flag=0;
u8 index_which = 0;    //用来表示当前请求的是哪个目录
u8 renzheng_flag=0;
u32 ulFlieLength = 0;
u32 ulFlashAdd = 0;
u8 Constpara1[]={'F','T','U','\0'};
u8 Constpara2[]={'R','T','X','\0'};
u8 Constpara3[]={'T','D','S','\0'};
u8 Constpara4[]={'H','V','0','9','.','0','9','\0'};
u8 Constpara5[]={'S','V','0','9','.','0','2','1','\0'};
u8 Constpara6[]={'6','6','4','4','\0'};
u8 Constpara7[]={'1','0','1','&','1','0','4','\0'};
u8 Constpara8[]={'Z','W','(','T','D','S',')','\0'};
char Constpara9[]={'F','3','0','X','X','X','X','0','2','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','\0'};
//char Constpara9[]={'F','3','0','X','X','X','X','0','2','0','6','6','2','0','1','9','0','8','1','7','0','0','1','2','\0'};
u8 Constpara10[]={'0','0','5','C','B','9','8','9','4','2','0','7','\0'};
u8 Constpara_Tag[10]={4,4,4,4,4,4,4,4,4,4};   //固有参数TAG类型
u8 Constpara_Len[10]={4,4,4,8,9,5,8,8,25,13};	//固有参数数据长度
static u8 Runpara_Tag[32]={0};        //运行参数TAG类型
static u8 Fixpara_Tag[64]={0,0,0,0,0,0,0,0,1,38,
                           38,1,1,1,38,38,1,1,38,38,
                           38,38,1,38,38,1,38,38,1,1,
                           1,38,1,38,1,38,38,1,38,38,
                           1,38,38,0,0,0,0,0}; //定值参数TAG类型
static u8 Runpara_Len[32]={0};	      //运行参数数据长度
static u8 Fixpara_Len[64]={0};	      //定值参数数据长度
static u8 Runpara_State[32]={0};      //运行参数状态
static u8 Fixpara_State[64]={0};      //定值参数状态
static u8 Constpara_State[10]={0};	
static u8 Params_total;
struct Params_Addr
{
	u8 params_addr[64];
	u8 params_num;
};
struct Params_Addr const_value,run_value,fix_value;

static volatile u8 ProtocolResolveState = 0;	// 协议解析函数状态机
static volatile u8 succ_flag_spi=0;
//just for 104
u8 y_104 =0 ;	//if y_104=0,the protocol is 101;if y_104=1,the protocol is 104
static u8 datdone = 0;
static u16 Recv_Num=0;        //实际I帧接收数据的次数，本机接收
static u16 Send_Num=0;				//实际I帧发送数据的次数，本机发送
static u16 Recv_NumBak=0;			//实际I帧接收数据的次数，客户端接收
static u16 Send_NumBak=0;			//实际I帧发送数据的次数，客户端发送
static u8 diff_flag=0;				//用来表示发送序列和接收序列只差超过12的第一次
static u8 call_num1=0;
static u8 Informat_addr[3]={0};

//101 & 104
static u8 channel = 0;			                  // 数据通道
static u8 num_flag=0;                         //标记接收证书的次数
static u8 datdone_serial1=0,YY_Style1=0,BW_Len1=0;
static u8 Wg_Random1[8]={0};
static u8 Zz_Random1[8]={0};				          //主站随机数，这个需要保存，后续有用
static u8 Ter_Random1[8]={0};
static u8 Ter_Random2[8]={0};
static u8 Wg_Sign1[70]={0};
static u8 Msg_Safty1[80]={0};
static u8 Msg_Safty_sj[80]={0};		    //远程升级安全信息
static u8 Spi_Recv1[1024]={0};
static u8 BW_Before_JM1[256]={0};
static u8 Cer_data[1024]={0};				  //证书管理工具证书
static u8 Cer_ID[8]={0};						  //证书管理工具ID
static u8 chip_ID[8]={0};						  //安全芯片ID
static u8 pubkey[64]={0};						  //芯片公钥
static u8 frame_num=0;							  //当前帧序号
static u8 frame_total=0;							//总帧数
static u8 flag_succ=0;                //初始化完成标记，1完成，0未完成
static u8 resend_num=0; 	            //重发次数
static u8 Test_Sign[7]={0};
static u8 Super_Sign_bak[15]={0};
static u8 send_buffer[512]={0};	      //发送缓存，每次发送都要缓存，等到下次接收成功则清除
static u8 call_num=0;
static u8 jm=0;
static u8 params_flag=0;
static u8 yx_data[10][10]={0};	      //遥信缓存数据
static u8 yx_data_flag=0;	            //是否有遥信缓存
static u8 yx_data_num=0;	            //遥信缓存个数
static u8 yk_state=0;	                //用来表示遥控的初始状态，选择状态，执行/撤销状态,0:初始状态；1：选择状态；2：撤销状态；3：执行状态
static u8 FCB_Flag_Main=0;    				//主站报文的FCB位
static u8 FCB_Flag_Slave=0;   				//终端报文的FCB位
static u8 List_Name1[50]={0};					//文件名
static u8 List_Num1=0;								//文件名长度
static u8 List_Name11[50]={0};				//目录名
static u8 List_Num11=0;								//目录名长度
static u8 Finish_Flag1=0;         		//写文件成功标志
static u8 Rec_Error_Flag1=0;        	//写文件错误标志

static u16 Spi_Sw1=0; 								//spi返回的状态字
static u16 Spi_Length1=0;             //spi返回的长度
static u16 Cer_len=0;       				  //终端证书总长度
static u16 ADDR=0;                    //地址域A
static u16 Asdu_addr=0;               //ASDU地址
static u16 Object_addr = 0;           //信息对象地址
static u16 send_buffer_len=0;
static u16 remain1=0,Length_serial1=0;
static u16 RxCounter_serial1=0;
static u16 MsgSafty_Len1=0;
static u16 SN_Num1=0;							    //当前定值区，初始默认为0
static u32 Send_Doc_Num1=0;           //发送文件数据字节数
static u32 Rec_Doc_Num1=0;					  //写入文件数据字节数
static u32 Rec_num_101=0;
static u32 segment_num_last=0;
static u32 A = 0x67452301, B = 0xefcdab89, C = 0x98badcfe, D = 0x10325476;
static u32 mid1 = 0, mid2 = 0, mid3 = 0, mid4 = 0;
static u32 a, b, c, d, x[16];
static u32 ulSampleIndex = 0;
static u32 ulDataIndex = 0;
static u32 ulReadCnt = 0;


//just for 104
/*************************U型帧应答函数****************************/
void IEC104_Answer_U(u8 link,u8 USARTxChannel)//固定帧	地址域1个字节
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);   		//长度高字节
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0C);   		//长度低字节
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);					//报文类型：不加密
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);				//应用类型：只有原始报文
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x6);				//原始报文长度
	sum+=0x6;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);				//应答报文
	sum+=0x68;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x04);
	sum+=0x04;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],link);
	sum+=link;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);					//安全信息体长度
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);				//校验码
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);				//帧尾
}

void IEC104_Answer_S(u8 USARTxChannel)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);   		//长度高字节
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0C);   		//长度低字节
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);					//报文类型：不加密
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);				//应用类型：只有原始报文
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x6);				//原始报文长度
	sum+=0x6;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);				//应答报文
	sum+=0x68;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x04);
	sum+=0x04;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x1);
	sum+=0x1;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(Recv_Num<<1)&0xFE);
	sum+=(u8)(Recv_Num<<1)&0xFE;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(Recv_Num>>7)&0xFF);
	sum+=(u8)(Recv_Num>>7)&0xFF;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);					//安全信息体长度
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);				//校验码
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);				//帧尾
}

void MD5(void)
{
	a = A, b = B, c = C, d = D;

	FF(a, b, c, d, x[0], 7, 0xd76aa478);
	FF(d, a, b, c, x[1], 12, 0xe8c7b756);
	FF(c, d, a, b, x[2], 17, 0x242070db);
	FF(b, c, d, a, x[3], 22, 0xc1bdceee);
	FF(a, b, c, d, x[4], 7, 0xf57c0faf);
	FF(d, a, b, c, x[5], 12, 0x4787c62a);
	FF(c, d, a, b, x[6], 17, 0xa8304613);
	FF(b, c, d, a, x[7], 22, 0xfd469501);
	FF(a, b, c, d, x[8], 7, 0x698098d8);
	FF(d, a, b, c, x[9], 12, 0x8b44f7af);
	FF(c, d, a, b, x[10],17, 0xffff5bb1);
	FF(b, c, d, a, x[11],22, 0x895cd7be);
	FF(a, b, c, d, x[12],7, 0x6b901122);
	FF(d, a, b, c, x[13],12, 0xfd987193);
	FF(c, d, a, b, x[14],17, 0xa679438e);
	FF(b, c, d, a, x[15],22, 0x49b40821);

	GG(a, b, c, d, x[1], 5, 0xf61e2562);
	GG(d, a, b, c, x[6], 9, 0xc040b340);
	GG(c, d, a, b, x[11],14, 0x265e5a51);
	GG(b, c, d, a, x[0], 20, 0xe9b6c7aa);
	GG(a, b, c, d, x[5], 5, 0xd62f105d);
	GG(d, a, b, c, x[10],9, 0x02441453);
	GG(c, d, a, b, x[15],14, 0xd8a1e681);
	GG(b, c, d, a, x[4], 20, 0xe7d3fbc8);
	GG(a, b, c, d, x[9], 5, 0x21e1cde6);
	GG(d, a, b, c, x[14],9, 0xc33707d6);
	GG(c, d, a, b, x[3], 14, 0xf4d50d87);
	GG(b, c, d, a, x[8], 20, 0x455a14ed);
	GG(a, b, c, d, x[13],5, 0xa9e3e905);
	GG(d, a, b, c, x[2], 9, 0xfcefa3f8);
	GG(c, d, a, b, x[7], 14, 0x676f02d9);
	GG(b, c, d, a, x[12],20, 0x8d2a4c8a);

	HH(a, b, c, d, x[5], 4, 0xfffa3942);
	HH(d, a, b, c, x[8], 11, 0x8771f681);
	HH(c, d, a, b, x[11],16, 0x6d9d6122);
	HH(b, c, d, a, x[14],23, 0xfde5380c);
	HH(a, b, c, d, x[1], 4, 0xa4beea44);
	HH(d, a, b, c, x[4], 11, 0x4bdecfa9);
	HH(c, d, a, b, x[7], 16, 0xf6bb4b60);
	HH(b, c, d, a, x[10],23, 0xbebfbc70);
	HH(a, b, c, d, x[13],4, 0x289b7ec6);
	HH(d, a, b, c, x[0], 11, 0xeaa127fa);
	HH(c, d, a, b, x[3], 16, 0xd4ef3085);
	HH(b, c, d, a, x[6], 23, 0x04881d05);
	HH(a, b, c, d, x[9], 4, 0xd9d4d039);
	HH(d, a, b, c, x[12],11, 0xe6db99e5);
	HH(c, d, a, b, x[15],16, 0x1fa27cf8);
	HH(b, c, d, a, x[2], 23, 0xc4ac5665);

	II(a, b, c, d, x[0], 6, 0xf4292244);
	II(d, a, b, c, x[7], 10, 0x432aff97);
	II(c, d, a, b, x[14],15, 0xab9423a7);
	II(b, c, d, a, x[5], 21, 0xfc93a039);
	II(a, b, c, d, x[12],6, 0x655b59c3);
	II(d, a, b, c, x[3], 10, 0x8f0ccc92);
	II(c, d, a, b, x[10],15, 0xffeff47d);
	II(b, c, d, a, x[1], 21, 0x85845dd1);
	II(a, b, c, d, x[8], 6, 0x6fa87e4f);
	II(d, a, b, c, x[15],10, 0xfe2ce6e0);
	II(c, d, a, b, x[6], 15, 0xa3014314);
	II(b, c, d, a, x[13],21, 0x4e0811a1);
	II(a, b, c, d, x[4], 6, 0xf7537e82);
	II(d, a, b, c, x[11],10, 0xbd3af235);
	II(c, d, a, b, x[2], 15, 0x2ad7d2bb);
	II(b, c, d, a, x[9], 21, 0xeb86d391);

	A += a;
	B += b;
	C += c;
	D += d;
}
void data_reset(void)
{
	ulSampleIndex = 0;
	ulDataIndex = 0;
	ulReadCnt = 0;
	ulFlashAdd = 0;
	A = 0x67452301;
	B = 0xefcdab89;
	C = 0x98badcfe;
	D = 0x10325476;
	a = 0;
	b = 0;
	c = 0;
	d = 0;
	memset(x, 0x00, sizeof(x));
}
void ReadGroupTempBuf(void)
{
	u8 j, k;

	memset(x, 0, 64);
	ulSampleIndex = 0;
	for (j = 0; j < 16; j++)
	{
		for (k = 0; k < 4; k++)
		{
			if ((ulReadCnt >= Rec_Doc_Num1 / 1024) && (ulDataIndex >= Rec_Doc_Num1 % 1024))
				break;
			((char *)x)[ulSampleIndex] = ucaFlashBuf[ulDataIndex];
			ulDataIndex++;
			ulSampleIndex++;
		}
	}
}

void GetBinFileMd5(u8 *pEsult)
{
	u32 usCnt = 0;
	u32 ulFileLen[2] = {0};


	/* ?????? */
	for (ulReadCnt = 0; ulReadCnt < Rec_Doc_Num1 / 1024; ulReadCnt++)
	{
		SPI_Flash_Read(ucaFlashBuf,FLASH_KZQUPDATE_ADDR+ulFlashAdd,1024);
		ulFlashAdd += 1024;
		for (usCnt = 0; usCnt < 16; usCnt++)
		{
			ReadGroupTempBuf();
			MD5();
		}
		ulDataIndex = 0;
	}
	/* ?????? */
	memset(ucaFlashBuf, 0, 1025);

	SPI_Flash_Read(ucaFlashBuf,FLASH_KZQUPDATE_ADDR+ulFlashAdd,1024);
	ReadGroupTempBuf();
	for (usCnt = 0; usCnt < (Rec_Doc_Num1 % 1024) / 64; usCnt++)
	{
		MD5();
		ReadGroupTempBuf();
	}
	/* ?????1,?0??,128????10000000 */
	((char *)x)[Rec_Doc_Num1 % 64] = 128;
	if (Rec_Doc_Num1 % 64 > 55)
	{
		MD5(), memset(x, 0, 64);
	}
	/* ??????????bit?? */
	ulFileLen[1] = Rec_Doc_Num1 / 0x20000000;
	ulFileLen[0] = (Rec_Doc_Num1 % 0x20000000) * 8;
	memcpy(x + 14, ulFileLen, 8);
	MD5();

	//	sprintf(pEsult, "%08X%08X%08X%08X", PP(A), PP(B), PP(C), PP(D));
	mid1 = PP(A);
	mid2 = PP(B);
	mid3 = PP(C);
	mid4 = PP(D);
	pEsult[0] |= (u8)(mid1 >> 24);;
	pEsult[1] |= (u8)(mid1 >> 16);
	pEsult[2] |= (u8)(mid1 >> 8);
	pEsult[3] |= (u8)(mid1);
	pEsult[4] |= (u8)(mid2 >> 24);
	pEsult[5] |= (u8)(mid2 >> 16);
	pEsult[6] |= (u8)(mid2 >> 8);
	pEsult[7] |= (u8)(mid2);
	pEsult[8] |= (u8)(mid3 >> 24);
	pEsult[9] |= (u8)(mid3 >> 16);
	pEsult[10] |= (u8)(mid3 >> 8);
	pEsult[11] |= (u8)(mid3);
	pEsult[12] |= (u8)(mid4 >> 24);
	pEsult[13] |= (u8)(mid4 >> 16);
	pEsult[14] |= (u8)(mid4 >> 8);
	pEsult[15] |= (u8)(mid4);
}

u8 Ter_WriteRead_Spi1(void)
{
	u16 i=0,delay_n=0,count=0;	
	u8 mid=0,crc_yhf=0;
	u8 spi_crc=0,spi_flag=0,delay_flag=0,succ_flag=0;
	u8 succ_flag1=0;
	static u8 spi_flag1=0;

	while(succ_flag!=3)
	{ 
		SSN_L();																	 	 //再次拉低准备接收状态字
		delay_flag=0;
		while(delay_flag==0)												 //延时超时跳出
		{
			if(SPI2_ReadWrite(0)==0x55)
			{
				crc_yhf=0;
				Spi_Sw1  = ((u16)SPI2_ReadWrite(0)&0xFF)<<8;
				crc_yhf^=((u8)(Spi_Sw1>>8)&0xFF);
				Spi_Sw1 |= SPI2_ReadWrite(0)&0xFF;								 //接收状态字
				crc_yhf^=((u8)Spi_Sw1&0xFF);
				Spi_Length1 = ((u16)SPI2_ReadWrite(0)&0xFF)<<8;
				crc_yhf^=((u8)(Spi_Length1>>8)&0xFF);
				Spi_Length1 |= SPI2_ReadWrite(0)&0xFF;						 //接收返回字节数
				crc_yhf^=((u8)Spi_Length1&0xFF);
//				mid=SPI2_ReadWrite(0)&0xFF;
//				Spi_Sw1  = (u16)mid<<8&0xff00;
//				crc_yhf^=(mid&0xFF);
//				mid=SPI2_ReadWrite(0)&0xFF;
//				Spi_Sw1 |= mid&0xFF;
//				crc_yhf^=(mid&0xFF);
//				mid=SPI2_ReadWrite(0)&0xFF;
//				Spi_Length1 = (u16)mid<<8&0xff00;
//				crc_yhf^=(mid&0xFF);
//				mid=SPI2_ReadWrite(0)&0xFF;
//				Spi_Length1 |= mid&0xFF;
//				crc_yhf^=(mid&0xFF);

				if(Spi_Length1>1000) Spi_Length1=0;
				else
				{
					for(i=0;i<Spi_Length1;i++)
					{
						Spi_Recv1[i]=SPI2_ReadWrite(0);
						crc_yhf^=Spi_Recv1[i];
					}
				}
				mid=~crc_yhf;
				crc_yhf=mid;
				spi_crc=SPI2_ReadWrite(0);
				SSN_H();
				if(spi_crc!=crc_yhf) 
				{
					delay_flag=1;				 									 //验证错误只跳出当前循环	
					spi_flag  =3;											
					succ_flag1++;	
					succ_flag=succ_flag1;
					SSN_H();	
				}
				else if(Spi_Sw1==0x6A90) 
				{
					delay_flag =1;
					succ_flag  =3;											
					spi_flag1++;													//三次后自动跳出	
					spi_flag=spi_flag1;
					if(spi_flag1==3)  spi_flag1=0;
				}
				else if (Spi_Sw1==0x9000)
				{
					delay_flag =1;
					succ_flag  =3;			
					spi_flag   =3;												//接收成功也跳出这san个循环		
					succ_flag_spi=1;
				}
				else 
				{
					delay_flag =1;
					succ_flag  ++;			
					spi_flag   =3;												//接收成功也跳出这san个循环		
				}
			}	
			else 
			{
				while(count++<600);
				count=0;
				delay_n++;	
				if(delay_n>=2000)
				{
					delay_n  =0;
					delay_flag =1;
					succ_flag  ++;			
					spi_flag   =3;												//延时超时跳出san个循环
					SSN_H();	
				}
			}																		
		}
	}	
	return spi_flag;
}

u8 YY_00_WR_SPI1(u16 lc,u8 Send_len)
{
	u16 i=0;	
	u8 crc_yhf=0;
	u8 spi_flag=0;
	while(spi_flag!=3)															
	{
		SSN_L();	
		SPI2_ReadWrite(0x55);														//帧头
		for(i=0;i<4;i++)															  //参数
		{
			SPI2_ReadWrite(Send_JM[i]);	
			crc_yhf^=Send_JM[i];
		}
		SPI2_ReadWrite((u8)(lc>>8)&0xFF);								//总长度
		crc_yhf^=(u8)(lc>>8)&0xFF;
		SPI2_ReadWrite((u8)lc&0xFF);
		crc_yhf^=(u8)lc&0xFF;
		for(i=0;i<8;i++) 															  //主站随机数
		{
			SPI2_ReadWrite(Zz_Random1[i]);
			crc_yhf^=Zz_Random1[i];
		}	
		for(i=0;i<8;i++) 															  //主站随机数反码
		{
			SPI2_ReadWrite(~Zz_Random1[i]);
			crc_yhf^=(~Zz_Random1[i]);
		}	
		SPI2_ReadWrite(0x00);														//应用类型
		crc_yhf^=0x00;
		SPI2_ReadWrite(Send_len);												//报文长度
		crc_yhf^=Send_len;
		for(i=0;i<Send_len;i++)												  //报文内容
		{
			SPI2_ReadWrite(BW_Before_JM1[i]);	
			crc_yhf^=BW_Before_JM1[i];
		}
		SPI2_ReadWrite(0x00);													   //随机数长度高字节
		crc_yhf^=0x00;		
		SPI2_ReadWrite(0x00);														 //随机数长度低字节
		crc_yhf^=0x00;		
		crc_yhf=~crc_yhf;
		SPI2_ReadWrite(crc_yhf);	
		crc_yhf=0;
		SSN_H();											                   //拉高片选
		spi_flag=Ter_WriteRead_Spi1();
	}
	return succ_flag_spi;
}

void Receive_Doc_101(void)          //处理接收到的文件数据
{
	u8 i=0,sum=0;
	u8 number = 0,number1 = 0;
	u8 pbuffer[255]={0};
	u32 mid=0;
	u32 segment_num=0;
	
	if(y_104==0)
	{
		number =23;
		number1 = 3;
	}
	else
	{
		number = 25;
		number1 = 1;
	}
	segment_num|=((u32)Decryption.DataBuffer[21]&0x000000ff);
	segment_num|=((((u32)Decryption.DataBuffer[22])<<8)&0x0000ff00);
	segment_num|=((((u32)Decryption.DataBuffer[23])<<16)&0x00ff0000);
	segment_num|=((((u32)Decryption.DataBuffer[24])<<24)&0xff000000);
	if(segment_num!=Rec_num_101) 
	{
		Rec_Error_Flag1=1;																				//当前帧的段号应该等于前面几次发送文件数据字节数之和，如果不等于则置位错误标志	
	}
	segment_num_last=segment_num;

	for(i=0;i<(Decryption.DataBuffer[1]-number);i++)								//将本次接收数据存储起来
	{
		//ucaFlashBuf[i]=Decryption.DataBuffer[26+i];							//ucaFlashBuf只有100字节，会溢出								
		pbuffer[i]=Decryption.DataBuffer[26+i];						        //每次接收的数据先存到pbuffer[]中去
		sum+=Decryption.DataBuffer[26+i];
	}
	if(sum!=Decryption.DataBuffer[Decryption.DataBuffer[1]+number1])	//如果校验值不对，错误标志位置一
	{
		Rec_Error_Flag1=2;
	}
	if(Rec_Error_Flag1==0)//说明没有上面的错误
	{
		if(Decryption.DataBuffer[25]==0)																			//无后续，本次即为最后一帧，那么就要判断总字节数对不对
		{
			mid=Rec_num_101;
			Rec_num_101+=Decryption.DataBuffer[1]-number;																  //直到本次接收数据个数之和	
			if(Rec_num_101!=Rec_Doc_Num1)
			{
				Rec_Error_Flag1=3;																	//接收的所有帧的字节之和与写文件激活帧里最后发来的四字节（表文件长度）里的长度对不对
				Rec_num_101=mid;
			}
			else
			{
				CS2BZ=0;
				while(1)
				{
					if(Flash_Writeflag == 0)
					{
						SPI_Flash_Write(pbuffer,FLASH_KZQUPDATE_ADDR+mid,Decryption.DataBuffer[1]-number);				//地址为接收的段号，因为我们存储程序的首地址从0开始，正好与段号相对应，长度为每次接收到的字节数		
						break;
					}
				}	
				Rec_num_101=0;																																	//对于这个静态变量，最后一次需要清0
			}				
		}
		else //之前帧接收
		{
			CS2BZ=0;
			while(1)
			{
				if(Flash_Writeflag == 0)
				{
					SPI_Flash_Write(pbuffer,FLASH_KZQUPDATE_ADDR+Rec_num_101,Decryption.DataBuffer[1]-number);				//地址为接收的段号，因为我们存储程序的首地址从0开始，正好与段号相对应，长度为每次接收到的字节数
					break;
				}
			}	
			Rec_num_101+=Decryption.DataBuffer[1]-number;																  //直到本次接收数据个数之和	
		}	
	}
}

void FZ_Reback(u8 USARTxChannel,u8 style,u16 sign)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //报头
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //报文类型
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],style); //应用类型
	sum+=style;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x02); //安全信息长度
	sum+=0x02;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(sign>>8)&0xFF);	                //响应结果
	sum+=(u8)(sign>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)sign&0xFF);	                //响应结果
	sum+=(u8)sign&0xFF;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //安全信息
}

void Send_Random(u8 USARTxChannel,u8 sign,u8 *ID)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0D);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //报头
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //报文类型
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sign); //应用类型
	sum+=sign;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x08); //安全信息长度
	sum+=0x08;
	
	for(i=0;i<8;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],ID[i]);
		sum+=ID[i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //安全信息
}
void Send_Ver(u8 USARTxChannel,u8 ver,u8 *rand)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0E);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //报头
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //报文类型
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x35); //应用类型
	sum+=0x35;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x09); //安全信息长度
	sum+=0x09;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],ver);  //版本号
	sum+=ver;
	
	for(i=0;i<8;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],rand[i]);
		sum+=rand[i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //安全信息
}


void Send_Ter_ID(u8 USARTxChannel)
{
	u8 i=0,sum=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x1D);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //报头
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //报文类型
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x37); //应用类型
	sum+=0x37;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x18); //安全信息长度
	sum+=0x18;
	
	for(i=0;i<24;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Constpara9[i]);
		sum+=Constpara9[i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //安全信息
}

void Send_sign(u8 USARTxChannel,u8 *rand)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x45);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //报头
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //报文类型
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x3D); //应用类型
	sum+=0x3D;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //安全信息长度
	sum+=0x40;
	
	for(i=0;i<64;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],rand[i]);
		sum+=rand[i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //安全信息
}

void Send_Ter_Cer(u8 USARTxChannel,u8 frame_num,u8 frame_total,u8 num)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],num+8);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //报头
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //报文类型
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x43); //应用类型
	sum+=0x43;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],num+3); //安全信息长度
	sum+=num+3;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],6);
	sum+=6;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],frame_total);
	sum+=frame_total;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],frame_num);
	sum+=frame_num;

	for(i=0;i<num;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Cer_data[(frame_num-1)*200+i]);
		sum+=Cer_data[(frame_num-1)*200+i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //安全信息报尾
}

void Ter_Cer_zz(u8 USARTxChannel,u8 frame_num,u8 frame_total,u8 num)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],num+8);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //报头
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x01); //报文类型
	sum+=0x01;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x75); //应用类型
	sum+=0x75;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],num+3); //安全信息长度
	sum+=num+3;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],6);
	sum+=6;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],frame_total);
	sum+=frame_total;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],frame_num);
	sum+=frame_num;

	for(i=0;i<num;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Cer_data[(frame_num-1)*200+i]);
		sum+=Cer_data[(frame_num-1)*200+i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //安全信息报尾
}

void Ter_Req_WgZz1(u16 msg_len,u8 USARTxChannel,u8 sign,u8 yy_sty)
{
	u16 i=0,bw_len=0;
	u8 sum=0;
	bw_len=msg_len+5;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(bw_len>>8)&0xFF);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)bw_len&0xFF);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	if(yy_sty<0x30)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x80);									//报文标识不知道是否正确
		sum+=0x80;
	}
	else
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
		sum+=0x00;
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],yy_sty);									//应用类型：终端确认并请求网关认证
	sum+=yy_sty;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(msg_len>>8)&0xFF);//安全信息体长度
	sum+=(u8)(msg_len>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)msg_len&0xFF);	
	sum+=(u8)msg_len&0xFF;
	if(yy_sty==0x55||yy_sty==0x61)
	{
		for(i=0;i<msg_len;i++)																																							//安全信息体数据
		{
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Spi_Recv1[i]);
			sum+=Spi_Recv1[i];
		}	
	}
	else
	{
		for(i=0;i<msg_len-1;i++)																																							//安全信息体数据
		{
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Spi_Recv1[i]);
			sum+=Spi_Recv1[i];
		}	
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sign);	                //签名密钥标识
		sum+=sign;
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);									//校验和
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);									//帧尾
}

void Ter_Re_WgZz1(u8 USARTxChannel,u16 sign,u8 yy_sty)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	if(yy_sty<0x30)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x80);									//报文标识不知道是否正确
		sum+=0x080;
	}
	else
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
		sum+=0x00;
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],yy_sty);									//应用类型：终端响应网关
	sum+=yy_sty;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],00);//安全信息体长度
	sum+=00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x02);	
	sum+=0x02;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(sign>>8)&0xFF);	                //响应结果
	sum+=(u8)(sign>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)sign&0xFF);	                //响应结果
	sum+=(u8)sign&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);									//校验和
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);									//帧尾

}

void Common_Style1(u16 length,u8 USARTxChannel,u8 * Send)
{
	u16 i=0,bw_len=0;
	u8 sum=0;
	bw_len=length+2;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(bw_len>>8)&0xFF);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)bw_len&0xFF);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x48);									//报文标识不知道是否正确
	sum+=0x48;
	for(i=0;i<length;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Send[i]);	
		sum+=Send[i];
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);	
}

//明通用发送函数
void Common_MW(u8 USARTxChannel,u8 ca)
{
	u8 sum=0,sum1=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0C);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);									//报文类型
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);									//应用类型
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x06);									//应用类型长度
	sum+=0x06;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x10);
	sum+=0x10;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],ca);	
	sum+=ca;
	sum1+=ca;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(ADDR&0xFF));	
	sum1+=(ADDR&0xFF);
	sum+=(ADDR&0xFF);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],ADDR>>8);
	sum+=(ADDR>>8);
	sum1+=(ADDR>>8);	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum1);
	sum+=sum1;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	sum+=0x16;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);									//安全信息长度
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);		
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);									//安全信息长度
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);		
}

void Re_error_msg(u8 USARTxChannel,u8 sty,u8 style,u16 sign)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //报头
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sty); //报文类型
	sum+=sty;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],style); //应用类型
	sum+=style;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x02); //安全信息长度
	sum+=0x02;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(sign>>8)&0xFF);	                //响应结果
	sum+=(u8)(sign>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)sign&0xFF);	                //响应结果
	sum+=(u8)sign&0xFF;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //安全信息
}

u8 Conclude_Initinal_BefJM(u8 length,u8 ca)
{
	u8 P_Reaet[1]={0};
	u8 sum=0,i=0;
	if(y_104==1)
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=M_EI_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=INIT;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;

		SPI_Flash_Read(P_Reaet,FLASH_RESETYY_ADDR,1);
		if(P_Reaet[0]) 	BW_Before_JM1[i++]=2;
		else 	BW_Before_JM1[i++]=1;		
	}
	else
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_EI_NA_1;
		sum+=M_EI_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=INIT;
		sum+=INIT;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------初始化原因------------*/
		SPI_Flash_Read(P_Reaet,FLASH_RESETYY_ADDR,1);
		if(P_Reaet[0]) 	
		{
			BW_Before_JM1[i++]=2;
			sum+=2;
		}
		else 
		{
			BW_Before_JM1[i++]=1;	
			sum+=1;
		}		
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	return i;		
}

u8 GeneralCall_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_IC_NA_1;
		sum+=C_IC_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------召唤限定词------------*/
		BW_Before_JM1[i++]=QOI_GenCall;
		sum+=QOI_GenCall;
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_IC_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=QOI_GenCall;	
	}
	return i;		
}

u8 YX_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0,j=0;
	u8 buffer[YX_Number] = {0};
	
	buffer[Measure101Para.para.far_positoin] = FAR_IN6^0x01;//远方
	buffer[Measure101Para.para.local_positoin] = LOCAL_IN7^0x01;//就地
	if(DYXH1 == 1 && DYXH2 == 1)	
	{
	  buffer[Measure101Para.para.AC_loss] = 0x01;               //交流失电告警
	}
	else
	  buffer[Measure101Para.para.AC_loss] = 0;
	buffer[Measure101Para.para.Bat_low] = KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage;//电池欠压告警
	buffer[Measure101Para.para.low_airlock] = 0;
	buffer[Measure101Para.para.lock_on] = Switchon_Lock | Switchon_Lock1 | Switchon_Lock2; //闭锁合闸
	buffer[Measure101Para.para.lock_off] = Switchoff_Lock | Switchoff_Lock1;                //闭锁分闸  
	buffer[Measure101Para.para.break_on] = HW_IN2^0x01;//合位
	buffer[Measure101Para.para.break_off] = FW_IN1^0x01;//分位
	//buffer[Measure101Para.para.break_off] = 0x01;//分位
	buffer[Measure101Para.para.wcn_state] = WCN_IN3^0x01;//未储能
	//buffer[Measure101Para.para.wcn_state] = 0x01;//未储能
	buffer[Measure101Para.para.max_current1_alarm] = 0;//10
	buffer[Measure101Para.para.max_current2_alarm] = 0;
	buffer[Measure101Para.para.max_load_alarm] = KZQMeasureData.para.AlarmFlag.bit.max_load;//过负荷告警
	buffer[Measure101Para.para.zero_current1_alarm] = KZQMeasureData.para.AlarmFlag.bit.zero_max_current;
	buffer[Measure101Para.para.ground_current_alarm] = KZQMeasureData.para.AlarmFlag.bit.ground;//单相接地故障告警
	buffer[Measure101Para.para.max_current1_protec] = KZQMeasureData.para.ProtectFlag1.bit.fast_off;//过流I段
	buffer[Measure101Para.para.max_current2_protec] = KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off;//过流II段
	buffer[Measure101Para.para.zero_current1_protec] = KZQMeasureData.para.ProtectFlag1.bit.zero_max_current;//零序过流保护
	buffer[Measure101Para.para.zero_currentafter_protec] = 0;
	buffer[Measure101Para.para.reon] = KZQMeasureData.para.ProtectFlag2.bit.reon_act; //重合闸
	buffer[Measure101Para.para.max_currentafter_protec] = KZQMeasureData.para.ProtectFlag1.bit.max_current_after;//过流后加速保护//20
	buffer[Measure101Para.para.max_voltage] = KZQMeasureData.para.ProtectFlag2.bit.max_voltage;
	buffer[Measure101Para.para.high_freq] = KZQMeasureData.para.ProtectFlag2.bit.max_freq;
	buffer[Measure101Para.para.low_freq] = KZQMeasureData.para.ProtectFlag2.bit.low_freq;
	if(ProtectFlag==1 || (KZQMeasureData.para.AlarmFlag.word & 0x01ff))  //事故总
	  buffer[Measure101Para.para.event_all] = 0x01;
	else
		 buffer[Measure101Para.para.event_all] = 0;
	buffer[Measure101Para.para.max_current3_protec] = KZQMeasureData.para.ProtectFlag1.bit.max_current; //过流III段
	if(KZQMeasureData.para.SelfCheck.word & 0x3f0f)
	{
	  buffer[Measure101Para.para.break_err] = 0x01;                                           //装置异常告警
	}
	else
	  buffer[Measure101Para.para.break_err] = 0; 
	
	 if(AutoswitchSet.para.segment_contact_mode == 0x01)	                  //分段点工作模式
	{
	  buffer[Measure101Para.para.segment_mode] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.segment_mode] = 0;                              
  if(AutoswitchSet.para.segment_contact_mode == 0x02)	                  //联络点工作模式
	{
	  buffer[Measure101Para.para.contact_mode] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.contact_mode] = 0;                           
	if(ProtectTimeout.para.novoltage_value_A == 1)	                      //电源侧无压
	{
	  buffer[Measure101Para.para.novoltage_A] = 0x01;       
	}
	else
	  buffer[Measure101Para.para.novoltage_A] = 0;    
  if(ProtectTimeout.para.novoltage_value_B == 1)	                      //负载侧无压
	{
	  buffer[Measure101Para.para.novoltage_B] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.novoltage_B] = 0;    
	if(KZQMeasureData.para.ProtectFlag1.bit.loss_power)	                  //失压分闸
	{
	  buffer[Measure101Para.para.powerloss_off] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.powerloss_off] = 0;    
	buffer[Measure101Para.para.bat_active] = KZQMeasureData.para.AlarmFlag.bit.bat_active;//电池活化
	
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_SP_NA_1;
		sum+=M_SP_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0xA8;
		sum+=0xA8;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=INTROGEN;
		sum+=INTROGEN;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------遥信------------*/
		 for(j=0;j<YX_Number;j++)
		{
			BW_Before_JM1[i++]=buffer[j];
			sum+=buffer[j];	
		}

		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_SP_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0xA8;
		sum+=0xA8;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=INTROGEN;
		BW_Before_JM1[i++]=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		/*-----------遥信------------*/
		 for(j=0;j<YX_Number;j++)
		{
			BW_Before_JM1[i++]=buffer[j];
		}
	}
	return i;		
}

u8 YC_BefJM(u8 length,u8 ca)
{
	u8 sum=0,j=0,i=0;
	
	if(y_104==0)
	{
		yc_send.yc_meas[Measure101Para.para.AC_power].floatdata=0;
		yc_send.yc_meas[Measure101Para.para.Bat_volt].floatdata=(float)MeasureData.V_BAT/100;
		yc_send.yc_meas[Measure101Para.para.UAB].floatdata=(float)MeasureData.Display_UAB_val/1000;
		yc_send.yc_meas[Measure101Para.para.UCB].floatdata=(float)MeasureData.Display_UBC_val/1000;
		//yc_send.yc_meas[Measure101Para.para.UAB].floatdata=9.9;
		//yc_send.yc_meas[Measure101Para.para.UCB].floatdata=10.1;
		yc_send.yc_meas[Measure101Para.para.U0].floatdata=(float)MeasureData.Display_U0_val/1000;
		yc_send.yc_meas[Measure101Para.para.Sig_dBm].floatdata=0;
		yc_send.yc_meas[Measure101Para.para.freq].floatdata=(float)MeasureData.freq/100;
		yc_send.yc_meas[Measure101Para.para.res7].floatdata=0;
		yc_send.yc_meas[Measure101Para.para.res8].floatdata=0;
		yc_send.yc_meas[Measure101Para.para.IA].floatdata=(float)MeasureData.Display_IA_val*3/25;
		yc_send.yc_meas[Measure101Para.para.IB].floatdata=(float)MeasureData.Display_IB_val*3/25;
		yc_send.yc_meas[Measure101Para.para.IC].floatdata=(float)MeasureData.Display_IC_val*3/25;
		//yc_send.yc_meas[Measure101Para.para.IA].floatdata=599;
		//yc_send.yc_meas[Measure101Para.para.IB].floatdata=600;
		//yc_send.yc_meas[Measure101Para.para.IC].floatdata=601;
		
		if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
			yc_send.yc_meas[Measure101Para.para.I0].floatdata=(float)MeasureData.Display_I0_val/1000;
		else
			yc_send.yc_meas[Measure101Para.para.I0].floatdata=(float)MeasureData.Protect_10I0_val/100;
		
		yc_send.yc_meas[Measure101Para.para.P].floatdata=MeasureData.P_val*12;
		yc_send.yc_meas[Measure101Para.para.Q].floatdata=(float)MeasureData.Q_val*12;
		yc_send.yc_meas[Measure101Para.para.cos].floatdata=(float)MeasureData.cosALL/1000;
		yc_send.yc_meas[Measure101Para.para.res9].floatdata=0;
		yc_send.yc_meas[Measure101Para.para.res10].floatdata=0;
		yc_send.yc_meas[Measure101Para.para.res11].floatdata=0;
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_ME_NC_1;
		sum+=M_ME_NC_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x97;	//实际使用
		sum+=0x97;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=INTROGEN;
		sum+=INTROGEN;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址和遥测值------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		BW_Before_JM1[i++]=0x40;
		sum+=0x40;
		
		for(j=0;j<YC_Number;j++)
		{
			BW_Before_JM1[i++]=yc_send.yc_meas[j].bytedata[0];
			sum+=yc_send.yc_meas[j].bytedata[0];
			BW_Before_JM1[i++]=yc_send.yc_meas[j].bytedata[1];
			sum+=yc_send.yc_meas[j].bytedata[1];
			BW_Before_JM1[i++]=yc_send.yc_meas[j].bytedata[2];
			sum+=yc_send.yc_meas[j].bytedata[2];
			BW_Before_JM1[i++]=yc_send.yc_meas[j].bytedata[3];
			sum+=yc_send.yc_meas[j].bytedata[3];
			BW_Before_JM1[i++]=0x00;	
			sum+=0x00;
		}
	//*****************************************************
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		yc_send.yc_meas[Measure101Para.para.AC_power].longdata=0;
		yc_send.yc_meas[Measure101Para.para.Bat_volt].longdata=MeasureData.V_BAT/100;
		yc_send.yc_meas[Measure101Para.para.UAB].longdata=MeasureData.Display_UAB_val/1000;
		yc_send.yc_meas[Measure101Para.para.UCB].longdata=MeasureData.Display_UBC_val/1000;
		//yc_send.yc_meas[Measure101Para.para.UAB].longdata=99;
		//yc_send.yc_meas[Measure101Para.para.UCB].longdata=101;
		yc_send.yc_meas[Measure101Para.para.U0].longdata=MeasureData.Display_U0_val/1000;
		yc_send.yc_meas[Measure101Para.para.Sig_dBm].longdata=0;
		yc_send.yc_meas[Measure101Para.para.freq].longdata=MeasureData.freq/100;
		yc_send.yc_meas[Measure101Para.para.res7].longdata=0;
		yc_send.yc_meas[Measure101Para.para.res8].longdata=0;
		yc_send.yc_meas[Measure101Para.para.IA].longdata=MeasureData.Display_IA_val*3/25;
		yc_send.yc_meas[Measure101Para.para.IB].longdata=MeasureData.Display_IB_val*3/25;
		yc_send.yc_meas[Measure101Para.para.IC].longdata=MeasureData.Display_IC_val*3/25;
		//yc_send.yc_meas[Measure101Para.para.IA].longdata=599;
		//yc_send.yc_meas[Measure101Para.para.IB].longdata=600;
		//yc_send.yc_meas[Measure101Para.para.IC].longdata=601;
		
		if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
			yc_send.yc_meas[Measure101Para.para.I0].longdata=MeasureData.Display_I0_val/1000;
		else
			yc_send.yc_meas[Measure101Para.para.I0].longdata=MeasureData.Protect_10I0_val/100;
		
		yc_send.yc_meas[Measure101Para.para.P].longdata=(s32)MeasureData.P_val*12;
		yc_send.yc_meas[Measure101Para.para.Q].longdata=MeasureData.Q_val*12;
		yc_send.yc_meas[Measure101Para.para.cos].longdata=MeasureData.cosALL;
		yc_send.yc_meas[Measure101Para.para.res9].longdata=0;
		yc_send.yc_meas[Measure101Para.para.res10].longdata=0;
		yc_send.yc_meas[Measure101Para.para.res11].longdata=0;
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_ME_NA_1;         // 被测值，归一化值
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x97;	//实际使用
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=INTROGEN;
		BW_Before_JM1[i++]=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		/*-----------信息对象地址和遥测值------------*/
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=0x40;
		BW_Before_JM1[i++]=0;
		for(j=0;j<YC_Number;j++)
		{
			BW_Before_JM1[i++]=yc_send.yc_meas[j].longdata&0xFF;
			BW_Before_JM1[i++]=yc_send.yc_meas[j].longdata>>8;
			BW_Before_JM1[i++]=0x00;	
		}		
	}
	return i;		
}

u8 getfloat_yc_state(void)  // 20%的变化
{
	yc_send.yc_num=0;
	if(abs(yc_send.yc_meas[Measure101Para.para.Bat_volt].floatdata*100-MeasureData.V_BAT)>100) 
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.Bat_volt;
		yc_send.yc_meas[Measure101Para.para.Bat_volt].floatdata=(float)MeasureData.V_BAT/100;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.UAB].floatdata*1000-MeasureData.Display_UAB_val)>2000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.UAB;
		yc_send.yc_meas[Measure101Para.para.UAB].floatdata=(float)MeasureData.Display_UAB_val/1000;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.UCB].floatdata*1000-MeasureData.Display_UBC_val)>2000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.UCB;
		yc_send.yc_meas[Measure101Para.para.UCB].floatdata=(float)MeasureData.Display_UBC_val/1000;
		yc_send.yc_num++;
	}
	
	if(abs(yc_send.yc_meas[Measure101Para.para.U0].floatdata*1000-MeasureData.Display_U0_val)>2000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.U0;
		yc_send.yc_meas[Measure101Para.para.U0].floatdata=(float)MeasureData.Display_U0_val/1000;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.freq].floatdata*100-MeasureData.freq)>100)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.freq;
		yc_send.yc_meas[Measure101Para.para.freq].floatdata=(float)MeasureData.freq/100;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.IA].floatdata*25/3-MeasureData.Display_IA_val)>1000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.IA;
		yc_send.yc_meas[Measure101Para.para.IA].floatdata=(float)MeasureData.Display_IA_val*3/25;
		yc_send.yc_num++;
	}
		
	if(abs(yc_send.yc_meas[Measure101Para.para.IB].floatdata*25/3-MeasureData.Display_IB_val)>1000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.IB;
		yc_send.yc_meas[Measure101Para.para.IB].floatdata=(float)MeasureData.Display_IB_val*3/25;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.IC].floatdata*25/3-MeasureData.Display_IC_val)>1000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.IC;
		yc_send.yc_meas[Measure101Para.para.IC].floatdata=(float)MeasureData.Display_IC_val*3/25;
		yc_send.yc_num++;
	}
	
	if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
	{
		if(abs(yc_send.yc_meas[Measure101Para.para.I0].floatdata*1000-MeasureData.Display_I0_val)>4000)
		{
			yc_send.yc_state|=0x01<<Measure101Para.para.I0;
			yc_send.yc_meas[Measure101Para.para.I0].floatdata=(float)MeasureData.Display_I0_val/1000;
			yc_send.yc_num++;
		}
	}
	else
	{
		if(abs(yc_send.yc_meas[Measure101Para.para.I0].floatdata*100-MeasureData.Protect_10I0_val)>400)
		{
			yc_send.yc_state|=0x01<<Measure101Para.para.I0;
			yc_send.yc_meas[Measure101Para.para.I0].floatdata=(float)MeasureData.Protect_10I0_val/100;
			yc_send.yc_num++;
		}
	}

	if(abs(yc_send.yc_meas[Measure101Para.para.P].floatdata-MeasureData.P_val*12)>3600)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.P;
		yc_send.yc_meas[Measure101Para.para.P].floatdata=MeasureData.P_val*12;
		yc_send.yc_num++;
	}

	if(abs(yc_send.yc_meas[Measure101Para.para.Q].floatdata-MeasureData.Q_val*12)>3600)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.Q;
		yc_send.yc_meas[Measure101Para.para.Q].floatdata=(float)MeasureData.Q_val*12;
		yc_send.yc_num++;
	}

	if(abs(yc_send.yc_meas[Measure101Para.para.cos].floatdata*1000-MeasureData.cosALL)>200)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.cos;
		yc_send.yc_meas[Measure101Para.para.cos].floatdata=(float)MeasureData.cosALL/1000;
		yc_send.yc_num++;
	}
	
	return yc_send.yc_num;
}

u8 getint_yc_state(void)  // 20%的变化
{
	yc_send.yc_num=0;
	if(abs(yc_send.yc_meas[Measure101Para.para.Bat_volt].longdata*100-MeasureData.V_BAT)>100) 
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.Bat_volt;
		yc_send.yc_meas[Measure101Para.para.Bat_volt].longdata=MeasureData.V_BAT/100;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.UAB].longdata*100-MeasureData.Display_UAB_val)>2000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.UAB;
		yc_send.yc_meas[Measure101Para.para.UAB].longdata=MeasureData.Display_UAB_val/100;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.UCB].longdata*100-MeasureData.Display_UBC_val)>2000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.UCB;
		yc_send.yc_meas[Measure101Para.para.UCB].longdata=MeasureData.Display_UBC_val/100;
		yc_send.yc_num++;
	}
	
	if(abs(yc_send.yc_meas[Measure101Para.para.U0].longdata*100-MeasureData.Display_U0_val)>2000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.U0;
		yc_send.yc_meas[Measure101Para.para.U0].longdata=MeasureData.Display_U0_val/100;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.freq].longdata*100-MeasureData.freq)>100)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.freq;
		yc_send.yc_meas[Measure101Para.para.freq].longdata=MeasureData.freq/100;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.IA].longdata*25/3-MeasureData.Display_IA_val)>1000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.IA;
		yc_send.yc_meas[Measure101Para.para.IA].longdata=MeasureData.Display_IA_val*3/25;
		yc_send.yc_num++;
	}
		
	if(abs(yc_send.yc_meas[Measure101Para.para.IB].longdata*25/3-MeasureData.Display_IB_val)>1000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.IB;
		yc_send.yc_meas[Measure101Para.para.IB].longdata=MeasureData.Display_IB_val*3/25;
		yc_send.yc_num++;
	}
	if(abs(yc_send.yc_meas[Measure101Para.para.IC].longdata*25/3-MeasureData.Display_IC_val)>1000)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.IC;
		yc_send.yc_meas[Measure101Para.para.IC].longdata=MeasureData.Display_IC_val*3/25;
		yc_send.yc_num++;
	}
	
	if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
	{
		if(abs(yc_send.yc_meas[Measure101Para.para.I0].longdata*1000-MeasureData.Display_I0_val)>4000)
		{
			yc_send.yc_state|=0x01<<Measure101Para.para.I0;
			yc_send.yc_meas[Measure101Para.para.I0].longdata=MeasureData.Display_I0_val/1000;
			yc_send.yc_num++;
		}
	}
	else
	{
		if(abs(yc_send.yc_meas[Measure101Para.para.I0].longdata*100-MeasureData.Protect_10I0_val)>400)
		{
			yc_send.yc_state|=0x01<<Measure101Para.para.I0;
			yc_send.yc_meas[Measure101Para.para.I0].longdata=MeasureData.Protect_10I0_val/100;
			yc_send.yc_num++;
		}
	}

	if(abs(yc_send.yc_meas[Measure101Para.para.P].longdata-(s32)MeasureData.P_val*12)>3600)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.P;
		yc_send.yc_meas[Measure101Para.para.P].longdata=(s32)MeasureData.P_val*12;
		yc_send.yc_num++;
	}

	if(abs(yc_send.yc_meas[Measure101Para.para.Q].longdata-MeasureData.Q_val*12)>3600)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.Q;
		yc_send.yc_meas[Measure101Para.para.Q].longdata=MeasureData.Q_val*12;
		yc_send.yc_num++;
	}

	if(abs(yc_send.yc_meas[Measure101Para.para.cos].longdata-MeasureData.cosALL)>200)
	{
		yc_send.yc_state|=0x01<<Measure101Para.para.cos;
		yc_send.yc_meas[Measure101Para.para.cos].longdata=MeasureData.cosALL;
		yc_send.yc_num++;
	}
	return yc_send.yc_num;
}

u8 AutoSend_YC(u8 length,u8 ca)
{
	u8 sum=0,i=0,j=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_ME_NC_1;       // 被测值，短浮点数
		sum+=M_ME_NC_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=yc_send.yc_num;
		sum+=yc_send.yc_num;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=SPONT;
		sum+=SPONT;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址和遥测报文------------*/
		for(j=0;j<YC_Number;j++)
		{
			if(yc_send.yc_state>>j&0x01)
			{
				BW_Before_JM1[i++]=j+1;
				sum+=j+1;
				BW_Before_JM1[i++]=0x40;
				sum+=0x40;
				BW_Before_JM1[i++]=yc_send.yc_meas[j].bytedata[0];
				sum+=yc_send.yc_meas[j].bytedata[0];
				BW_Before_JM1[i++]=yc_send.yc_meas[j].bytedata[1];
				sum+=yc_send.yc_meas[j].bytedata[1];
				BW_Before_JM1[i++]=yc_send.yc_meas[j].bytedata[2];
				sum+=yc_send.yc_meas[j].bytedata[2];
				BW_Before_JM1[i++]=yc_send.yc_meas[j].bytedata[3];
				sum+=yc_send.yc_meas[j].bytedata[3];
				BW_Before_JM1[i++]=0x00;	
				sum+=0x00;
			}
		}
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=M_ME_NA_1;     // 被测值，归一化值
		BW_Before_JM1[i++]=yc_send.yc_num;
		BW_Before_JM1[i++]=SPONT;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;	

		for(j=0;j<YC_Number;j++)
		{
			if(yc_send.yc_state>>j&0x01)
			{
				BW_Before_JM1[i++]=j+1;
				BW_Before_JM1[i++]=0x40;
				BW_Before_JM1[i++]=0;
			  BW_Before_JM1[i++]=yc_send.yc_meas[j].longdata&0xFF;
			  BW_Before_JM1[i++]=yc_send.yc_meas[j].longdata>>8;
				BW_Before_JM1[i++]=0x00;	
			}
		}
	}
	return i;		
}

u8 Conclude_GeneralCall_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_IC_NA_1;
		sum+=C_IC_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTTERM;
		sum+=ACTTERM;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------遥信------------*/
		BW_Before_JM1[i++]= QOI_GenCall;
		sum+= QOI_GenCall;	
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_IC_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTTERM;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=QOI_GenCall;
	}
	return i;		
}

u8 Ctrl_General_BefJm(u8 ti,u8 length,u8 ca,u8 reason)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=ti;
		sum+=ti;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=reason;
		sum+=reason;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=Object_addr&0xFF;
		sum+=(Object_addr&0xFF);
		BW_Before_JM1[i++]=Object_addr>>8;
		sum+=(Object_addr>>8);
		/*-----------遥控------------*/
		BW_Before_JM1[i++]=control_jm.byte;
		sum+=control_jm.byte;
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=ti;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=reason;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=control_jm.byte;
	}
	return i;		
}

u8 SOE_Send_PD(void)
{
	u8 i=0,j=0,soenum;
  if(yx_data_flag==0)
	{
		soenum = GetQueueDataNum(&QueueSOE);
		if((soenum%10)==0 && soenum > 9)
		{
			yx_data_flag = 1;
			if(soenum<100)
			{
				yx_data_num = soenum/10;
				for(j=0;j<yx_data_num;j++)
				{
					for(i=0;i<10;i++)
						yx_data[j][i] = GetDataFromQueue(&QueueSOE,BufferSOE);
				}
			}
			else
			{
				yx_data_num = 10;
				for(j=0;j<10;j++)
				{
					for(i=0;i<10;i++)
						yx_data[j][i] = GetDataFromQueue(&QueueSOE,BufferSOE);
				}
			}
			return 0x0D;
		}
		else if(soenum>0)
		{
			for(i=0;i<(soenum%10);i++)
				GetDataFromQueue(&QueueSOE,BufferSOE);					
		}
		soenum = GetQueueDataNum(&QueueSOE);
		if(soenum == 0)
			SOE_Flag = 0;
  }
	else if(yx_data_flag==1)
	{
		return 0x0D;
	}
	return 0;
}

u8 AutoSend_YX_TIME(u8 length,u8 ca,u8 num)
{
	u8 sum=0,i=0,j=0;
	u16 msec=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_SP_TB_1;
		sum+=M_SP_TB_1;
		/*-----------帧长限定词------------*///可变结构限定词（信息元素数目)
		BW_Before_JM1[i++]=num;
		sum+=num;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=SPONT;
		sum+=SPONT;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址和遥信报文------------*/
		for(j=0;j<yx_data_num;j++)
		{
			BW_Before_JM1[i++]=yx_data[j][8]+1;
			sum+=(yx_data[j][8]+1);
			BW_Before_JM1[i++]=0;
			sum+=0;
			BW_Before_JM1[i++]=yx_data[j][9];
			sum+=yx_data[j][9];	
			msec=(yx_data[j][5]*1000+(yx_data[j][6]<<8)+yx_data[j][7])%60000;
			BW_Before_JM1[i++]=msec&0xFF;
			sum+=(msec&0xFF);
			BW_Before_JM1[i++]=msec>>8;
			sum+=(msec>>8);
			BW_Before_JM1[i++]=yx_data[j][4];
			sum+=yx_data[j][4];
			BW_Before_JM1[i++]=yx_data[j][3];
			sum+=yx_data[j][3];
			BW_Before_JM1[i++]=yx_data[j][2];
			sum+=yx_data[j][2];
			BW_Before_JM1[i++]=yx_data[j][1];
			sum+=yx_data[j][1];
			BW_Before_JM1[i++]=yx_data[j][0];
			sum+=yx_data[j][0];	
		}
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=M_SP_NA_1;
		BW_Before_JM1[i++]=num;//每次只发送一个
		BW_Before_JM1[i++]=SPONT;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		/*-----------信息对象地址和遥信报文------------*/
		for(j=0;j<yx_data_num;j++)
		{
			BW_Before_JM1[i++]=yx_data[j][8]+1;
			BW_Before_JM1[i++]=0;
			BW_Before_JM1[i++]=0;
			BW_Before_JM1[i++]=yx_data[j][9];
			msec=(yx_data[j][5]*1000+(yx_data[j][6]<<8)+yx_data[j][7])%60000;
			BW_Before_JM1[i++]=msec&0xFF;
			BW_Before_JM1[i++]=msec>>8;
			BW_Before_JM1[i++]=yx_data[j][4];
			BW_Before_JM1[i++]=yx_data[j][3];
			BW_Before_JM1[i++]=yx_data[j][2];
			BW_Before_JM1[i++]=yx_data[j][1];
			BW_Before_JM1[i++]=yx_data[j][0];
		}
	}
	return i;		
}

u8 AutoSend_YX(u8 length,u8 ca,u8 num)
{
	u8 sum=0,i=0,j=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_SP_NA_1;
		sum+=M_SP_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=num;
		sum+=num;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=SPONT;
		sum+=SPONT;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址和遥信报文------------*/
		for(j=0;j<yx_data_num;j++)
		{
			BW_Before_JM1[i++]=yx_data[j][8]+1;
			sum+=(yx_data[j][8]+1);
			BW_Before_JM1[i++]=0;
			sum+=0;
			BW_Before_JM1[i++]=yx_data[j][9];
			sum+=yx_data[j][9];	
		}
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
  }
	else 
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=M_SP_NA_1;
		BW_Before_JM1[i++]=num;//每次只发送一个
		BW_Before_JM1[i++]=SPONT;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		/*-----------信息对象地址和遥信报文------------*/
		for(j=0;j<yx_data_num;j++)
		{
			BW_Before_JM1[i++]=yx_data[j][8]+1;
			BW_Before_JM1[i++]=0;
			BW_Before_JM1[i++]=0;
			BW_Before_JM1[i++]=yx_data[j][9];
		}		
	}
	return i;		
}

u8 Cal_Time_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_CS_NA_1;
		sum+=C_CS_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------时标------------*/
		BW_Before_JM1[i++]=(u8)((TimeNow.second*1000+TimeNow.msec)&0xFF);
		sum+=(u8)((TimeNow.second*1000+TimeNow.msec)&0xFF);
		BW_Before_JM1[i++]=(u8)(((TimeNow.second*1000+TimeNow.msec)>>8)&0xFF);
		sum+=(u8)(((TimeNow.second*1000+TimeNow.msec)>>8)&0xFF);
		BW_Before_JM1[i++]=(u8)(TimeNow.minute&0x3F);
		sum+=(u8)(TimeNow.minute&0x3F);
		BW_Before_JM1[i++]=(u8)(TimeNow.hour&0x1F);
		sum+=(u8)(TimeNow.hour&0x1F);
		BW_Before_JM1[i++]=(u8)(TimeNow.date&0x1F);
		sum+=(u8)(TimeNow.date&0x1F);
		BW_Before_JM1[i++]=(u8)(TimeNow.month&0x0F);
		sum+=(u8)(TimeNow.month&0x0F);
		BW_Before_JM1[i++]=(u8)(TimeNow.year&0x7F);
		sum+=(u8)(TimeNow.year&0x7F);
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_CS_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=(u8)((TimeNow.second*1000+TimeNow.msec)&0xFF);
		BW_Before_JM1[i++]=(u8)(((TimeNow.second*1000+TimeNow.msec)>>8)&0xFF);
		BW_Before_JM1[i++]=(u8)(TimeNow.minute&0x3F);
		BW_Before_JM1[i++]=(u8)(TimeNow.hour&0x1F);
		BW_Before_JM1[i++]=(u8)(TimeNow.date&0x1F);
		BW_Before_JM1[i++]=(u8)(TimeNow.month&0x0F);
		BW_Before_JM1[i++]=(u8)(TimeNow.year&0x7F);
	}


	return i;		
}

u8 Read_Time_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_CS_NA_1;
		sum+=C_CS_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=REQ;
		sum+=REQ;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------时标------------*/
		BW_Before_JM1[i++]=(u8)((TimeNow.second*1000+TimeNow.msec)&0xFF);
		sum+=(u8)((TimeNow.second*1000+TimeNow.msec)&0xFF);
		BW_Before_JM1[i++]=(u8)(((TimeNow.second*1000+TimeNow.msec)>>8)&0xFF);
		sum+=(u8)(((TimeNow.second*1000+TimeNow.msec)>>8)&0xFF);
		BW_Before_JM1[i++]=(u8)(TimeNow.minute&0x3f);
		sum+=(u8)(TimeNow.minute&0x3f);
		BW_Before_JM1[i++]=(u8)(TimeNow.hour&0x1F);
		sum+=(u8)(TimeNow.hour&0x1F);
		BW_Before_JM1[i++]=(u8)(TimeNow.date&0x1F);
		sum+=(u8)(TimeNow.date&0x1F);
		BW_Before_JM1[i++]=(u8)(TimeNow.month&0x0F);
		sum+=(u8)(TimeNow.month&0x0F);
		BW_Before_JM1[i++]=(u8)(TimeNow.year&0x7F);
		sum+=(u8)(TimeNow.year&0x7F);
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_CS_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=REQ;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=(u8)((TimeNow.second*1000+TimeNow.msec)&0xFF);
		BW_Before_JM1[i++]=(u8)(((TimeNow.second*1000+TimeNow.msec)>>8)&0xFF);
		BW_Before_JM1[i++]=(u8)(TimeNow.minute&0x3F);
		BW_Before_JM1[i++]=(u8)(TimeNow.hour&0x1F);
		BW_Before_JM1[i++]=(u8)(TimeNow.date&0x1F);
		BW_Before_JM1[i++]=(u8)(TimeNow.month&0x0F);
		BW_Before_JM1[i++]=(u8)(TimeNow.year&0x7F);
	}
	return i;	
}

u8 Test_Mirror_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	/*-----------报头------------*/
	BW_Before_JM1[i++]=0x68;
	BW_Before_JM1[i++]=length;
	BW_Before_JM1[i++]=length;
	BW_Before_JM1[i++]=0x68;
	/*-----------控制域-----------*/
	BW_Before_JM1[i++]=ca;
	sum+=ca;
	/*-----------地址域------------*/
	BW_Before_JM1[i++]=Test_Sign[0];
	sum+=Test_Sign[0];
	BW_Before_JM1[i++]=Test_Sign[1];	
	sum+=Test_Sign[1];
	/*-----------类型标识------------*/
	BW_Before_JM1[i++]=C_TS_NA_1;
	sum+=C_TS_NA_1;
	/*-----------帧长限定词------------*/
	BW_Before_JM1[i++]=Test_Sign[2];
	sum+=Test_Sign[2];
	/*-----------传送原因------------*/
	BW_Before_JM1[i++]=ACTCON;
	sum+=ACTCON;
	BW_Before_JM1[i++]=0;
	sum+=0;
	/*-----------ASDU地址------------*/
	BW_Before_JM1[i++]=Test_Sign[3];
	sum+=Test_Sign[3];
	BW_Before_JM1[i++]=Test_Sign[4];
	sum+=Test_Sign[4];
	/*-----------信息对象地址------------*/
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	/*-----------测试图像FBP------------*/
	BW_Before_JM1[i++]=Test_Sign[5];
	sum+=Test_Sign[5];
	BW_Before_JM1[i++]=Test_Sign[6];
	sum+=Test_Sign[6];
	/*-----------校验码------------*/
	BW_Before_JM1[i++]=sum;	
	/*-----------帧尾------------*/
	BW_Before_JM1[i++]=0x16;

	return i;	
}

u8 Confirm_Reset_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_RP_NA_1;
		sum+=C_RP_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------限定词------------*/
		BW_Before_JM1[i++]=QRP_RstCall;
		sum+=QRP_RstCall;
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_RP_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=QRP_RstCall;	
	}
	return i;	
}


u8 Confirm_Enegy_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_CI_NA_1;
		sum+=C_CI_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------限定词------------*/
		BW_Before_JM1[i++]=20;
		sum+=20;
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_CI_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=20;
	}
	return i;	
}

u8 Confirm_Enegy_Pluse_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_IT_NB_1;
		sum+=M_IT_NB_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x88;
		sum+=0x88;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=PER_CYC;
		sum+=PER_CYC;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		BW_Before_JM1[i++]=0x64;
		sum+=0x64;
		/*-----------电能脉冲------------*/
		YC_Float.floatdata=MeasureData.Wp_all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		sum+=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		sum+=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		sum+=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		sum+=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		sum+=0;
		YC_Float.floatdata=MeasureData.Wq_1all+MeasureData.Wq_2all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		sum+=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		sum+=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		sum+=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		sum+=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		sum+=0;
		YC_Float.floatdata=MeasureData.Wq_1all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		sum+=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		sum+=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		sum+=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		sum+=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		sum+=0;
		YC_Float.floatdata=MeasureData.Wq_4all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		sum+=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		sum+=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		sum+=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		sum+=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		sum+=0;
		YC_Float.floatdata=MeasureData.WpFX_all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		sum+=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		sum+=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		sum+=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		sum+=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		sum+=0;
		YC_Float.floatdata=MeasureData.Wq_3all+MeasureData.Wq_4all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		sum+=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		sum+=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		sum+=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		sum+=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		sum+=0;
		YC_Float.floatdata=MeasureData.Wq_2all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		sum+=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		sum+=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		sum+=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		sum+=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		sum+=0;
		YC_Float.floatdata=MeasureData.Wq_3all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		sum+=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		sum+=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		sum+=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		sum+=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		sum+=0;
		
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=M_IT_NB_1;
		BW_Before_JM1[i++]=0x88;
		BW_Before_JM1[i++]=REQCOGCN;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=0x64;
		BW_Before_JM1[i++]=0x00;
		
		YC_Float.floatdata=MeasureData.Wp_all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		YC_Float.floatdata=MeasureData.Wq_1all+MeasureData.Wq_2all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		YC_Float.floatdata=MeasureData.Wq_1all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		YC_Float.floatdata=MeasureData.Wq_4all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		YC_Float.floatdata=MeasureData.WpFX_all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		YC_Float.floatdata=MeasureData.Wq_3all+MeasureData.Wq_4all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		YC_Float.floatdata=MeasureData.Wq_2all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
		YC_Float.floatdata=MeasureData.Wq_3all;
		BW_Before_JM1[i++]=YC_Float.bytedata[0];
		BW_Before_JM1[i++]=YC_Float.bytedata[1];
		BW_Before_JM1[i++]=YC_Float.bytedata[2];
		BW_Before_JM1[i++]=YC_Float.bytedata[3];
		BW_Before_JM1[i++]=0;
	}
	return i;	
}

u8 Confirm_Enegy_CP56Time2a_BefJM(u8 length,u8 ca) //带时标的电能数据
{
	u8 sum=0,i=0,j=0;
	u8 buffer[Fix_Num];
	u16 dqms = 0;
	
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=M_IT_TC_1;
		sum+=M_IT_TC_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x88;
		sum+=0x88;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=PER_CYC;
		sum+=PER_CYC;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x09;
		sum+=0x09;
		BW_Before_JM1[i++]=0x64;
		sum+=0x64;
		/*-----------电能脉冲------------*/
		if(LineLockNum.Read_Place==0) 
			LineLockNum.Read_Place=59;
		else 
			LineLockNum.Read_Place=LineLockNum.Fix_Place-1;
		CS2BZ=1;
		SPI_Flash_Read(buffer,Flash_LineLoss_Fix + LineLockNum.Read_Place*Fix_Num,Fix_Num);
		CS2BZ=0;
		for(j=0;j<8;j++)
		{
			BW_Before_JM1[i++]=buffer[4*j+6];
			sum+=buffer[4*j+6];
			BW_Before_JM1[i++]=buffer[4*j+7];
			sum+=buffer[4*j+7];
			BW_Before_JM1[i++]=buffer[4*j+8];
			sum+=buffer[4*j+8];
			BW_Before_JM1[i++]=buffer[4*j+9];
			sum+=buffer[4*j+9];
			BW_Before_JM1[i++]=0;
			sum+=0;
			dqms=buffer[5]*1000%60000;  //秒
			BW_Before_JM1[i++]=dqms&0xFF;
			sum+=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			sum+=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//分
			sum+=buffer[4];
			BW_Before_JM1[i++]=buffer[3];//时
			sum+=buffer[3];
			BW_Before_JM1[i++]=buffer[2];//日
			sum+=buffer[2];
			BW_Before_JM1[i++]=buffer[1];//月
			sum+=buffer[1];
			BW_Before_JM1[i++]=buffer[0];//年
			sum+=buffer[0];
		}
		
		if(LineLockNum.Read_Place==0) 
			LineLockNum.Read_Place=61;
		else 
			LineLockNum.Read_Place=LineLockNum.Frzd_Place-1;
		CS2BZ=1;
		SPI_Flash_Read(buffer,Flash_LineLoss_Frzd + LineLockNum.Read_Place*Frzd_Num,Frzd_Num);
		CS2BZ=0;
		for(j=0;j<8;j++)
		{
			BW_Before_JM1[i++]=buffer[4*j+6];
			sum+=buffer[4*j+6];
			BW_Before_JM1[i++]=buffer[4*j+7];
			sum+=buffer[4*j+7];
			BW_Before_JM1[i++]=buffer[4*j+8];
			sum+=buffer[4*j+8];
			BW_Before_JM1[i++]=buffer[4*j+9];
			sum+=buffer[4*j+9];
			BW_Before_JM1[i++]=0;
			sum+=0;
			dqms=buffer[5]*1000%60000;  //秒
			BW_Before_JM1[i++]=dqms&0xFF;
			sum+=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			sum+=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//分
			sum+=buffer[4];
			BW_Before_JM1[i++]=buffer[3];//时
			sum+=buffer[3];
			BW_Before_JM1[i++]=buffer[2];//日
			sum+=buffer[2];
			BW_Before_JM1[i++]=buffer[1];//月
			sum+=buffer[1];
			BW_Before_JM1[i++]=buffer[0];//年
			sum+=buffer[0];
		}
		
		if(LineLossNum.P_Place==0) 
			LineLockNum.Read_Place=9;
		else 
			LineLockNum.Read_Place=LineLossNum.P_Place-1;
		CS2BZ=1;
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_PReverse + LineLockNum.Read_Place*PReverse_Num,PReverse_Num);
		CS2BZ=0;
		for(j=0;j<8;j++)
		{
			BW_Before_JM1[i++]=buffer[4*j+12];
			sum+=buffer[4*j+12];
			BW_Before_JM1[i++]=buffer[4*j+13];
			sum+=buffer[4*j+13];
			BW_Before_JM1[i++]=buffer[4*j+14];
			sum+=buffer[4*j+14];
			BW_Before_JM1[i++]=buffer[4*j+15];
			sum+=buffer[4*j+15];
			BW_Before_JM1[i++]=0;
			sum+=0;
			dqms=buffer[5]*1000%60000;  //秒
			BW_Before_JM1[i++]=dqms&0xFF;
			sum+=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			sum+=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//分
			sum+=buffer[4];
			BW_Before_JM1[i++]=buffer[3];//时
			sum+=buffer[3];
			BW_Before_JM1[i++]=buffer[2];//日
			sum+=buffer[2];
			BW_Before_JM1[i++]=buffer[1];//月
			sum+=buffer[1];
			BW_Before_JM1[i++]=buffer[0];//年
			sum+=buffer[0];
		}
		
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=M_IT_TC_1;
		BW_Before_JM1[i++]=0x88;
		BW_Before_JM1[i++]=REQCOGCN;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=0x09;
		BW_Before_JM1[i++]=0x64;
		BW_Before_JM1[i++]=0x00;
		
		if(LineLockNum.Read_Place==0) 
			LineLockNum.Read_Place=59;
		else 
			LineLockNum.Read_Place=LineLockNum.Fix_Place-1;
		CS2BZ=1;
		SPI_Flash_Read(buffer,Flash_LineLoss_Fix + LineLockNum.Read_Place*Fix_Num,Fix_Num);
		CS2BZ=0;
		for(j=0;j<8;j++)
		{
			BW_Before_JM1[i++]=buffer[4*j+6];
			BW_Before_JM1[i++]=buffer[4*j+7];
			BW_Before_JM1[i++]=buffer[4*j+8];
			BW_Before_JM1[i++]=buffer[4*j+9];
			BW_Before_JM1[i++]=0;
			dqms=buffer[5]*1000%60000;  //秒
			BW_Before_JM1[i++]=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//分
			BW_Before_JM1[i++]=buffer[3];//时
			BW_Before_JM1[i++]=buffer[2];//日
			BW_Before_JM1[i++]=buffer[1];//月
			BW_Before_JM1[i++]=buffer[0];//年
		}
		
		if(LineLockNum.Read_Place==0) 
			LineLockNum.Read_Place=61;
		else 
			LineLockNum.Read_Place=LineLockNum.Frzd_Place-1;
		CS2BZ=1;
		SPI_Flash_Read(buffer,Flash_LineLoss_Frzd + LineLockNum.Read_Place*Frzd_Num,Frzd_Num);
		CS2BZ=0;
		for(j=0;j<8;j++)
		{
			BW_Before_JM1[i++]=buffer[4*j+6];
			BW_Before_JM1[i++]=buffer[4*j+7];
			BW_Before_JM1[i++]=buffer[4*j+8];
			BW_Before_JM1[i++]=buffer[4*j+9];
			BW_Before_JM1[i++]=0;
			dqms=buffer[5]*1000%60000;  //秒
			BW_Before_JM1[i++]=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//分
			BW_Before_JM1[i++]=buffer[3];//时
			BW_Before_JM1[i++]=buffer[2];//日
			BW_Before_JM1[i++]=buffer[1];//月
			BW_Before_JM1[i++]=buffer[0];//年
		}
		
		if(LineLossNum.P_Place==0) 
			LineLockNum.Read_Place=9;
		else 
			LineLockNum.Read_Place=LineLossNum.P_Place-1;
		CS2BZ=1;
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_PReverse + LineLockNum.Read_Place*PReverse_Num,PReverse_Num);
		CS2BZ=0;
		for(j=0;j<8;j++)
		{
			BW_Before_JM1[i++]=buffer[4*j+12];
			BW_Before_JM1[i++]=buffer[4*j+13];
			BW_Before_JM1[i++]=buffer[4*j+14];
			BW_Before_JM1[i++]=buffer[4*j+15];
			BW_Before_JM1[i++]=0;
			dqms=buffer[5]*1000%60000;  //秒
			BW_Before_JM1[i++]=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//分
			BW_Before_JM1[i++]=buffer[3];//时
			BW_Before_JM1[i++]=buffer[2];//日
			BW_Before_JM1[i++]=buffer[1];//月
			BW_Before_JM1[i++]=buffer[0];//年
		}
	}
	return i;	
}

u8 Conclude_Enegy_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_CI_NA_1;
		sum+=C_CI_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTTERM;
		sum+=ACTTERM;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------限定词------------*/
		BW_Before_JM1[i++]=0x05;
		sum+=0x05;
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_CI_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTTERM;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=05;	
	}
	return i;	
}

u8 Confirm_List_BefJM(u8 ca,u8 flag)
{
	u8 sum=0,i=0;
	/*-----------报头------------*/
	BW_Before_JM1[i++]=0x68;
	BW_Before_JM1[i++]=0;
	BW_Before_JM1[i++]=0;											//处理完再重新赋值
	BW_Before_JM1[i++]=0x68;
	/*-----------控制域-----------*/
	BW_Before_JM1[i++]=ca;
	sum+=ca;
	/*-----------地址域------------*/
  BW_Before_JM1[i++]=ADDR&0xFF;
	sum+=(ADDR&0xFF);
	BW_Before_JM1[i++]=ADDR>>8;	
	sum+=(ADDR>>8);
	/*-----------类型标识------------*/
	BW_Before_JM1[i++]=F_FR_NA_1;
	sum+=F_FR_NA_1;
	/*-----------帧长限定词------------*/
	BW_Before_JM1[i++]=0x01;
	sum+=0x01;
	/*-----------传送原因------------*/
	BW_Before_JM1[i++]=REQ;
	sum+=REQ;
	BW_Before_JM1[i++]=0;
	sum+=0;
	/*-----------ASDU地址------------*/
	BW_Before_JM1[i++]=Asdu_addr&0xFF;
	sum+=(Asdu_addr&0xFF);
	BW_Before_JM1[i++]=Asdu_addr>>8;
	sum+=(Asdu_addr>>8);
	/*-----------信息对象地址------------*/
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	/*-----------附加数据包类型-----------*/
	BW_Before_JM1[i++]=0x02;
	sum+=0x02;
	/*-----------操作标识-----------*/
	BW_Before_JM1[i++]=0x02;
	sum+=0x02;
	/*-----------结果描述字----------*/
	BW_Before_JM1[i++]=flag;
	sum+=flag;
	if(flag==0)                 //0表示成功
	{
		BW_Before_JM1[i++]=0x0;  //	ID
		sum+=0x0;		
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;	
		
//处理有文件的目录

//处理无文件的目录		
	
	}
	else												//无目录
	{
		BW_Before_JM1[i++]=0x0;  //	ID
		sum+=0x0;		
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;	
		
		BW_Before_JM1[i++]=0x0;  //后续标志
		sum+=0x0;		
		
		BW_Before_JM1[i++]=0x0;  //文件数量
		sum+=0x0;					
	}
	/*-----------校验码------------*/
	BW_Before_JM1[i++]=sum;	
	/*-----------帧尾------------*/
	BW_Before_JM1[i++]=0x16;

	return i;	
}

u8 Confirm_ReadDoc_BefJM(u8 ca,u8 flag)
{

	u8 sum=0,j=0,i=0;
	/*-----------报头------------*/
	BW_Before_JM1[i++]=0x68;
	BW_Before_JM1[i++]=0;
	BW_Before_JM1[i++]=0;											//处理完再重新赋值
	BW_Before_JM1[i++]=0x68;
	/*-----------控制域-----------*/
	BW_Before_JM1[i++]=ca;
	sum+=ca;
	/*-----------地址域------------*/
  BW_Before_JM1[i++]=ADDR&0xFF;
	sum+=(ADDR&0xFF);
	BW_Before_JM1[i++]=ADDR>>8;	
	sum+=(ADDR>>8);
	/*-----------类型标识------------*/
	BW_Before_JM1[i++]=F_FR_NA_1;
	sum+=F_FR_NA_1;
	/*-----------帧长限定词------------*/
	BW_Before_JM1[i++]=0x01;
	sum+=0x01;
	/*-----------传送原因------------*/
	BW_Before_JM1[i++]=ACTCON;
	sum+=ACTCON;
	BW_Before_JM1[i++]=0;
	sum+=0;
	/*-----------ASDU地址------------*/
	BW_Before_JM1[i++]=Asdu_addr&0xFF;
	sum+=(Asdu_addr&0xFF);
	BW_Before_JM1[i++]=Asdu_addr>>8;
	sum+=(Asdu_addr>>8);
	/*-----------信息对象地址------------*/
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	/*-----------附加数据包类型-----------*/
	BW_Before_JM1[i++]=0x02;
	sum+=0x02;
	/*-----------操作标识-----------*/
	BW_Before_JM1[i++]=0x04;
	sum+=0x04;
	/*-----------结果描述字----------*/
	BW_Before_JM1[i++]=flag;
	sum+=flag;
	if(flag==0)                 //0表示成功
	{
		BW_Before_JM1[i++]=List_Num1;
		sum+=List_Num1;
		for(j=0;j<List_Num1;j++)
		{
			BW_Before_JM1[i++]=List_Name1[j];
			sum+=List_Name1[j];	
		}
		BW_Before_JM1[i++]=0x0;  //	ID
		sum+=0x0;		
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;	
	
		BW_Before_JM1[i++]=(u8)(Send_Doc_Num1&0xFF);   //文件大小，这个应该在存储在flash中的时候顺便存储下来
		sum+=(u8)(Send_Doc_Num1&0xFF);	
		BW_Before_JM1[i++]=(u8)((Send_Doc_Num1>>8)&0xFF);   
		sum+=(u8)((Send_Doc_Num1>>8)&0xFF);	
		BW_Before_JM1[i++]=(u8)((Send_Doc_Num1>>16)&0xFF);   
		sum+=(u8)((Send_Doc_Num1>>16)&0xFF);	
		BW_Before_JM1[i++]=(u8)((Send_Doc_Num1>>24)&0xFF);  
		sum+=(u8)((Send_Doc_Num1>>24)&0xFF);	
	}
	else												//无文件
	{
		
	}
	/*-----------校验码------------*/
	BW_Before_JM1[i++]=sum;	
	/*-----------帧尾------------*/
	BW_Before_JM1[i++]=0x16;

	return i;	
}

u8 Send_Doc_BefJM(u8 ca)
{
	static u16 times=0;
	u32 duan_num=0;
	u8 sum=0,sum1=0,i=0;
	/*-----------报头------------*/
	BW_Before_JM1[i++]=0x68;
	BW_Before_JM1[i++]=0;
	BW_Before_JM1[i++]=0;											//处理完再重新赋值
	BW_Before_JM1[i++]=0x68;
	/*-----------控制域-----------*/
	BW_Before_JM1[i++]=ca;
	sum+=ca;
	/*-----------地址域------------*/
  BW_Before_JM1[i++]=ADDR&0xFF;
	sum+=(ADDR&0xFF);
	BW_Before_JM1[i++]=ADDR>>8;	
	sum+=(ADDR>>8);
	/*-----------类型标识------------*/
	BW_Before_JM1[i++]=F_FR_NA_1;
	sum+=F_FR_NA_1;
	/*-----------帧长限定词------------*/
	BW_Before_JM1[i++]=0x01;
	sum+=0x01;
	/*-----------传送原因------------*/
	BW_Before_JM1[i++]=REQ;
	sum+=REQ;
	BW_Before_JM1[i++]=0;
	sum+=0;
	/*-----------ASDU地址------------*/
	BW_Before_JM1[i++]=Asdu_addr&0xFF;
	sum+=(Asdu_addr&0xFF);
	BW_Before_JM1[i++]=Asdu_addr>>8;
	sum+=(Asdu_addr>>8);
	/*-----------信息对象地址------------*/
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	/*-----------附加数据包类型-----------*/
	BW_Before_JM1[i++]=0x02;
	sum+=0x02;
	/*-----------操作标识-----------*/
	BW_Before_JM1[i++]=0x05;
	sum+=0x05;
	/*-----------文件ID---------*/
	BW_Before_JM1[i++]=0x0;  //	ID
	sum+=0x0;		
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;			
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;			
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;	
	/*-----------段号---------*/	
	duan_num=times*200;	
	BW_Before_JM1[i++]=(u8)(duan_num&0xFF);  //	段号
	sum+=(u8)(duan_num&0xFF);		
	BW_Before_JM1[i++]=(u8)((duan_num>>8)&0xFF);
	sum+=(u8)((duan_num>>8)&0xFF);			
	BW_Before_JM1[i++]=(u8)((duan_num>>16)&0xFF);
	sum+=(u8)((duan_num>>16)&0xFF);			
	BW_Before_JM1[i++]=(u8)((duan_num>>24)&0xFF);
	sum+=(u8)((duan_num>>24)&0xFF);		
	times++;
	/*-----------后续标志---------*/	
	if((times*200)<Send_Doc_Num1)
	{
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;			
	}
	else
	{
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;		
	}
	/*-----------数据文件---------*/
	//还要判断是哪个文件
	if((times*200)<Send_Doc_Num1)	
	{
		switch(index_which)
		{
			case 1:
				for(i=0;i<200;i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+=0x01;
					sum1+= 0x01;		
				}
				break;
			case 2:
				for(i=0;i<200;i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;			
			case 3:
				for(i=0;i<200;i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;
			case 4:
				for(i=0;i<200;i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;					
			case 5:
				for(i=0;i<200;i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;
			case 6:
				for(i=0;i<200;i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;			
			case 7:
				for(i=0;i<200;i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;
			case 8:
				for(i=0;i<200;i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;					
		}
	}
	else
	{
		switch(index_which)
		{
			case 1:
			//区分是CFG文件还是DAT文件
				for(i=0;i<Send_Doc_Num1-((times-1)*200);i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+=0x01;
					sum1+= 0x01;		
				}
				break;
			case 2:
				for(i=0;i<Send_Doc_Num1-((times-1)*200);i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;			
			case 3:
				for(i=0;i<Send_Doc_Num1-((times-1)*200);i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;
			case 4:
				for(i=0;i<Send_Doc_Num1-((times-1)*200);i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;					
			case 5:
				for(i=0;i<Send_Doc_Num1-((times-1)*200);i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;
			case 6:
				for(i=0;i<Send_Doc_Num1-((times-1)*200);i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;			
			case 7:
				for(i=0;i<Send_Doc_Num1-((times-1)*200);i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;
			case 8:
				for(i=0;i<Send_Doc_Num1-((times-1)*200);i++)
				{
					BW_Before_JM1[i++]=0x01;
					sum+= 0x01;
					sum1+= 0x01;		
				}
				break;					
		}	
		times=0;
	}
	/*-----------文件校验码------------*/
	BW_Before_JM1[i++]=sum1;
	sum+= sum1;
	/*-----------校验码------------*/
	BW_Before_JM1[i++]=sum;	
	/*-----------帧尾------------*/
	BW_Before_JM1[i++]=0x16;

	return i;	
}

u8 Confirm_WriteDoc_BefJM(u8 ca,u8 flag)
{
	u8 sum=0,j=0,i=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=F_FR_NA_1;
		sum+=F_FR_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------附加数据包类型-----------*/
		BW_Before_JM1[i++]=0x02;
		sum+=0x02;
		/*-----------操作标识-----------*/
		BW_Before_JM1[i++]=0x08;
		sum+=0x08;
		/*-----------结果描述字----------*/
		BW_Before_JM1[i++]=flag;
		sum+=flag;
		if(flag==0)                 //0表示成功
		{
			BW_Before_JM1[i++]=List_Num1;
			sum+=List_Num1;
			for(j=0;j<List_Num1;j++)
			{
				BW_Before_JM1[i++]=List_Name1[j];
				sum+=List_Name1[j];	
			}
			BW_Before_JM1[i++]=0x0;  //	ID
			sum+=0x0;		
			BW_Before_JM1[i++]=0x0;
			sum+=0x0;			
			BW_Before_JM1[i++]=0x0;
			sum+=0x0;			
			BW_Before_JM1[i++]=0x0;
			sum+=0x0;	
		
			BW_Before_JM1[i++]=(u8)(Rec_Doc_Num1&0xFF);   //文件大小，这个应该在存储在flash中的时候顺便存储下来
			sum+=(u8)(Rec_Doc_Num1&0xFF);	
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>8)&0xFF);   
			sum+=(u8)((Rec_Doc_Num1>>8)&0xFF);	
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>16)&0xFF);   
			sum+=(u8)((Rec_Doc_Num1>>16)&0xFF);	
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>24)&0xFF);  
			sum+=(u8)((Rec_Doc_Num1>>24)&0xFF);	
		}
		else												//无文件
		{

		}
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		u8 j=0,i=0;
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=F_FR_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];

		BW_Before_JM1[i++]=2;															//附加数据包类型2，文件传输
		BW_Before_JM1[i++]=8;														  //操作标识8，写文件确认激活
		BW_Before_JM1[i++]=flag;										//0成功，1失败，2文件名不对，3长度超了

		if(flag==0)                 //0表示成功
		{
			BW_Before_JM1[i++]=List_Num1;
			for(j=0;j<List_Num1;j++)
			{
				BW_Before_JM1[i++]=List_Name1[j];
			}
			BW_Before_JM1[i++]=0x0;  //	ID
			BW_Before_JM1[i++]=0x0;
			BW_Before_JM1[i++]=0x0;
			BW_Before_JM1[i++]=0x0;

			BW_Before_JM1[i++]=(u8)(Rec_Doc_Num1&0xFF);   //文件大小，这个应该在存储在flash中的时候顺便存储下来
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>8)&0xFF);   
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>16)&0xFF);   
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>24)&0xFF);  
		}
		else												//无文件
		{
		}	
	}
	return i;	
}

u8 Confirm_Send_Dat_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=F_FR_NA_1;
		sum+=F_FR_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=REQ;
		sum+=REQ;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------附加数据包类型-----------*/
		BW_Before_JM1[i++]=0x02;
		sum+=0x02;
		/*-----------操作标识-----------*/
		BW_Before_JM1[i++]=0x0a;
		sum+=0x0a;
		/*-----------文件ID-----------*/	
		BW_Before_JM1[i++]=0x0;  //	ID
		sum+=0x0;		
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;	
		/*-----------数据段号-----------*/	
		BW_Before_JM1[i++]=(u8)(segment_num_last&0xFF);   //文件大小，这个应该在存储在flash中的时候顺便存储下来
		sum+=(u8)(segment_num_last&0xFF);	
		BW_Before_JM1[i++]=(u8)((segment_num_last>>8)&0xFF);   
		sum+=(u8)((segment_num_last>>8)&0xFF);	
		BW_Before_JM1[i++]=(u8)((segment_num_last>>16)&0xFF);   
		sum+=(u8)((segment_num_last>>16)&0xFF);	
		BW_Before_JM1[i++]=(u8)((segment_num_last>>24)&0xFF);  
		sum+=(u8)((segment_num_last>>24)&0xFF);	
		/*-----------结果描述字----------*/
		BW_Before_JM1[i++]=Rec_Error_Flag1;
		sum+=Rec_Error_Flag1;
		Rec_Error_Flag1=0;							//清0备用
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=F_FR_NA_1;
		BW_Before_JM1[i++]=0x00;
		BW_Before_JM1[i++]=REQ;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=2;															//附加数据包类型2，文件传输
		BW_Before_JM1[i++]=10;															//操作标识10，写文件传输数据确认激活
		
		BW_Before_JM1[i++]=0;															//文件ID，全为0，四字节
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;	
		BW_Before_JM1[i++]=0;
		
		BW_Before_JM1[i++]=((u8)segment_num_last)&0xFF;							    //段号
		BW_Before_JM1[i++]=((u8)segment_num_last>>8)&0xFF;	
		BW_Before_JM1[i++]=((u8)segment_num_last>>16)&0xFF;	
		BW_Before_JM1[i++]=((u8)segment_num_last>>24)&0xFF;	
		
		BW_Before_JM1[i++]=Rec_Error_Flag1;										//0成功，1失败，2校验和错误，3长度超了
		Rec_Error_Flag1=0;   //清0备用	
	}
	return i;	
}

u8 Confirm_Update_BefJM(u8 ca,u8 reason,u8 SE_FLAG)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=F_SR_NA_1;
		sum+=F_SR_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=reason;
		sum+=reason;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------命令类型CTYPE-----------*/
		BW_Before_JM1[i++]=SE_FLAG;
		sum+=SE_FLAG;

		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=F_SR_NA_1;
		BW_Before_JM1[i++]=0x00;
		BW_Before_JM1[i++]=reason;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=Informat_addr[0];
		BW_Before_JM1[i++]=Informat_addr[1];
		BW_Before_JM1[i++]=Informat_addr[2];
		BW_Before_JM1[i++]=SE_FLAG;	
	}

	return i;	
}

u8 Read_Area_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_RR_NA_1;
		sum+=C_RR_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x1;
		sum+=0x1;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=(u8)SN_Num1&0xFF;
		sum+=(u8)SN_Num1&0xFF;
		BW_Before_JM1[i++]=(u8)(SN_Num1>>8)&0xFF;
		sum+=(u8)(SN_Num1>>8)&0xFF;	
		/*-----------最小定值区号-----------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------最大定值区号-----------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_RR_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)SN_Num1&0xFF;			
		BW_Before_JM1[i++]=(u8)(SN_Num1>>8)&0xFF;		
		BW_Before_JM1[i++]=0;			
		BW_Before_JM1[i++]=0;		
		BW_Before_JM1[i++]=0;			
		BW_Before_JM1[i++]=0;			
	}
	return i;	
}

u8 Change_Area_BefJM(u8 length,u8 ca)
{

	u8 sum=0,i=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_SR_NA_1;
		sum+=C_SR_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0x1;
		sum+=0x1;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=(u8)SN_Num1&0xFF;
		sum+=(u8)SN_Num1&0xFF;
		BW_Before_JM1[i++]=(u8)(SN_Num1>>8)&0xFF;
		sum+=(u8)(SN_Num1>>8)&0xFF;	
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_SR_NA_1;
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)SN_Num1&0xFF;			
		BW_Before_JM1[i++]=(u8)(SN_Num1>>8)&0xFF;	
	}
	return i;	
}

u8 Read_All_Constparas_BefJM(u8 ca)						//读取全部固有参数
{
	u8 sum=0,j=0,i=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0xa;							//必须给值
		sum+=0xa;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------参数特征标识------------*/	
		BW_Before_JM1[i++]=0x01;							//必须要确定是否有后续，0：无后续；1：有后续
		sum+=0x01;	
		/*-----------信息对象地址------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[0];
		sum+=Constpara_Tag[0];
		BW_Before_JM1[i++]=Constpara_Len[0];
		sum+=Constpara_Len[0];
		for(j=0;j<Constpara_Len[0];j++)
		{
			BW_Before_JM1[i++]=Constpara1[j];
			sum+=Constpara1[j];
		}
		BW_Before_JM1[i++]=0x02;
		sum+=0x02;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[1];
		sum+=Constpara_Tag[1];
		BW_Before_JM1[i++]=Constpara_Len[1];
		sum+=Constpara_Len[1];
		for(j=0;j<Constpara_Len[1];j++)
		{
			BW_Before_JM1[i++]=Constpara2[j];
			sum+=Constpara2[j];
		}		
		BW_Before_JM1[i++]=0x03;
		sum+=0x03;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[2];
		sum+=Constpara_Tag[2];
		BW_Before_JM1[i++]=Constpara_Len[2];
		sum+=Constpara_Len[2];
		for(j=0;j<Constpara_Len[2];j++)
		{
			BW_Before_JM1[i++]=Constpara3[j];
			sum+=Constpara3[j];
		}	
		BW_Before_JM1[i++]=0x04;
		sum+=0x04;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[3];
		sum+=Constpara_Tag[3];
		BW_Before_JM1[i++]=Constpara_Len[3];
		sum+=Constpara_Len[3];
		for(j=0;j<Constpara_Len[3];j++)
		{
			BW_Before_JM1[i++]=Constpara4[j];
			sum+=Constpara4[j];
		}	
		BW_Before_JM1[i++]=0x05;
		sum+=0x05;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[4];
		sum+=Constpara_Tag[4];
		BW_Before_JM1[i++]=Constpara_Len[4];
		sum+=Constpara_Len[4];
		for(j=0;j<Constpara_Len[4];j++)
		{
			BW_Before_JM1[i++]=Constpara5[j];
			sum+=Constpara5[j];
		}	
		BW_Before_JM1[i++]=0x06;
		sum+=0x06;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[5];
		sum+=Constpara_Tag[5];
		BW_Before_JM1[i++]=Constpara_Len[5];
		sum+=Constpara_Len[5];
		for(j=0;j<Constpara_Len[5];j++)
		{
			BW_Before_JM1[i++]=Constpara6[j];
			sum+=Constpara6[j];
		}	
		BW_Before_JM1[i++]=0x07;
		sum+=0x07;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[6];
		sum+=Constpara_Tag[6];
		BW_Before_JM1[i++]=Constpara_Len[6];
		sum+=Constpara_Len[6];
		for(j=0;j<Constpara_Len[6];j++)
		{
			BW_Before_JM1[i++]=Constpara7[j];
			sum+=Constpara7[j];
		}
		BW_Before_JM1[i++]=0x08;
		sum+=0x08;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[7];
		sum+=Constpara_Tag[7];
		BW_Before_JM1[i++]=Constpara_Len[7];
		sum+=Constpara_Len[7];
		for(j=0;j<Constpara_Len[7];j++)
		{
			BW_Before_JM1[i++]=Constpara8[j];
			sum+=Constpara8[j];
		}
		BW_Before_JM1[i++]=0x09;
		sum+=0x09;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[8];
		sum+=Constpara_Tag[8];
		BW_Before_JM1[i++]=Constpara_Len[8];
		sum+=Constpara_Len[8];
		for(j=0;j<Constpara_Len[8];j++)
		{
			BW_Before_JM1[i++]=Constpara9[j];
			sum+=Constpara9[j];
		}
		BW_Before_JM1[i++]=0x0A;
		sum+=0x0A;
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;
		BW_Before_JM1[i++]=Constpara_Tag[9];
		sum+=Constpara_Tag[9];
		BW_Before_JM1[i++]=Constpara_Len[9];
		sum+=Constpara_Len[9];
		for(j=0;j<Constpara_Len[9];j++)
		{
			BW_Before_JM1[i++]=Constpara10[j];
			sum+=Constpara10[j];
		}
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_RS_NA_1;
		BW_Before_JM1[i++]=0x0A;								//可变长度结构限定词，需要根据发送的数据个数
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//定值区号，参数里为0，定值才需要
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x01;					//参数特征标识,有后续
	
		BW_Before_JM1[i++]=0x01;              //信息体地址
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[0];  //tag类型
		BW_Before_JM1[i++]=Constpara_Len[0];  //数据长度
		for(j=0;j<Constpara_Len[0];j++)
		{
			BW_Before_JM1[i++]=Constpara1[j];   //数据
		}
		BW_Before_JM1[i++]=0x02;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[1];
		BW_Before_JM1[i++]=Constpara_Len[1];
		for(j=0;j<Constpara_Len[1];j++)
		{
			BW_Before_JM1[i++]=Constpara2[j];
		}		
		BW_Before_JM1[i++]=0x03;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[2];
		BW_Before_JM1[i++]=Constpara_Len[2];
		for(j=0;j<Constpara_Len[2];j++)
		{
			BW_Before_JM1[i++]=Constpara3[j];
		}	
		BW_Before_JM1[i++]=0x04;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[3];
		BW_Before_JM1[i++]=Constpara_Len[3];
		for(j=0;j<Constpara_Len[3];j++)
		{
			BW_Before_JM1[i++]=Constpara4[j];
		}	
		BW_Before_JM1[i++]=0x05;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[4];
		BW_Before_JM1[i++]=Constpara_Len[4];
		for(j=0;j<Constpara_Len[4];j++)
		{
			BW_Before_JM1[i++]=Constpara5[j];
		}	
		BW_Before_JM1[i++]=0x06;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[5];
		BW_Before_JM1[i++]=Constpara_Len[5];
		for(j=0;j<Constpara_Len[5];j++)
		{
			BW_Before_JM1[i++]=Constpara6[j];
		}	
		BW_Before_JM1[i++]=0x07;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[6];
		BW_Before_JM1[i++]=Constpara_Len[6];
		for(j=0;j<Constpara_Len[6];j++)
		{
			BW_Before_JM1[i++]=Constpara7[j];
		}
		BW_Before_JM1[i++]=0x08;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[7];
		BW_Before_JM1[i++]=Constpara_Len[7];
		for(j=0;j<Constpara_Len[7];j++)
		{
			BW_Before_JM1[i++]=Constpara8[j];
		}
		BW_Before_JM1[i++]=0x09;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[8];
		BW_Before_JM1[i++]=Constpara_Len[8];
		for(j=0;j<Constpara_Len[8];j++)
		{
			BW_Before_JM1[i++]=Constpara9[j];
		}
		BW_Before_JM1[i++]=0x0A;
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[9];
		BW_Before_JM1[i++]=Constpara_Len[9];
		for(j=0;j<Constpara_Len[9];j++)
		{
			BW_Before_JM1[i++]=Constpara10[j];
		}
	}
	return i;		
}

u8 Read_All_Runparas_BefJM(u8 ca)					//读取全部运行参数
{
	u8 sum=0,i=0,j=0,k=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------帧长限定词------------*/
	  BW_Before_JM1[i++]=Runpara_Num;			      //全部的运行参数共Runpara_Num个
		sum+=Runpara_Num;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------参数特征标识------------*/	
		BW_Before_JM1[i++]=0x01;								//有后续
		sum+=0x01;	
		/*-----------信息对象------------*/
		for(j=0;j<Runpara_Num;j++)
		{
			BW_Before_JM1[i++]=0x20+j;
			sum+=0x20+j;
			BW_Before_JM1[i++]=0x80;
			sum+=0x80;
			BW_Before_JM1[i++]=Runpara_Tag[j];
			sum+=Runpara_Tag[j];
			if(Runpara_Tag[j] == 38)
			{
			  BW_Before_JM1[i++]=4;
			  sum+=4;
				for(k=0;k<4;k++)
				{
					BW_Before_JM1[i++]=run_send.runpara[j].bytedata[k];
					sum+=run_send.runpara[j].bytedata[k];
				}
			}
			else if(Runpara_Tag[j] == 1)
			{
				BW_Before_JM1[i++]=1;
			  sum+=1;
				BW_Before_JM1[i++]=run_send.runpara[j].bytedata[0];
				sum+=run_send.runpara[j].bytedata[0];
			}
		}
		
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_RS_NA_1;
		BW_Before_JM1[i++]=0x15;								//可变长度结构限定词，需要根据发送的数据个数
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//定值区号，参数里为0，定值才需要
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x01;					//参数特征标识,有后续
		
		for(j=0;j<Runpara_Num;j++)
		{
			BW_Before_JM1[i++]=0x20+j;
			BW_Before_JM1[i++]=0x80;
			BW_Before_JM1[i++]=0;
			BW_Before_JM1[i++]=Runpara_Tag[j];
			if(Runpara_Tag[j] == 38)
			{
			  BW_Before_JM1[i++]=4;
				for(k=0;k<4;k++)
				{
					BW_Before_JM1[i++]=run_send.runpara[j].bytedata[k];
				}
			}
			else if(Runpara_Tag[j] == 1)
			{
				BW_Before_JM1[i++]=1;
				BW_Before_JM1[i++]=run_send.runpara[j].bytedata[0];
			}
		}
	}
	return i;		
}

u8 Read_All_Fixparas_BefJM(u8 ca)
{
	u8 sum=0,i=0,j=0,k=0;
	
	fix_send.fixpara[8].bytedata[0] = ProtectSet.para.delay_fast_off_enable;
	fix_send.fixpara[9].floatdata = (float)ProtectSet.para.delay_fast_off / 100;//过流I段定值
  fix_send.fixpara[10].floatdata = (float)ProtectSet.para.delay_fast_off_time / 100;
	fix_send.fixpara[11].bytedata[0] = ProtectSet.para.delay_fast_off_voltage_lock;
	fix_send.fixpara[12].bytedata[0] = ProtectSet.para.delay_fast_off_direction_lock;
	
	fix_send.fixpara[13].bytedata[0] = ProtectSet.para.max_current_enable;
	fix_send.fixpara[14].floatdata = (float)ProtectSet.para.max_current / 100;//过流II段定值
  fix_send.fixpara[15].floatdata = (float)ProtectSet.para.max_current_time / 100;
	fix_send.fixpara[16].bytedata[0] = ProtectSet.para.max_current_voltage_lock;
	fix_send.fixpara[17].bytedata[0] = ProtectSet.para.max_current_direction_lock;
	
	fix_send.fixpara[18].floatdata = (float)ProtectSet.para.max_current_after / 100;//过流后加速定值
  fix_send.fixpara[19].floatdata = (float)ProtectSet.para.max_current_after_time / 100;
	
	fix_send.fixpara[20].floatdata = (float)ProtectSet.para.max_load / 100;//过负荷告警定值
  fix_send.fixpara[21].floatdata = (float)ProtectSet.para.max_load_time / 100;
	
	fix_send.fixpara[22].bytedata[0] = ProtectSet.para.zero_fast_off_enable;
	fix_send.fixpara[23].floatdata = (float)ProtectSet.para.zero_fast_off / 100;//零序I段定值
  fix_send.fixpara[24].floatdata = (float)ProtectSet.para.zero_fast_off_time / 100;
	fix_send.fixpara[25].bytedata[0] = 0;
	
	fix_send.fixpara[26].floatdata = (float)ProtectSet.para.zero_max_current_after / 100;//零序后加速定值
  fix_send.fixpara[27].floatdata = (float)ProtectSet.para.zero_max_current_after_time / 100;
		
	fix_send.fixpara[28].bytedata[0] = ProtectSet.para.once_reon_enable;//重合闸投退
	fix_send.fixpara[29].bytedata[0] = ProtectSet.para.reon_Un_enable;  //重合闸检无压投退
	fix_send.fixpara[30].bytedata[0] = ProtectSet.para.reon_synchron_enable;//重合闸检同期投退
	fix_send.fixpara[31].floatdata = (float)ProtectSet.para.once_reon_time / 100;//重合闸时间
	
	fix_send.fixpara[32].bytedata[0] = 1;        //小电流接地告警投退                      
	fix_send.fixpara[33].floatdata = (float)ProtectSet.para.zero_max_voltage / 100;//零序电压定值
		
	fix_send.fixpara[34].bytedata[0] = ProtectSet.para.max_voltage_enable; //过压保护投退                   
	fix_send.fixpara[35].floatdata = (float)ProtectSet.para.max_voltage / 100;//过压保护定值
	fix_send.fixpara[36].floatdata = (float)ProtectSet.para.max_voltage_time / 100;//过压保护延时
		
	fix_send.fixpara[37].bytedata[0] = ProtectSet.para.max_freq_enable; //高频保护投退                   
	fix_send.fixpara[38].floatdata = (float)ProtectSet.para.max_freq / 100;//高频保护定值
	fix_send.fixpara[39].floatdata = (float)ProtectSet.para.max_freq_time / 100;//高频保护延时
		
	fix_send.fixpara[40].bytedata[0] = ProtectSet.para.low_freq_enable; //低频保护投退                   
	fix_send.fixpara[41].floatdata = (float)ProtectSet.para.low_freq / 100;//低频保护定值
	fix_send.fixpara[42].floatdata = (float)ProtectSet.para.low_freq_time / 100;//低频保护延时
		
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------帧长限定词------------*/
	  BW_Before_JM1[i++]=Fixpara_Num;				     //全部定值Fixpara_Num个
		sum+=Fixpara_Num;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------参数特征标识------------*/	
		BW_Before_JM1[i++]=0x0;									//读取完定值，就没有后续了
		sum+=0x0;	
		/*-----------信息对象------------*/
		for(j=0;j<Fixpara_Num;j++)
		{
			BW_Before_JM1[i++]=0x20+j;
			sum+=0x20+j;
			BW_Before_JM1[i++]=0x82;
			sum+=0x82;
			BW_Before_JM1[i++]=Fixpara_Tag[j];
			sum+=Fixpara_Tag[j];
			if(Fixpara_Tag[j] == 38)
			{
				BW_Before_JM1[i++]=4;
				sum+=4;
				for(k=0;k<4;k++)
				{
					BW_Before_JM1[i++]=fix_send.fixpara[j].bytedata[k];
					sum+=fix_send.fixpara[j].bytedata[k];
				}
		  }
			else if(Fixpara_Tag[j] == 1)
			{
				BW_Before_JM1[i++]=1;
				sum+=1;
				BW_Before_JM1[i++]=fix_send.fixpara[j].bytedata[0];
				sum+=fix_send.fixpara[j].bytedata[0];
			}
		}

		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_RS_NA_1;
		BW_Before_JM1[i++]=0x01;								//可变长度结构限定词，需要根据发送的数据个数
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//定值区号		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x0;					//参数特征标识	
		for(j=0;j<Fixpara_Num;j++)
		{
			BW_Before_JM1[i++]=0x20+j;
			BW_Before_JM1[i++]=0x82;
			BW_Before_JM1[i++]=0x0;
			BW_Before_JM1[i++]=Fixpara_Tag[j];
			if(Fixpara_Tag[j] == 38)
			{
				BW_Before_JM1[i++]=4;
				for(k=0;k<4;k++)
				{
					BW_Before_JM1[i++]=fix_send.fixpara[j].bytedata[k];
				}
		  }
			else if(Fixpara_Tag[j] == 1)
			{
				BW_Before_JM1[i++]=1;
				BW_Before_JM1[i++]=fix_send.fixpara[j].bytedata[0];
			}
		}
	}
	return i;		
}

u8 Read_ConstParas_BefJM(u8 ca,u8 yw)      //固有参数读取
{
	u8 sum=0,j=0,k=0,i=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=const_value.params_num;
		sum+=const_value.params_num;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------参数特征标识------------*/	
		BW_Before_JM1[i++]=yw;
		sum+=yw;	
		/*-----------信息对象地址------------*/
		for(j=0;j<const_value.params_num;j++)
		{
			if(const_value.params_addr[j] > 0)
			{
				BW_Before_JM1[i++]=const_value.params_addr[j];
				sum+=const_value.params_addr[j];
				BW_Before_JM1[i++]=0x80;
				sum+=0x80;
				BW_Before_JM1[i++]=Constpara_Tag[const_value.params_addr[j]-1];
				sum+=Constpara_Tag[const_value.params_addr[j]-1];
				BW_Before_JM1[i++]=Constpara_Len[const_value.params_addr[j]-1];
				sum+=Constpara_Len[const_value.params_addr[j]-1];
				switch(const_value.params_addr[j])
				{
					case 1:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara1[k];
							sum+=Constpara1[k];
						}							
						break;
					case 2:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara2[k];
							sum+=Constpara2[k];
						}		
						break;
					case 3:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara3[k];
							sum+=Constpara3[k];
						}		
						break;
					case 4:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara4[k];
							sum+=Constpara4[k];
						}		
						break;
					case 5:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara5[k];
							sum+=Constpara5[k];
						}		
						break;
					case 6:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara6[k];
							sum+=Constpara6[k];
						}		
						break;
					case 7:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara7[k];
							sum+=Constpara7[k];
						}		
						break;
					case 8:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara8[k];
							sum+=Constpara8[k];
						}		
						break;
					case 9:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara9[k];
							sum+=Constpara9[k];
						}		
						break;
					case 10:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara10[k];
							sum+=Constpara10[k];
						}		
						break;
				}
		  }
		}
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_RS_NA_1;
		BW_Before_JM1[i++]=const_value.params_num;								//可变长度结构限定词，需要根据发送的数据个数
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//定值区号		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=yw;					//参数特征标识	
		
		
		/*-----------信息对象地址------------*/
		for(j=0;j<const_value.params_num;j++)
		{
			if(const_value.params_addr[j] > 0)
			{
				BW_Before_JM1[i++]=const_value.params_addr[j];
				BW_Before_JM1[i++]=0x80;
				BW_Before_JM1[i++]=0x0;
				BW_Before_JM1[i++]=Constpara_Tag[const_value.params_addr[j]-1];
				BW_Before_JM1[i++]=Constpara_Len[const_value.params_addr[j]-1];
				switch(const_value.params_addr[j])
				{
					case 1:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara1[k];
						}							
						break;
					case 2:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara2[k];
						}		
						break;
					case 3:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara3[k];
						}		
						break;
					case 4:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara4[k];
						}		
						break;
					case 5:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara5[k];
						}		
						break;
					case 6:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara6[k];
						}		
						break;
					case 7:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara7[k];
						}		
						break;
					case 8:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara8[k];
						}		
						break;
					case 9:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara9[k];
						}		
						break;
					case 10:
						for(k=0;k<Constpara_Len[const_value.params_addr[j]-1];k++)
						{
							BW_Before_JM1[i++]=Constpara10[k];
						}		
						break;
				}
		  }
		}
	}
	return i;
}

u8 Read_RunParas_BefJM(u8 ca,u8 yw)
{
	u8 sum=0,j=0,i=0,k=0;
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=run_value.params_num;
		sum+=run_value.params_num;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------参数特征标识------------*/	
		BW_Before_JM1[i++]=yw;
		sum+=yw;	
		/*-----------信息对象地址------------*/
		for(j=0;j<run_value.params_num;j++)
		{
			if(run_value.params_addr[j]>=0x20)
			{
				BW_Before_JM1[i++]=run_value.params_addr[j];
				sum+=run_value.params_addr[j];
				BW_Before_JM1[i++]=0x80;
				sum+=0x80;
				run_value.params_addr[j]-=0x20;
				BW_Before_JM1[i++]=Runpara_Tag[run_value.params_addr[j]];
				sum+=Runpara_Tag[run_value.params_addr[j]];
				if(Runpara_Tag[run_value.params_addr[j]] == 38)
				{
					BW_Before_JM1[i++]=4;
					sum+=4;
					for(k=0;k<4;k++)
					{
						BW_Before_JM1[i++]=run_send.runpara[run_value.params_addr[j]].bytedata[k];
						sum+=run_send.runpara[run_value.params_addr[j]].bytedata[k];
					}
				}
				else if(Runpara_Tag[run_value.params_addr[j]] == 1)
				{
					BW_Before_JM1[i++]=1;
					sum+=1;
					BW_Before_JM1[i++]=run_send.runpara[run_value.params_addr[j]].bytedata[0];
					sum+=run_send.runpara[run_value.params_addr[j]].bytedata[0];
				}
		  }
	  }

		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_RS_NA_1;
		BW_Before_JM1[i++]=run_value.params_num;								//可变长度结构限定词，需要根据发送的数据个数
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//定值区号		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x01;					//参数特征标识	
		
		/*-----------信息对象地址------------*/
		for(j=0;j<run_value.params_num;j++)
		{
			if(run_value.params_addr[j]>=0x20)
		  {
				BW_Before_JM1[i++]=run_value.params_addr[j];
				BW_Before_JM1[i++]=0x80;
				BW_Before_JM1[i++]=0x0;
				run_value.params_addr[j]-=0x20;
				BW_Before_JM1[i++]=Runpara_Tag[run_value.params_addr[j]];
				if(Runpara_Tag[run_value.params_addr[j]] == 38)
				{
					BW_Before_JM1[i++]=4;
					for(k=0;k<4;k++)
					{
						BW_Before_JM1[i++]=run_send.runpara[run_value.params_addr[j]].bytedata[k];
					}
				}
				else if(Runpara_Tag[run_value.params_addr[j]] == 1)
				{
					BW_Before_JM1[i++]=1;
					BW_Before_JM1[i++]=run_send.runpara[run_value.params_addr[j]].bytedata[0];
				}
      }
		}
	}
	return i;
}

u8 Read_FixvalueParas_BefJM(u8 ca,u8 yw)
{
	u8 sum=0,j=0,i=0,k=0;
	
	fix_send.fixpara[8].bytedata[0] = ProtectSet.para.delay_fast_off_enable;
	fix_send.fixpara[9].floatdata = (float)ProtectSet.para.delay_fast_off / 100;//过流I段定值
  fix_send.fixpara[10].floatdata = (float)ProtectSet.para.delay_fast_off_time / 100;
	fix_send.fixpara[11].bytedata[0] = ProtectSet.para.delay_fast_off_voltage_lock;
	fix_send.fixpara[12].bytedata[0] = ProtectSet.para.delay_fast_off_direction_lock;
	
	fix_send.fixpara[13].bytedata[0] = ProtectSet.para.max_current_enable;
	fix_send.fixpara[14].floatdata = (float)ProtectSet.para.max_current / 100;//过流II段定值
  fix_send.fixpara[15].floatdata = (float)ProtectSet.para.max_current_time / 100;
	fix_send.fixpara[16].bytedata[0] = ProtectSet.para.max_current_voltage_lock;
	fix_send.fixpara[17].bytedata[0] = ProtectSet.para.max_current_direction_lock;
	
	fix_send.fixpara[18].floatdata = (float)ProtectSet.para.max_current_after / 100;//过流后加速定值
  fix_send.fixpara[19].floatdata = (float)ProtectSet.para.max_current_after_time / 100;
	
	fix_send.fixpara[20].floatdata = (float)ProtectSet.para.max_load / 100;//过负荷告警定值
  fix_send.fixpara[21].floatdata = (float)ProtectSet.para.max_load_time / 100;
	
	fix_send.fixpara[22].bytedata[0] = ProtectSet.para.zero_fast_off_enable;
	fix_send.fixpara[23].floatdata = (float)ProtectSet.para.zero_fast_off / 100;//零序I段定值
  fix_send.fixpara[24].floatdata = (float)ProtectSet.para.zero_fast_off_time / 100;
	fix_send.fixpara[25].bytedata[0] = 0;
	
	fix_send.fixpara[26].floatdata = (float)ProtectSet.para.zero_max_current_after / 100;//零序后加速定值
  fix_send.fixpara[27].floatdata = (float)ProtectSet.para.zero_max_current_after_time / 100;
		
	fix_send.fixpara[28].bytedata[0] = ProtectSet.para.once_reon_enable;//重合闸投退
	fix_send.fixpara[29].bytedata[0] = ProtectSet.para.reon_Un_enable;  //重合闸检无压投退
	fix_send.fixpara[30].bytedata[0] = ProtectSet.para.reon_synchron_enable;//重合闸检同期投退
	fix_send.fixpara[31].floatdata = (float)ProtectSet.para.once_reon_time / 100;//重合闸时间
	
	fix_send.fixpara[32].bytedata[0] = 1;        //小电流接地告警投退                      
	fix_send.fixpara[33].floatdata = (float)ProtectSet.para.zero_max_voltage / 100;//零序电压定值
		
	fix_send.fixpara[34].bytedata[0] = ProtectSet.para.max_voltage_enable; //过压保护投退                   
	fix_send.fixpara[35].floatdata = (float)ProtectSet.para.max_voltage / 100;//过压保护定值
	fix_send.fixpara[36].floatdata = (float)ProtectSet.para.max_voltage_time / 100;//过压保护延时
		
	fix_send.fixpara[37].bytedata[0] = ProtectSet.para.max_freq_enable; //高频保护投退                   
	fix_send.fixpara[38].floatdata = (float)ProtectSet.para.max_freq / 100;//高频保护定值
	fix_send.fixpara[39].floatdata = (float)ProtectSet.para.max_freq_time / 100;//高频保护延时
		
	fix_send.fixpara[40].bytedata[0] = ProtectSet.para.low_freq_enable; //低频保护投退                   
	fix_send.fixpara[41].floatdata = (float)ProtectSet.para.low_freq / 100;//低频保护定值
	fix_send.fixpara[42].floatdata = (float)ProtectSet.para.low_freq_time / 100;//低频保护延时
		
	if(y_104==0)
	{
		/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=fix_value.params_num;
		sum+=fix_value.params_num;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------参数特征标识------------*/	
		BW_Before_JM1[i++]=yw;
		sum+=yw;	
		/*-----------信息对象地址------------*/
		for(j=0;j<fix_value.params_num;j++)
		{
			if(fix_value.params_addr[j] > 0x20)
			{
				BW_Before_JM1[i++]=fix_value.params_addr[j];
				sum+=fix_value.params_addr[j];
				BW_Before_JM1[i++]=0x82;
				sum+=0x82;
				fix_value.params_addr[j]-=0x20;
				BW_Before_JM1[i++]=Fixpara_Tag[fix_value.params_addr[j]];
				sum+=Fixpara_Tag[fix_value.params_addr[j]];
				if(Fixpara_Tag[fix_value.params_addr[j]] == 38)
				{
					BW_Before_JM1[i++]=4;
					sum+=4;
					for(k=0;k<4;k++)
					{
						BW_Before_JM1[i++]=fix_send.fixpara[fix_value.params_addr[j]].bytedata[k];
						sum+=fix_send.fixpara[fix_value.params_addr[j]].bytedata[k];
					}
			  }
				else if(Fixpara_Tag[fix_value.params_addr[j]] == 1)
				{
					BW_Before_JM1[i++]=1;
					sum+=1;
					BW_Before_JM1[i++]=fix_send.fixpara[fix_value.params_addr[j]].bytedata[0];
					sum+=fix_send.fixpara[fix_value.params_addr[j]].bytedata[0];
			  }
		  }
		}
		/*-----------校验码------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------帧尾------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_RS_NA_1;
		BW_Before_JM1[i++]=fix_value.params_num;								//可变长度结构限定词，需要根据发送的数据个数
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//定值区号		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x0;					//参数特征标识	
		for(j=0;j<fix_value.params_num;j++)
		{
			if(fix_value.params_addr[j] > 0x20)
			{
				BW_Before_JM1[i++]=fix_value.params_addr[j];
				BW_Before_JM1[i++]=0x82;
				BW_Before_JM1[i++]=0x0;
				fix_value.params_addr[j]-=0x20;
				BW_Before_JM1[i++]=Fixpara_Tag[fix_value.params_addr[j]];
				if(Fixpara_Tag[fix_value.params_addr[j]] == 38)
				{
					BW_Before_JM1[i++]=4;
					for(k=0;k<4;k++)
					{
						BW_Before_JM1[i++]=fix_send.fixpara[fix_value.params_addr[j]].bytedata[k];
					}
				}
				else if(Fixpara_Tag[fix_value.params_addr[j]] == 1)
				{
					BW_Before_JM1[i++]=1;
					BW_Before_JM1[i++]=fix_send.fixpara[fix_value.params_addr[j]].bytedata[0];
				}
		  }
		}
	}
	return i;	
}

u8 Pre_Set_BefJM(u8 ca)
{
	u8 sum=0,j=0,i=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_WS_NA_1;
		sum+=C_WS_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=Decryption.DataBuffer[8];
		sum+=Decryption.DataBuffer[8];
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------参数特征标识------------*/	
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;	
		/*-----------信息体数据------------*/		
		for(j=0;j<Decryption.DataBuffer[1]-12;j++)
		{
			BW_Before_JM1[i++]=Decryption.DataBuffer[16+j];
			sum+=Decryption.DataBuffer[16+j];
		}
		BW_Before_JM1[i++]=sum;
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_WS_NA_1;
		BW_Before_JM1[i++]=Decryption.DataBuffer[7];								//可变长度结构限定词，需要根据发送的数据个数
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//定值区号		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x80;					//参数特征标识	
		
		for(j=0;j<Decryption.DataBuffer[1]-13;j++)
		{
			BW_Before_JM1[i++]=Decryption.DataBuffer[15+j];
		}	
	}
	return i;
}

u8 Act_Set_BefJM(u8 ca,u8 reason,u8 sign)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
			/*-----------报头------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0xc;
		BW_Before_JM1[i++]=0xc;											//处理完再重新赋值
		BW_Before_JM1[i++]=0x68;
		/*-----------控制域-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------地址域------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------类型标识------------*/
		BW_Before_JM1[i++]=C_WS_NA_1;
		sum+=C_WS_NA_1;
		/*-----------帧长限定词------------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------传送原因------------*/
		BW_Before_JM1[i++]=reason;
		sum+=reason;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU地址------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------当前定值区号-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------参数特征标识------------*/	
		BW_Before_JM1[i++]=sign;
		sum+=sign;	

		BW_Before_JM1[i++]=sum;
		BW_Before_JM1[i++]=0x16;
	}
	else
	{
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		BW_Before_JM1[i++]=C_WS_NA_1;
		BW_Before_JM1[i++]=0;								//可变长度结构限定词，需要根据发送的数据个数
		BW_Before_JM1[i++]=reason;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//定值区号		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=sign;					//参数特征标识	
	}

	return i;
}

/*-----------------这个加密报文的通用发送文件和104是一样的，这两者没有区别------------------*/
void Common_Style(u16 length,u8 USARTxChannel,u8 * Send)
{
	u16 i=0,bw_len=0,j=0;
	u8 sum=0;
	bw_len=length+2;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	send_buffer[j++]=0xEB;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(bw_len>>8)&0xFF);
	send_buffer[j++]=(u8)(bw_len>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)bw_len&0xFF);
	send_buffer[j++]=(u8)bw_len&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	send_buffer[j++]=0xEB;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	send_buffer[j++]=0x00;
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x08);									//报文标识不知道是否正确
	send_buffer[j++]=0x08;
	sum+=0x08;
	for(i=0;i<length;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Send[i]);	
		send_buffer[j++]=Send[i];
		sum+=Send[i];
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);	
	send_buffer[j++]=sum;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);	
	send_buffer[j++]=0xD7;
	if(y_104==0)
	{
		send_buffer_len=j;
		flag_succ=1;
		timer_enable=1;	  //使能计时器
		delay_time_101=0;	//每次发送出去一帧，就开始计时	
	}
}

u8 Check_List_Name1()
{
	u8 i=0,flag=1;
	if(List_Num11==8)								//说明有可能是COMTRADE
	{
		for(i=0;i<8;i++)
		{
			if(List_Name11[i]!=Index_1[i]) flag=0;
		}
		if(flag)
		{
			index_which=1;
		}
	}
	else if(List_Num11==10)          //HISTORY/CO遥控记录
	{
		for(i=0;i<10;i++)
		{
			if(List_Name11[i]!=Index_3[i]) flag=0;
		}
		if(flag)
		{
			index_which=2;
		}		
	}
	else if(List_Num11==11)
	{
		if(List_Name11[8]=='S')
		{
			for(i=0;i<11;i++)
			{
				if(List_Name11[i]!=Index_2[i]) flag=0;
			}
			if(flag)
			{
				index_which=3;
			}					
		}
		else if(List_Name11[8]=='E')
		{
			for(i=0;i<11;i++)
			{
				if(List_Name11[i]!=Index_4[i]) flag=0;
			}
			if(flag)
			{
				index_which=4;
			}								
		}
		else if(List_Name11[8]=='F')
		{
			for(i=0;i<11;i++)
			{
				if(List_Name11[i]!=Index_6[i]) flag=0;
			}
			if(flag)
			{
				index_which=5;
			}					
		}
	}
	else if(List_Num11==12)
	{
		for(i=0;i<12;i++)
		{
			if(List_Name11[i]!=Index_8[i]) flag=0;
		}
		if(flag)
		{
			index_which=6;
		}						
	}
	else if(List_Num11==13)
	{
		for(i=0;i<13;i++)
		{
			if(List_Name11[i]!=Index_5[i]) flag=0;
		}
		if(flag)
		{
			index_which=7;
		}								
	}
	else if(List_Num11==15)
	{
		for(i=0;i<15;i++)
		{
			if(List_Name11[i]!=Index_7[i]) flag=0;
		}
		if(flag)
		{
			index_which=8;
		}				
	}

	return flag;
}

u8 Check_File_Name()
{
	u8 i=0,flag=1;
	switch(index_which)
	{
		case 1:													//Comtrade文件
			for(i=0;i<34;i++)
			{
				if(List_Name1[i]) flag=0;		
			}
			if(flag) Send_Doc_Num1=0;
			if(flag==0)
			{
				flag=1;
				for(i=0;i<34;i++)
				{
					if(List_Name1[i]) flag=0;		
				}			
			}
			if(flag) Send_Doc_Num1=0;
			break;
		case 2:												//HISTORY/CO遥控记录
			for(i=0;i<6;i++)
			{
				if(List_Name1[i]) flag=0;
			}
			if(flag) Send_Doc_Num1=0;
			break;
		case 3:													//HISTORY/SOE  
			for(i=0;i<7;i++)
			{
				if(List_Name1[i]) flag=0;
			}
			if(flag) Send_Doc_Num1=0;
			break;
		case 4:														//HISTORY/EXV
			for(i=0;i<15;i++)
			{
				if(List_Name1[i]) flag=0;
			}
			if(flag) Send_Doc_Num1=0;
			break;	
		case 5:															//HISTORY/FRZ
			for(i=0;i<15;i++)
			{
				if(List_Name1[i]) flag=0;
			}
			if(flag) Send_Doc_Num1=0;
			break;
		case 6:															//HISTORY/ULOG  
			for(i=0;i<8;i++)
			{
				if(List_Name1[i]) flag=0;
			}
			if(flag) Send_Doc_Num1=0;
			break;
		case 7:															//HISTORY/FIXPT
			for(i=0;i<17;i++)
			{
				if(List_Name1[i]) flag=0;
			}
			if(flag) Send_Doc_Num1=0;
			break;
		case 8:															//HISTORY/FLOWREV 
			for(i=0;i<11;i++)
			{
				if(List_Name1[i]) flag=0;
			}
			if(flag) Send_Doc_Num1=0;
			break;		
	}
	return flag;
}

u8 Check_WriteFile_Name()
{
	u8 flag=0;
	if((List_Name1[0]=='H')&&(List_Name1[1]=='W')&&(List_Name1[2]=='K')&&(List_Name1[3]=='Z')&&(List_Name1[4]=='Q'))
	//&&(List_Name1[5]=='.')&&(List_Name1[6]=='B')&&(List_Name1[7]=='I')&&(List_Name1[8]=='N')) //判断是不是.bin文件
	{
		flag=1;
	}
	else 
		Finish_Flag1=2;       //文件名不支持
	return flag;
}

/****************************************************************************
*
*名    称：void Encryption_Handle(u8 receivedata)
*
*功    能：加密解析函数
*
*入口参数：串口1数据，104规约用网口
*
*返回参数：
*
****************************************************************************/

void Encryption_Handle(u8 receivedata,u8 USARTxnumber)//处理从串口接收到的数据，剥离安全报文
{
	static u8 sum=0;
	switch(ProtocolResolveState)
	{
		case 0:  											//接收到新的数据帧
			if(receivedata == 0xEB)			// 判断数据头，主站向子站发送的启动字符
			{
				if(USARTxnumber == 1)
				{
					if(Usart1RxReady == 0)		//未接收完成
					{
						channel = 0;
						BackstageProtocol1.DataBuffer[0] = receivedata;//这个和bak两个像是一个用来实时处理，另一个像是备份，channel=0对应实时处理，channel=1对应备份
					}
					else if(Usart1bakRxReady == 0)
					{
						channel = 1;
						BackstageProtocol1bak.DataBuffer[0] = receivedata;
					}
			  }
				else if(USARTxnumber == 3)
				{
					if(Usart3RxReady == 0)		//未接收完成
					{
						channel = 0;
						BackstageProtocol3.DataBuffer[0] = receivedata;//这个和bak两个像是一个用来实时处理，另一个像是备份，channel=0对应实时处理，channel=1对应备份
					}
					else if(Usart3bakRxReady == 0)
					{
						channel = 1;
						BackstageProtocol3bak.DataBuffer[0] = receivedata;
					}
				}
				sum=0;
				RxCounter_serial1=1;
				remain1=3;								//还剩san个字符没有接收
				ProtocolResolveState = 1;//接收到一个字符
			}
			else 
				ProtocolResolveState = 0;
		break;
		case 1:													      //之前已经接收到数据0xeb
			if(USARTxnumber == 1)
			{
				if(channel == 0)                    //存在BackstageProtocol1中的数据
				{ 
					remain1--;			                    //剩余数量减1
					BackstageProtocol1.DataBuffer[RxCounter_serial1++] = receivedata;
					if(remain1==0)								      //如果已经接收到第四个字符了
					{
						if(BackstageProtocol1.DataBuffer[3]==0xEB) //第一个数据和第四个数据都为0xEB
						{
							Length_serial1 = ((u16)BackstageProtocol1.DataBuffer[1]<<8)|BackstageProtocol1.DataBuffer[2];
							remain1 = Length_serial1;
							ProtocolResolveState=2;
						}
						else													  //都不对，准备重新接收
							ProtocolResolveState = 0;
					}
				}			
				else
				{
					remain1--;			                    //剩余数量减1
					BackstageProtocol1bak.DataBuffer[RxCounter_serial1++] = receivedata;
					if(remain1==0)								   
					{
						if(BackstageProtocol1.DataBuffer[3]==0xEB) //第一个数据和第四个数据都为0xEB
						{
							Length_serial1  =((u16)BackstageProtocol1bak.DataBuffer[1]<<8)|BackstageProtocol1bak.DataBuffer[2];
							remain1=Length_serial1;
							ProtocolResolveState=2;				
						}
						else													  //都不对，准备重新接收
							ProtocolResolveState = 0;
					}
				}
		  }
			else if(USARTxnumber == 3)
			{
				if(channel == 0)                    //存在BackstageProtocol1中的数据
				{ 
					remain1--;			                    //剩余数量减1
					BackstageProtocol3.DataBuffer[RxCounter_serial1++] = receivedata;
					if(remain1==0)								      //如果已经接收到第四个字符了
					{
						if(BackstageProtocol3.DataBuffer[3]==0xEB) //第一个数据和第四个数据都为0xEB
						{
							Length_serial1 = ((u16)BackstageProtocol3.DataBuffer[1]<<8)|BackstageProtocol3.DataBuffer[2];
							remain1 = Length_serial1;
							ProtocolResolveState=2;
						}
						else													  //都不对，准备重新接收
							ProtocolResolveState = 0;
					}
				}			
				else
				{
					remain1--;			                    //剩余数量减1
					BackstageProtocol3bak.DataBuffer[RxCounter_serial1++] = receivedata;
					if(remain1==0)								   
					{
						if(BackstageProtocol3.DataBuffer[3]==0xEB) //第一个数据和第四个数据都为0xEB
						{
							Length_serial1  =((u16)BackstageProtocol3bak.DataBuffer[1]<<8)|BackstageProtocol3bak.DataBuffer[2];
							remain1=Length_serial1;
							ProtocolResolveState=2;				
						}
						else													  //都不对，准备重新接收
							ProtocolResolveState = 0;
					}
				}
		  }
		break;
		case 2:
      remain1--;			
      if(USARTxnumber == 1)
			{			
				if(channel == 0) 
				{
					BackstageProtocol1.DataBuffer[RxCounter_serial1++] = receivedata;								
				}																										
				else
				{
					BackstageProtocol1bak.DataBuffer[RxCounter_serial1++] = receivedata;										
				}			
		  }
			else if(USARTxnumber == 3)
			{			
				if(channel == 0) 
				{
					BackstageProtocol3.DataBuffer[RxCounter_serial1++] = receivedata;										
				}																										
				else
				{
					BackstageProtocol3bak.DataBuffer[RxCounter_serial1++] = receivedata;									
				}	
		  }
			sum+=receivedata;		
			if(remain1==0) 																										//已经接收完数据区
			{
				if(Length_serial1==(RxCounter_serial1-4))																//验证长度是否一致					
					ProtocolResolveState = 3;					
				else 
					ProtocolResolveState = 0;
			}			
		break;
		case 3:	
      if(USARTxnumber == 1)
			{			
				if(channel == 0) 
				{
					BackstageProtocol1.DataBuffer[RxCounter_serial1++] = receivedata;
					if(sum==receivedata)																							//查看校验和是否正确
					{
						ProtocolResolveState = 4;	
					}
					else
					{
						ProtocolResolveState = 0;
						if((BackstageProtocol1.DataBuffer[5]&0xF0)==0)
							Re_error_msg(3,0x01,0x1F,0x9110);
						else if((BackstageProtocol1.DataBuffer[5]&0xF0)==0x01)
							Re_error_msg(3,0x42,0x1F,0x9110);
						else if((BackstageProtocol1.DataBuffer[5]&0xF0)==0x10)
							Re_error_msg(3,0x80,0x1F,0x9110);
					}
				}																										
				else
				{
					BackstageProtocol1bak.DataBuffer[RxCounter_serial1++] = receivedata;
					if(sum==receivedata)																							//查看校验和是否正确
					{
						ProtocolResolveState = 4;	
					}
					else
					{
						ProtocolResolveState = 0;
						if((BackstageProtocol1bak.DataBuffer[5]&0xF0)==0)
							Re_error_msg(3,0x01,0x1F,0x9110);
						else if((BackstageProtocol1bak.DataBuffer[5]&0xF0)==0x01)
							Re_error_msg(3,0x42,0x1F,0x9110);
						else if((BackstageProtocol1bak.DataBuffer[5]&0xF0)==0x10)
							Re_error_msg(3,0x80,0x1F,0x9110);
					}
				}
        sum=0;				
		  }
			else if(USARTxnumber == 3)
			{
				if(channel == 0) 
				{
					BackstageProtocol3.DataBuffer[RxCounter_serial1++] = receivedata;
					if(sum==receivedata)																							//查看校验和是否正确
					{
						ProtocolResolveState = 4;	
					}
					else
					{
						ProtocolResolveState = 0;
						if((BackstageProtocol3.DataBuffer[5]&0xF0)==0)
							Re_error_msg(3,0x01,0x1F,0x9110);
						else if((BackstageProtocol3.DataBuffer[5]&0xF0)==0x01)
							Re_error_msg(3,0x42,0x1F,0x9110);
						else if((BackstageProtocol3.DataBuffer[5]&0xF0)==0x10)
							Re_error_msg(3,0x80,0x1F,0x9110);
					}
				}																										
				else
				{
					BackstageProtocol3bak.DataBuffer[RxCounter_serial1++] = receivedata;
					if(sum==receivedata)																							//查看校验和是否正确
					{
						ProtocolResolveState = 4;	
					}
					else
					{
						ProtocolResolveState = 0;
						if((BackstageProtocol3bak.DataBuffer[5]&0xF0)==0)
							Re_error_msg(3,0x01,0x1F,0x9110);
						else if((BackstageProtocol3bak.DataBuffer[5]&0xF0)==0x01)
							Re_error_msg(3,0x42,0x1F,0x9110);
						else if((BackstageProtocol3bak.DataBuffer[5]&0xF0)==0x10)
							Re_error_msg(3,0x80,0x1F,0x9110);
					}
				}
			}
			sum=0;	
		break;
		case 4:	
			if(receivedata==0xD7)																							//查看帧尾是否正确
			{
				if(USARTxnumber == 1)
				{				
					if(channel == 0) 
					{
						BackstageProtocol1.DataBuffer[RxCounter_serial1++] = receivedata;
						Usart1RxReady=1;
						if(BackstageProtocol1.DataBuffer[5]&0x08)
						{
							if(BackstageProtocol1.DataBuffer[5]&0xC0)
								datdone_serial1=3;					//运维工具加密
							else
								datdone_serial1=1;					//主站加密
						}
						else
							datdone_serial1=2;					//不加密
						if(BackstageProtocol1.DataBuffer[4]==0xFF)
							datdone_serial1=4;					//测试
					}																										
					else
					{
						BackstageProtocol1bak.DataBuffer[RxCounter_serial1++] = receivedata;
							Usart1bakRxReady=1;
							if(BackstageProtocol1bak.DataBuffer[5]&0x08)
							{
								if(BackstageProtocol1bak.DataBuffer[5]&0xC0)
									datdone_serial1=3;					//运维工具加密
								else
									datdone_serial1=1;					//主站加密
							}
							else
								datdone_serial1=2;					//不加密
							if(BackstageProtocol1bak.DataBuffer[4]==0xFF)
								datdone_serial1=4;					//测试
					}
		
				}
				else if(USARTxnumber == 3)
				{				
					if(channel == 0) 
					{
						BackstageProtocol3.DataBuffer[RxCounter_serial1++] = receivedata;
						Usart3RxReady=1;
						if(BackstageProtocol3.DataBuffer[5]&0x08)
						{
							if(BackstageProtocol3.DataBuffer[5]&0xC0)
								datdone_serial1=3;					//运维工具加密
							else
								datdone_serial1=1;					//主站加密
						}
						else
							datdone_serial1=2;					//不加密
						if(BackstageProtocol3.DataBuffer[4]==0xFF)
							datdone_serial1=4;					//测试
					}																										
					else
					{
						BackstageProtocol3bak.DataBuffer[RxCounter_serial1++] = receivedata;
							Usart3bakRxReady=1;
							if(BackstageProtocol3bak.DataBuffer[5]&0x08)
							{
								if(BackstageProtocol3bak.DataBuffer[5]&0xC0)
									datdone_serial1=3;					//运维工具加密
								else
									datdone_serial1=1;					//主站加密
							}
							else
								datdone_serial1=2;					//不加密
							if(BackstageProtocol3bak.DataBuffer[4]==0xFF)
								datdone_serial1=4;					//测试
					}
				}		
				isr_evt_set (0x0080, t_Task4); //这个任务4专门用来处理这些协议，除了这个之外还有其他的一些协议
		  }
      ProtocolResolveState = 0;			
		break;
		default:
			ProtocolResolveState=0;
			break;
	}
}

/*---------------------------------*/
//----------------加密报文处理
void ProtocolResolve1_JM(u8 USARTxChannel)	
{
	u16 Add=0,lc=0,i=0;	
	u8 Params_Num=0,sum=0,len=0,jm_ok=0,j=0,crc_yhf=0;
	u8 spi_flag=0,cd=0,ca=0;
  //先判断是不是可变长度帧报文
	if((Decryption.DataBuffer[0]==0x68)&&(Decryption.DataBuffer[3]==0x68)&&(Decryption.DataBuffer[1]==Decryption.DataBuffer[2])&&(Decryption.DataBuffer[BW_Len1-1]==0x16)) 	//说明是101报文
	{
		delay_time_101=0;
		timer_enable=0;
		resend_num=0;
		len=Decryption.DataBuffer[1];      //控制域+地址域+ASDU
		for(i=0;i<len;i++) sum+=Decryption.DataBuffer[4+i];
		if(sum==Decryption.DataBuffer[BW_Len1-2])					//校验和也检验完毕
		{
			ADDR=(u16)Decryption.DataBuffer[6]<<8|Decryption.DataBuffer[5];   //链路地址
			Asdu_addr=(u16)Decryption.DataBuffer[12]<<8|Decryption.DataBuffer[11]; //ASDU地址
			if(Decryption.DataBuffer[4]&0x10)						//FCV位为1，说明FCB有效，而且要判断是第几次进入
			{																											//如果FCV位为0，说明无效，那么就不需要在意这个FCB是否变位，也认为是正确的
				if(jm==0)																						//只进入一次
				{
					if(Decryption.DataBuffer[4]&0x20)        //FCB位为1
						FCB_Flag_Main=1;
					else 
						FCB_Flag_Main=0;															//FCB位为0
					jm=1;
					jm_ok=1;				//这个要加上，之前没加一直没进入下面的判断
				}
				else	//第二次进入
				{
					if((Decryption.DataBuffer[4]&0x20)&&(FCB_Flag_Main==0))	//主站FCB变位成功
					{
						FCB_Flag_Main=1;
						jm_ok=1;
					}
					else if(((Decryption.DataBuffer[4]&0x20)==0)&&(FCB_Flag_Main==1))	//主站FCB变位成功
					{
						FCB_Flag_Main=0;
						jm_ok=1;																			//ok_flag=1说明最少是第二次进入且FCB翻转正确
					}
					else 
					{
						jm_ok=2;																		//ok_flag=0说明最少是第二次进入且是主站重发的数据帧
					}
				}
			}
			else 
				jm_ok=1;
		}
		else
		{
			Re_error_msg(USARTxChannel,0x01,0x1F,0x9110);	
			return;
		}
	}
	else
	{
		Re_error_msg(USARTxChannel,0x01,0x1F,0x9110);	
		return;
	}
	if(jm_ok==1)												//报文正确接收且变位正确
	{
		send_buffer_len=0;	//一旦变位正确就把上一帧的备份删掉
		switch(Decryption.DataBuffer[7])     //类型标识符，下面就是处理真正的101规约报文了
		{
			case C_IC_NA_1:															//总召唤 
				ca=0x80;
				Common_MW(USARTxChannel,ca);
				if(call_num==0)                         //如果是第一次进入，那么对应初始化之后的第一次总召唤
				{
					Super_Sign[1]=1;
				}
				else																		//如果是第二次进入，是非初始化总召，可以被打断
				{
					Super_Sign[4]=1;
				}						
				break;
			case C_CS_NA_1: 																		//对时
				ca=0x80;
				Common_MW(USARTxChannel,ca);	
				if(Decryption.DataBuffer[9]==ACT)					//时钟同步
				{
					TimeNow.msec    = ((u16)(Decryption.DataBuffer[15] | (u16)(Decryption.DataBuffer[16]<<8))%1000)&0x3FF;
					TimeNow.second  = ((u16)(Decryption.DataBuffer[15] | (u16)(Decryption.DataBuffer[16]<<8))/1000)&0x3F;  
					TimeNow.minute  = Decryption.DataBuffer[17]&0x3F;
					TimeNow.hour    = Decryption.DataBuffer[18]&0x1F;
					TimeNow.date    = Decryption.DataBuffer[19]&0x1F;
					TimeNow.month   = Decryption.DataBuffer[20]&0x0F;
					TimeNow.year    = Decryption.DataBuffer[21]&0x7F;
					correct_time();		                      
					Super_Sign[6]=1;                       //生成时钟同步确认报文
				}
				else if(Decryption.DataBuffer[9]==REQ)		//时钟请求
				{
					Super_Sign[6]=2;                       //生成时钟请求确认报文
				}							
				break;
			case C_TS_NA_1:                             //链路测试命令
				ca=0x80;
				Common_MW(USARTxChannel,ca);		
				Test_Sign[0]=Decryption.DataBuffer[5];
				Test_Sign[1]=Decryption.DataBuffer[6];
				Test_Sign[2]=Decryption.DataBuffer[8];
				Test_Sign[3]=Decryption.DataBuffer[11];
				Test_Sign[4]=Decryption.DataBuffer[12];
				Test_Sign[5]=Decryption.DataBuffer[15];
				Test_Sign[6]=Decryption.DataBuffer[16];	
				Super_Sign[14]=1;
				break;
			case C_RP_NA_1:     																//复位进程命令
				ca=0x80;
				Common_MW(USARTxChannel,ca);	
				Super_Sign[8]=1;									
				break;
			case C_CI_NA_1:     								 								//电能脉冲召唤	
				ca=0x80;
				Common_MW(USARTxChannel,ca);	
				Super_Sign[11]=1;			
				break;	
			case C_SC_NA_1:																			//单点遥控
			case C_DC_NA_1:																			//双点点遥控						
																							            //一旦进入，立马验证签名的正确性		
				if(Decryption.DataBuffer[9]==ACT)			//选择或执行
				{
					if(Decryption.DataBuffer[15]&0x80)   	//判断S/E位是0还是1，在SCO(单命令),DCO(双命令)的最高位（第8位）
					{
						switch(YY_Style1)
						{
							case 1:	//签名
							case 4:	//签名
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
				  }
					else		//执行
					{
						switch(YY_Style1)
						{
							case 3:	//签名
							case 5:
							case 6:
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
				  }
			  }
					
				lc=BW_Len1+MsgSafty_Len1-1;												//验证发送数据长度，报文长度+时间信息长度+签名值
				spi_flag=0;
				while(spi_flag!=3)															  //验证签名的正确性
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															//帧头
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:签名密钥索引
					crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
					SPI2_ReadWrite((u8)(lc>>8)&0xFF);
					crc_yhf^=(u8)(lc>>8)&0xFF;
					SPI2_ReadWrite((u8)lc&0xFF);
					crc_yhf^=(u8)lc&0xFF;
					for(i=0;i<BW_Len1;i++) 
					{
						SPI2_ReadWrite(Decryption.DataBuffer[i]);
						crc_yhf^=Decryption.DataBuffer[i];
					}	
					for(i=0;i<MsgSafty_Len1-1;i++) 
					{
						SPI2_ReadWrite(Msg_Safty1[i]);
						crc_yhf^=Msg_Safty1[i];
					}	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)										//签名正确
				{
					succ_flag_spi=0;
					control_jm.byte = Decryption.DataBuffer[15];
					Object_addr=(u16)Decryption.DataBuffer[14]<<8|Decryption.DataBuffer[13];
					if(Decryption.DataBuffer[9]==ACT)				//选择或执行
					{
						if(control_jm.bit.S_E == 1)   	//判断S/E位是0还是1，在SCO(单命令),DCO(双命令)的最高位（第8位）
						{
							if(yk_state==0 && Select101Addr==0)	//处在初始状态，可以选择
							{
								ca=0x80;
								Common_MW(USARTxChannel,ca);		
								if(Decryption.DataBuffer[7] == C_SC_NA_1)
								  Super_Sign[2]=1;	
								else if(Decryption.DataBuffer[7] == C_DC_NA_1)
									Super_Sign[2]=5;	
								Select101Addr=1;
								yk_state=1;	//选择状态
							}
							else
							{
								ca=0x81;
								Common_MW(USARTxChannel,ca);	
								Select101Addr=0;
								yk_state=0;	//恢复到初始状态
							}
						}
						else if(control_jm.bit.S_E == 0)		//遥控执行
						{
							for(i=0;i<8;i++)
							{
								if(Msg_Safty1[6+i]!=Ter_Random1[i])	//验证随机数是否是我们发出去的
								{
									Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//验证随机数失败
									return;
								}
							}
							
							if(yk_state==1 && Select101Addr==1)		//只有处在选择状态才能执行
							{
								ca=0x80;
								Common_MW(USARTxChannel,ca);					//固定帧长确认帧		
								if(Decryption.DataBuffer[7] == C_SC_NA_1)
								  Super_Sign[2]=2;	
								else if(Decryption.DataBuffer[7] == C_DC_NA_1)
									Super_Sign[2]=6;	
								yk_state=3;	//处在执行状态
							}
							else
							{
								ca=0x81;	//否定确认
								Common_MW(USARTxChannel,ca);	
								yk_state=0;	//恢复到初始状态
								Select101Addr=0;
							}
						}
					}
					else if(Decryption.DataBuffer[9]==DEACT)  //终止\撤销命令
					{
						if(yk_state==1)	//只有处在选择状态才能撤销
						{
							ca=0x80;
							Common_MW(USARTxChannel,ca);					//固定帧长确认帧	
              if(Decryption.DataBuffer[7] == C_SC_NA_1)
								Super_Sign[2]=3;	//这里需要执行遥控取消操作					
							else if(Decryption.DataBuffer[7] == C_DC_NA_1)
								Super_Sign[2]=7;														
							yk_state=2;	        //处在撤销
							Select101Addr=0;								
						}
						else
						{
							ca=0x81;	//否定确认
							Common_MW(USARTxChannel,ca);	
							yk_state=0;	//恢复到初始状态
							Select101Addr=0;
						}
					}
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//验证签名失败
				break;
			case F_SC_NA_1:												//召唤目录
	//				ca=0x80;
	//				Common_MW(USARTxChannel,ca);	
	//				ca=0xc3;
	//				if(Check_List_Name1())									//说明目录存在
	//				{
	//					Send_len=Confirm_List_BefJM(ca,0);				 //0表示成功，1表示失败
	//					BW_Before_JM1[1]=Send_len-6;
	//					BW_Before_JM1[2]=Send_len-6;
	//					lc=16+4+Send_len;		
	//					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
	//					{
	//						succ_flag_spi=0;
	//						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
	//					}					
	//				}
	//				else
	//				{
	//					Send_len=Confirm_List_BefJM(ca,1);				 //0表示成功，1表示失败
	//					BW_Before_JM1[1]=Send_len-6;
	//					BW_Before_JM1[2]=Send_len-6;
	//					lc=16+4+Send_len;		
	//					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
	//					{
	//            succ_flag_spi=0;
	//						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
	//					}									
	//				}			
				break;
			case F_FR_NA_1:		  									//文件传输,包括：文件目录召唤，读写文件激活，读写文件传输，主要依靠操作标识进行识别
				if(Decryption.DataBuffer[16]==1)								//召唤文件目录
				{
					ca=0x80;
					Common_MW(USARTxChannel,ca);
					if(Decryption.DataBuffer[21]==0)								//默认目录,COMTRADE
					{
						List_Num11=8;
						List_Name11[0]=0x43;
						List_Name11[1]=0x4F;
						List_Name11[2]=0x4D;
						List_Name11[3]=0x54;
						List_Name11[4]=0x52;
						List_Name11[5]=0x41;
						List_Name11[6]=0x44;
						List_Name11[7]=0x45;
					}
					else
					{
						List_Num11=Decryption.DataBuffer[21];							//召唤的目录名长度
						for(i=0;i<List_Num11;i++)
						{
							List_Name11[i]=Decryption.DataBuffer[22+i];			//ASC的形式存储的目录名
						}					
					}
					if(Decryption.DataBuffer[22+List_Num11])					//所要求的文件在此时间段内
					{
						List_Name11[49]=1;
						for(i=0;i<7;i++)
						{
							//Start_Time1[i]=Decryption.DataBuffer[23+List_Num11+i]; //时间段区间
							//End_Time1[i]=Decryption.DataBuffer[30+List_Num11+i];
						}				
					}
					else
						List_Name11[49]=0;																					//所有文件
					Super_Sign[9]=1;	
				}	
				else if(Decryption.DataBuffer[16]==3)					       	//读文件激活
				{
					ca=0x80;
					Common_MW(USARTxChannel,ca);
					List_Num1=Decryption.DataBuffer[17];									//要读取的文件名长度
					for(i=0;i<List_Num1;i++)													
					{
						List_Name1[i]=Decryption.DataBuffer[18+i];			   	//ASC的形式存储文件名，包括扩展名，即后缀，比如.dat
					}
					Super_Sign[10]=1;
				}
				else if(Decryption.DataBuffer[16]==6)						//读文件数据传输确认
				{
					ca=0x80;																
					Common_MW(USARTxChannel,ca);					//固定帧长确认帧				
					Super_Sign[10]=0;									
				}	
				else if((Decryption.DataBuffer[9]==ACT)&&(Decryption.DataBuffer[16]==7))						//写文件激活
				{			
					ca=0x80;																
					Common_MW(USARTxChannel,ca);					//固定帧长确认帧
					List_Num1=Decryption.DataBuffer[17];									    //要写入的文件名的长度
					for(i=0;i<List_Num1;i++)
					{
						List_Name1[i]=Decryption.DataBuffer[18+i];			  		 	//ASC的形式存储文件名
					}	
					Rec_Doc_Num1 |= (((u32)Decryption.DataBuffer[22+List_Num1])&0x000000FF);		
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[23+List_Num1])<<8)&0x0000FF00);	
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[24+List_Num1])<<16)&0x00FF0000);		
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[25+List_Num1])<<24)&0xFF000000);					
					if(Rec_Doc_Num1>0x80000)	Finish_Flag1=3;											 			//长度检查，不能大于512k
					lc=BW_Len1+MsgSafty_Len1-1;												//验证发送数据长度，报文长度+时间信息长度+签名值
					spi_flag=0;
					while(spi_flag!=3)															//验证签名的正确性
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);															//帧头
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;
						SPI2_ReadWrite(0x08);
						crc_yhf^=0x08;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:签名密钥索引
						crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<BW_Len1;i++) 
						{
							SPI2_ReadWrite(Decryption.DataBuffer[i]);
							crc_yhf^=Decryption.DataBuffer[i];
						}	
						for(i=0;i<MsgSafty_Len1-1;i++) 
						{
							SPI2_ReadWrite(Msg_Safty1[i]);
							crc_yhf^=Msg_Safty1[i];
						}	
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi) 																		
					{
						succ_flag_spi=0;
						Super_Sign[10]=3;
						Rec_num_101=0;	
					}
					else		
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//验证签名失败
				}	
				else if((Decryption.DataBuffer[9]==ACT)&&(Decryption.DataBuffer[16]==9))						//写文件传输
				{
					Rec_Error_Flag1=0;//每次进来都需要把这个清0，不然如果上次有错，会一直报错
					Receive_Doc_101();	//实际使用																													//每次接受到主站的写文件数据都会进入
					ca=0x80;																
					Common_MW(USARTxChannel,ca);					//固定帧长确认帧

					if((Decryption.DataBuffer[25]==0)||(Rec_Error_Flag1))//无后续或者出错了，错误必须要上报，不然主站不可能知道错误
					{
						ulFlieLength=Decryption.DataBuffer[1]-23;
						Super_Sign[10]=5;					
					}
				}				
		    //需要对目录进行查询，我们flash内存应该先存好几条目录，COMTRADE(其目录下存故障录播文件)对应ASCII为43 4F 4D 54 52 41 44 45	,HISTORY/SOE,HISTORY/CO,HISTORY/EXV,HISTORY/FIXPT等等	
		    //查到目录后，将该目录下的文件名发送到主站		
				break;	
			case F_SR_NA_1:												//软件升级
				if(Decryption.DataBuffer[9]==ACT)
				{
					if(Decryption.DataBuffer[15]&0x80)							  //判断S/E，1升级启动，
					{
						switch(YY_Style1)
						{
							case 0:	//签名
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
						ca=0x80;
						Common_MW(USARTxChannel,ca);
						lc=BW_Len1+MsgSafty_Len1-1;											//验证发送数据长度，报文长度+时间信息长度+签名值
						spi_flag=0;
						while(spi_flag!=3)															//验证签名的正确性
						{
							SSN_L();	
							SPI2_ReadWrite(0x55);													//帧头
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;
							SPI2_ReadWrite(0x08);
							crc_yhf^=0x08;
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;
							SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);	//asKID:签名密钥索引
							crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
							SPI2_ReadWrite((u8)(lc>>8)&0xFF);
							crc_yhf^=(u8)(lc>>8)&0xFF;
							SPI2_ReadWrite((u8)lc&0xFF);
							crc_yhf^=(u8)lc&0xFF;
							for(i=0;i<BW_Len1;i++) 
							{
								SPI2_ReadWrite(Decryption.DataBuffer[i]);
								crc_yhf^=Decryption.DataBuffer[i];
							}	
							for(i=0;i<MsgSafty_Len1-1;i++) 
							{
								SPI2_ReadWrite(Msg_Safty1[i]);
								crc_yhf^=Msg_Safty1[i];
							}	
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);	
							crc_yhf=0;
							SSN_H();											   //拉高片选
							spi_flag=Ter_WriteRead_Spi1();
						}
						if(succ_flag_spi)
						{
							succ_flag_spi=0;
							Super_Sign[10]=6;
              Rec_num_101=0;								
						}
						else
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);		//验证签名失败
					}
					else																								//0升级结束
					{
						ca=0x80;																
						Common_MW(USARTxChannel,ca);				        	//固定帧长确认帧	
						Super_Sign[10]=7;						
					}
				}
				else if(Decryption.DataBuffer[9]==DEACT)					      //撤销升级
				{
					ca=0x80;																
					Common_MW(USARTxChannel,ca);					          //固定帧长确认帧	
					Super_Sign[10]=8;							
				}	
				break;
			case C_RR_NA_1:                     	                  //读当前定值区
				ca=0x80;																
				Common_MW(USARTxChannel,ca);					              //固定帧长确认帧
				Super_Sign[12]=1;	
				break;	
			case C_SR_NA_1:                     	                  //切换定值区
				switch(YY_Style1)
				{
					case 0:	//签名
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
						return;
					default:
						break;
				}
				ca=0x80;																
				Common_MW(USARTxChannel,ca);						
				lc=BW_Len1+MsgSafty_Len1-1;												    //验证发送数据长度，报文长度+时间信息长度+签名值
				spi_flag=0;
				while(spi_flag!=3)															      //验证签名的正确性
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															  //帧头
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:签名密钥索引，以此来判断是哪个主站证书
					crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
					SPI2_ReadWrite((u8)(lc>>8)&0xFF);
					crc_yhf^=(u8)(lc>>8)&0xFF;
					SPI2_ReadWrite((u8)lc&0xFF);
					crc_yhf^=(u8)lc&0xFF;
					for(i=0;i<BW_Len1;i++) 
					{
						SPI2_ReadWrite(Decryption.DataBuffer[i]);
						crc_yhf^=Decryption.DataBuffer[i];
					}	
					for(i=0;i<MsgSafty_Len1-1;i++) 
					{
						SPI2_ReadWrite(Msg_Safty1[i]);
						crc_yhf^=Msg_Safty1[i];
					}	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 																
				{
					succ_flag_spi=0;
					SN_Num1=Decryption.DataBuffer[15] | (Decryption.DataBuffer[16]<<8);
					Super_Sign[12]=2;	
				}
				else			
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//验证签名失败
				break;	
			case C_RS_NA_1:                     											//读参数和定值,读取参数时，定值区号无用
//			SN_Num1=Decryption.DataBuffer[13] | (Decryption.DataBuffer[14]<<8);	//不需要读取定值区号了，由前面切换或者读取的定值区号决定后面我们读写参数时用的定值区是哪个
				ca=0x80;																
				Common_MW(USARTxChannel,ca);											  //固定帧长确认帧		

				Params_total = 0;
				fix_value.params_num = 0;
				run_value.params_num = 0;
				const_value.params_num = 0;
			
				if((Decryption.DataBuffer[8]&0x7F)==0)					        //读取全部参数和定值
				{
					Super_Sign[12]=3;
				}
				else																							      //读取多个参数和定值
				{
					for(i=0;i<(Decryption.DataBuffer[8]&0x7F);i++)
					{
						if((Decryption.DataBuffer[16+2*i]==0x82)&&(Decryption.DataBuffer[15+2*i]>=0x20))     //定值
						{
							fix_value.params_addr[fix_value.params_num++]=Decryption.DataBuffer[15+2*i];//只取低地址
							Params_total++;
						}
						else if((Decryption.DataBuffer[16+2*i]==0x80)&&(Decryption.DataBuffer[15+2*i]>=0x20))							
						{
							run_value.params_addr[run_value.params_num++]=Decryption.DataBuffer[15+2*i];//只取低地址
							Params_total++;
						}
						else if((Decryption.DataBuffer[16+2*i]==0x80)&&(Decryption.DataBuffer[15+2*i]<0x20))
						{
							const_value.params_addr[const_value.params_num++]=Decryption.DataBuffer[15+2*i];//只取低地址
							Params_total++;	
						}
					}
					Super_Sign[12]=4;
				}		
				break;	
			case C_WS_NA_1:                     								//写参数和定值
				if((Decryption.DataBuffer[9]==0x06)&&((Decryption.DataBuffer[15]&0x80)==0x80))	//参数预置
				{
					switch(YY_Style1)
					{
						case 0:	//签名
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
							return;
						default:
							break;
					}
				}
				else if((Decryption.DataBuffer[9]==0x06)&&((Decryption.DataBuffer[15]&0xC0)==0))   //参数固化
				{
					switch(YY_Style1)
					{
						case 1:	//签名
						case 2:	//签名
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
							return;
						default:
							break;
					}
				}
					
				ca=0x80;																
				Common_MW(USARTxChannel,ca);	
				//全部先验证签名
				lc=BW_Len1+MsgSafty_Len1-1;												//验证发送数据长度，报文长度+时间信息长度+签名值
				spi_flag=0;
				while(spi_flag!=3)															  //验证签名的正确性
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);													  //帧头
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);		//asKID:签名密钥索引
					crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
					SPI2_ReadWrite((u8)(lc>>8)&0xFF);
					crc_yhf^=(u8)(lc>>8)&0xFF;
					SPI2_ReadWrite((u8)lc&0xFF);
					crc_yhf^=(u8)lc&0xFF;
					for(i=0;i<BW_Len1;i++) 
					{
						SPI2_ReadWrite(Decryption.DataBuffer[i]);
						crc_yhf^=Decryption.DataBuffer[i];
					}	
					for(i=0;i<MsgSafty_Len1-1;i++) 
					{
						SPI2_ReadWrite(Msg_Safty1[i]);
						crc_yhf^=Msg_Safty1[i];
					}	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;							
					SSN_H();											    //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi)										//签名正确
				{
					succ_flag_spi=0;
					if((Decryption.DataBuffer[9]==ACT)&&((Decryption.DataBuffer[15]&0x80)==0x80))	//参数预置
					{
            //参数预置，缓存一下，固化则需要，取消则不要
						Params_Num=Decryption.DataBuffer[8]&0x7F;																		//要预置几个参数
						
						for(i=0;i<Params_Num;i++)
						{
							Add=(u16)Decryption.DataBuffer[17+4*i+cd]<<8|Decryption.DataBuffer[16+4*i+cd];
							if(Add<0x8020)                             //固有参数，应该不会轻易修改固有参数
							{
								Add-=0x8000;														 //判断参数类型
								if(Add==1)
								{
									Constpara_State[0]=1;
									Constpara_Tag[0] =Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[0]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[0];j++)  Constpara1[j]= Decryption.DataBuffer[20+4*i+j+cd];
								}
								else if(Add==2)
								{
									Constpara_State[1]=1;
									Constpara_Tag[1]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[1]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[1];j++)  Constpara2[j]= Decryption.DataBuffer[20+4*i+j+cd];							
								}
								else if(Add==3)
								{
									Constpara_State[2]=1;
									Constpara_Tag[2]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[2]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[2];j++)  Constpara3[j]= Decryption.DataBuffer[20+4*i+j+cd];						
								}
								else if(Add==4)
								{
									Constpara_State[3]=1;
									Constpara_Tag[3]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[3]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[3];j++)  Constpara4[j]= Decryption.DataBuffer[20+4*i+j+cd];							
								}
								else if(Add==5)
								{
									Constpara_State[4]=1;
									Constpara_Tag[4]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[4]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[4];j++)  Constpara5[j]= Decryption.DataBuffer[20+4*i+j+cd];						
								}
								else if(Add==6)
								{
									Constpara_State[5]=1;
									Constpara_Tag[5]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[5]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[5];j++)  Constpara6[j]= Decryption.DataBuffer[20+4*i+j+cd];						
								}
								else if(Add==7)
								{
									Constpara_State[6]=1;
									Constpara_Tag[6]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[6]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[6];j++)  Constpara7[j]= Decryption.DataBuffer[20+4*i+j+cd];							
								}
								else if(Add==8)
								{
									Constpara_State[7]=1;
									Constpara_Tag[7]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[7]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[7];j++)  Constpara8[j]= Decryption.DataBuffer[20+4*i+j+cd];							
								}
								else if(Add==9)
								{
									Constpara_State[8]=1;
									Constpara_Tag[8]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[8]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[8];j++)  Constpara9[j]= Decryption.DataBuffer[20+4*i+j+cd];							
								}
								else if(Add==10)
								{
									Constpara_State[9]=1;
									Constpara_Tag[9]=Decryption.DataBuffer[18+4*i+cd];
									Constpara_Len[9]=Decryption.DataBuffer[19+4*i+cd];
									for(j=0;j<Constpara_Len[9];j++)  Constpara10[j]= Decryption.DataBuffer[20+4*i+j+cd];						
								}							
							}
							else if((Add>=0x8020)&&(Add<=0x821F))										//运行参数
							{
								Add-=0x8020;
								Runpara_State[Add]=1;
								Runpara_Tag[Add]=Decryption.DataBuffer[18+4*i+cd];
								Runpara_Len[Add]=Decryption.DataBuffer[19+4*i+cd];
								if(Runpara_Len[Add] <= 4)
								{
								  for(j=0;j<Runpara_Len[Add];j++)
									  run_send.runpara[Add].bytedata[j]=Decryption.DataBuffer[20+4*i+j+cd];										
							
								}
							}
							else if((Add>=0x8220)&&(Add<=0x85EF))										//定值
							{
								Add-=0x8220;
								Fixpara_State[Add]=1;
								Fixpara_Tag[Add]=Decryption.DataBuffer[18+4*i+cd];
								Fixpara_Len[Add]=Decryption.DataBuffer[19+4*i+cd];
								if(Fixpara_Len[Add] <= 4)
								{
								  for(j=0;j<Fixpara_Len[Add];j++)
									  fix_send.fixpara[Add].bytedata[j]=Decryption.DataBuffer[20+4*i+j+cd];											
							
								}
							}
							cd+=Decryption.DataBuffer[19+4*i+cd];
						}
						Super_Sign[12]=7;
					}
					else if((Decryption.DataBuffer[9]==ACT)&&((Decryption.DataBuffer[15]&0xC0)==0))   //参数固化
					{
//          SN_Num1=Decryption.DataBuffer[13] | (Decryption.DataBuffer[14]<<8);
            //固化一下，将接收值放入存储
						for(i=0;i<8;i++)
						{
							if(Msg_Safty1[i]!=Ter_Random1[i])	//验证随机数是否是我们发出去的,应用类型为3，没有时间，直接从随机数开始
							{
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//验证签名失败
								return;
							}
						}
						
						for(i=0;i<10;i++)
						{
							if(Constpara_State[i]==1)
							{
								switch(i)
								{
									case 0:

										break;
									case 1:

										break;
									case 2:

										break;
									case 3:

										break;										
									case 4:

										break;
									case 5:

										break;
									case 6:

										break;
									case 7:

										break;	
									case 8:

										break;
									case 9:

										break;										
								}
								Constpara_State[i]=0;
							}
						}
						for(i=0;i<Runpara_Num;i++)
						{
							if(Runpara_State[i]==1)
							{
							
								Runpara_State[i]=0;
							}							
						}
						for(i=0;i<Fixpara_Num;i++)
						{
							if(Fixpara_State[i]==1)
							{
								switch(i)
								{
									case 8:
                    ProtectSet.para.delay_fast_off_enable = fix_send.fixpara[8].bytedata[0];
										break;
									case 9:
                    ProtectSet.para.delay_fast_off = (u16)(fix_send.fixpara[9].floatdata * 100);//过流I段定值
										break;
									case 10:
                    ProtectSet.para.delay_fast_off_time = (u16)(fix_send.fixpara[10].floatdata * 100);
										break;
									case 11:
                    ProtectSet.para.delay_fast_off_voltage_lock = fix_send.fixpara[11].bytedata[0];
										break;										
									case 12:
                    ProtectSet.para.delay_fast_off_direction_lock = fix_send.fixpara[12].bytedata[0];
										break;
									case 13:
                    ProtectSet.para.max_current_enable = fix_send.fixpara[13].bytedata[0];
										break;
									case 14:
                    ProtectSet.para.max_current = (u16)(fix_send.fixpara[14].floatdata * 100);//过流II段定值
										break;
									case 15:
                    ProtectSet.para.max_current_time = (u16)(fix_send.fixpara[15].floatdata * 100);
										break;	
									case 16:
                    ProtectSet.para.max_current_voltage_lock = fix_send.fixpara[16].bytedata[0];
										break;
									case 17:
                    ProtectSet.para.max_current_direction_lock = fix_send.fixpara[17].bytedata[0];
										break;		
									case 18:
                    ProtectSet.para.max_current_after = (u16)(fix_send.fixpara[18].floatdata * 100);//过流后加速定值
										break;
									case 19:
                    ProtectSet.para.max_current_after_time = (u16)(fix_send.fixpara[19].floatdata * 100);
										break;
									case 20:
                    ProtectSet.para.max_load = (u16)(fix_send.fixpara[20].floatdata * 100);//过负荷告警定值
										break;
									case 21:
                    ProtectSet.para.max_load_time = (u16)(fix_send.fixpara[21].floatdata * 100);
										break;										
	                case 22:
                    ProtectSet.para.zero_fast_off_enable = fix_send.fixpara[22].bytedata[0];
										break;
									case 23:
                    ProtectSet.para.zero_fast_off = (u16)(fix_send.fixpara[23].floatdata * 100);//过流后加速定值
										break;
									case 24:
                    ProtectSet.para.zero_fast_off_time = (u16)(fix_send.fixpara[24].floatdata * 100);//过负荷告警定值
										break;	
	                case 25: //零序I段经方向闭锁
                    
										break;	
									case 26:
                    ProtectSet.para.zero_max_current_after = (u16)(fix_send.fixpara[26].floatdata * 100);//零序后加速定值
										break;
									case 27:
                    ProtectSet.para.zero_max_current_after_time = (u16)(fix_send.fixpara[27].floatdata * 100);
										break;	
	                case 28:
                    ProtectSet.para.once_reon_enable = fix_send.fixpara[28].bytedata[0];//重合闸投退
										break;	
	                case 29:
                    ProtectSet.para.reon_Un_enable = fix_send.fixpara[29].bytedata[0];//重合闸检无压投退
										break;	
	                case 30:
                    ProtectSet.para.reon_synchron_enable = fix_send.fixpara[30].bytedata[0];//重合闸检同期投退
										break;
									case 31:
                    ProtectSet.para.once_reon_time = (u16)(fix_send.fixpara[31].floatdata * 100);//重合闸时间
										break;									
		              case 32: //小电流接地告警投退
                    
										break;
									case 33:
                    ProtectSet.para.zero_max_voltage = (u16)(fix_send.fixpara[33].floatdata * 100);//零序电压定值
										break;
	               	case 34:
                    ProtectSet.para.max_voltage_enable = fix_send.fixpara[34].bytedata[0];//过压保护投退     
										break;
									case 35:
                    ProtectSet.para.max_voltage = (u16)(fix_send.fixpara[35].floatdata * 100);//过压保护定值
										break;
									case 36:
                    ProtectSet.para.max_voltage_time = (u16)(fix_send.fixpara[36].floatdata * 100);//过压保护延时
										break;
								  case 37:
                    ProtectSet.para.max_freq_enable = fix_send.fixpara[37].bytedata[0];//高频保护投退       
										break;
									case 38:
                    ProtectSet.para.max_freq = (u16)(fix_send.fixpara[38].floatdata * 100);//高频保护定值
										break;
									case 39:
                    ProtectSet.para.max_freq_time = (u16)(fix_send.fixpara[39].floatdata * 100);//高频保护延时
										break;
									case 40:
                    ProtectSet.para.low_freq_enable = fix_send.fixpara[40].bytedata[0];//低频保护投退
										break;
									case 41:
                    ProtectSet.para.low_freq = (u16)(fix_send.fixpara[41].floatdata * 100);//低频保护定值
										break;
									case 42:
                    ProtectSet.para.low_freq_time = (u16)(fix_send.fixpara[42].floatdata * 100);//低频保护延时
										break;
									default:	
							      break;	
								}									
							}
							Fixpara_State[i]=0;
						}
						Super_Sign[12]=8;		
					}
//						else if((Decryption.DataBuffer[9]==ACT)&&(Decryption.DataBuffer[15]>>6&0x01))  //取消预置
//						{
//								if(succ_flag_spi)
//							{
//								succ_flag_spi=0;
//                SN_Num1=Decryption.DataBuffer[13] | (Decryption.DataBuffer[14]<<8);
//	              //取消预置，按理应该清楚缓存
//								Super_Sign[12]=9;
//							}								
//						}
					else if((Decryption.DataBuffer[9]==DEACT)&&((Decryption.DataBuffer[15]&0x80)==0))       //终止,取消
					{
//          SN_Num1=Decryption.DataBuffer[13] | (Decryption.DataBuffer[14]<<8);
	          //固化一下，将接收值放入存储
						for(i=0;i<10;i++)							//清除固有参数预置
						{
							Constpara_State[i]=0;
							Constpara_Tag[i]=0;
							Constpara_Len[i]=0;
						}
						for(i=0;i<32;i++)							//清除运行参数预置
						{
							Runpara_State[i]=0;
							Runpara_Tag[i]=0;
							Runpara_Len[i]=0;
						}
						for(i=0;i<64;i++)							//清除定值预置
						{
							Fixpara_State[i]=0;
							Fixpara_Tag[i]=0;
							Fixpara_Len[i]=0;
						}
						Super_Sign[12]=10;										
					}										
				}		
				else					
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//验证签名失败
				break;		
		}
	}
	else if(jm_ok==2)	//报文正确接收但变位不正确
	{
		ca=0x80;
		Common_MW(USARTxChannel,ca);
		for(i=0;i<send_buffer_len;i++)
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],send_buffer[i]); //上一帧的备份
	}
}	

void Active_Upload1(u8 USARTxChannel)					
{
	u16 p_Reset=0,lc=0,i=0;	
	u8 Send_len=0,crc_yhf=0;
	u8 ca=0,spi_flag=0;
	u8 buf[12]={0};
	
	if((zz_ok==1)&&(wg_ok==1))//&&(flag_succ))
	{
//*********************************************************************************************
	/*---------------------------------初始化总召------------------------------*/
			if(Super_Sign[1]==1)
			{
				//Callend_Flag = 0;
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}		
				Send_len=GeneralCall_BefJM(0x0C,ca);	
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 		
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  			
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[1]=0;
				}	
			}
			else if(Super_Sign[1]==2)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=YX_BefJM(11+YX_Number,ca);							//生成遥信报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[1]=0;
				}
			}
			else if(Super_Sign[1]==3)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=YC_BefJM(11+YC_Number*5,ca); 						//生成遥测报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[1]=0;
				}
			}
			else if(Super_Sign[1]==4)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Conclude_GeneralCall_BefJM(0x0C,ca);	//生成总召结束报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Super_Sign[1]=0;		
					call_num=1;	//下次再召唤就不是总召了	
				}	
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[1]=0;
				}
			}
	/*---------------------------------遥控------------------------------*/
			else if(Super_Sign[2]==1)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Ctrl_General_BefJm(C_SC_NA_1,0x0C,ca,ACTCON);                   //生成遥控选择确认报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				spi_flag=0;
				while(spi_flag!=3)															//获取终端随机数
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);														  //帧头
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									//包含初始向量，终端随机数，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
					spi_flag=0;
					while(spi_flag!=3)															//
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														  //帧头
						for(i=0;i<4;i++)															//参数
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															//主站随机数
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															//主站随机数反码
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);															//应用类型
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);													//报文长度
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												//报文内容
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);															//随机数长度高字节
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);															//随机数长度低字节
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															//随机数
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi) 
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);
						Super_Sign[2]=0;
					}	
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
						Super_Sign[2]=0;
					}
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[2]=0;
				}
			}
			else if(Super_Sign[2]==2 || Super_Sign[2]==6)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				if(Super_Sign[2]==2)
				  Send_len=Ctrl_General_BefJm(C_SC_NA_1,0x0C,ca,ACTCON);               //生成遥控执行确认报文
				else if(Super_Sign[2]==6)
					Send_len=Ctrl_General_BefJm(C_DC_NA_1,0x0C,ca,ACTCON);               //生成遥控执行确认报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									//包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[2]=0;
				}			
			}
			else if(Super_Sign[2]==3 || Super_Sign[2]==7)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				if(Super_Sign[2]==3)
				  Send_len=Ctrl_General_BefJm(C_SC_NA_1,0x0C,ca,DEACTCON);        //生成遥控执行确认报文
				else if(Super_Sign[2]==7)
				  Send_len=Ctrl_General_BefJm(C_DC_NA_1,0x0C,ca,DEACTCON);        //生成遥控执行确认报文
			  BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 								     	    //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len	
				spi_flag=0;
				crc_yhf=0;
				if(YY_00_WR_SPI1(lc,Send_len)) 				                //加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Super_Sign[2]=0;
					yk_state=0;	//撤销之后恢复初始状态
				}	
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[2]=0;
				}			
			}
			else if(Super_Sign[2]==4 || Super_Sign[2]==8)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				if(Super_Sign[2]==4 && Object_addr == (0x6001+ Ctrl_offset))
				{
					if((control_jm.bit.QCS_RCS==1) && (Select101Addr==1)) //这里需要执行遥控
					{
						if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
						{
							RecordSwitchOn(TYPE_ACT_PAD_ON);// 操作合闸记录
							SwitchFlag = 1;
						}
					}
					else if((control_jm.bit.QCS_RCS==0) && (Select101Addr==1))
					{
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
						{
							RecordSwitchOff(TYPE_ACT_PAD_OFF);// 操作分闸记录
							SwitchFlag = 2;
						}
					}
					Send_len=Ctrl_General_BefJm(C_SC_NA_1,0x0C,ca,ACTTERM);          //生成遥控结束报文
			  }		
        else if(Super_Sign[2]==8 && Object_addr == (0x6001+ Ctrl_offset))
				{
					if((control_jm.bit.QCS_RCS==2) && (Select101Addr==1)) //这里需要执行遥控
					{
						if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
						{
							RecordSwitchOn(TYPE_ACT_PAD_ON);// 操作合闸记录
							SwitchFlag = 1;
						}
					}
					else if((control_jm.bit.QCS_RCS==1) && (Select101Addr==1))
					{
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
						{
							RecordSwitchOff(TYPE_ACT_PAD_OFF);// 操作分闸记录
							SwitchFlag = 2;
						}
					}
					Send_len=Ctrl_General_BefJm(C_DC_NA_1,0x0C,ca,ACTTERM);          //生成遥控结束报文
			  }					
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len				
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文	
					Select101Addr=0;
					Super_Sign[2]=0;
					yk_state=0;	//恢复初始状态
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[2]=0;
				}			
			}
	/*---------------------------------遥信上送-------------------------------------*/
			else if(Super_Sign[3])
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				if(Super_Sign[3] == 1)
				  Send_len=AutoSend_YX_TIME(10*yx_data_num+9,ca,yx_data_num); //生成带时标的遥信上送报文
				else if(Super_Sign[3] == 2)
					Send_len=AutoSend_YX(3*yx_data_num+9,ca,yx_data_num);       //生成不带时标遥信上送报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 				  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len	
				spi_flag=0;
				crc_yhf=0;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					if(Super_Sign[3] == 2)
					  yx_data_flag=0;//遥信缓存标志位清0
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文			
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[3]=0;
				}			
			}
	/*---------------------------------非初始化总召------------------------------*/
			else if(Super_Sign[4]==1)    													//非初始化总召确认，可以被打断
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=GeneralCall_BefJM(0x0C,ca);	
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 		
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1); 	
					Super_Sign_bak[4]=1;												
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[4]=0;
				}			
			}
			else if(Super_Sign[4]==2)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=YX_BefJM(11+YX_Number,ca);							//生成遥信报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Super_Sign_bak[4]=2;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[4]=0;
				}			
			}	
			else if(Super_Sign[4]==3)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=YC_BefJM(11+YC_Number*5,ca); 						//生成遥测报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Super_Sign_bak[4]=3;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[4]=0;
				}			
			}	
			else if(Super_Sign[4]==4)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Conclude_GeneralCall_BefJM(0x0C,ca);	//生成总召结束报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文	
					Super_Sign[4]=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[4]=0;
				}			
			}
	/*----------------------------------时钟同步/请求----------------------------------*/
			else if(Super_Sign[6]==1)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Cal_Time_BefJM(0x12,ca);
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  
          Callend_Flag = 1;					
					Super_Sign[6]=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[6]=0;
				}			
			}
			else if(Super_Sign[6]==2)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Read_Time_BefJM(0x12,ca);
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  
					Super_Sign[6]=0;				
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[6]=0;
				}			
			}
	/*----------------------------------遥测上送----------------------------------------*/
			else if(Super_Sign[7]==1)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}		
				Send_len=AutoSend_YC(7*yc_send.yc_num+9,ca);               //生成遥信上送报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									//包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len	
				spi_flag=0;
				crc_yhf=0;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Super_Sign[7]=0;
					yc_send.yc_state=0;					//遥测状态位清0
					yc_send.yc_num=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
				}			
			}
	/*----------------------------------复位确认----------------------------------*/
			else if(Super_Sign[8]==1)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Confirm_Reset_BefJM(0x0C,ca);
				Super_Sign[8]=2;
				p_Reset=2;													//复位原因，远方复位
				while(1)
				{
					if(Flash_Writeflag == 0)
					{
						SPI_Flash_Write((u8*)&p_Reset,FLASH_RESETYY_ADDR,2);	
						break;
					}
				}
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1); 														
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[8]=0;					
				}
			}
	/*----------------------------------目录召唤----------------------------------*/
			else if(Super_Sign[9]==1)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				if(Check_List_Name1())									//说明目录存在
				{
					Send_len=Confirm_List_BefJM(ca,0);				 //0表示成功，1表示失败
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文	
						Super_Sign[9]=0;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[9]=0;						
					}				
				}
				else                                                         
				{
					Send_len=Confirm_List_BefJM(ca,1);				 //0表示成功，1表示失败
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文	
						Super_Sign[9]=0;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
						Super_Sign[9]=0;						
					}
				}				
			}
	/*----------------------------------文件传输----------------------------------*/
			else if(Super_Sign[10]==1)    //读文件激活确认
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				if(Check_File_Name())
				{
					Send_len=Confirm_ReadDoc_BefJM(ca,0);												//读取文件激活确认，文件名，大小，长度等									
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
						Super_Sign_bak[10]=1;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[10]=0;						
					}				
				}
				else
				{
					Send_len=Confirm_ReadDoc_BefJM(ca,1);												//读取文件激活确认，文件名，大小，长度等									
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
						Super_Sign_bak[10]=1;
					}	
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[10]=0;							
					}				
				}			
			}
			else if(Super_Sign[10]==2)								//读文件传输
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Send_Doc_BefJM(ca);
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Super_Sign[10]=2;	
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[10]=0;						
				}
			}
			else if(Super_Sign[10]==3)										//写文件激活确认
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				if(Check_WriteFile_Name()&&Finish_Flag1==0)			//文件名正确
				{
					Send_len=Confirm_WriteDoc_BefJM(ca,0);	
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
						Super_Sign[10]=0;					
					}	
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[10]=0;							
					}				
				}
				else
				{
					Send_len=Confirm_WriteDoc_BefJM(ca,1);		
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
						Super_Sign[10]=0;	
					}	
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[10]=0;							
					}				
				}			
			}
	//		else if(Super_Sign[10]==4)							//接收数据，可以被打断
	//		{
	//			ca=0x80;																
	//			Common_MW(USARTxChannel,ca);					//固定帧长确认帧	
	//			Super_Sign[10]=0;				
	//		}
			else if(Super_Sign[10]==5)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Confirm_Send_Dat_BefJM(0x16,ca);	
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;	
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1); 
					Super_Sign[10]=0;					
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[10]=0;						
				}			
				Rec_Error_Flag1=0;		
			}
	/*----------------------------------升级----------------------------------*/		
			else if(Super_Sign[10]==6)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}			
				Send_len=Confirm_Update_BefJM(ca,ACTCON,0x80);   
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;				
				spi_flag=0;			
				while(spi_flag!=3)															//获取终端随机数
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);														  //帧头
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									//包含初始向量，终端随机数，报文内容，应用类型，报文长度，终端随机数长度    /Send_len	
					spi_flag=0;				
					while(spi_flag!=3)															//
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														  //帧头
						for(i=0;i<4;i++)															//参数
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															//主站随机数
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															//主站随机数反码
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);															//应用类型
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);													//报文长度
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												//报文内容
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);															//随机数长度高字节
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);															//随机数长度低字节
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															//随机数
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi) 
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);
						Super_Sign[10]=0;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[10]=0;							
					}					
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[10]=0;						
				}	
			}
			else if(Super_Sign[10]==7)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}			
				Send_len=Confirm_Update_BefJM(ca,ACTCON,0); 							
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;						
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //总召加密报文	
//******************存入升级数据长度时间信息****************************************************
					buf[0] = TimeNow.year;
					buf[1] = TimeNow.month;
					buf[2] = TimeNow.date;
					buf[3] = TimeNow.hour;
					buf[4] = TimeNow.minute;
					buf[5] = TimeNow.second;
					buf[6] = Rec_Doc_Num1 & 0x000000FF;
					buf[7] = (Rec_Doc_Num1 & 0x0000FF00) >> 8;
					buf[8] = (Rec_Doc_Num1 & 0x00FF0000) >> 16;
					buf[9] = (Rec_Doc_Num1 & 0xFF000000) >> 24;
					CS2BZ = 0;
					while(1)
					{
						if(Flash_Writeflag == 0)
						{
					    SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10); 	// 存入总字节数
							break;
						}
					}
//******************************************************************************
					//计算摘要值
					for(i=0;i<8;i++)
					{
						if(Msg_Safty1[6+i]!=Ter_Random1[i])	//验证随机数是否是我们发出去的
						{
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//验证随机数失败
							Super_Sign[10]=0;
							return;
						}
					}
					data_reset();
					GetBinFileMd5(decrypt);
					//验证摘要值
					lc=16+6+8+64;												//验证发送数据长度，摘要值+时间信息长度+随机数+签名值
					spi_flag=0;
					while(spi_flag!=3)															//验证签名的正确性
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);															//帧头
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;
						SPI2_ReadWrite(0x08);
						crc_yhf^=0x08;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(Msg_Safty_sj[78]);				//asKID:签名密钥索引
						crc_yhf^=Msg_Safty_sj[78];
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<16;i++) 
						{
							SPI2_ReadWrite(decrypt[i]);
							crc_yhf^=decrypt[i];
						}	
						for(i=0;i<78;i++) 
						{
							SPI2_ReadWrite(Msg_Safty_sj[i]);
							crc_yhf^=Msg_Safty_sj[i];
						}	
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)										//签名正确
					{
						succ_flag_spi=0;
						RecoverKZQBackupFlag =1 ;		      //说明升级文件接收完毕，可以进行升级了	
						Super_Sign[10]=0;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
						Super_Sign[10]=0;	
					}
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[10]=0;						
				}				
			}
			else if(Super_Sign[10]==8)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}				
				Send_len=Confirm_Update_BefJM(ca,DEACTCON,0); 									
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;						
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1); 
					Super_Sign[10]=0;				
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[10]=0;						
				}				
			}
	/*----------------------------------能量召唤----------------------------------*/
			else if(Super_Sign[11]==1)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Confirm_Enegy_BefJM(0x0C,ca); 							//生成电能数据召唤确认报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									    //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //总召加密报文
					Super_Sign_bak[11]=1;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[11]=0;				
				}				
			}
			else if(Super_Sign[11]==2)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Confirm_Enegy_Pluse_BefJM(0x33,ca);							//生成电能数据报文
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Super_Sign_bak[11]=2;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[11]=0;					
				}				
			}
			else if(Super_Sign[11]==3)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Conclude_Enegy_BefJM(0x0C,ca);	//生成电能召唤结束报文
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Super_Sign[11]=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[11]=0;					
				}				
			}
	/*----------------------------------参数读写----------------------------------*/	
			else if(Super_Sign[12]==1)								//读取定值区
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}					
				Send_len=Read_Area_BefJM(0x11,ca);	
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  
					Super_Sign[12]=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[12]=0;					
				}				
			}		
			else if(Super_Sign[12]==2)							//切换定值区
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Change_Area_BefJM(0x0D,ca);	
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1); 	
					Super_Sign[12]=0;				
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[12]=0;					
				}				
			}
			else if(Super_Sign[12]==3)							//读取全部参数和定值
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				if(params_flag==0)
				{
					params_flag=1;
					Send_len=Read_All_Constparas_BefJM(ca);
				}
				else if(params_flag==1)
				{
					params_flag=2;
					Send_len=Read_All_Runparas_BefJM(ca);
				}
				else if(params_flag==2)
				{
					params_flag=3;
					Send_len=Read_All_Fixparas_BefJM(ca);			
				}							
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;							
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1); 	
					if(params_flag==3)
					{
						params_flag=0;
						Super_Sign[12]=0;
					}					
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[12]=0;					
				}				
			}
			else if(Super_Sign[12]==4)													//读取部分参数和定值
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				
        if(fix_value.params_num)             //定值参数
				{
					if(run_value.params_num||const_value.params_num)
						Send_len=Read_FixvalueParas_BefJM(0,1);	
					else
					{
						params_flag=1;
						Send_len=Read_FixvalueParas_BefJM(0,0);	
					}
				}
				else if(run_value.params_num)             //运行参数
				{
					if(const_value.params_num)
						Send_len=Read_RunParas_BefJM(0,1);
					else
					{
						params_flag=1;
						Send_len=Read_RunParas_BefJM(0,0);
					}	
				}
				else if(const_value.params_num)            //固有参数
				{
					params_flag=1;
					Send_len=Read_ConstParas_BefJM(0,0);		
				}
					
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;							
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  
					if(params_flag==1)
					{
						params_flag=0;
						Super_Sign[12]=0;
					}			
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[12]=0;					
				}				
			}
			else if(Super_Sign[12]==7)												//参数预置
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}				
				Send_len=Pre_Set_BefJM(ca); 
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;																							
				spi_flag=0;			
				while(spi_flag!=3)															//获取终端随机数
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);														  //帧头
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									//包含初始向量，终端随机数，报文内容，应用类型，报文长度，终端随机数长度    /Send_len	
					spi_flag=0;				
					while(spi_flag!=3)															//
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														  //帧头
						for(i=0;i<4;i++)															//参数
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															//主站随机数
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															//主站随机数反码
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);															//应用类型
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);													//报文长度
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												//报文内容
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);															//随机数长度高字节
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);															//随机数长度低字节
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															//随机数
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi) 
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);	
						Super_Sign[12]=0;	
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[12]=0;						
					}					
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[12]=0;					
				}	
			}
			else if(Super_Sign[12]==8)
			{
				ProtectSet.para.fast_off_voltage_lock = 0;
				ProtectSet.para.fast_off_direction_lock = 0; 
				ProtectSet.para.delay_fast_off_voltage_lock = 0; 
				ProtectSet.para.delay_fast_off_direction_lock = 0; 
				ProtectSet.para.max_current_time_mode = 0; 
				ProtectSet.para.max_current_voltage_lock = 0;
				ProtectSet.para.max_current_direction_lock = 0;
				SetProtectFlag=1;
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}		
				Send_len=Act_Set_BefJM(ca,0x07,0x0);	
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  
					Super_Sign[12]=0;				
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[12]=0;					
				}				
			}
	//		else if(Super_Sign[12]==9)
	//		{
	//			if(FCB_Flag_Slave)	
	//			{
	//				FCB_Flag_Slave=0;
	//				ca=0xF3;
	//			}
	//			else
	//			{
	//				FCB_Flag_Slave=1;
	//				ca=0xD3;
	//			}						
	//			Send_len=Act_Set_BefJM(ca,0x07,0xC0);		          //取消预置
	//			lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
	//			if(YY_00_WR_SPI1(lc,Send_len)) 
	//			{
	//				succ_flag_spi=0;
	//				Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  
	//				Super_Sign[12]=0;	
	//			}												
	//		}
			else if(Super_Sign[12]==10)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Act_Set_BefJM(ca,0x09,0x40);		          //取消预置	
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;							
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);	
					Super_Sign[12]=0;					
				}	
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[12]=0;					
				}				
			}
	/*----------------------------------链路测试----------------------------------*/
			else if(Super_Sign[14]==1)
			{
				if(FCB_Flag_Slave)	
				{
					FCB_Flag_Slave=0;
					ca=0xD3;
				}
				else
				{
					FCB_Flag_Slave=1;
					ca=0xF3;
				}	
				Send_len=Test_Mirror_BefJM(0x12,ca);
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 			
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  	
					Super_Sign[14]=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[14]=0;					
				}				
			}
	}
}

/*---------------------------------*/
//----------------明文处理
void ProtocolResolve1_MW(u8 USARTxChannel)	
{
	u16 lc=0;	
	u8 Send_len=0,ca=0;
	if((zz_ok==1)&&(wg_ok==1))
	{
		ADDR= Decryption.DataBuffer[2]+(Decryption.DataBuffer[3]<<8);
		switch(Decryption.DataBuffer[1])
		{
	/*-------------------------------初始化----------------------------------*/	
			case 0x42:													//心跳测试
				ca=0x80;
				Common_MW(USARTxChannel,ca);					
				break;
			case 0x49:													//请求链路状态，主站为启动站
				flag_succ=0;											//每次初始化都要清0，不然第二次初始化不会进入初始化结束响应
				Super_Sign[0]=1;
				ca=0x8B;
				Common_MW(USARTxChannel,ca);			//链路状态完好
				break;
			case 0x40:													//复位远方链路
				Super_Sign[0]=1;
				ca=0x80;
				Common_MW(USARTxChannel,ca);			//链路复位确认
				flag_succ=0;
				os_dly_wait (30);
				ca=0xC9;
				Common_MW(USARTxChannel,ca);			//请求链路状态，终端为启动站	
				break;
			case 0x0B:												  //链路状态完好，主站回复
				Super_Sign[0]=1;
				call_num=0;
				yk_state=0;
				flag_succ=0;
				jm=0;
				ca=0xC0;
				Common_MW(USARTxChannel,ca);			//复位远方链路，复位主站	
				break;
			case 0x00:													//链路复位确认，主站确认，初始化结束，后面即为等待召唤	
				if(flag_succ==0)												//未完成就进入，完成则跳过
				{
					FCB_Flag_Slave=0;								//这个认为是终端首次使能FCV位，FCB位从0开始有效，后面开始翻转
					ca=0xD3;					
					Send_len=Conclude_Initinal_BefJM(0x0C,ca);	
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 		
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  
						Super_Sign[0]=0;
					}	
				}
				else
				{
					delay_time_101=0;												    //每次进入任务4函数都会自加1
					send_buffer_len=0;													//接收到主站的确认帧就把上一帧的备份删掉
					timer_enable=0;
					resend_num=0;
	/*-------------------------------初始化总召唤----------------------------------*/	
	/*------这个可以不放入active_upload函数，因为是初始化总召无法被打断------*/
	/*----但是为了保证在active_upload函数中不发送别的，这几个选项要加进去----*/
					if(Super_Sign[1]==1)
					{
						Super_Sign[1]=2;	
					}
					else if(Super_Sign[1]==2)
					{
						Super_Sign[1]=3;
					}
					else if(Super_Sign[1]==3)
					{
						Super_Sign[1]=4;
					}
	/*-------------------------------遥控----------------------------------*/			
	/*------------------遥控的优先级也比较高，不足以打断-------------------*/					
					else if(Super_Sign[2]==2)
					{	
	//执行结束之后生成遥控结束报文
						Super_Sign[2]=4;
					}
					else if(Super_Sign[2]==6)
					{	
	//执行结束之后生成遥控结束报文
						Super_Sign[2]=8;
					}
					else if(Super_Sign[3]==1)
					{	
						Super_Sign[3] = 2;
					}
					else if(Super_Sign[3] == 2)
					{
            Super_Sign[3] = 0;		
					}
	/*-------------------------------非初始化总召唤----------------------------------*/	
					else if(Super_Sign[4]==1)
					{
						if(Super_Sign_bak[4]==1)	//说明执行过了
						{
							Super_Sign_bak[4]=0;
							Super_Sign[4]=2;
						}	
					}
					else if(Super_Sign[4]==2)
					{
						if(Super_Sign_bak[4]==2)	//说明执行过了
						{
							Super_Sign_bak[4]=0;
							Super_Sign[4]=3;
						}
					}
					else if(Super_Sign[4]==3)
					{
						if(Super_Sign_bak[4]==3)	//说明执行过了
						{
							Super_Sign_bak[4]=0;
							Super_Sign[4]=4;
						}
					}
					
	/*-------------------------------复位----------------------------------*/
					else if(Super_Sign[8]==2)								//复位确认得到回复之后再进行复位
					{
					  __set_FAULTMASK(1);
						NVIC_SystemReset();			
					}
	/*-------------------------------文件传输----------------------------------*/
					else if(Super_Sign[10]==1)
					{
						if(Super_Sign_bak[10]==1)	//说明执行过了
						{
							Super_Sign_bak[10]=0;
							Super_Sign[10]=2;
						}										
					}
	/*-------------------------------能量----------------------------------*/
					else if(Super_Sign[11]==1)
					{
						if(Super_Sign_bak[11]==1)
						{
							Super_Sign_bak[11]=0;
							Super_Sign[11]=2;
						}
					}
					else if(Super_Sign[11]==2)
					{
						if(Super_Sign_bak[11]==2)
						{
							Super_Sign_bak[11]=0;
							Super_Sign[11]=3;
						}						
					}
					else
					{
					}
				}
				break;
		}	
	}
	else
		Re_error_msg(USARTxChannel,0x01,0x1F,0x9107);
}	

void IEC_104_Resolve_JM(u8 USARTxChannel)					//处理从spi接收到的数据，凡是需要解密的都是I帧数据，U型数据和S型数据不加密
{
	u16 Add=0,lc=0,i=0;	
	u8 Send_len=0,Params_Num=0,ok_flag=0,j=0,crc_yhf=0,sum=0;
	u8 spi_flag=0;
	u8 cd=0;

	if((Decryption.DataBuffer[0]==0x68)&&(Decryption.DataBuffer[1]>4)&&(Decryption.DataBuffer[1]<=253)&&((Decryption.DataBuffer[2]&1)==0)&&((Decryption.DataBuffer[4]&1)==0))//I型
	{
		Send_NumBak=((((u16)Decryption.DataBuffer[3])<<8)&0xFF00)|(Decryption.DataBuffer[2]&0xFF);		
		Send_NumBak>>=1;																//主机发来的发送序列号，主机认为自己发送的次数
	  //这个其实不需要，我们不需要去判断这个正确与否，这个是主站认为自己发送的个数，终端只需要接收序列号
		Recv_NumBak=((((u16)Decryption.DataBuffer[5])<<8)&0xFF00)|(Decryption.DataBuffer[4]&0xFF);			//主机发来的接收序列号，主机认为自己接收的次数
		Recv_NumBak>>=1;
		//如果Send_Num-Recv_NumBak>12则停止发送，此时BackstageProtocol0中存放的是当前收到的报文
		Recv_Num++; 																										//本机接收加1，本机实际接收到的I帧数据次数
		
		cd=Send_Num-Recv_NumBak;	//发送未被确认的总数，这个数字应该是小于等于queen_sum的
		if(cd>=12)
		{
			diff_flag=1;	//说明未确认帧数达到k值上限，不再发送
		}
		else if(cd==0)	//全部确认，没有未确认帧
		{
			diff_flag=0;
			ok_flag=1;   												//如果是I帧，则置一
		}
		else
		{
			diff_flag=0;
			ok_flag=1;   												//如果是I帧，则置一
		}
		cd=0;
		Asdu_addr=(u16)Decryption.DataBuffer[11]<<8|Decryption.DataBuffer[10]; //ASDU地址
		Informat_addr[0]=Decryption.DataBuffer[12];	  //信息地址
		Informat_addr[1]=Decryption.DataBuffer[13];
		Informat_addr[2]=Decryption.DataBuffer[14];
	}
	else
	{
		Re_error_msg(USARTxChannel,0x01,0x1F,0x9110);	
		return;
	}
	if(ok_flag==1)	
	{
		switch(Decryption.DataBuffer[6])
		{
			case C_IC_NA_1:																			//总召唤 
				if(call_num1==0)
				{
					Send_len=GeneralCall_BefJM(0x0C,0);							//生成总召激活报文
					BW_Before_JM1[1]=Send_len-2;
					lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
					if(YY_00_WR_SPI1(lc,Send_len)) 
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //总召加密报文
						Send_Num++;
					}	
					else
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);		
		//下面发送总召遥信和遥测报文，首先要生成遥信和遥测报文
					Send_len=YX_BefJM(13+YX_Number,0);								//生成遥信报文
					BW_Before_JM1[1]=Send_len-2;
					lc=16+4+Send_len;
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
						Send_Num++;
					}	
					else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);						
					Send_len=YC_BefJM(13+YC_Number*3,0); 						//生成遥测报文
					BW_Before_JM1[1]=Send_len-2;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
						Send_Num++;
					}	
					else
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);						
		//结束总召唤
					Send_len=Conclude_GeneralCall_BefJM(0x0C,0);	//生成总召结束报文
					BW_Before_JM1[1]=Send_len-2;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
						Send_Num++;
						call_num1=1;	//下次不再进入这个判断语句
					}	
					else
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);						
				}
				else
				{
					Super_Sign[4]=1;
				}
				break;
			case C_CS_NA_1: 																		//对时
				if(Decryption.DataBuffer[8]==ACT)					//时钟同步
				{
					TimeNow.msec    = ((u16)(Decryption.DataBuffer[15] | (u16)(Decryption.DataBuffer[16]<<8))%1000)&0x3FF;
					TimeNow.second  = ((u16)(Decryption.DataBuffer[15] | (u16)(Decryption.DataBuffer[16]<<8))/1000)&0x3F;  
					TimeNow.minute  = Decryption.DataBuffer[17]&0x3F;
					TimeNow.hour    = Decryption.DataBuffer[18]&0x1F;
					TimeNow.date    = Decryption.DataBuffer[19]&0x1F;
					TimeNow.month   = Decryption.DataBuffer[20]&0x0F;
					TimeNow.year    = Decryption.DataBuffer[21]&0x7F;
					correct_time();
	//生成时钟同步确认报文
					Super_Sign[6]=1;
				}
				else if(Decryption.DataBuffer[8]==REQ)		//时钟请求
				{
	//生成时钟请求确认报文
					Super_Sign[6]=2;				
				}
				break;
			case C_RP_NA_1:     																//复位进程命令
				Super_Sign[8]=1;
				break;
			case C_CI_NA_1:     								 								//电能脉冲召唤	
				Super_Sign[11]=1;			
				break;	

			case C_SC_NA_1:																			//双点遥控改成单点遥控
			case C_DC_NA_1:																			//双点点遥控
			  if(Decryption.DataBuffer[8]==ACT)   //判断S/E位是0还是1，在SCO(单命令),DCO(双命令)的最高位（第8位），
				{
					if(Decryption.DataBuffer[15]&0x80)	//遥控选择
					{
						switch(YY_Style1)
						{
							case 1:	//签名
							case 4:	//签名
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
					}
					else	//遥控执行
					{
						switch(YY_Style1)
						{
							case 3:	//签名
							case 5:
							case 6:
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}			
					}
				}
				
				lc=BW_Len1+MsgSafty_Len1-1;												//验证发送数据长度，报文长度+时间信息长度+签名值
				spi_flag=0;
				while(spi_flag!=3)															//验证签名的正确性
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															//帧头
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:签名密钥索引
					crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
					SPI2_ReadWrite((u8)(lc>>8)&0xFF);
					crc_yhf^=(u8)(lc>>8)&0xFF;
					SPI2_ReadWrite((u8)lc&0xFF);
					crc_yhf^=(u8)lc&0xFF;
					for(i=0;i<BW_Len1;i++) 
					{
						SPI2_ReadWrite(Decryption.DataBuffer[i]);
						crc_yhf^=Decryption.DataBuffer[i];
					}	
					for(i=0;i<MsgSafty_Len1-1;i++) 
					{
						SPI2_ReadWrite(Msg_Safty1[i]);
						crc_yhf^=Msg_Safty1[i];
					}	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					control_jm.byte=Decryption.DataBuffer[15];
					if(Decryption.DataBuffer[8]==ACT)  // 激活
					{
						if(control_jm.bit.S_E == 1)      //判断S/E位是0还是1，在SCO(单命令),DCO(双命令)的最高位（第8位）
						{
							if(yk_state==0 && Select101Addr==0)	//初始状态
							{
								Select101Addr=1;
								Send_len=Ctrl_General_BefJm(Decryption.DataBuffer[6],0x0C,0,ACTCON);                  //生成遥控选择确认报文
								BW_Before_JM1[1]=Send_len-2;							
								spi_flag=0;
								while(spi_flag!=3)															//获取终端随机数
								{
									SSN_L();	
									SPI2_ReadWrite(0x55);														  //帧头
									for(i=0;i<6;i++)
									{
										SPI2_ReadWrite(Get_Ter_Random[i]);	
										crc_yhf^=Get_Ter_Random[i];
									}
									crc_yhf=~crc_yhf;
									SPI2_ReadWrite(crc_yhf);	
									crc_yhf=0;
									SSN_H();											   //拉高片选
									spi_flag=Ter_WriteRead_Spi1();
								}	
								if(succ_flag_spi) 
								{
									succ_flag_spi=0;
									for(i=0;i<8;i++)
										Ter_Random1[i]=Spi_Recv1[i];
									lc=16+8+4+Send_len;						 									//包含初始向量，终端随机数，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
									spi_flag=0;								
									while(spi_flag!=3)															//
									{
										SSN_L();	
										SPI2_ReadWrite(0x55);														  //帧头
										for(i=0;i<4;i++)															//参数
										{
											SPI2_ReadWrite(Send_JM[i]);	
											crc_yhf^=Send_JM[i];
										}
										SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
										crc_yhf^=(u8)(lc>>8)&0xFF;
										SPI2_ReadWrite((u8)lc&0xFF);
										crc_yhf^=(u8)lc&0xFF;
										for(i=0;i<8;i++) 															//主站随机数
										{
											SPI2_ReadWrite(Zz_Random1[i]);
											crc_yhf^=Zz_Random1[i];
										}	
										for(i=0;i<8;i++) 															//主站随机数反码
										{
											SPI2_ReadWrite(~Zz_Random1[i]);
											crc_yhf^=(~Zz_Random1[i]);
										}	
										SPI2_ReadWrite(0x02);															//应用类型
										crc_yhf^=0x02;
										SPI2_ReadWrite(Send_len);													//报文长度
										crc_yhf^=Send_len;
										for(i=0;i<Send_len;i++)												//报文内容
										{
											SPI2_ReadWrite(BW_Before_JM1[i]);	
											crc_yhf^=BW_Before_JM1[i];
										}
										SPI2_ReadWrite(0x00);															//随机数长度高字节
										crc_yhf^=0x00;		
										SPI2_ReadWrite(0x08);															//随机数长度低字节
										crc_yhf^=0x08;		
										for(i=0;i<8;i++)															//随机数
										{
											SPI2_ReadWrite(Ter_Random1[i]);	
											crc_yhf^=Ter_Random1[i];
										}
										crc_yhf=~crc_yhf;
										SPI2_ReadWrite(crc_yhf);	
										crc_yhf=0;
										SSN_H();											   //拉高片选
										spi_flag=Ter_WriteRead_Spi1();
									}	
									if(succ_flag_spi) 
									{
										succ_flag_spi=0;
										Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);
										Send_Num++;
										yk_state=1;	//选择状态
									}
									else
										Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);									
								}
								else
									Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);								
							}
							else
							{
								Select101Addr=0;
								yk_state=0;	//恢复初始状态
							}
						}
						else	if(control_jm.bit.S_E == 0)		//遥控执行
						{
							for(i=0;i<8;i++)
							{
								if(Msg_Safty1[6+i]!=Ter_Random1[i])	//验证随机数是否是我们发出去的
								{
									Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//验证随机数失败
									return;
								}
							}
							if(yk_state==1 && Select101Addr==1)
							{
								Send_len=Ctrl_General_BefJm(Decryption.DataBuffer[6],0x0C,0,ACTCON);  //生成遥控执行确认报文
								BW_Before_JM1[1]=Send_len-2;
								lc=16+4+Send_len;						 									         //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
								spi_flag=0;
								crc_yhf=0;
								if(YY_00_WR_SPI1(lc,Send_len)) 				                 //加密遥信报文成功
								{
									succ_flag_spi=0;
									Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);   //发送遥信加密报文
									Send_Num++;
									yk_state=3;	//遥控执行，执行结束之后生成遥控结束报文
									
									if(Informat_addr[0] == (0x01+ Ctrl_offset))
									{
										if(Decryption.DataBuffer[6] == C_SC_NA_1)          // 单点遥控命令
									  {
											if((control_jm.bit.QCS_RCS==1) && (Select101Addr==1))
											{
												if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
												{
													RecordSwitchOn(TYPE_ACT_PAD_ON);// 操作合闸记录
													SwitchFlag = 1;
												}
											}
											else if((control_jm.bit.QCS_RCS==0) && (Select101Addr==1))
											{
												if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
												{
													RecordSwitchOff(TYPE_ACT_PAD_OFF);// 操作分闸记录
													SwitchFlag = 2;
												}
											}
									  }
										else if(Decryption.DataBuffer[6] == C_DC_NA_1)     // 双点遥控命令
									  {
											if((control_jm.bit.QCS_RCS==2) && (Select101Addr==1))
											{
												if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
												{
													RecordSwitchOn(TYPE_ACT_PAD_ON);// 操作合闸记录
													SwitchFlag = 1;
												}
											}
											else if((control_jm.bit.QCS_RCS==1) && (Select101Addr==1))
											{
												if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
												{
													RecordSwitchOff(TYPE_ACT_PAD_OFF);// 操作分闸记录
													SwitchFlag = 2;
												}
											}
									  }
								  }										
									Send_len=Ctrl_General_BefJm(Decryption.DataBuffer[6],0x0C,0,ACTTERM);         //生成遥控结束报文
									BW_Before_JM1[1]=Send_len-2;
									lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
									spi_flag=0;
									crc_yhf=0;	
									if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
									{
										succ_flag_spi=0;
										Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
										Send_Num++;
										Select101Addr=0;
										yk_state=0;	//遥控执行结束
										Super_Sign[2]=0;
									}
									else
										Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);									
								}	
								else
									Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);								
							}
							else
							{
								yk_state=0;
								Select101Addr=0;
							}
						}
					}
					else if(Decryption.DataBuffer[8]==DEACT)  //这里需要执行遥控取消操作，遥控撤销报文没有随机数
					{
						if(yk_state==1)
						{
							Select101Addr=0;	
							Send_len=Ctrl_General_BefJm(Decryption.DataBuffer[6],0x0C,0,DEACTCON);             //生成遥控执行确认报文
							BW_Before_JM1[1]=Send_len-2;
							lc=16+4+Send_len;						 									//包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len	
							spi_flag=0;
							crc_yhf=0;
							if(YY_00_WR_SPI1(lc,Send_len)) 				//加密遥信报文成功
							{
								succ_flag_spi=0;
								Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
								Send_Num++;
								yk_state=0;
								Super_Sign[2]=0;
							}	
							else
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);							
						}
						else
						{
							yk_state=0;
						}
					}					
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);			//验签失败
				break;
	//		case F_SC_NA_1:												//召唤目录,这里不一定是召唤目录，也有可能是文件传输
	//			break;
			case F_FR_NA_1:		  									//文件传输,包括：文件目录召唤，读写文件激活，读写文件传输，主要依靠操作标识进行识别
				if(Decryption.DataBuffer[16]==1)								//召唤文件目录
				{
					if(Decryption.DataBuffer[21]==0)								//默认目录,COMTRADE
					{
						List_Num1=8;
						List_Name1[0]=0x43;
						List_Name1[1]=0x4F;
						List_Name1[2]=0x4D;
						List_Name1[3]=0x54;
						List_Name1[4]=0x52;
						List_Name1[5]=0x41;
						List_Name1[6]=0x44;
						List_Name1[7]=0x45;
					}
					else
					{
						List_Num1=Decryption.DataBuffer[21];							//召唤的目录名长度
						for(i=0;i<List_Num1;i++)
						{
							List_Name1[i]=Decryption.DataBuffer[22+i];			//ASC的形式存储的目录名
						}				
					}
					if(Decryption.DataBuffer[22+List_Num1])					//所要求的文件在此时间段内
					{
						List_Name1[49]=1;
						for(i=0;i<7;i++)
						{
//							Start_Time[i]=Decryption.DataBuffer[23+List_Num+i]; //时间段区间
//							End_Time[i]=Decryption.DataBuffer[30+List_Num+i];
						}				
					}
					else
						List_Name1[49]=0;
					Super_Sign[9]=1;
				}
				else if(Decryption.DataBuffer[16]==3)					       	//读文件激活
				{
					List_Num1=Decryption.DataBuffer[17];									//要读取的文件名长度
					for(i=0;i<List_Num1;i++)													
					{
						List_Name1[i]=Decryption.DataBuffer[18+i];			   	//ASC的形式存储文件名，包括扩展名，即后缀，比如.dat
					}
					Super_Sign[10]=1;
				}
				else if(Decryption.DataBuffer[16]==6)						//读文件数据传输确认
				{
					Super_Sign[10]=0;	
				}
				else if((Decryption.DataBuffer[8]==0x06)&&(Decryption.DataBuffer[16]==7))					//写文件激活
				{
					List_Num1=Decryption.DataBuffer[17];									    //要写入的文件名的长度
					for(i=0;i<List_Num1;i++)
					{
						List_Name1[i]=Decryption.DataBuffer[18+i];			  		 	//ASC的形式存储文件名
					}	
					Rec_Doc_Num1	|= (((u32)Decryption.DataBuffer[22+List_Num1])&0x000000FF);		
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[23+List_Num1])<<8)&0x0000FF00);	
					Rec_Doc_Num1	|= ((((u32)Decryption.DataBuffer[24+List_Num1])<<16)&0x00FF0000);		
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[25+List_Num1])<<24)&0xFF000000);	
					if(Rec_Doc_Num1>0x80000)	Finish_Flag1=3;											 			//长度检查，不能大于512k
				  //if(Check_List_Name(List_Name)) 	Finish_Flag=2;//文件名检查，读写文件中，主要是读取，写文件应该只有软件升级才会用到
					//写文件确认激活	
					lc=BW_Len1+MsgSafty_Len1-1;												//验证发送数据长度，报文长度+时间信息长度+签名值
					spi_flag=0;
					while(spi_flag!=3)															//验证签名的正确性
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);															//帧头
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;
						SPI2_ReadWrite(0x08);
						crc_yhf^=0x08;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:签名密钥索引
						crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<BW_Len1;i++) 
						{
							SPI2_ReadWrite(Decryption.DataBuffer[i]);
							crc_yhf^=Decryption.DataBuffer[i];
						}	
						for(i=0;i<MsgSafty_Len1-1;i++) 
						{
							SPI2_ReadWrite(Msg_Safty1[i]);
							crc_yhf^=Msg_Safty1[i];
						}	
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi) 																
					{
						succ_flag_spi=0;
						Super_Sign[10]=3;
					}
					else
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);			//验签失败	
				}
				else if((Decryption.DataBuffer[8]==0x05)&&(Decryption.DataBuffer[16]==9)) //写文件传输
				{
					sum=0;
					Rec_Error_Flag1=0;
					Receive_Doc_101();			//实际使用																	//每次接受到主站的写文件数据都会进入
					for(i=0;i<(Decryption.DataBuffer[1]-25);i++)												//将本次接收数据存储起来
					{
						ucaFlashBuf[i]=Decryption.DataBuffer[26+i];												//每次接收的数据先存到pbuffer[]中去
						sum+=Decryption.DataBuffer[26+i];
					}
					if(sum!=Decryption.DataBuffer[Decryption.DataBuffer[1]+1])		//如果校验值不对，错误标志位置一
					{
						Rec_Error_Flag1=2;
					}
					Super_Sign[10]=4;	//实际使用
					if((Decryption.DataBuffer[25]==0)||(Rec_Error_Flag1))   //无后续或者出错
					{
						ulFlieLength=Decryption.DataBuffer[1]-25;
						Super_Sign[10]=5;	
					}
				}
				break;	
			case F_SR_NA_1:												//软件升级
				if(Decryption.DataBuffer[8]==0x06)
				{
					if(Decryption.DataBuffer[15]&0x80)							//判断S/E，1升级启动，
					{
						switch(YY_Style1)
						{
							case 0:	//签名
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
						
						lc=BW_Len1+MsgSafty_Len1-1;												//验证发送数据长度，报文长度+时间信息长度+签名值
						spi_flag=0;
						while(spi_flag!=3)															//验证签名的正确性
						{
							SSN_L();	
							SPI2_ReadWrite(0x55);															//帧头
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;
							SPI2_ReadWrite(0x08);
							crc_yhf^=0x08;
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;
							SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:签名密钥索引
							crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
							SPI2_ReadWrite((u8)(lc>>8)&0xFF);
							crc_yhf^=(u8)(lc>>8)&0xFF;
							SPI2_ReadWrite((u8)lc&0xFF);
							crc_yhf^=(u8)lc&0xFF;
							for(i=0;i<BW_Len1;i++) 
							{
								SPI2_ReadWrite(Decryption.DataBuffer[i]);
								crc_yhf^=Decryption.DataBuffer[i];
							}	
							for(i=0;i<MsgSafty_Len1-1;i++) 
							{
								SPI2_ReadWrite(Msg_Safty1[i]);
								crc_yhf^=Msg_Safty1[i];
							}	
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);	
							crc_yhf=0;
							SSN_H();											   //拉高片选
							spi_flag=Ter_WriteRead_Spi1();
						}	
						if(succ_flag_spi) 																		
						{
							succ_flag_spi=0;
							Super_Sign[10]=6;
						}
						else
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);			//验签失败
					}
					else																									//0升级结束
					{
						Super_Sign[10]=7;										
					}
				}
				else if(Decryption.DataBuffer[8]==0x08)					//撤销升级
				{
					Super_Sign[10]=8;   							
				}		
				break;
			case C_RR_NA_1:                     	//读当前定值区
				Super_Sign[12]=1;		
				break;	
			case C_SR_NA_1:                     	            //切换定值区
				switch(YY_Style1)
				{
					case 0:	//签名
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
						return;
					default:
						break;
				}
						
				lc=BW_Len1+MsgSafty_Len1-1;											//验证发送数据长度，报文长度+时间信息长度+签名值
				spi_flag=0;
				while(spi_flag!=3)															//验证签名的正确性
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															//帧头
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:签名密钥索引
					crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
					SPI2_ReadWrite((u8)(lc>>8)&0xFF);
					crc_yhf^=(u8)(lc>>8)&0xFF;
					SPI2_ReadWrite((u8)lc&0xFF);
					crc_yhf^=(u8)lc&0xFF;
					for(i=0;i<BW_Len1;i++) 
					{
						SPI2_ReadWrite(Decryption.DataBuffer[i]);
						crc_yhf^=Decryption.DataBuffer[i];
					}	
					for(i=0;i<MsgSafty_Len1-1;i++) 
					{
						SPI2_ReadWrite(Msg_Safty1[i]);
						crc_yhf^=Msg_Safty1[i];
					}	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 																
				{
					succ_flag_spi=0;
					SN_Num1=(u16)Decryption.DataBuffer[15]&0xFF;
					SN_Num1|=(u16)Decryption.DataBuffer[16]<<8&0xFF00;
					Super_Sign[12]=2;	
	//需要切换到SN_Num区
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//验证签名失败
				break;	
			case C_RS_NA_1:                     											//读参数和定值
	//			SN_Num=(u16)Decryption.DataBuffer[12]&0xFF;
	//			SN_Num=(u16)Decryption.DataBuffer[13]<<8&0xff00;	
				Params_total = 0;
				fix_value.params_num = 0;
				run_value.params_num = 0;
				const_value.params_num = 0;
				
				if((Decryption.DataBuffer[7]&0x7F)==0)								//读取全部参数
				{
					Super_Sign[12]=3;	
				}
				else
				{	
					for(i=0;i<(Decryption.DataBuffer[7]&0x7F);i++)
					{
						if((Decryption.DataBuffer[15+3*i]==0x82)&&(Decryption.DataBuffer[14+3*i]>=0x20))     //定值参数
						{
							fix_value.params_addr[fix_value.params_num++]=Decryption.DataBuffer[14+3*i];//只取低地址
							Params_total++;
						}
						else if((Decryption.DataBuffer[15+3*i]==0x80)&&(Decryption.DataBuffer[14+3*i]>=0x20))	//运行参数			
						{
							run_value.params_addr[run_value.params_num++]=Decryption.DataBuffer[14+3*i];//只取低地址
							Params_total++;
						}
						else if((Decryption.DataBuffer[15+3*i]==0x80)&&(Decryption.DataBuffer[14+3*i]<0x20))  //固有参数
						{
							const_value.params_addr[const_value.params_num++]=Decryption.DataBuffer[14+3*i];//只取低地址
							Params_total++;
						}
					}
					Super_Sign[12]=4;	
				}
				break;	
			case C_WS_NA_1:                     											//写参数和定值
				if((Decryption.DataBuffer[8]==0x06)&&(Decryption.DataBuffer[14]&0x80))	//参数预置
				{	
					switch(YY_Style1)
					{
						case 0:	//签名
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
							return;
						default:
							break;
					}
				}
				else if((Decryption.DataBuffer[8]==0x06)&&((Decryption.DataBuffer[14]&0xc0)==0))   //参数固化
				{
					switch(YY_Style1)
					{
						case 1:	//签名
						case 2:	//签名
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
							return;
						default:
							break;
					}
				}
					
				lc=BW_Len1+MsgSafty_Len1-1;												//验证发送数据长度，报文长度+时间信息长度+签名值
				spi_flag=0;
				while(spi_flag!=3)															//验证签名的正确性
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															//帧头
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:签名密钥索引
					crc_yhf^=Msg_Safty1[MsgSafty_Len1-1];
					SPI2_ReadWrite((u8)(lc>>8)&0xFF);
					crc_yhf^=(u8)(lc>>8)&0xFF;
					SPI2_ReadWrite((u8)lc&0xFF);
					crc_yhf^=(u8)lc&0xFF;
					for(i=0;i<BW_Len1;i++) 
					{
						SPI2_ReadWrite(Decryption.DataBuffer[i]);
						crc_yhf^=Decryption.DataBuffer[i];
					}	
					for(i=0;i<MsgSafty_Len1-1;i++) 
					{
						SPI2_ReadWrite(Msg_Safty1[i]);
						crc_yhf^=Msg_Safty1[i];
					}	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 																		
				{
					succ_flag_spi=0;
					if((Decryption.DataBuffer[8]==0x06)&&(Decryption.DataBuffer[14]&0x80))	//参数预置
					{
	//参数预置，缓存一下，固化则需要，取消则不要
						Params_Num=Decryption.DataBuffer[7]&0x7F;																		//要预置几个参数
	//						SN_Num=(u16)Decryption.DataBuffer[13]<<8|Decryption.DataBuffer[12]; //区号
						for(i=0;i<Params_Num;i++)
						{
							Add=(u16)Decryption.DataBuffer[16+5*i+cd]<<8|Decryption.DataBuffer[15+5*i+cd];
							if(Add>0x8000 && Add<0x8020)               //固有参数
							{
								Add-=0x8000;														 //判断参数类型
								if(Add==1)
								{
									Constpara_State[0]=1;
									Constpara_Tag[0]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[0]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[0];j++)  Constpara1[j]= Decryption.DataBuffer[20+5*i+j+cd];
								}
								else if(Add==2)
								{
									Constpara_State[1]=1;
									Constpara_Tag[1]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[1]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[1];j++)  Constpara2[j]= Decryption.DataBuffer[20+5*i+j+cd];							
								}
								else if(Add==3)
								{
									Constpara_State[2]=1;
									Constpara_Tag[2]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[2]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[2];j++)  Constpara3[j]= Decryption.DataBuffer[20+5*i+j+cd];						
								}
								else if(Add==4)
								{
									Constpara_State[3]=1;
									Constpara_Tag[3]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[3]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[3];j++)  Constpara4[j]= Decryption.DataBuffer[20+5*i+j+cd];							
								}
								else if(Add==5)
								{
									Constpara_State[4]=1;
									Constpara_Tag[4]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[4]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[4];j++)  Constpara5[j]= Decryption.DataBuffer[20+5*i+j+cd];						
								}
								else if(Add==6)
								{
									Constpara_State[5]=1;
									Constpara_Tag[5]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[5]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[5];j++)  Constpara6[j]= Decryption.DataBuffer[20+5*i+j+cd];						
								}
								else if(Add==7)
								{
									Constpara_State[6]=1;
									Constpara_Tag[6]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[6]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[6];j++)  Constpara7[j]= Decryption.DataBuffer[20+5*i+j+cd];							
								}
								else if(Add==8)
								{
									Constpara_State[7]=1;
									Constpara_Tag[7]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[7]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[7];j++)  Constpara8[j]= Decryption.DataBuffer[20+5*i+j+cd];							
								}
								else if(Add==9)
								{
									Constpara_State[8]=1;
									Constpara_Tag[8]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[8]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[8];j++)  Constpara9[j]= Decryption.DataBuffer[20+5*i+j+cd];							
								}
								else if(Add==10)
								{
									Constpara_State[9]=1;
									Constpara_Tag[9]=Decryption.DataBuffer[18+5*i+cd];
									Constpara_Len[9]=Decryption.DataBuffer[19+5*i+cd];
									for(j=0;j<Constpara_Len[9];j++)  Constpara10[j]= Decryption.DataBuffer[20+5*i+j+cd];						
								}							
							}
							else if((Add>=0x8020)&&(Add<=0x821F))										//运行参数
							{
								Add-=0x8020;
								if(Add<=0x20)
								{
									Runpara_State[Add]=1;
									Runpara_Tag[Add]=Decryption.DataBuffer[18+5*i+cd];
									Runpara_Len[Add]=Decryption.DataBuffer[19+5*i+cd];
									if(Runpara_Len[Add] <= 4)
									{
										for(j=0;j<Runpara_Len[Add];j++)
											run_send.runpara[Add].bytedata[j]=Decryption.DataBuffer[20+5*i+j+cd];										
									}
								
							  }
							}
							else if((Add>=0x8220)&&(Add<=0x85EF))										//定值
							{
								Add-=0x8220;
								Fixpara_State[Add]=1;
								Fixpara_Tag[Add]=Decryption.DataBuffer[18+5*i+cd];
								Fixpara_Len[Add]=Decryption.DataBuffer[19+5*i+cd];
								if(Fixpara_Len[Add] <= 4)
								{
								  for(j=0;j<Fixpara_Len[Add];j++)
									  fix_send.fixpara[Add].bytedata[j]=Decryption.DataBuffer[20+5*i+j+cd];								
							
								}
							}
							cd+=Decryption.DataBuffer[19+5*i+cd];
						}
						Super_Sign[12]=7;
					}
					else if((Decryption.DataBuffer[8]==0x06)&&((Decryption.DataBuffer[14]&0xc0)==0))   //参数固化
					{
	//						SN_Num=(u16)Decryption.DataBuffer[15]&0xFF;
	//						SN_Num=(u16)Decryption.DataBuffer[16]<<8&0xff00;
		//固化一下，将接收值放入存储
						for(i=0;i<8;i++)
						{
							if(Msg_Safty1[i]!=Ter_Random1[i])	//验证随机数是否是我们发出去的
							{
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//验证随机数失败
								return;
							}
						}
						for(i=0;i<10;i++)
						{
							if(Constpara_State[i]==1)
							{
								switch(i)
								{
									case 0:

										break;
									case 1:

										break;
									case 2:

										break;
									case 3:

										break;										
									case 4:

										break;
									case 5:

										break;
									case 6:

										break;
									case 7:

										break;	
									case 8:

										break;
									case 9:

										break;										
								}
								Constpara_State[i]=0;
							}
						}
						for(i=0;i<Runpara_Num;i++)
						{
							if(Runpara_State[i]==1)
							{
						
								Runpara_State[i]=0;
							}							
						}
						for(i=0;i<Fixpara_Num;i++)
						{
							if(Fixpara_State[i]==1)
							{
								switch(i)
								{
									case 8:
                    ProtectSet.para.delay_fast_off_enable = fix_send.fixpara[8].bytedata[0];
										break;
									case 9:
                    ProtectSet.para.delay_fast_off = (u16)(fix_send.fixpara[9].floatdata * 100);//过流I段定值
										break;
									case 10:
                    ProtectSet.para.delay_fast_off_time = (u16)(fix_send.fixpara[10].floatdata * 100);
										break;
									case 11:
                    ProtectSet.para.delay_fast_off_voltage_lock = fix_send.fixpara[11].bytedata[0];
										break;										
									case 12:
                    ProtectSet.para.delay_fast_off_direction_lock = fix_send.fixpara[12].bytedata[0];
										break;
									case 13:
                    ProtectSet.para.max_current_enable = fix_send.fixpara[13].bytedata[0];
										break;
									case 14:
                    ProtectSet.para.max_current = (u16)(fix_send.fixpara[14].floatdata * 100);//过流II段定值
										break;
									case 15:
                    ProtectSet.para.max_current_time = (u16)(fix_send.fixpara[15].floatdata * 100);
										break;	
									case 16:
                    ProtectSet.para.max_current_voltage_lock = fix_send.fixpara[16].bytedata[0];
										break;
									case 17:
                    ProtectSet.para.max_current_direction_lock = fix_send.fixpara[17].bytedata[0];
										break;		
									case 18:
                    ProtectSet.para.max_current_after = (u16)(fix_send.fixpara[18].floatdata * 100);//过流后加速定值
										break;
									case 19:
                    ProtectSet.para.max_current_after_time = (u16)(fix_send.fixpara[19].floatdata * 100);
										break;
									case 20:
                    ProtectSet.para.max_load = (u16)(fix_send.fixpara[20].floatdata * 100);//过负荷告警定值
										break;
									case 21:
                    ProtectSet.para.max_load_time = (u16)(fix_send.fixpara[21].floatdata * 100);
										break;										
	                case 22:
                    ProtectSet.para.zero_fast_off_enable = fix_send.fixpara[22].bytedata[0];
										break;
									case 23:
                    ProtectSet.para.zero_fast_off = (u16)(fix_send.fixpara[23].floatdata * 100);//过流后加速定值
										break;
									case 24:
                    ProtectSet.para.zero_fast_off_time = (u16)(fix_send.fixpara[24].floatdata * 100);//过负荷告警定值
										break;	
	                case 25: //零序I段经方向闭锁
                    
										break;	
									case 26:
                    ProtectSet.para.zero_max_current_after = (u16)(fix_send.fixpara[26].floatdata * 100);//零序后加速定值
										break;
									case 27:
                    ProtectSet.para.zero_max_current_after_time = (u16)(fix_send.fixpara[27].floatdata * 100);
										break;	
	                case 28:
                    ProtectSet.para.once_reon_enable = fix_send.fixpara[28].bytedata[0];//重合闸投退
										break;	
	                case 29:
                    ProtectSet.para.reon_Un_enable = fix_send.fixpara[29].bytedata[0];//重合闸检无压投退
										break;	
	                case 30:
                    ProtectSet.para.reon_synchron_enable = fix_send.fixpara[30].bytedata[0];//重合闸检同期投退
										break;
									case 31:
                    ProtectSet.para.once_reon_time = (u16)(fix_send.fixpara[31].floatdata * 100);//重合闸时间
										break;									
		              case 32: //小电流接地告警投退
                    
										break;
									case 33:
                    ProtectSet.para.zero_max_voltage = (u16)(fix_send.fixpara[33].floatdata * 100);//零序电压定值
										break;
	               	case 34:
                    ProtectSet.para.max_voltage_enable = fix_send.fixpara[34].bytedata[0];//过压保护投退     
										break;
									case 35:
                    ProtectSet.para.max_voltage = (u16)(fix_send.fixpara[35].floatdata * 100);//过压保护定值
										break;
									case 36:
                    ProtectSet.para.max_voltage_time = (u16)(fix_send.fixpara[36].floatdata * 100);//过压保护延时
										break;
								  case 37:
                    ProtectSet.para.max_freq_enable = fix_send.fixpara[37].bytedata[0];//高频保护投退       
										break;
									case 38:
                    ProtectSet.para.max_freq = (u16)(fix_send.fixpara[38].floatdata * 100);//高频保护定值
										break;
									case 39:
                    ProtectSet.para.max_freq_time = (u16)(fix_send.fixpara[39].floatdata * 100);//高频保护延时
										break;
									case 40:
                    ProtectSet.para.low_freq_enable = fix_send.fixpara[40].bytedata[0];//低频保护投退
										break;
									case 41:
                    ProtectSet.para.low_freq = (u16)(fix_send.fixpara[41].floatdata * 100);//低频保护定值
										break;
									case 42:
                    ProtectSet.para.low_freq_time = (u16)(fix_send.fixpara[42].floatdata * 100);//低频保护延时
										break;
									default:	
							      break;
							  }
                Fixpara_State[i]=0;		
						  }							
						}
						Super_Sign[12]=8;			
					}
	//				else if((Decryption.DataBuffer[8]==0x06)&&(Decryption.DataBuffer[14]>>6&0x01))  //取消预置
	//				{
	//					if(succ_flag_spi)
	//					{
	//						succ_flag_spi=0;
	//						SN_Num=(u16)Decryption.DataBuffer[15]&0xFF;
	//						SN_Num=(u16)Decryption.DataBuffer[16]<<8&0xff00;
	//	//固化一下，将接收值放入存储
	//						Super_Sign[12]=9;									
	//					}								
	//				}
					else if((Decryption.DataBuffer[8]==0x08)&&((Decryption.DataBuffer[14]&0x80)==0))       //终止
					{
	//							SN_Num=(u16)Decryption.DataBuffer[15]&0xFF;
	//							SN_Num=(u16)Decryption.DataBuffer[16]<<8&0xff00;
	//固化一下，将接收值放入存储
						for(i=0;i<10;i++)							//清除固有参数预置
						{
							Constpara_State[i]=0;
							Constpara_Tag[i]=0;
							Constpara_Len[i]=0;
						}
						for(i=0;i<32;i++)							//清除运行参数预置
						{
							Runpara_State[i]=0;
							Runpara_Tag[i]=0;
							Runpara_Len[i]=0;
						}
						for(i=0;i<64;i++)							//清除定值预置
						{
							Fixpara_State[i]=0;
							Fixpara_Tag[i]=0;
							Fixpara_Len[i]=0;
						}
						Super_Sign[12]=10;																	
					}			
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//验证签名失败
				break;	
		}
	}
	else
	{
	
	}
}




void IEC_104_Resolve_MW(u8 USARTxChannel)
{
	u8 Send_len=0,i=0;
	u16 lc=0;
	if((zz_ok==1)&&(wg_ok==1))	//if((yw_ok==1)&&(zz_ok_104==1)&&(wg_ok_104==1))是否需要
	{
		if(datdone == 2)     //U型
		{
			if(Decryption.DataBuffer[2]==0x7)	  				 //启动命令，只能由主站发起
			{
				Recv_Num=0;        
				Send_Num=0;			
				Recv_NumBak=0;			
				Send_NumBak=0;		//每次初始化把计数清0	
				diff_flag=0;	
				call_num1=0;	//重启时清0，可以再次总召
				yk_state=0;
				IEC104_Answer_U(0x0B,USARTxChannel);  							 //启动确认  
	//初始化结束报文
				Send_len=Conclude_Initinal_BefJM(0x0E,0);		//the second parameter is unuseful
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  
					Send_Num++;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
				}					
			}	
			else if(Decryption.DataBuffer[2]==0x13)			 //停止命令
			{
				IEC104_Answer_U(0x23,USARTxChannel);               //停止确认		
			}
			else if(Decryption.DataBuffer[2]==0x43) 		 //测试命令
			{			
				IEC104_Answer_U(0x83,USARTxChannel);							 //测试确认
			}
		}
		else if(datdone == 1)//S型
		{
			Recv_NumBak=((((u16)Decryption.DataBuffer[5])<<8)&0xFF00)|(Decryption.DataBuffer[4]&0xFF);			//主机发来的接收序列号，主机认为自己接收的次数
			Recv_NumBak>>=1;
			i=Send_Num-Recv_NumBak;
			if(i==0)
			{
				diff_flag=0;
			}
			else if(i>=12)
			{
				diff_flag=1;
			}
			else
			{
				diff_flag=0;
			}
		}	
	}
	else 
		Re_error_msg(USARTxChannel,0x01,0x1F,0x9107);
}

void Active_Upload(u8 USARTxChannel)					//
{
	u16 p_Reset=0,lc=0,i=0;	
	u8 Send_len=0,crc_yhf=0,spi_flag=0;
	u8 buf[12]={0};

	if((zz_ok==1)&&(wg_ok==1))//&&(init_finish))
	{
		if((Send_Num-Recv_NumBak)>=12)	diff_flag=1;//这种情况是防止自发无回应
		if(diff_flag==1)	//如果有6个未确认就暂停发送
		{}
		else
		{
      if(Callend_Flag)
		  {
				if(Super_Sign[3]==0)
				{
					if(SOE_Send_PD() == 0x0D)
					{
						Super_Sign[3]=1;
					}
				}

				if(Soesend_cnt == 0 && Super_Sign[4] == 0)	
				{
					if(getint_yc_state())
					{
						Super_Sign[7]=1;
					}
					Soesend_cnt=Max_Time;	
				}
		  }

      if(Super_Sign[8]==2)
			{
				__set_FAULTMASK(1);
				NVIC_SystemReset();		
			}
			else if(Super_Sign[10]==4)
			{
			  //add S message
				IEC104_Answer_S(USARTxChannel);
			}
			else if(Super_Sign[10]==6)                     //升级
			{
				Send_len=Confirm_Update_BefJM(0,ACTCON,0x80); 
        Super_Sign[10]=0;				
				BW_Before_JM1[1]=Send_len-2;		
				spi_flag=0;
				
				while(spi_flag!=3)															//获取终端随机数
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);													//帧头
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											                //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									  //包含初始向量，终端随机数，报文内容，应用类型，报文长度，终端随机数长度    /Send_len	
					spi_flag=0;			
					while(spi_flag!=3)															
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														//帧头
						for(i=0;i<4;i++)															  //参数
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);								//总长度
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															  //主站随机数
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															  //主站随机数反码
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);														//应用类型
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);												//报文长度
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												  //报文内容
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);														//随机数长度高字节
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);														//随机数长度低字节
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															  //随机数
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi) 
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);
						Send_Num++;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);					
					}					
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);					
				}		
			}
			else if(Super_Sign[10]==7)
			{
		    Send_len=Confirm_Update_BefJM(0,ACTCON,0x0); 
				BW_Before_JM1[1]=Send_len-2;		
				lc=16+4+Send_len;						 									  //包含初始向量，报文内容，应用类型，报文长度，终端随机数长度    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //加密报文
					Send_Num++;
//*************************实际使用*****************************************************************
					buf[0] = TimeNow.year;
					buf[1] = TimeNow.month;
					buf[2] = TimeNow.date;
					buf[3] = TimeNow.hour;
					buf[4] = TimeNow.minute;
					buf[5] = TimeNow.second;
					buf[6] = Rec_Doc_Num1 & 0x000000FF;
					buf[7] = (Rec_Doc_Num1 & 0x0000FF00) >> 8;
					buf[8] = (Rec_Doc_Num1 & 0x00FF0000) >> 16;
					buf[9] = (Rec_Doc_Num1 & 0xFF000000) >> 24;
					CS2BZ = 0;
					while(1)
					{
						if(Flash_Writeflag == 0)
						{
					    SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10); 	// 存入总字节数
							break;
						}
					}
//***************************************************************************************************
					//计算摘要值
					//验证摘要值
					for(i=0;i<8;i++)
					{
						if(Msg_Safty1[6+i]!=Ter_Random1[i])	//验证随机数是否是我们发出去的
						{
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//验证随机数失败
							Super_Sign[10]=0;
							return;
						}
					}
					
					data_reset();
					GetBinFileMd5(decrypt);
					lc=16+6+8+64;												//验证发送数据长度，摘要值+时间信息长度+随机数+签名值
					spi_flag=0;
					while(spi_flag!=3)															//验证签名的正确性
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);															//帧头
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;
						SPI2_ReadWrite(0x08);
						crc_yhf^=0x08;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(Msg_Safty_sj[78]);				//asKID:签名密钥索引
						crc_yhf^=Msg_Safty_sj[78];
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<16;i++) 
						{
							SPI2_ReadWrite(decrypt[i]);
							crc_yhf^=decrypt[i];
						}	
						for(i=0;i<78;i++) 
						{
							SPI2_ReadWrite(Msg_Safty_sj[i]);
							crc_yhf^=Msg_Safty_sj[i];
						}	
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)										//签名正确
					{
						succ_flag_spi=0;
						RecoverKZQBackupFlag =1 ;		//说明升级文件接收完毕，可以进行升级了	
						Super_Sign[10]=0;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
						Super_Sign[10]=0;
					}
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[10]=0;					
				}				
			}
			else if(Super_Sign[12]==7)
			{
				Send_len=Pre_Set_BefJM(0); 
				BW_Before_JM1[1]=Send_len-2;		
				spi_flag=0;
				while(spi_flag!=3)															//获取终端随机数
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);														  //帧头
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									//包含初始向量，终端随机数，报文内容，应用类型，报文长度，终端随机数长度    /Send_len	
					spi_flag=0;			
					while(spi_flag!=3)															//
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														  //帧头
						for(i=0;i<4;i++)															//参数
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															//主站随机数
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															//主站随机数反码
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);															//应用类型
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);													//报文长度
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												//报文内容
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);															//随机数长度高字节
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);															//随机数长度低字节
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															//随机数
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi) 
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);
						Send_Num++;
						Super_Sign[12]=0;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[12]=0;
					}				
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[12]=0;
				}	
			}
      else if(Super_Sign[3]!=0 || Super_Sign[4]!=0 || Super_Sign[6]!=0 || Super_Sign[7]!=0 || Super_Sign[8]!=0 ||
				      Super_Sign[9]!=0 || Super_Sign[10]!=0|| Super_Sign[11]!=0|| Super_Sign[12]!=0)
			{
        if(Super_Sign[3]==1)
				{					
				  Send_len=AutoSend_YX_TIME(11*yx_data_num+10,0,yx_data_num); //生成带时标的遥信上送报文
					Super_Sign[3] = 2;
				}
				else if(Super_Sign[3]==2)
				{
					Send_len=AutoSend_YX(4*yx_data_num+10,0,yx_data_num);       //生成不带时标遥信上送报文
					yx_data_flag = 0;
					Super_Sign[3]=0;
				}
				else if(Super_Sign[4]==1)                           //非初始化总召
				{
					Send_len=GeneralCall_BefJM(0x0C,0);	 			    //生成总召激活报文
					Super_Sign[4]=2;	
				}
				else if(Super_Sign[4]==2)
				{
					Send_len=YX_BefJM(13+YX_Number,0);						//生成遥信报文
					Super_Sign[4]=3;
				}
				else if(Super_Sign[4]==3)
				{
					Send_len=YC_BefJM(13+YC_Number*3,0); 	        //生成遥测报文
					Super_Sign[4]=4;
				}				
				else if(Super_Sign[4]==4)
				{
					Send_len=Conclude_GeneralCall_BefJM(0x0C,0);	//生成总召结束报文
					Super_Sign[4]=0;
				}				
				else if(Super_Sign[6]==1)                 //时钟同步、请求
				{
					Send_len=Cal_Time_BefJM(0x12,0);    //生成时钟同步确认报文
					Super_Sign[6]=0;
					Callend_Flag=1;
				}
				else if(Super_Sign[6]==2)
				{
					Send_len=Read_Time_BefJM(0x12,0);   //生成时钟请求确认报文
					Super_Sign[6]=0;
				}		
				else if(Super_Sign[7]==1)
				{
					Send_len=AutoSend_YC(6*yc_send.yc_num+10,0); 		    //生成变化遥测报文
					Super_Sign[7]=0;
					yc_send.yc_state=0;					      //遥测状态位清0
					yc_send.yc_num=0;			
				}
				else if(Super_Sign[8]==1)           //复位进程
				{
					Send_len=Confirm_Reset_BefJM(0x0C,0);
					Super_Sign[8]=2;	
					p_Reset=2;													//复位原因，远方复位
					while(1)
					{
						if(Flash_Writeflag == 0)
						{
					    SPI_Flash_Write((u8*)&p_Reset,FLASH_RESETYY_ADDR,2);
							break;
						}
					}			
				}
				else if(Super_Sign[9]==1)  //文件传输
				{
			//需要对目录进行查询，我们flash内存应该先存好几条目录，COMTRADE(其目录下存故障录播文件)对应ASCII为43 4F 4D 54 52 41 44 45	,HISTORY/SOE,HISTORY/CO,HISTORY/EXV,HISTORY/FIXPT等等	
			//查到目录后，将该目录下的文件名发送到主站				
	//			  if(Check_List_Name())									//说明目录存在
	//				{
	//					Send_len=Confirm_List_Bef_JM(0);				 //0表示成功，1表示失败
	//				}
	//				else                                                         
	//				{
	//					Send_len=Confirm_List_Bef_JM(1);				 //0表示成功，1表示失败
	//				}
					Super_Sign[9]=0;
				}
				else if(Super_Sign[10]==1)
				{
//					if(Check_File_Name())
//					{
//						Send_len=Confirm_ReadDoc_Bef_JM(0);												//读取文件激活确认，文件名，大小，长度等
//						Super_Sign[10]=2;					
//					}
//					else
//					{
//						Send_len=Confirm_ReadDoc_Bef_JM(1);												//读取文件激活确认，文件名，大小，长度等
//						Super_Sign[10]=0;
//					}		
				}
				else if(Super_Sign[10]==2)
				{
					//Send_len=Send_Doc_Bef_JM();
					//Super_Sign[10]=2;
				}	
				else if(Super_Sign[10]==3)
				{
					if(Check_WriteFile_Name()&&Finish_Flag1==0)			
					{
						Send_len=Confirm_WriteDoc_BefJM(0,0);	
					}
					else
					{
						Send_len=Confirm_WriteDoc_BefJM(0,1);		
					}
					Super_Sign[10]=0;
				}		
				else if(Super_Sign[10]==5)
				{
					Send_len=Confirm_Send_Dat_BefJM(0x16,0);
					Super_Sign[10]=0;
					Rec_Error_Flag1=0;
				}	
				else if(Super_Sign[10]==8)
				{
					Send_len=Confirm_Update_BefJM(0,DEACTCON,0);
					Super_Sign[10]=0;					
				}							
				else if(Super_Sign[11]==1)                    //电能召唤
				{
					Send_len=Confirm_Enegy_BefJM(0x0C,0); 			 //生成电能数据召唤确认报文
					Super_Sign[11]=2;
				}
				else if(Super_Sign[11]==2)
				{
					Send_len=Confirm_Enegy_Pluse_BefJM(0x33,0);	 //生成电能数据报文
					Super_Sign[11]=3;
				}		
				else if(Super_Sign[11]==3)
				{
					Send_len=Conclude_Enegy_BefJM(0x0C,0);	    //生成电能召唤结束报文
					Super_Sign[11]=0;
				}
				else if(Super_Sign[12]==1)                   //参数读写
				{
					Send_len=Read_Area_BefJM(0x11,0);
					Super_Sign[12]=0;
				}				
				else if(Super_Sign[12]==2)
				{
					Send_len=Change_Area_BefJM(0x0D,0);
					Super_Sign[12]=0;	
				}				
				else if(Super_Sign[12]==3)
				{
					if(params_flag==0)
					{
						params_flag=1;
						Send_len=Read_All_Constparas_BefJM(0);
					}
					else if(params_flag==1)
					{
						params_flag=2;
						Send_len=Read_All_Runparas_BefJM(0);
					}
					else if(params_flag==2)
					{
						params_flag=3;
						Send_len=Read_All_Fixparas_BefJM(0);			
					}
					if(params_flag==3)
					{
						params_flag=0;
						Super_Sign[12]=0;
					}	
				}	
				else if(Super_Sign[12]==4)
				{
					if(fix_value.params_num)             //定值参数
					{
						if(run_value.params_num||const_value.params_num)
							Send_len=Read_FixvalueParas_BefJM(0,1);	
						else
						{
							params_flag=1;
							Send_len=Read_FixvalueParas_BefJM(0,0);	
						}
					}
					else if(run_value.params_num)             //运行参数
					{
						if(const_value.params_num)
							Send_len=Read_RunParas_BefJM(0,1);
						else
						{
							params_flag=1;
							Send_len=Read_RunParas_BefJM(0,0);
						}	
					}
					else if(const_value.params_num)            //固有参数
					{
						params_flag=1;
						Send_len=Read_ConstParas_BefJM(0,0);		
					}
					if(params_flag==1)
					{
						params_flag=0;
						Super_Sign[12]=0;
					}	
				}
				else if(Super_Sign[12]==8)
				{
					ProtectSet.para.fast_off_voltage_lock = 0;
					ProtectSet.para.fast_off_direction_lock = 0; 
					ProtectSet.para.delay_fast_off_voltage_lock = 0; 
					ProtectSet.para.delay_fast_off_direction_lock = 0; 
					ProtectSet.para.max_current_time_mode = 0; 
					ProtectSet.para.max_current_voltage_lock = 0;
					ProtectSet.para.max_current_direction_lock = 0;
					SetProtectFlag=1;
					Send_len=Act_Set_BefJM(0,0x07,0x0);
					Super_Sign[12]=0;				
				}
				else if(Super_Sign[12]==10)
				{
					Send_len=Act_Set_BefJM(0,0x09,0x40);		          //取消预置
					Super_Sign[12]=0;
				}
				BW_Before_JM1[1]=Send_len-2;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				        //加密报文成功
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //发送遥信加密报文
					Send_Num++;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[3]=0;
					Super_Sign[4]=0;
					Super_Sign[6]=0;
					Super_Sign[7]=0;
					Super_Sign[8]=0;
					Super_Sign[9]=0;
					Super_Sign[10]=0;
					Super_Sign[11]=0;
					Super_Sign[12]=0;				
				}	
		  }			
		}
	}
}

void CommandProcess_Encryption_Prepare(u8 USARTxnumber)
{
	u16 i = 0;	
	if(USARTxnumber == 1)
	{
	  for(i=0;i<RxCounter_serial1;i++)
		  BackstageProtocol1.DataBuffer[i] = BackstageProtocol1bak.DataBuffer[i];	
	}
	else if(USARTxnumber == 3)
	{
	  for(i=0;i<RxCounter_serial1;i++)
		  BackstageProtocol3.DataBuffer[i] = BackstageProtocol3bak.DataBuffer[i];	
	}
}

//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  2020.05.09| 9 May 2020 | S.H.Q. |
//###########################################################################
void CommandProcess_101_Encryption(u8 USARTxChannel)
{
  u16 i=0,lc=0,num=0;
	static u16 nu=0;
	u8 idx=0,crc_yhf=0,sum=0,spi_flag=0,key_ver=0;
	
	zz_ok=1;
	wg_ok=1;
	if(datdone_serial1==1)                         //加密报文，需要解密
	{
		if((zz_ok==1)&&(wg_ok==1))
		{
			lc=16+Length_serial1-2;											//需要解密的数据长度
			spi_flag=0;
			while(spi_flag!=3)
			{
				SSN_L();	
				SPI2_ReadWrite(0x55);														//帧头
				for(i=0;i<4;i++)
				{
					SPI2_ReadWrite(Recv_JM[i]);
					crc_yhf^=Recv_JM[i];			
				}
				SPI2_ReadWrite(((u8)(lc>>8))&0xFF);
				crc_yhf^=(u8)(lc>>8)&0xFF;
				SPI2_ReadWrite((u8)lc&0xFF);
				crc_yhf^=(u8)lc&0xFF;
				for(i=0;i<8;i++) 
				{
					SPI2_ReadWrite(Zz_Random1[i]);
					crc_yhf^=Zz_Random1[i];
				}	
				for(i=0;i<8;i++) 
				{
					SPI2_ReadWrite(~Zz_Random1[i]);
					crc_yhf^=(~Zz_Random1[i]);
				}	
				for(i=0;i<Length_serial1-2;i++)
				{
					SPI2_ReadWrite(BackstageProtocol1.DataBuffer[6+i]);
					crc_yhf^=BackstageProtocol1.DataBuffer[6+i];
				}
				crc_yhf=~crc_yhf;
				SPI2_ReadWrite(crc_yhf);	
				crc_yhf=0;					//很重要
				SSN_H();											   //拉高片选
				spi_flag=Ter_WriteRead_Spi1();
			}	
			if(succ_flag_spi)	
			{
				succ_flag_spi=0;
													/*这里有些疑问，解密之后会把这些长度数据去掉吗，还是单纯的只解密*/
				YY_Style1= Spi_Recv1[0];																						//应用类型
				if(YY_Style1<0x20)			//加密报文只有0~8和0x1f
				{
					if(YY_Style1==0x08) 	//没有原始报文
					{
						MsgSafty_Len1=(((u16)Spi_Recv1[1])<<8|Spi_Recv1[2]);		//安全信息体长度
						for(i=0;i<MsgSafty_Len1;i++)
							Msg_Safty_sj[i]=Spi_Recv1[3+i];										////验证摘要值备用					
					}
					else
					{
						BW_Len1 = Spi_Recv1[1];																						//报文长度
						for(i=0;i<BW_Len1;i++) 
							Decryption.DataBuffer[i]=Spi_Recv1[2+i]; 								//报文数据，需要解析
						MsgSafty_Len1=(((u16)Spi_Recv1[2+BW_Len1])<<8|Spi_Recv1[3+BW_Len1]);		//安全信息体长度
						for(i=0;i<MsgSafty_Len1;i++)
							Msg_Safty1[i]=Spi_Recv1[4+BW_Len1+i];															//安全信息体数据
//						switch(YY_Style1)
//						{
//							case 1:	//签名
//								if(MsgSafty_Len1<64)	//说明没有签名
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
//									return;
//								}
//								break;
//							case 3:	//随机数+签名
//								if(MsgSafty_Len1<60)	//说明没有签名
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
//									return;
//								}
//								else if((MsgSafty_Len1>65)&&(MsgSafty_Len1<70))//说明没有随机数
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);
//									return;
//								}
//								break;
//							case 5:	//时间+签名
//								if(MsgSafty_Len1<60)	//说明没有签名
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
//									return;
//								}	
//								else if((MsgSafty_Len1>65)&&(MsgSafty_Len1<70))//说明没有时间
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9105);
//									return;
//								}
//								break;
//							case 7:	//时间+随机数+签名
//								if(MsgSafty_Len1<64)//说明没有签名
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
//									return;
//								}
//								else if((MsgSafty_Len1>65)&&(MsgSafty_Len1<72))//	说明没有随机数
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);
//									return;
//								}
//								else if((MsgSafty_Len1>72)&&(MsgSafty_Len1<78))	//说明没有时间
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9105);
//									return;
//								}
//								break;
//						}
						if((YY_Style1>=4)&&(YY_Style1<=7))
						{
							if(((TimeNow.year&0x7F)==(Msg_Safty1[0]&0x7F))&&((TimeNow.month&0x0F)==(Msg_Safty1[1]&0x0F))&&
								 ((TimeNow.date&0x1F)==(Msg_Safty1[2]&0x1F)))
							{
								if(abs((TimeNow.hour*60+TimeNow.minute)  - (Msg_Safty1[3]*60+Msg_Safty1[4])) < 3)
								{
									if(y_104==0)
										ProtocolResolve1_JM(USARTxChannel);	//解析报文数据
									else
										IEC_104_Resolve_JM(USARTxChannel);
								}
								else
								{
									Re_error_msg(USARTxChannel,0x01,0x1F,0x9105);
								} 	
							}
							else
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9105);
						}
						else
							if(y_104==0)
								ProtocolResolve1_JM(USARTxChannel);	//解析报文数据	
							else
								IEC_104_Resolve_JM(USARTxChannel);
					}
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
			}
			else
				Re_error_msg(USARTxChannel,0x01,0x1F,0x9103);	
		}
		else
			Re_error_msg(USARTxChannel,0x01,0x1F,0x9107);
	}
	else if(datdone_serial1==2)                    //不加密报文
	{
		switch(BackstageProtocol1.DataBuffer[6])
		{
			case 0x00:																//说明为固定帧长格式报文
				if(BackstageProtocol1.DataBuffer[7]>7)//||(BackstageProtocol1.DataBuffer[8]==0x68))
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9106);
				}
				else
				{
					if((BackstageProtocol1.DataBuffer[7]==6)&&(BackstageProtocol1.DataBuffer[8]==0x10)&&(BackstageProtocol1.DataBuffer[13]==0x16))
					{
						sum+=BackstageProtocol1.DataBuffer[9];
						sum+=BackstageProtocol1.DataBuffer[10];
						sum+=BackstageProtocol1.DataBuffer[11];
						if(sum==BackstageProtocol1.DataBuffer[12])
						{
							for(i=0;i<6;i++)   
								Decryption.DataBuffer[i]=BackstageProtocol1.DataBuffer[8+i];
							y_104=0;
						  ProtocolResolve1_MW(USARTxChannel);
						}	
					}	
					else if((BackstageProtocol1.DataBuffer[7]==6)&&(BackstageProtocol1.DataBuffer[8]==0x68)&&(BackstageProtocol1.DataBuffer[9]==4))
					{
						if(((BackstageProtocol1.DataBuffer[10]&0x3)==1)&&((BackstageProtocol1.DataBuffer[12]&1)==0)) datdone=1;  //S型 
						else if(((BackstageProtocol1.DataBuffer[10]&0x3)==3)&&((BackstageProtocol1.DataBuffer[12]&1)==0)) datdone=2;//U型
						for(i=0;i<Length_serial1-6;i++)  Decryption.DataBuffer[i]=BackstageProtocol1.DataBuffer[8+i];
						y_104=1;
						IEC_104_Resolve_MW(USARTxChannel);
					}						
				}
				break;
			case 0x20:
				for(i=0;i<8;i++)   
			    Wg_Random1[i]=BackstageProtocol1.DataBuffer[9+i];				  //存储网关随机数
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Wg_ca1[i]);
						crc_yhf^=Wg_ca1[i];	
					}
					for(i=0;i<8;i++)    												   //发送网关随机数
					{
						SPI2_ReadWrite(Wg_Random1[i]);
						crc_yhf^=Wg_Random1[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();																		   //拉高片选
				  spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Ter_Req_WgZz1(Spi_Length1+1,USARTxChannel,0x06,0x21);   //这里0x01为签名密钥标识，但是不知道是0x01还是0x06，两个文献不一样
				}
				break;
			case 0x22:				//网关对终端认证请求的响应
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //安全信息长度
				if(num<60)	//没有签名
				{
					Ter_Re_WgZz1(USARTxChannel,0x9090,0x23); 
					return;
				}
				for(i=0;i<64;i++)
					Wg_Sign1[i]=BackstageProtocol1.DataBuffer[9+i]; //保存网关签名结果
					//spi的步骤基本一致
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Wg_ca2[i]);
						crc_yhf^=Wg_ca2[i];	
					}
					for(i=0;i<64;i++)    												   //发送网关签名
					{
						SPI2_ReadWrite(Wg_Sign1[i]);
						crc_yhf^=Wg_Sign1[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();																		   //拉高片选
				  spi_flag=Ter_WriteRead_Spi1();
				}
//				if(num<64)	Ter_Re_WgZz1(USARTxChannel,0x9090,0x23);   //
				if(succ_flag_spi)									//应该是不管成功与否都要返回状态
				{
					wg_ok=1;						//网关置位
					succ_flag_spi=0;
					Ter_Re_WgZz1(USARTxChannel,0x9000,0x23);   //
				}
				else
				{
					succ_flag_spi=0;
					Ter_Re_WgZz1(USARTxChannel,0x9090,0x23);   //
				}
				break;
/*---------------------------------------------------------------------------------------*/
/*--------------------------证书管理工具-------------------------------------------------*/
/*---------------------------------------------------------------------------------------*/				
			case 0x30:     //证书工具与终端认证
				num_flag++;
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //安全信息长度
				if(BackstageProtocol1.DataBuffer[10]>BackstageProtocol1.DataBuffer[11])									//总帧数大于当前帧序号，说明还有后续
				{
					if(num_flag!=BackstageProtocol1.DataBuffer[11])
					{
						nu=0;
						num_flag=1;
					}
						
					for(i=0;i<num-11;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-11);
					FZ_Reback(USARTxChannel,0x45,0x9000);
				}
				else		//总帧数等于当前帧序号，最后一帧
				{
					num_flag=0;
					for(i=0;i<num-11;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-11);

					for(i=0;i<8;i++)
					{
						Cer_ID[i]=BackstageProtocol1.DataBuffer[num+1+i];
					}
					spi_flag=0;
					while(spi_flag!=3)
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x1E);
						crc_yhf^=0x1E;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(nu>>8)&0xFF;
						SPI2_ReadWrite((u8)nu&0xFF);
						crc_yhf^=(u8)nu&0xFF;
						for(i=0;i<nu;i++)
						{
							SPI2_ReadWrite(Cer_data[i]);
							crc_yhf^=Cer_data[i];	
						}
						nu=0;
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();						//拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						spi_flag=0;
						while(spi_flag!=3)															//获取终端随机数
						{
							SSN_L();	
							SPI2_ReadWrite(0x55);														  //帧头
							for(i=0;i<6;i++)
							{
								SPI2_ReadWrite(Get_Ter_Random[i]);	
								crc_yhf^=Get_Ter_Random[i];
							}
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);	
							crc_yhf=0;					//很重要
							SSN_H();											   //拉高片选
							spi_flag=Ter_WriteRead_Spi1();
						}	
						if(succ_flag_spi) 
						{
							succ_flag_spi=0;
							for(i=0;i<8;i++)
								Ter_Random1[i]=Spi_Recv1[i];
							Send_Random(USARTxChannel,0x31,Ter_Random1);
						}
					}
				}
				break;
			case 0x32:
				spi_flag=0;
				while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);	
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;	
					SPI2_ReadWrite(0x20);
					crc_yhf^=0x20;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x40);
					crc_yhf^=0x40;	
					for(i=0;i<64;i++)
					{
						SPI2_ReadWrite(BackstageProtocol1.DataBuffer[9+i]);
						crc_yhf^=BackstageProtocol1.DataBuffer[9+i];	
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();						//拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					yw_ok=1;//运维认证置位
					succ_flag_spi=0;
					FZ_Reback(USARTxChannel,0x33,0x9000);
				}
				else 
				{
					succ_flag_spi=0;
					FZ_Reback(USARTxChannel,0x33,0x9090);
				}
				break;
			case 0x34:   				//获取终端密钥版本
				if(yw_ok==1)
				{
					spi_flag=0;
					while(spi_flag!=3)
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
						for(i=0;i<6;i++)
						{
							SPI2_ReadWrite(Sec_ca1[i]);
							crc_yhf^=Sec_ca1[i];	
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();																		   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						key_ver=Spi_Recv1[0];     //密钥版本
						spi_flag=0;
						while(spi_flag!=3)															//获取终端随机数
						{
							SSN_L();	
							SPI2_ReadWrite(0x55);														  //帧头
							for(i=0;i<6;i++)
							{
								SPI2_ReadWrite(Get_Ter_Random[i]);	
								crc_yhf^=Get_Ter_Random[i];
							}
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);	
							crc_yhf=0;					//很重要
							SSN_H();											   //拉高片选
							spi_flag=Ter_WriteRead_Spi1();
						}	
						if(succ_flag_spi) 
						{
							succ_flag_spi=0;
							for(i=0;i<8;i++)
								Ter_Random2[i]=Spi_Recv1[i];      //终端随机数
							Send_Ver(USARTxChannel,key_ver,Ter_Random2);
						}
					}			
				}
				else
					Re_error_msg(USARTxChannel,0x40,0x35,0x9108);
				break;
			case 0x36:					//提取终端序列号，不是安全芯片序列号
				Send_Ter_ID(USARTxChannel);
				break;
			case 0x38:  				//提取安全芯片序列号
				spi_flag=0;
				while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0xb0);
					crc_yhf^=0xb0;	
					SPI2_ReadWrite(0x99);
					crc_yhf^=0x99;	
					SPI2_ReadWrite(0x05);
					crc_yhf^=0x05;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x02);
					crc_yhf^=0x02;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();						//拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						chip_ID[i]=Spi_Recv1[i];
					Send_Random(USARTxChannel,0x39,chip_ID);
				}
				break;
			case 0x3A:
				spi_flag=0;
				while(spi_flag!=3)											//取公钥
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);	
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;	
					SPI2_ReadWrite(0x30);
					crc_yhf^=0x30;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();						//拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<64;i++)
						pubkey[i]=Spi_Recv1[i];
					lc=8+8+16+1+2+64;  		//证书ID，终端芯片随机数，初始向量，应用类型，报文长度，报文（公钥）
					spi_flag=0;
					while(spi_flag!=3)											//取公钥
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x26);
						crc_yhf^=0x26;	
						SPI2_ReadWrite(0x62);
						crc_yhf^=0x62;	
						SPI2_ReadWrite(0x02);
						crc_yhf^=0x02;	
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++)
						{
							SPI2_ReadWrite(Cer_ID[i]);
							crc_yhf^=Cer_ID[i];
						}
						for(i=0;i<8;i++)
						{
							SPI2_ReadWrite(Ter_Random1[i]);
							crc_yhf^=Ter_Random1[i];
						}
						for(i=0;i<8;i++)
						{
							SPI2_ReadWrite(Ter_Random1[i]);
							crc_yhf^=Ter_Random1[i];
						}
						for(i=0;i<8;i++)
						{
							SPI2_ReadWrite(~Ter_Random1[i]);
							crc_yhf^=~Ter_Random1[i];
						}
						SPI2_ReadWrite(0x3B);
						crc_yhf^=0x3B;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(0x40);
						crc_yhf^=0x40;
						for(i=0;i<64;i++)
						{
							SPI2_ReadWrite(pubkey[i]);
							crc_yhf^=pubkey[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();						//拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						Common_Style1(Spi_Length1,USARTxChannel,Spi_Recv1); 		
					}
				}
	
				break;
/*			case 0x3c:
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //安全信息长度
				lc=8+8+16+num-2;  		//证书ID，终端芯片随机数，初始向量，请求签名报文和MAC
				while(spi_flag!=3)										
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);	
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;	
					SPI2_ReadWrite(0x2C);
					crc_yhf^=0x2C;	
					SPI2_ReadWrite(0x62);
					crc_yhf^=0x62;	
					SPI2_ReadWrite(0x02);
					crc_yhf^=0x02;	
					SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
					crc_yhf^=(u8)(lc>>8)&0xFF;
					SPI2_ReadWrite((u8)lc&0xFF);
					crc_yhf^=(u8)lc&0xFF;
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(Cer_ID[i]);
						crc_yhf^=Cer_ID[i];
					}
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(Ter_Random1[i]);
						crc_yhf^=Ter_Random1[i];
					}
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(Ter_Random1[i]);
						crc_yhf^=Ter_Random1[i];
					}
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(~Ter_Random1[i]);
						crc_yhf^=~Ter_Random1[i];
					}
					for(i=0;i<num-2;i++)
					{
						SPI2_ReadWrite(BackstageProtocol1.DataBuffer[11+i]);
						crc_yhf^=BackstageProtocol1.DataBuffer[11+i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					SSN_H();						//拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					for(i=0;i<Spi_Length1;i++)
					{
						Cer_data[nu+i]=Spi_Recv1[i];
					}
					nu+=Spi_Length1;
					if(BackstageProtocol1.DataBuffer[9]>BackstageProtocol1.DataBuffer[10])									//总帧数大于当前帧序号，说明还有后续
					{
						FZ_Reback(0,0x45,0x9000);
					}
					else
					{
						spi_flag=0;
						while(spi_flag!=3)						
						{
							SSN_L();																		   //拉低片选
							SPI2_ReadWrite(0x55);	
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite(0x0A);
							crc_yhf^=0x0A;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//总长度
							crc_yhf^=(u8)(nu>>8)&0xFF;
							SPI2_ReadWrite((u8)nu&0xFF);
							crc_yhf^=(u8)nu&0xFF;
							for(i=0;i<nu;i++)
							{
								SPI2_ReadWrite(Cer_data[i]);
								crc_yhf^=Cer_data[i];
							}
							nu=0;
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);
							SSN_H();						//拉高片选
							spi_flag=Ter_WriteRead_Spi1();
						}
						if(succ_flag_spi)
						{
						succ_flag_spi=0;
							Send_sign(0,Spi_Recv1);
						}
					}
				}						
				break;*/
			case 0x3E:
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //安全信息长度
				idx=BackstageProtocol1.DataBuffer[9];     																							//证书标识
				if(BackstageProtocol1.DataBuffer[10]>BackstageProtocol1.DataBuffer[11])									//总帧数大于当前帧序号，说明还有后续
				{
					for(i=0;i<num-3;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-3);
					FZ_Reback(USARTxChannel,0x45,0x9000);
				}
				else		//总帧数等于当前帧序号，最后一帧
				{
					for(i=0;i<num-3;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-3);

					spi_flag=0;
					while(spi_flag!=3)											
					{
						crc_yhf=0;
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);	
						if(idx<=0x05)
						{
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite(0x22);
							crc_yhf^=0x22;	
							SPI2_ReadWrite(idx);
							crc_yhf^=idx;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
						}
						else
						{
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite(0x24);
							crc_yhf^=0x24;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
						}
						SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(nu>>8)&0xFF;
						SPI2_ReadWrite((u8)nu&0xFF);
						crc_yhf^=(u8)nu&0xFF;
						for(i=0;i<nu;i++)
						{
							SPI2_ReadWrite(Cer_data[i]);
							crc_yhf^=Cer_data[i];
						}
						nu=0;
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();						//拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						FZ_Reback(USARTxChannel,0x3F,0x9000);
					}
					else
					{
						succ_flag_spi=0;
						FZ_Reback(USARTxChannel,0x3F,0x9093);
					}
				}
				break;
			case 0x40:						//初始证书回写命令
				spi_flag=0;
				while(spi_flag!=3)												 //取公钥
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);	
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(Read_Cer_len[i]);
						crc_yhf^=Read_Cer_len[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();						//拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					num=((u16)Spi_Recv1[0]<<8)|(Spi_Recv1[1]&0xFF);
					spi_flag=0;
					while(spi_flag!=3)											//取公钥
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0xb0);
						crc_yhf^=0xb0;	
						SPI2_ReadWrite(0x81);
						crc_yhf^=0x81;	
						SPI2_ReadWrite(0x02);
						crc_yhf^=0x02;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x02);
						crc_yhf^=0x02;	
						SPI2_ReadWrite((u8)(num>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(num>>8)&0xFF;
						SPI2_ReadWrite((u8)num&0xFF);
						crc_yhf^=(u8)num&0xFF;
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();						//拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						spi_flag=0;
						for(i=0;i<Spi_Length1;i++)
						{
							Cer_data[i]=Spi_Recv1[i];
						}
						num=Spi_Length1;
						spi_flag=0;
						while(spi_flag!=3)											//取公钥
						{
							SSN_L();																		   //拉低片选
							SPI2_ReadWrite(0x55);	
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite(0x24);
							crc_yhf^=0x24;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
							SPI2_ReadWrite((u8)(num>>8)&0xFF);									//总长度
							crc_yhf^=(u8)(num>>8)&0xFF;
							SPI2_ReadWrite((u8)num&0xFF);
							crc_yhf^=(u8)num&0xFF;
							for(i=0;i<num;i++)
							{
								SPI2_ReadWrite(Cer_data[i]);
								crc_yhf^=Cer_data[i];			
							}
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);
							crc_yhf=0;					//很重要
							SSN_H();						//拉高片选
							spi_flag=Ter_WriteRead_Spi1();
						}
						if(succ_flag_spi)
						{
							succ_flag_spi=0;
							FZ_Reback(USARTxChannel,0x41,0x9000);
						}
					}
				}
				break;
			case 0x42:								//导出终端证书
				frame_num=0;
				spi_flag=0;
				while(spi_flag!=3)											//
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);	
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;	
					SPI2_ReadWrite(0x30);
					crc_yhf^=0x30;	
					SPI2_ReadWrite(0x01);
					crc_yhf^=0x01;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();						//拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Cer_len=Spi_Length1;    //证书的总长度
					for(i=0;i<Spi_Length1;i++)
					{
						Cer_data[i]=Spi_Recv1[i];
					}
					frame_num=1;
					frame_total=Cer_len/200;
					if((Cer_len-frame_total*200)>0)
						frame_total+=1;				//总帧数		
					if(frame_total==0)   	  //证书长度小于200
					{
						frame_total=1; 
						Send_Ter_Cer(USARTxChannel,frame_num,frame_total,Cer_len);
					}
					else
					{
						Send_Ter_Cer(USARTxChannel,frame_num,frame_total,200);
					}
					frame_num++;
				}
				break;
			case 0x45:
				if(frame_num==0)
					return;
				if(frame_num<frame_total)
				{
					Send_Ter_Cer(USARTxChannel,frame_num,frame_total,200);
					frame_num++;
				}
				else if(frame_num==frame_total)
				{
					Send_Ter_Cer(USARTxChannel,frame_num,frame_total,Cer_len-200*(frame_total-1));
					frame_num=0;
				}
				break;
			case 0x44:
				
				break;
			case 0x46:
				if(yw_ok==1)
				{
					spi_flag=0;
					while(spi_flag!=3)											//
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x1C);
						crc_yhf^=0x1C;	
						SPI2_ReadWrite(0x02);
						crc_yhf^=0x02;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x00);									//总长度
						crc_yhf^=0x00;
						SPI2_ReadWrite(0xB9);
						crc_yhf^=0xB9;
						for(i=0;i<185;i++)
						{
							SPI2_ReadWrite(BackstageProtocol1.DataBuffer[9+i]);
							crc_yhf^=BackstageProtocol1.DataBuffer[9+i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();						//拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						FZ_Reback(USARTxChannel,0x47,0x9000);
					}
					else
					{
						succ_flag_spi=0;
						FZ_Reback(USARTxChannel,0x47,0x9092);
					}				
				}
				else
					Re_error_msg(USARTxChannel,0x40,0x47,0x9108);
				break;
			case 0x48:
				break;
/*-----------------------------------------------------------------------*/			
/*----------------证书管理工具报文结束-----------------*/
/*-----------------------------------------------------------------------*/
/*----------------继续主站报文-----------------*/

			case 0x50:
				for(i=0;i<8;i++)   Zz_Random1[i]=BackstageProtocol1.DataBuffer[9+i];				  //存储主站随机数
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);	
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Wg_ca1[i]);
						crc_yhf^=Wg_ca1[i];	
					}
					for(i=0;i<8;i++)    										
					{
						SPI2_ReadWrite(Zz_Random1[i]);
						crc_yhf^=Zz_Random1[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();						//拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Ter_Req_WgZz1(Spi_Length1+1,USARTxChannel,0x06,0x51);   //这里0x01为签名密钥标识，但是不知道是0x01还是0x06，两个文献不一样
				}
				SSN_H();
				break;
			case 0x52:
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //安全信息长度
				if(num<60)	//没有签名
				{
					Ter_Re_WgZz1(USARTxChannel,0x9090,0x53); 
					return;
				}
				for(i=0;i<65;i++)
					Wg_Sign1[i]=BackstageProtocol1.DataBuffer[9+i]; //保存主站签名结果
					//spi的步骤基本一致
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;	
					SPI2_ReadWrite(0x18);
					crc_yhf^=0x18;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(Wg_Sign1[64]);
					crc_yhf^=Wg_Sign1[64];	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x40);
					crc_yhf^=0x40;	
					for(i=0;i<64;i++)    												   //发送主站签名
					{
						SPI2_ReadWrite(Wg_Sign1[i]);
						crc_yhf^=Wg_Sign1[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();																		   //拉高片选
				  spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)									//应该是不管成功与否都要返回状态
				{
					zz_ok=1;					//主站认证置位
					succ_flag_spi=0;
					Ter_Re_WgZz1(USARTxChannel,0x9000,0x53);   //
				}
				else
				{
					succ_flag_spi=0;
					Ter_Re_WgZz1(USARTxChannel,0x9090,0x53);
				}
				break;
			case 0x54:
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(ID_Spi[i]);
						crc_yhf^=ID_Spi[i];	
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();																		   //拉高片选
				  spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Ter_Req_WgZz1(Spi_Length1,USARTxChannel,0x06,0x55);   //这里0x01为签名密钥标识，但是不知道是0x01还是0x06，两个文献不一样
				}				
				break;
			case 0x60:
				if((zz_ok==1)&&(wg_ok==1))
				{
					spi_flag=0;
					while(spi_flag!=3)
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
						for(i=0;i<6;i++)
						{
							SPI2_ReadWrite(Sec_ca1[i]);
							crc_yhf^=Sec_ca1[i];	
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();							//拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						key_ver=Spi_Recv1[0];
						spi_flag =0;				//延时超时跳出san个循环
						while(spi_flag!=3)
						{
							SSN_L();																		   //拉低片选
							SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
							for(i=0;i<6;i++)
							{
								SPI2_ReadWrite(Sec_ca2[i]);
								crc_yhf^=Sec_ca2[i];	
							}
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);		
							crc_yhf=0;					//很重要						
							SSN_H();																		   //拉高片选
							spi_flag=Ter_WriteRead_Spi1();
						}	
						if(succ_flag_spi)
						{
							succ_flag_spi=0;
							for(i=0;i<8;i++)
								Ter_Random1[i]=Spi_Recv1[i];
							Spi_Recv1[0]=key_ver;
							for(i=0;i<8;i++)  
							{
								Spi_Recv1[i+1]=Ter_Random1[i];
							}
							Ter_Req_WgZz1(0x09,USARTxChannel,0x06,0x61);   	//这里0x01为签名密钥标识，但是不知道是0x01还是0x06，两个文献不一样
						}								
					}			
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x61,0x9108);
				break;
			case 0x62:
			case 0x64:
				if((zz_ok==1)&&(wg_ok==1))
				{
					num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //安全信息长度
					if(num<190)
					{
						Ter_Re_WgZz1(USARTxChannel,0x9091,0x63);
						return;
					}
					spi_flag=0;
					while(spi_flag!=3)
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;						
						SPI2_ReadWrite(0x1C);
						crc_yhf^=0x1C;			
						if(BackstageProtocol1.DataBuffer[9]>0)  //密钥版本大于0为更新
						{
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;										
						}
						else if(BackstageProtocol1.DataBuffer[9]==0)//等于0为恢复
						{
							SPI2_ReadWrite(0x01);
							crc_yhf^=0x01;										
						}
						SPI2_ReadWrite(BackstageProtocol1.DataBuffer[254]);
						crc_yhf^=BackstageProtocol1.DataBuffer[254];	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;								
						SPI2_ReadWrite(0xF5);
						crc_yhf^=0xF5;	
						for(i=0;i<245;i++) 
						{
							SPI2_ReadWrite(BackstageProtocol1.DataBuffer[9+i]);
							crc_yhf^=BackstageProtocol1.DataBuffer[9+i];						
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();																		   //拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						if(BackstageProtocol1.DataBuffer[6]==0x62)
						Ter_Re_WgZz1(USARTxChannel,0x9000,0x63); 
						else if(BackstageProtocol1.DataBuffer[6]==0x64)
						Ter_Re_WgZz1(USARTxChannel,0x9000,0x65); 			
					}
					else
					{
						if(BackstageProtocol1.DataBuffer[6]==0x62)
						Ter_Re_WgZz1(USARTxChannel,0x9091,0x63); 
						else if(BackstageProtocol1.DataBuffer[6]==0x64)
						Ter_Re_WgZz1(USARTxChannel,0x9092,0x65); 
					}				
				}
				else
				{
					if(BackstageProtocol1.DataBuffer[6]==0x62)
						Re_error_msg(USARTxChannel,0x01,0x63,0x9108); 
					else if(BackstageProtocol1.DataBuffer[6]==0x64)
						Re_error_msg(USARTxChannel,0x01,0x65,0x9108); 		
				}
				break;
			case 0x70:
				if((zz_ok==1)&&(wg_ok==1))
				{
					num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //安全信息长度
					idx=BackstageProtocol1.DataBuffer[9];     																							//证书标识
					if(BackstageProtocol1.DataBuffer[10]>BackstageProtocol1.DataBuffer[11])									//总帧数大于当前帧序号，说明还有后续
					{
						for(i=0;i<num-3;i++)
						{
							Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
						}
						nu+=(num-3);
					}
					else		//总帧数等于当前帧序号，最后一帧
					{
						for(i=0;i<num-3;i++)
						{
							Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
						}
						nu+=(num-3);
						lc=16+nu;
						spi_flag=0;
						while(spi_flag!=3)											//
						{
							SSN_L();																		   //拉低片选
							SPI2_ReadWrite(0x55);	
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite(0x2C);
							crc_yhf^=0x2C;	
							SPI2_ReadWrite(0x60);
							crc_yhf^=0x60;	
							SPI2_ReadWrite(0x01);
							crc_yhf^=0x01;
							
							SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
							crc_yhf^=(u8)(lc>>8)&0xFF;
							SPI2_ReadWrite((u8)lc&0xFF);
							crc_yhf^=(u8)lc&0xFF;
							
							for(i=0;i<8;i++) 															//主站随机数
							{
								SPI2_ReadWrite(Zz_Random1[i]);
								crc_yhf^=Zz_Random1[i];
							}	
							for(i=0;i<8;i++) 															//主站随机数反码
							{
								SPI2_ReadWrite(~Zz_Random1[i]);
								crc_yhf^=(~Zz_Random1[i]);
							}	
							
							for(i=0;i<nu;i++)
							{
								SPI2_ReadWrite(Cer_data[i]);
								crc_yhf^=Cer_data[i];
							}
							nu=0;
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);
							crc_yhf=0;					//很重要
							SSN_H();						//拉高片选
							spi_flag=Ter_WriteRead_Spi1();
						}
						if(succ_flag_spi)
						{
							succ_flag_spi=0;
							for(i=0;i<Spi_Length1;i++)
							{
								Cer_data[i]=Spi_Recv1[i];
							}
							nu=Spi_Length1;
							lc=nu-1;
							spi_flag=0;
							while(spi_flag!=3)											//
							{
								SSN_L();																		   //拉低片选
								SPI2_ReadWrite(0x55);	
								SPI2_ReadWrite(0x80);
								crc_yhf^=0x80;	
								SPI2_ReadWrite(0x08);
								crc_yhf^=0x08;	
								SPI2_ReadWrite(0x00);
								crc_yhf^=0x00;	
								SPI2_ReadWrite(Cer_data[nu-1]);
								crc_yhf^=Cer_data[nu-1];	
								
								SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
								crc_yhf^=(u8)(lc>>8)&0xFF;
								SPI2_ReadWrite((u8)lc&0xFF);
								crc_yhf^=(u8)lc&0xFF;
								
								for(i=0;i<lc;i++)
								{
									SPI2_ReadWrite(Cer_data[i]);
									crc_yhf^=Cer_data[i];
								}
								crc_yhf=~crc_yhf;
								SPI2_ReadWrite(crc_yhf);
								crc_yhf=0;					//很重要
								SSN_H();						//拉高片选
								spi_flag=Ter_WriteRead_Spi1();
							}
							if(succ_flag_spi)
							{
								succ_flag_spi=0;
								lc=nu-79;
								spi_flag=0;
								while(spi_flag!=3)											//
								{
									SSN_L();																		   //拉低片选
									SPI2_ReadWrite(0x55);	
									SPI2_ReadWrite(0x80);
									crc_yhf^=0x80;	
									SPI2_ReadWrite(0x22);
									crc_yhf^=0x22;	
									
									for(i=0;i<8;i++)
									{
										SPI2_ReadWrite(Cer_data[i]);
										crc_yhf^=Cer_data[i];
									}
									SPI2_ReadWrite(0x00);
									crc_yhf^=0x00;	
									
									SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
									crc_yhf^=(u8)(lc>>8)&0xFF;
									SPI2_ReadWrite((u8)lc&0xFF);
									crc_yhf^=(u8)lc&0xFF;
									
									for(i=0;i<lc;i++)
									{
										SPI2_ReadWrite(Cer_data[8+i]);
										crc_yhf^=Cer_data[8+i];
									}
									nu=0;
									crc_yhf=~crc_yhf;
									SPI2_ReadWrite(crc_yhf);
									crc_yhf=0;					//很重要
									SSN_H();						//拉高片选
									spi_flag=Ter_WriteRead_Spi1();
								}
								if(succ_flag_spi)
								{
									succ_flag_spi=0;
									Ter_Re_WgZz1(USARTxChannel,0x9000,0x71);
								}
								else
								{
									succ_flag_spi=0;
									Ter_Re_WgZz1(USARTxChannel,0x9097,0x71);
								}
							}
						}
					}				
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x71,0x9108);
				break;
			case 0x72:
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //安全信息长度
				idx=BackstageProtocol1.DataBuffer[9];     																							//证书标识
				if(BackstageProtocol1.DataBuffer[10]>BackstageProtocol1.DataBuffer[11])									//总帧数大于当前帧序号，说明还有后续
				{
					for(i=0;i<num-3;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-3);
				}
				else		//总帧数等于当前帧序号，最后一帧
				{
					for(i=0;i<num-3;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-3);
					spi_flag=0;
					while(spi_flag!=3)											//
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x24);
						crc_yhf^=0x24;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						
						SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(nu>>8)&0xFF;
						SPI2_ReadWrite((u8)nu&0xFF);
						crc_yhf^=(u8)nu&0xFF;
						
						for(i=0;i<nu;i++)
						{
							SPI2_ReadWrite(Cer_data[i]);
							crc_yhf^=Cer_data[i];
						}
						nu=0;
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();						//拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						Ter_Re_WgZz1(USARTxChannel,0x9000,0x73);
					}
					else
					{
						succ_flag_spi=0;
						Ter_Re_WgZz1(USARTxChannel,0x9097,0x73);
					}
				}
				break;
			case 0x74:     		//主站提取终端证书
				spi_flag=0;
				while(spi_flag!=3)											//
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);	
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;	
					SPI2_ReadWrite(0x30);
					crc_yhf^=0x30;	
					SPI2_ReadWrite(0x01);
					crc_yhf^=0x01;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;	
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();						//拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Cer_len=Spi_Length1;    //证书的总长度
					for(i=0;i<Spi_Length1;i++)
					{
						Cer_data[i]=Spi_Recv1[i];
					}
					frame_total=Cer_len/200;
					if((Cer_len-frame_total*200)>0)
						frame_total+=1;				//总帧数		
					if(frame_total==0)   	  //证书长度小于200
					{
						frame_total=1; 
						Ter_Cer_zz(USARTxChannel,1,frame_total,Cer_len);
					}
					else
					{
						for(i=1;i<=frame_total;i++)
						{
							if(i<frame_total)
							{
								Ter_Cer_zz(USARTxChannel,i,frame_total,200);
							}
							else
							{
								Ter_Cer_zz(USARTxChannel,i,frame_total,Cer_len-200*(i-1));
							}
						}
					}
				}
				break;	
			case 0x76:
				break;
			default:
				Re_error_msg(USARTxChannel,0x01,0x1F,0x9106);
				break;
		}
	}
	else if(datdone_serial1==3)
	{
		lc=8+8+16+Length_serial1-2;											//需要解密的数据长度
		spi_flag=0;
		while(spi_flag!=3)										
		{
			SSN_L();																		   //拉低片选
			SPI2_ReadWrite(0x55);	
			SPI2_ReadWrite(0x80);
			crc_yhf^=0x80;	
			SPI2_ReadWrite(0x2C);
			crc_yhf^=0x2C;	
			SPI2_ReadWrite(0x62);
			crc_yhf^=0x62;	
			SPI2_ReadWrite(0x02);
			crc_yhf^=0x02;	
			SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//总长度
			crc_yhf^=(u8)(lc>>8)&0xFF;
			SPI2_ReadWrite((u8)lc&0xFF);
			crc_yhf^=(u8)lc&0xFF;
			for(i=0;i<8;i++)
			{
				SPI2_ReadWrite(Cer_ID[i]);
				crc_yhf^=Cer_ID[i];
			}
			for(i=0;i<8;i++)
			{
				SPI2_ReadWrite(Ter_Random1[i]);
				crc_yhf^=Ter_Random1[i];
			}
			for(i=0;i<8;i++)
			{
				SPI2_ReadWrite(Ter_Random1[i]);
				crc_yhf^=Ter_Random1[i];
			}
			for(i=0;i<8;i++)
			{
				SPI2_ReadWrite(~Ter_Random1[i]);
				crc_yhf^=~Ter_Random1[i];
			}
			for(i=0;i<Length_serial1-2;i++)
			{
				SPI2_ReadWrite(BackstageProtocol1.DataBuffer[6+i]);
				crc_yhf^=BackstageProtocol1.DataBuffer[6+i];
			}
			crc_yhf=~crc_yhf;
			SPI2_ReadWrite(crc_yhf);
			crc_yhf=0;					//很重要
			SSN_H();						//拉高片选
			spi_flag=Ter_WriteRead_Spi1();
		}
		if(succ_flag_spi)
		{
			succ_flag_spi=0;
			if(Spi_Recv1[0]==0x3C)
			{
				for(i=0;i<Spi_Length1-5;i++)
				{
					Cer_data[nu+i]=Spi_Recv1[5+i];
				}
				nu+=(Spi_Length1-5);
//				for(i=0;i<Spi_Length1-3;i++)
//				{
//					Cer_data[nu+i]=Spi_Recv1[3+i];
//				}
//				nu+=(Spi_Length1-3);
				if(Spi_Recv1[3]>Spi_Recv1[4])									//总帧数大于当前帧序号，说明还有后续
				{
					FZ_Reback(USARTxChannel,0x45,0x9000);
				}
				else
				{
					spi_flag=0;
					while(spi_flag!=3)						
					{
						SSN_L();																		   //拉低片选
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x0A);
						crc_yhf^=0x0A;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//总长度
						crc_yhf^=(u8)(nu>>8)&0xFF;
						SPI2_ReadWrite((u8)nu&0xFF);
						crc_yhf^=(u8)nu&0xFF;
						for(i=0;i<nu;i++)
						{
							SPI2_ReadWrite(Cer_data[i]);
							crc_yhf^=Cer_data[i];
						}
						nu=0;
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//很重要
						SSN_H();						//拉高片选
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						Send_sign(USARTxChannel,Spi_Recv1);
					}
				}		
			}
		}
	}
	else if(datdone_serial1==4)
	{
		switch(BackstageProtocol1.DataBuffer[6])
		{
			case 1:
				for(i=0;i<13;i++)
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],cpu_style[i]);
				break;
			case 2:
				for(i=0;i<6;i++)
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],frequence[i]);
				break;
			case 3:
				for(i=0;i<3;i++)
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Constpara2[i]);	
				break;
			case 4:
				for(i=0;i<12;i++)
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],communcation_modle[i]);
				break;	
			case 5:
				for(i=0;i<8;i++)
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Constpara5[i]);
				break;
			case 6:
				for(i=0;i<7;i++)
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Constpara4[i]);
				break;
			case 7:
				for(i=0;i<24;i++)
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Constpara9[i]);
				break;
			case 8:
				for(i=0;i<7;i++)
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Constpara7[i]);
				break;
			case 9:
				spi_flag=0;
				while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(ID_Spi[i]);
						crc_yhf^=ID_Spi[i];	
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();																		   //拉高片选
				  spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Spi_Recv1[i]);					
				}				
				break;
			case 10:
				spi_flag=0;
				while(spi_flag!=3)
				{
					SSN_L();																		   //拉低片选
					SPI2_ReadWrite(0x55);													     //是否需要，测试中没有使用帧头和校验
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Sec_ca1[i]);
						crc_yhf^=Sec_ca1[i];	
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//很重要
					SSN_H();																		   //拉高片选
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Spi_Recv1[0]);
				}
				break;			
		}
	}
	if(y_104==0)
		Active_Upload1(USARTxChannel);	
	else
		Active_Upload(USARTxChannel);
	renzheng_flag=1;
}

void Send_Again(u8 USARTxChannel)	//重发
{
	u16 i=0;
	resend_num++;
	if(resend_num>2)	  //之前已经重发两次了，都没有成功，这一次要重启
	{
		resend_num=0;	    //次数清0
		delay_time_101=0;	//计数清0
		timer_enable=0;	  //定时器关闭
		Common_MW(USARTxChannel,0xC9);			//请求链路状态，终端为启动站	
		flag_succ=0;
		return;
	}
	for(i=0;i<send_buffer_len;i++)
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],send_buffer[i]); //上一帧的备份
	delay_time_101=0;
	timer_enable=1;
}


