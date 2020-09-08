//###########################################################################
//
// 101��104��Լ���ܽ���
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
//Super_Sign��0��11�ֱ��ʾ12�����ȼ������ԽС�����ȼ�Խ��
//�ұ�ŵ�ֵ��һ����ʾ����������ȼ���ĳ���׶�
//Super_Sign[0]=0,��ʾ������ȼ�������û�з�����Super_Sign[0]=1��ʾ������ȼ������鷢���ˣ����ڳ�ʼ�׶�
//Super_Sign[0]=2,��ʾ�����ȼ����鴦��ִ�н׶Σ�Super_Sign[0]=3��ʾ���ڽ����׶�
//ÿ�����ȼ��Ľ׶β�һ�����׶�����Ҳ��һ��
//��������ȼ������鷢���ˣ���ô��־λ����λ�����Ǻ�������ȼ��ַ����ˣ���ô�����ȼ�����λ
//ÿ�ζ����ж�������飬�и����ȼ��ľ���ִ�и����ȼ��ģ����ܵ����ȼ�����û��ִ��
/*-----------------------��ƽ�����ȼ�----------------------------*/
/*---------------------һ���������ȼ�-----------------------------*/
//   ���ȼ�                Ӧ������
//      1                 ��ʼ������                  Super_Sign[0]
//      2                ���٣���ʼ����               Super_Sign[1]
//      3                    ң��                     Super_Sign[2]
//      4             �¼����棨״̬���仯TCOS��      Super_Sign[3]
//      5               ���٣��ǳ�ʼ����              Super_Sign[4]
/*---------------------�����������ȼ�-----------------------------*/
//   ���ȼ�                Ӧ������
//      1                  ʱ��ͬ��                   Super_Sign[5]
//      2                  ��������                   Super_Sign[6]
//      3               ģ������Խ����                Super_Sign[7]
//      4                  ��λ����                   Super_Sign[8]
//      5                  �ļ��ٻ�                   Super_Sign[9]
//      6                  �ļ�����                   Super_Sign[10]
//      7                  �����ٻ�                   Super_Sign[11]
//      8                  ������д                   Super_Sign[12]
//      �����						 ������Ź���               Super_Sign[13]



/*-----------------------ƽ�����ȼ�----------------------------*/
//   ���ȼ�                Ӧ������																						�����ϴ�
//      1                 ��ʼ������                  Super_Sign[0]
//      2                ���٣���ʼ����               Super_Sign[1]
//      3                    ң��                     Super_Sign[2]
//      4             �¼����棨״̬���仯TCOS��      Super_Sign[3]						ң�ű�λ
//      5               ���٣��ǳ�ʼ����              Super_Sign[4]
//      6                  �����¼�                   Super_Sign[5]						�����¼�
//      7                  ʱ��ͬ��                   Super_Sign[6]
//      8                  �仯ң��                		Super_Sign[7]						ң��仯
//      9                  ��λ����                   Super_Sign[8]
//      10                 �ļ��ٻ�                   Super_Sign[9]
//      11                 �ļ�����                   Super_Sign[10]
//      12                 �����ٻ�                   Super_Sign[11]
//      13                 ������д                   Super_Sign[12]
//      14                 ��·����                   Super_Sign[14]
//      �����						 ������Ź���               Super_Sign[13]

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
u8 zz_ok=0,wg_ok=0,yw_ok=0;	             //��վ�����ء���ά��֤��־λ
u8 Wg_ca1[6]={0x80,0x16,0x00,0x80,0x00,0x08};				//������֤оƬ��ȡ��1
u8 Wg_ca2[6]={0x80,0x18,0x00,0x05,0x00,0x40};				//������֤оƬ��ȡ��2
u8 ID_Spi[8]={0x00,0xb0,0x99,0x05,0x00,0x02,0x00,0x08};
u8 Get_Ter_Random[6]={0x00,0x84,0x00,0x08,0x00,0x00};
u8 Sec_ca1[6]={0x80,0x1A,0x00,0x00,0x00,0x00};
u8 Sec_ca2[6]={0x00,0x84,0x00,0x08,0x00,0x00};	
u8 Recv_JM[4]={0x80,0x2C,0x60,0x01};
u8 Send_JM[4]={0x80,0x26,0x60,0x01};
u8 Read_Cer_len[8]={0x00,0xb0,0x81,0x00,0x00,0x02,0x00,0x02};
u8 decrypt[16]={0};              //�����ļ�ժҪֵ
u8 ucaFlashBuf[1025]={0};
u8 Super_Sign[15]={0};
u8 Callend_Flag=0;
u8 index_which = 0;    //������ʾ��ǰ��������ĸ�Ŀ¼
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
u8 Constpara_Tag[10]={4,4,4,4,4,4,4,4,4,4};   //���в���TAG����
u8 Constpara_Len[10]={4,4,4,8,9,5,8,8,25,13};	//���в������ݳ���
static u8 Runpara_Tag[32]={0};        //���в���TAG����
static u8 Fixpara_Tag[64]={0,0,0,0,0,0,0,0,1,38,
                           38,1,1,1,38,38,1,1,38,38,
                           38,38,1,38,38,1,38,38,1,1,
                           1,38,1,38,1,38,38,1,38,38,
                           1,38,38,0,0,0,0,0}; //��ֵ����TAG����
static u8 Runpara_Len[32]={0};	      //���в������ݳ���
static u8 Fixpara_Len[64]={0};	      //��ֵ�������ݳ���
static u8 Runpara_State[32]={0};      //���в���״̬
static u8 Fixpara_State[64]={0};      //��ֵ����״̬
static u8 Constpara_State[10]={0};	
static u8 Params_total;
struct Params_Addr
{
	u8 params_addr[64];
	u8 params_num;
};
struct Params_Addr const_value,run_value,fix_value;

static volatile u8 ProtocolResolveState = 0;	// Э���������״̬��
static volatile u8 succ_flag_spi=0;
//just for 104
u8 y_104 =0 ;	//if y_104=0,the protocol is 101;if y_104=1,the protocol is 104
static u8 datdone = 0;
static u16 Recv_Num=0;        //ʵ��I֡�������ݵĴ�������������
static u16 Send_Num=0;				//ʵ��I֡�������ݵĴ�������������
static u16 Recv_NumBak=0;			//ʵ��I֡�������ݵĴ������ͻ��˽���
static u16 Send_NumBak=0;			//ʵ��I֡�������ݵĴ������ͻ��˷���
static u8 diff_flag=0;				//������ʾ�������кͽ�������ֻ���12�ĵ�һ��
static u8 call_num1=0;
static u8 Informat_addr[3]={0};

//101 & 104
static u8 channel = 0;			                  // ����ͨ��
static u8 num_flag=0;                         //��ǽ���֤��Ĵ���
static u8 datdone_serial1=0,YY_Style1=0,BW_Len1=0;
static u8 Wg_Random1[8]={0};
static u8 Zz_Random1[8]={0};				          //��վ������������Ҫ���棬��������
static u8 Ter_Random1[8]={0};
static u8 Ter_Random2[8]={0};
static u8 Wg_Sign1[70]={0};
static u8 Msg_Safty1[80]={0};
static u8 Msg_Safty_sj[80]={0};		    //Զ��������ȫ��Ϣ
static u8 Spi_Recv1[1024]={0};
static u8 BW_Before_JM1[256]={0};
static u8 Cer_data[1024]={0};				  //֤�������֤��
static u8 Cer_ID[8]={0};						  //֤�������ID
static u8 chip_ID[8]={0};						  //��ȫоƬID
static u8 pubkey[64]={0};						  //оƬ��Կ
static u8 frame_num=0;							  //��ǰ֡���
static u8 frame_total=0;							//��֡��
static u8 flag_succ=0;                //��ʼ����ɱ�ǣ�1��ɣ�0δ���
static u8 resend_num=0; 	            //�ط�����
static u8 Test_Sign[7]={0};
static u8 Super_Sign_bak[15]={0};
static u8 send_buffer[512]={0};	      //���ͻ��棬ÿ�η��Ͷ�Ҫ���棬�ȵ��´ν��ճɹ������
static u8 call_num=0;
static u8 jm=0;
static u8 params_flag=0;
static u8 yx_data[10][10]={0};	      //ң�Ż�������
static u8 yx_data_flag=0;	            //�Ƿ���ң�Ż���
static u8 yx_data_num=0;	            //ң�Ż������
static u8 yk_state=0;	                //������ʾң�صĳ�ʼ״̬��ѡ��״̬��ִ��/����״̬,0:��ʼ״̬��1��ѡ��״̬��2������״̬��3��ִ��״̬
static u8 FCB_Flag_Main=0;    				//��վ���ĵ�FCBλ
static u8 FCB_Flag_Slave=0;   				//�ն˱��ĵ�FCBλ
static u8 List_Name1[50]={0};					//�ļ���
static u8 List_Num1=0;								//�ļ�������
static u8 List_Name11[50]={0};				//Ŀ¼��
static u8 List_Num11=0;								//Ŀ¼������
static u8 Finish_Flag1=0;         		//д�ļ��ɹ���־
static u8 Rec_Error_Flag1=0;        	//д�ļ������־

static u16 Spi_Sw1=0; 								//spi���ص�״̬��
static u16 Spi_Length1=0;             //spi���صĳ���
static u16 Cer_len=0;       				  //�ն�֤���ܳ���
static u16 ADDR=0;                    //��ַ��A
static u16 Asdu_addr=0;               //ASDU��ַ
static u16 Object_addr = 0;           //��Ϣ�����ַ
static u16 send_buffer_len=0;
static u16 remain1=0,Length_serial1=0;
static u16 RxCounter_serial1=0;
static u16 MsgSafty_Len1=0;
static u16 SN_Num1=0;							    //��ǰ��ֵ������ʼĬ��Ϊ0
static u32 Send_Doc_Num1=0;           //�����ļ������ֽ���
static u32 Rec_Doc_Num1=0;					  //д���ļ������ֽ���
static u32 Rec_num_101=0;
static u32 segment_num_last=0;
static u32 A = 0x67452301, B = 0xefcdab89, C = 0x98badcfe, D = 0x10325476;
static u32 mid1 = 0, mid2 = 0, mid3 = 0, mid4 = 0;
static u32 a, b, c, d, x[16];
static u32 ulSampleIndex = 0;
static u32 ulDataIndex = 0;
static u32 ulReadCnt = 0;


//just for 104
/*************************U��֡Ӧ����****************************/
void IEC104_Answer_U(u8 link,u8 USARTxChannel)//�̶�֡	��ַ��1���ֽ�
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);   		//���ȸ��ֽ�
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0C);   		//���ȵ��ֽ�
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);					//�������ͣ�������
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);				//Ӧ�����ͣ�ֻ��ԭʼ����
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x6);				//ԭʼ���ĳ���
	sum+=0x6;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);				//Ӧ����
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

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);					//��ȫ��Ϣ�峤��
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);				//У����
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);				//֡β
}

void IEC104_Answer_S(u8 USARTxChannel)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);   		//���ȸ��ֽ�
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0C);   		//���ȵ��ֽ�
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);					//�������ͣ�������
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);				//Ӧ�����ͣ�ֻ��ԭʼ����
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x6);				//ԭʼ���ĳ���
	sum+=0x6;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);				//Ӧ����
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

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);					//��ȫ��Ϣ�峤��
	sum+=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	sum+=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);				//У����
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);				//֡β
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
		SSN_L();																	 	 //�ٴ�����׼������״̬��
		delay_flag=0;
		while(delay_flag==0)												 //��ʱ��ʱ����
		{
			if(SPI2_ReadWrite(0)==0x55)
			{
				crc_yhf=0;
				Spi_Sw1  = ((u16)SPI2_ReadWrite(0)&0xFF)<<8;
				crc_yhf^=((u8)(Spi_Sw1>>8)&0xFF);
				Spi_Sw1 |= SPI2_ReadWrite(0)&0xFF;								 //����״̬��
				crc_yhf^=((u8)Spi_Sw1&0xFF);
				Spi_Length1 = ((u16)SPI2_ReadWrite(0)&0xFF)<<8;
				crc_yhf^=((u8)(Spi_Length1>>8)&0xFF);
				Spi_Length1 |= SPI2_ReadWrite(0)&0xFF;						 //���շ����ֽ���
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
					delay_flag=1;				 									 //��֤����ֻ������ǰѭ��	
					spi_flag  =3;											
					succ_flag1++;	
					succ_flag=succ_flag1;
					SSN_H();	
				}
				else if(Spi_Sw1==0x6A90) 
				{
					delay_flag =1;
					succ_flag  =3;											
					spi_flag1++;													//���κ��Զ�����	
					spi_flag=spi_flag1;
					if(spi_flag1==3)  spi_flag1=0;
				}
				else if (Spi_Sw1==0x9000)
				{
					delay_flag =1;
					succ_flag  =3;			
					spi_flag   =3;												//���ճɹ�Ҳ������san��ѭ��		
					succ_flag_spi=1;
				}
				else 
				{
					delay_flag =1;
					succ_flag  ++;			
					spi_flag   =3;												//���ճɹ�Ҳ������san��ѭ��		
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
					spi_flag   =3;												//��ʱ��ʱ����san��ѭ��
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
		SPI2_ReadWrite(0x55);														//֡ͷ
		for(i=0;i<4;i++)															  //����
		{
			SPI2_ReadWrite(Send_JM[i]);	
			crc_yhf^=Send_JM[i];
		}
		SPI2_ReadWrite((u8)(lc>>8)&0xFF);								//�ܳ���
		crc_yhf^=(u8)(lc>>8)&0xFF;
		SPI2_ReadWrite((u8)lc&0xFF);
		crc_yhf^=(u8)lc&0xFF;
		for(i=0;i<8;i++) 															  //��վ�����
		{
			SPI2_ReadWrite(Zz_Random1[i]);
			crc_yhf^=Zz_Random1[i];
		}	
		for(i=0;i<8;i++) 															  //��վ���������
		{
			SPI2_ReadWrite(~Zz_Random1[i]);
			crc_yhf^=(~Zz_Random1[i]);
		}	
		SPI2_ReadWrite(0x00);														//Ӧ������
		crc_yhf^=0x00;
		SPI2_ReadWrite(Send_len);												//���ĳ���
		crc_yhf^=Send_len;
		for(i=0;i<Send_len;i++)												  //��������
		{
			SPI2_ReadWrite(BW_Before_JM1[i]);	
			crc_yhf^=BW_Before_JM1[i];
		}
		SPI2_ReadWrite(0x00);													   //��������ȸ��ֽ�
		crc_yhf^=0x00;		
		SPI2_ReadWrite(0x00);														 //��������ȵ��ֽ�
		crc_yhf^=0x00;		
		crc_yhf=~crc_yhf;
		SPI2_ReadWrite(crc_yhf);	
		crc_yhf=0;
		SSN_H();											                   //����Ƭѡ
		spi_flag=Ter_WriteRead_Spi1();
	}
	return succ_flag_spi;
}

void Receive_Doc_101(void)          //������յ����ļ�����
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
		Rec_Error_Flag1=1;																				//��ǰ֡�Ķκ�Ӧ�õ���ǰ�漸�η����ļ������ֽ���֮�ͣ��������������λ�����־	
	}
	segment_num_last=segment_num;

	for(i=0;i<(Decryption.DataBuffer[1]-number);i++)								//�����ν������ݴ洢����
	{
		//ucaFlashBuf[i]=Decryption.DataBuffer[26+i];							//ucaFlashBufֻ��100�ֽڣ������								
		pbuffer[i]=Decryption.DataBuffer[26+i];						        //ÿ�ν��յ������ȴ浽pbuffer[]��ȥ
		sum+=Decryption.DataBuffer[26+i];
	}
	if(sum!=Decryption.DataBuffer[Decryption.DataBuffer[1]+number1])	//���У��ֵ���ԣ������־λ��һ
	{
		Rec_Error_Flag1=2;
	}
	if(Rec_Error_Flag1==0)//˵��û������Ĵ���
	{
		if(Decryption.DataBuffer[25]==0)																			//�޺��������μ�Ϊ���һ֡����ô��Ҫ�ж����ֽ����Բ���
		{
			mid=Rec_num_101;
			Rec_num_101+=Decryption.DataBuffer[1]-number;																  //ֱ�����ν������ݸ���֮��	
			if(Rec_num_101!=Rec_Doc_Num1)
			{
				Rec_Error_Flag1=3;																	//���յ�����֡���ֽ�֮����д�ļ�����֡������������ֽڣ����ļ����ȣ���ĳ��ȶԲ���
				Rec_num_101=mid;
			}
			else
			{
				CS2BZ=0;
				while(1)
				{
					if(Flash_Writeflag == 0)
					{
						SPI_Flash_Write(pbuffer,FLASH_KZQUPDATE_ADDR+mid,Decryption.DataBuffer[1]-number);				//��ַΪ���յĶκţ���Ϊ���Ǵ洢������׵�ַ��0��ʼ��������κ����Ӧ������Ϊÿ�ν��յ����ֽ���		
						break;
					}
				}	
				Rec_num_101=0;																																	//���������̬���������һ����Ҫ��0
			}				
		}
		else //֮ǰ֡����
		{
			CS2BZ=0;
			while(1)
			{
				if(Flash_Writeflag == 0)
				{
					SPI_Flash_Write(pbuffer,FLASH_KZQUPDATE_ADDR+Rec_num_101,Decryption.DataBuffer[1]-number);				//��ַΪ���յĶκţ���Ϊ���Ǵ洢������׵�ַ��0��ʼ��������κ����Ӧ������Ϊÿ�ν��յ����ֽ���
					break;
				}
			}	
			Rec_num_101+=Decryption.DataBuffer[1]-number;																  //ֱ�����ν������ݸ���֮��	
		}	
	}
}

void FZ_Reback(u8 USARTxChannel,u8 style,u16 sign)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //��ͷ
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //��������
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],style); //Ӧ������
	sum+=style;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x02); //��ȫ��Ϣ����
	sum+=0x02;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(sign>>8)&0xFF);	                //��Ӧ���
	sum+=(u8)(sign>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)sign&0xFF);	                //��Ӧ���
	sum+=(u8)sign&0xFF;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //��ȫ��Ϣ
}

void Send_Random(u8 USARTxChannel,u8 sign,u8 *ID)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0D);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //��ͷ
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //��������
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sign); //Ӧ������
	sum+=sign;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x08); //��ȫ��Ϣ����
	sum+=0x08;
	
	for(i=0;i<8;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],ID[i]);
		sum+=ID[i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //��ȫ��Ϣ
}
void Send_Ver(u8 USARTxChannel,u8 ver,u8 *rand)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0E);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //��ͷ
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //��������
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x35); //Ӧ������
	sum+=0x35;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x09); //��ȫ��Ϣ����
	sum+=0x09;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],ver);  //�汾��
	sum+=ver;
	
	for(i=0;i<8;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],rand[i]);
		sum+=rand[i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //��ȫ��Ϣ
}


void Send_Ter_ID(u8 USARTxChannel)
{
	u8 i=0,sum=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x1D);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //��ͷ
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //��������
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x37); //Ӧ������
	sum+=0x37;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x18); //��ȫ��Ϣ����
	sum+=0x18;
	
	for(i=0;i<24;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Constpara9[i]);
		sum+=Constpara9[i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //��ȫ��Ϣ
}

void Send_sign(u8 USARTxChannel,u8 *rand)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x45);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //��ͷ
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //��������
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x3D); //Ӧ������
	sum+=0x3D;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //��ȫ��Ϣ����
	sum+=0x40;
	
	for(i=0;i<64;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],rand[i]);
		sum+=rand[i];
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //��ȫ��Ϣ
}

void Send_Ter_Cer(u8 USARTxChannel,u8 frame_num,u8 frame_total,u8 num)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],num+8);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //��ͷ
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x40); //��������
	sum+=0x40;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x43); //Ӧ������
	sum+=0x43;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],num+3); //��ȫ��Ϣ����
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
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //��ȫ��Ϣ��β
}

void Ter_Cer_zz(u8 USARTxChannel,u8 frame_num,u8 frame_total,u8 num)
{
	u8 i=0,sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],num+8);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //��ͷ
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x01); //��������
	sum+=0x01;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x75); //Ӧ������
	sum+=0x75;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],num+3); //��ȫ��Ϣ����
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
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //��ȫ��Ϣ��β
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
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x80);									//���ı�ʶ��֪���Ƿ���ȷ
		sum+=0x80;
	}
	else
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
		sum+=0x00;
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],yy_sty);									//Ӧ�����ͣ��ն�ȷ�ϲ�����������֤
	sum+=yy_sty;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(msg_len>>8)&0xFF);//��ȫ��Ϣ�峤��
	sum+=(u8)(msg_len>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)msg_len&0xFF);	
	sum+=(u8)msg_len&0xFF;
	if(yy_sty==0x55||yy_sty==0x61)
	{
		for(i=0;i<msg_len;i++)																																							//��ȫ��Ϣ������
		{
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Spi_Recv1[i]);
			sum+=Spi_Recv1[i];
		}	
	}
	else
	{
		for(i=0;i<msg_len-1;i++)																																							//��ȫ��Ϣ������
		{
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Spi_Recv1[i]);
			sum+=Spi_Recv1[i];
		}	
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sign);	                //ǩ����Կ��ʶ
		sum+=sign;
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);									//У���
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);									//֡β
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
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x80);									//���ı�ʶ��֪���Ƿ���ȷ
		sum+=0x080;
	}
	else
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
		sum+=0x00;
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],yy_sty);									//Ӧ�����ͣ��ն���Ӧ����
	sum+=yy_sty;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],00);//��ȫ��Ϣ�峤��
	sum+=00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x02);	
	sum+=0x02;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(sign>>8)&0xFF);	                //��Ӧ���
	sum+=(u8)(sign>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)sign&0xFF);	                //��Ӧ���
	sum+=(u8)sign&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);									//У���
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);									//֡β

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
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x48);									//���ı�ʶ��֪���Ƿ���ȷ
	sum+=0x48;
	for(i=0;i<length;i++)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Send[i]);	
		sum+=Send[i];
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);	
}

//��ͨ�÷��ͺ���
void Common_MW(u8 USARTxChannel,u8 ca)
{
	u8 sum=0,sum1=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x0C);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);									//��������
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);									//Ӧ������
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x06);									//Ӧ�����ͳ���
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
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);									//��ȫ��Ϣ����
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);		
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);									//��ȫ��Ϣ����
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7);		
}

void Re_error_msg(u8 USARTxChannel,u8 sty,u8 style,u16 sign)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xEB); //��ͷ
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sty); //��������
	sum+=sty;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],style); //Ӧ������
	sum+=style;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x00);
	sum+=0x00;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x02); //��ȫ��Ϣ����
	sum+=0x02;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)(sign>>8)&0xFF);	                //��Ӧ���
	sum+=(u8)(sign>>8)&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(u8)sign&0xFF);	                //��Ӧ���
	sum+=(u8)sign&0xFF;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xD7); //��ȫ��Ϣ
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_EI_NA_1;
		sum+=M_EI_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=INIT;
		sum+=INIT;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------��ʼ��ԭ��------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	return i;		
}

u8 GeneralCall_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_IC_NA_1;
		sum+=C_IC_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------�ٻ��޶���------------*/
		BW_Before_JM1[i++]=QOI_GenCall;
		sum+=QOI_GenCall;
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
	
	buffer[Measure101Para.para.far_positoin] = FAR_IN6^0x01;//Զ��
	buffer[Measure101Para.para.local_positoin] = LOCAL_IN7^0x01;//�͵�
	if(DYXH1 == 1 && DYXH2 == 1)	
	{
	  buffer[Measure101Para.para.AC_loss] = 0x01;               //����ʧ��澯
	}
	else
	  buffer[Measure101Para.para.AC_loss] = 0;
	buffer[Measure101Para.para.Bat_low] = KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage;//���Ƿѹ�澯
	buffer[Measure101Para.para.low_airlock] = 0;
	buffer[Measure101Para.para.lock_on] = Switchon_Lock | Switchon_Lock1 | Switchon_Lock2; //������բ
	buffer[Measure101Para.para.lock_off] = Switchoff_Lock | Switchoff_Lock1;                //������բ  
	buffer[Measure101Para.para.break_on] = HW_IN2^0x01;//��λ
	buffer[Measure101Para.para.break_off] = FW_IN1^0x01;//��λ
	//buffer[Measure101Para.para.break_off] = 0x01;//��λ
	buffer[Measure101Para.para.wcn_state] = WCN_IN3^0x01;//δ����
	//buffer[Measure101Para.para.wcn_state] = 0x01;//δ����
	buffer[Measure101Para.para.max_current1_alarm] = 0;//10
	buffer[Measure101Para.para.max_current2_alarm] = 0;
	buffer[Measure101Para.para.max_load_alarm] = KZQMeasureData.para.AlarmFlag.bit.max_load;//�����ɸ澯
	buffer[Measure101Para.para.zero_current1_alarm] = KZQMeasureData.para.AlarmFlag.bit.zero_max_current;
	buffer[Measure101Para.para.ground_current_alarm] = KZQMeasureData.para.AlarmFlag.bit.ground;//����ӵع��ϸ澯
	buffer[Measure101Para.para.max_current1_protec] = KZQMeasureData.para.ProtectFlag1.bit.fast_off;//����I��
	buffer[Measure101Para.para.max_current2_protec] = KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off;//����II��
	buffer[Measure101Para.para.zero_current1_protec] = KZQMeasureData.para.ProtectFlag1.bit.zero_max_current;//�����������
	buffer[Measure101Para.para.zero_currentafter_protec] = 0;
	buffer[Measure101Para.para.reon] = KZQMeasureData.para.ProtectFlag2.bit.reon_act; //�غ�բ
	buffer[Measure101Para.para.max_currentafter_protec] = KZQMeasureData.para.ProtectFlag1.bit.max_current_after;//��������ٱ���//20
	buffer[Measure101Para.para.max_voltage] = KZQMeasureData.para.ProtectFlag2.bit.max_voltage;
	buffer[Measure101Para.para.high_freq] = KZQMeasureData.para.ProtectFlag2.bit.max_freq;
	buffer[Measure101Para.para.low_freq] = KZQMeasureData.para.ProtectFlag2.bit.low_freq;
	if(ProtectFlag==1 || (KZQMeasureData.para.AlarmFlag.word & 0x01ff))  //�¹���
	  buffer[Measure101Para.para.event_all] = 0x01;
	else
		 buffer[Measure101Para.para.event_all] = 0;
	buffer[Measure101Para.para.max_current3_protec] = KZQMeasureData.para.ProtectFlag1.bit.max_current; //����III��
	if(KZQMeasureData.para.SelfCheck.word & 0x3f0f)
	{
	  buffer[Measure101Para.para.break_err] = 0x01;                                           //װ���쳣�澯
	}
	else
	  buffer[Measure101Para.para.break_err] = 0; 
	
	 if(AutoswitchSet.para.segment_contact_mode == 0x01)	                  //�ֶε㹤��ģʽ
	{
	  buffer[Measure101Para.para.segment_mode] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.segment_mode] = 0;                              
  if(AutoswitchSet.para.segment_contact_mode == 0x02)	                  //����㹤��ģʽ
	{
	  buffer[Measure101Para.para.contact_mode] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.contact_mode] = 0;                           
	if(ProtectTimeout.para.novoltage_value_A == 1)	                      //��Դ����ѹ
	{
	  buffer[Measure101Para.para.novoltage_A] = 0x01;       
	}
	else
	  buffer[Measure101Para.para.novoltage_A] = 0;    
  if(ProtectTimeout.para.novoltage_value_B == 1)	                      //���ز���ѹ
	{
	  buffer[Measure101Para.para.novoltage_B] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.novoltage_B] = 0;    
	if(KZQMeasureData.para.ProtectFlag1.bit.loss_power)	                  //ʧѹ��բ
	{
	  buffer[Measure101Para.para.powerloss_off] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.powerloss_off] = 0;    
	buffer[Measure101Para.para.bat_active] = KZQMeasureData.para.AlarmFlag.bit.bat_active;//��ػ
	
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_SP_NA_1;
		sum+=M_SP_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0xA8;
		sum+=0xA8;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=INTROGEN;
		sum+=INTROGEN;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ң��------------*/
		 for(j=0;j<YX_Number;j++)
		{
			BW_Before_JM1[i++]=buffer[j];
			sum+=buffer[j];	
		}

		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
		BW_Before_JM1[i++]=0x16;	
	}
	else
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_SP_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0xA8;
		sum+=0xA8;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=INTROGEN;
		BW_Before_JM1[i++]=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x01;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;
		/*-----------ң��------------*/
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
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_ME_NC_1;
		sum+=M_ME_NC_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x97;	//ʵ��ʹ��
		sum+=0x97;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=INTROGEN;
		sum+=INTROGEN;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ��ң��ֵ------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=(u8)(Send_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Send_Num>>7)&0xFF;
		BW_Before_JM1[i++]=(u8)(Recv_Num<<1)&0xFE;
		BW_Before_JM1[i++]=(u8)(Recv_Num>>7)&0xFF;
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_ME_NA_1;         // ����ֵ����һ��ֵ
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x97;	//ʵ��ʹ��
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=INTROGEN;
		BW_Before_JM1[i++]=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		/*-----------��Ϣ�����ַ��ң��ֵ------------*/
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

u8 getfloat_yc_state(void)  // 20%�ı仯
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

u8 getint_yc_state(void)  // 20%�ı仯
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
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_ME_NC_1;       // ����ֵ���̸�����
		sum+=M_ME_NC_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=yc_send.yc_num;
		sum+=yc_send.yc_num;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=SPONT;
		sum+=SPONT;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ��ң�ⱨ��------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=M_ME_NA_1;     // ����ֵ����һ��ֵ
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_IC_NA_1;
		sum+=C_IC_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTTERM;
		sum+=ACTTERM;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;	
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ң��------------*/
		BW_Before_JM1[i++]= QOI_GenCall;
		sum+= QOI_GenCall;	
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=ti;
		sum+=ti;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=reason;
		sum+=reason;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=Object_addr&0xFF;
		sum+=(Object_addr&0xFF);
		BW_Before_JM1[i++]=Object_addr>>8;
		sum+=(Object_addr>>8);
		/*-----------ң��------------*/
		BW_Before_JM1[i++]=control_jm.byte;
		sum+=control_jm.byte;
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_SP_TB_1;
		sum+=M_SP_TB_1;
		/*-----------֡���޶���------------*///�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
		BW_Before_JM1[i++]=num;
		sum+=num;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=SPONT;
		sum+=SPONT;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ��ң�ű���------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=num;//ÿ��ֻ����һ��
		BW_Before_JM1[i++]=SPONT;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		/*-----------��Ϣ�����ַ��ң�ű���------------*/
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_SP_NA_1;
		sum+=M_SP_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=num;
		sum+=num;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=SPONT;
		sum+=SPONT;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ��ң�ű���------------*/
		for(j=0;j<yx_data_num;j++)
		{
			BW_Before_JM1[i++]=yx_data[j][8]+1;
			sum+=(yx_data[j][8]+1);
			BW_Before_JM1[i++]=0;
			sum+=0;
			BW_Before_JM1[i++]=yx_data[j][9];
			sum+=yx_data[j][9];	
		}
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=num;//ÿ��ֻ����һ��
		BW_Before_JM1[i++]=SPONT;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		/*-----------��Ϣ�����ַ��ң�ű���------------*/
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_CS_NA_1;
		sum+=C_CS_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------ʱ��------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_CS_NA_1;
		sum+=C_CS_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=REQ;
		sum+=REQ;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------ʱ��------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
	/*-----------��ͷ------------*/
	BW_Before_JM1[i++]=0x68;
	BW_Before_JM1[i++]=length;
	BW_Before_JM1[i++]=length;
	BW_Before_JM1[i++]=0x68;
	/*-----------������-----------*/
	BW_Before_JM1[i++]=ca;
	sum+=ca;
	/*-----------��ַ��------------*/
	BW_Before_JM1[i++]=Test_Sign[0];
	sum+=Test_Sign[0];
	BW_Before_JM1[i++]=Test_Sign[1];	
	sum+=Test_Sign[1];
	/*-----------���ͱ�ʶ------------*/
	BW_Before_JM1[i++]=C_TS_NA_1;
	sum+=C_TS_NA_1;
	/*-----------֡���޶���------------*/
	BW_Before_JM1[i++]=Test_Sign[2];
	sum+=Test_Sign[2];
	/*-----------����ԭ��------------*/
	BW_Before_JM1[i++]=ACTCON;
	sum+=ACTCON;
	BW_Before_JM1[i++]=0;
	sum+=0;
	/*-----------ASDU��ַ------------*/
	BW_Before_JM1[i++]=Test_Sign[3];
	sum+=Test_Sign[3];
	BW_Before_JM1[i++]=Test_Sign[4];
	sum+=Test_Sign[4];
	/*-----------��Ϣ�����ַ------------*/
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	/*-----------����ͼ��FBP------------*/
	BW_Before_JM1[i++]=Test_Sign[5];
	sum+=Test_Sign[5];
	BW_Before_JM1[i++]=Test_Sign[6];
	sum+=Test_Sign[6];
	/*-----------У����------------*/
	BW_Before_JM1[i++]=sum;	
	/*-----------֡β------------*/
	BW_Before_JM1[i++]=0x16;

	return i;	
}

u8 Confirm_Reset_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_RP_NA_1;
		sum+=C_RP_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------�޶���------------*/
		BW_Before_JM1[i++]=QRP_RstCall;
		sum+=QRP_RstCall;
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_CI_NA_1;
		sum+=C_CI_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------�޶���------------*/
		BW_Before_JM1[i++]=20;
		sum+=20;
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_IT_NB_1;
		sum+=M_IT_NB_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x88;
		sum+=0x88;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=PER_CYC;
		sum+=PER_CYC;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		BW_Before_JM1[i++]=0x64;
		sum+=0x64;
		/*-----------��������------------*/
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
		
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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

u8 Confirm_Enegy_CP56Time2a_BefJM(u8 length,u8 ca) //��ʱ��ĵ�������
{
	u8 sum=0,i=0,j=0;
	u8 buffer[Fix_Num];
	u16 dqms = 0;
	
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=M_IT_TC_1;
		sum+=M_IT_TC_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x88;
		sum+=0x88;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=PER_CYC;
		sum+=PER_CYC;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x09;
		sum+=0x09;
		BW_Before_JM1[i++]=0x64;
		sum+=0x64;
		/*-----------��������------------*/
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
			dqms=buffer[5]*1000%60000;  //��
			BW_Before_JM1[i++]=dqms&0xFF;
			sum+=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			sum+=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//��
			sum+=buffer[4];
			BW_Before_JM1[i++]=buffer[3];//ʱ
			sum+=buffer[3];
			BW_Before_JM1[i++]=buffer[2];//��
			sum+=buffer[2];
			BW_Before_JM1[i++]=buffer[1];//��
			sum+=buffer[1];
			BW_Before_JM1[i++]=buffer[0];//��
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
			dqms=buffer[5]*1000%60000;  //��
			BW_Before_JM1[i++]=dqms&0xFF;
			sum+=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			sum+=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//��
			sum+=buffer[4];
			BW_Before_JM1[i++]=buffer[3];//ʱ
			sum+=buffer[3];
			BW_Before_JM1[i++]=buffer[2];//��
			sum+=buffer[2];
			BW_Before_JM1[i++]=buffer[1];//��
			sum+=buffer[1];
			BW_Before_JM1[i++]=buffer[0];//��
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
			dqms=buffer[5]*1000%60000;  //��
			BW_Before_JM1[i++]=dqms&0xFF;
			sum+=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			sum+=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//��
			sum+=buffer[4];
			BW_Before_JM1[i++]=buffer[3];//ʱ
			sum+=buffer[3];
			BW_Before_JM1[i++]=buffer[2];//��
			sum+=buffer[2];
			BW_Before_JM1[i++]=buffer[1];//��
			sum+=buffer[1];
			BW_Before_JM1[i++]=buffer[0];//��
			sum+=buffer[0];
		}
		
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
			dqms=buffer[5]*1000%60000;  //��
			BW_Before_JM1[i++]=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//��
			BW_Before_JM1[i++]=buffer[3];//ʱ
			BW_Before_JM1[i++]=buffer[2];//��
			BW_Before_JM1[i++]=buffer[1];//��
			BW_Before_JM1[i++]=buffer[0];//��
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
			dqms=buffer[5]*1000%60000;  //��
			BW_Before_JM1[i++]=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//��
			BW_Before_JM1[i++]=buffer[3];//ʱ
			BW_Before_JM1[i++]=buffer[2];//��
			BW_Before_JM1[i++]=buffer[1];//��
			BW_Before_JM1[i++]=buffer[0];//��
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
			dqms=buffer[5]*1000%60000;  //��
			BW_Before_JM1[i++]=dqms&0xFF;
			BW_Before_JM1[i++]=dqms>>8;
			BW_Before_JM1[i++]=buffer[4];//��
			BW_Before_JM1[i++]=buffer[3];//ʱ
			BW_Before_JM1[i++]=buffer[2];//��
			BW_Before_JM1[i++]=buffer[1];//��
			BW_Before_JM1[i++]=buffer[0];//��
		}
	}
	return i;	
}

u8 Conclude_Enegy_BefJM(u8 length,u8 ca)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_CI_NA_1;
		sum+=C_CI_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTTERM;
		sum+=ACTTERM;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------�޶���------------*/
		BW_Before_JM1[i++]=0x05;
		sum+=0x05;
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
	/*-----------��ͷ------------*/
	BW_Before_JM1[i++]=0x68;
	BW_Before_JM1[i++]=0;
	BW_Before_JM1[i++]=0;											//�����������¸�ֵ
	BW_Before_JM1[i++]=0x68;
	/*-----------������-----------*/
	BW_Before_JM1[i++]=ca;
	sum+=ca;
	/*-----------��ַ��------------*/
  BW_Before_JM1[i++]=ADDR&0xFF;
	sum+=(ADDR&0xFF);
	BW_Before_JM1[i++]=ADDR>>8;	
	sum+=(ADDR>>8);
	/*-----------���ͱ�ʶ------------*/
	BW_Before_JM1[i++]=F_FR_NA_1;
	sum+=F_FR_NA_1;
	/*-----------֡���޶���------------*/
	BW_Before_JM1[i++]=0x01;
	sum+=0x01;
	/*-----------����ԭ��------------*/
	BW_Before_JM1[i++]=REQ;
	sum+=REQ;
	BW_Before_JM1[i++]=0;
	sum+=0;
	/*-----------ASDU��ַ------------*/
	BW_Before_JM1[i++]=Asdu_addr&0xFF;
	sum+=(Asdu_addr&0xFF);
	BW_Before_JM1[i++]=Asdu_addr>>8;
	sum+=(Asdu_addr>>8);
	/*-----------��Ϣ�����ַ------------*/
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	/*-----------�������ݰ�����-----------*/
	BW_Before_JM1[i++]=0x02;
	sum+=0x02;
	/*-----------������ʶ-----------*/
	BW_Before_JM1[i++]=0x02;
	sum+=0x02;
	/*-----------���������----------*/
	BW_Before_JM1[i++]=flag;
	sum+=flag;
	if(flag==0)                 //0��ʾ�ɹ�
	{
		BW_Before_JM1[i++]=0x0;  //	ID
		sum+=0x0;		
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;	
		
//�������ļ���Ŀ¼

//�������ļ���Ŀ¼		
	
	}
	else												//��Ŀ¼
	{
		BW_Before_JM1[i++]=0x0;  //	ID
		sum+=0x0;		
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;	
		
		BW_Before_JM1[i++]=0x0;  //������־
		sum+=0x0;		
		
		BW_Before_JM1[i++]=0x0;  //�ļ�����
		sum+=0x0;					
	}
	/*-----------У����------------*/
	BW_Before_JM1[i++]=sum;	
	/*-----------֡β------------*/
	BW_Before_JM1[i++]=0x16;

	return i;	
}

u8 Confirm_ReadDoc_BefJM(u8 ca,u8 flag)
{

	u8 sum=0,j=0,i=0;
	/*-----------��ͷ------------*/
	BW_Before_JM1[i++]=0x68;
	BW_Before_JM1[i++]=0;
	BW_Before_JM1[i++]=0;											//�����������¸�ֵ
	BW_Before_JM1[i++]=0x68;
	/*-----------������-----------*/
	BW_Before_JM1[i++]=ca;
	sum+=ca;
	/*-----------��ַ��------------*/
  BW_Before_JM1[i++]=ADDR&0xFF;
	sum+=(ADDR&0xFF);
	BW_Before_JM1[i++]=ADDR>>8;	
	sum+=(ADDR>>8);
	/*-----------���ͱ�ʶ------------*/
	BW_Before_JM1[i++]=F_FR_NA_1;
	sum+=F_FR_NA_1;
	/*-----------֡���޶���------------*/
	BW_Before_JM1[i++]=0x01;
	sum+=0x01;
	/*-----------����ԭ��------------*/
	BW_Before_JM1[i++]=ACTCON;
	sum+=ACTCON;
	BW_Before_JM1[i++]=0;
	sum+=0;
	/*-----------ASDU��ַ------------*/
	BW_Before_JM1[i++]=Asdu_addr&0xFF;
	sum+=(Asdu_addr&0xFF);
	BW_Before_JM1[i++]=Asdu_addr>>8;
	sum+=(Asdu_addr>>8);
	/*-----------��Ϣ�����ַ------------*/
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	/*-----------�������ݰ�����-----------*/
	BW_Before_JM1[i++]=0x02;
	sum+=0x02;
	/*-----------������ʶ-----------*/
	BW_Before_JM1[i++]=0x04;
	sum+=0x04;
	/*-----------���������----------*/
	BW_Before_JM1[i++]=flag;
	sum+=flag;
	if(flag==0)                 //0��ʾ�ɹ�
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
	
		BW_Before_JM1[i++]=(u8)(Send_Doc_Num1&0xFF);   //�ļ���С�����Ӧ���ڴ洢��flash�е�ʱ��˳��洢����
		sum+=(u8)(Send_Doc_Num1&0xFF);	
		BW_Before_JM1[i++]=(u8)((Send_Doc_Num1>>8)&0xFF);   
		sum+=(u8)((Send_Doc_Num1>>8)&0xFF);	
		BW_Before_JM1[i++]=(u8)((Send_Doc_Num1>>16)&0xFF);   
		sum+=(u8)((Send_Doc_Num1>>16)&0xFF);	
		BW_Before_JM1[i++]=(u8)((Send_Doc_Num1>>24)&0xFF);  
		sum+=(u8)((Send_Doc_Num1>>24)&0xFF);	
	}
	else												//���ļ�
	{
		
	}
	/*-----------У����------------*/
	BW_Before_JM1[i++]=sum;	
	/*-----------֡β------------*/
	BW_Before_JM1[i++]=0x16;

	return i;	
}

u8 Send_Doc_BefJM(u8 ca)
{
	static u16 times=0;
	u32 duan_num=0;
	u8 sum=0,sum1=0,i=0;
	/*-----------��ͷ------------*/
	BW_Before_JM1[i++]=0x68;
	BW_Before_JM1[i++]=0;
	BW_Before_JM1[i++]=0;											//�����������¸�ֵ
	BW_Before_JM1[i++]=0x68;
	/*-----------������-----------*/
	BW_Before_JM1[i++]=ca;
	sum+=ca;
	/*-----------��ַ��------------*/
  BW_Before_JM1[i++]=ADDR&0xFF;
	sum+=(ADDR&0xFF);
	BW_Before_JM1[i++]=ADDR>>8;	
	sum+=(ADDR>>8);
	/*-----------���ͱ�ʶ------------*/
	BW_Before_JM1[i++]=F_FR_NA_1;
	sum+=F_FR_NA_1;
	/*-----------֡���޶���------------*/
	BW_Before_JM1[i++]=0x01;
	sum+=0x01;
	/*-----------����ԭ��------------*/
	BW_Before_JM1[i++]=REQ;
	sum+=REQ;
	BW_Before_JM1[i++]=0;
	sum+=0;
	/*-----------ASDU��ַ------------*/
	BW_Before_JM1[i++]=Asdu_addr&0xFF;
	sum+=(Asdu_addr&0xFF);
	BW_Before_JM1[i++]=Asdu_addr>>8;
	sum+=(Asdu_addr>>8);
	/*-----------��Ϣ�����ַ------------*/
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;
	/*-----------�������ݰ�����-----------*/
	BW_Before_JM1[i++]=0x02;
	sum+=0x02;
	/*-----------������ʶ-----------*/
	BW_Before_JM1[i++]=0x05;
	sum+=0x05;
	/*-----------�ļ�ID---------*/
	BW_Before_JM1[i++]=0x0;  //	ID
	sum+=0x0;		
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;			
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;			
	BW_Before_JM1[i++]=0x0;
	sum+=0x0;	
	/*-----------�κ�---------*/	
	duan_num=times*200;	
	BW_Before_JM1[i++]=(u8)(duan_num&0xFF);  //	�κ�
	sum+=(u8)(duan_num&0xFF);		
	BW_Before_JM1[i++]=(u8)((duan_num>>8)&0xFF);
	sum+=(u8)((duan_num>>8)&0xFF);			
	BW_Before_JM1[i++]=(u8)((duan_num>>16)&0xFF);
	sum+=(u8)((duan_num>>16)&0xFF);			
	BW_Before_JM1[i++]=(u8)((duan_num>>24)&0xFF);
	sum+=(u8)((duan_num>>24)&0xFF);		
	times++;
	/*-----------������־---------*/	
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
	/*-----------�����ļ�---------*/
	//��Ҫ�ж����ĸ��ļ�
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
			//������CFG�ļ�����DAT�ļ�
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
	/*-----------�ļ�У����------------*/
	BW_Before_JM1[i++]=sum1;
	sum+= sum1;
	/*-----------У����------------*/
	BW_Before_JM1[i++]=sum;	
	/*-----------֡β------------*/
	BW_Before_JM1[i++]=0x16;

	return i;	
}

u8 Confirm_WriteDoc_BefJM(u8 ca,u8 flag)
{
	u8 sum=0,j=0,i=0;
	if(y_104==0)
	{
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=F_FR_NA_1;
		sum+=F_FR_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x01;
		sum+=0x01;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------�������ݰ�����-----------*/
		BW_Before_JM1[i++]=0x02;
		sum+=0x02;
		/*-----------������ʶ-----------*/
		BW_Before_JM1[i++]=0x08;
		sum+=0x08;
		/*-----------���������----------*/
		BW_Before_JM1[i++]=flag;
		sum+=flag;
		if(flag==0)                 //0��ʾ�ɹ�
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
		
			BW_Before_JM1[i++]=(u8)(Rec_Doc_Num1&0xFF);   //�ļ���С�����Ӧ���ڴ洢��flash�е�ʱ��˳��洢����
			sum+=(u8)(Rec_Doc_Num1&0xFF);	
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>8)&0xFF);   
			sum+=(u8)((Rec_Doc_Num1>>8)&0xFF);	
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>16)&0xFF);   
			sum+=(u8)((Rec_Doc_Num1>>16)&0xFF);	
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>24)&0xFF);  
			sum+=(u8)((Rec_Doc_Num1>>24)&0xFF);	
		}
		else												//���ļ�
		{

		}
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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

		BW_Before_JM1[i++]=2;															//�������ݰ�����2���ļ�����
		BW_Before_JM1[i++]=8;														  //������ʶ8��д�ļ�ȷ�ϼ���
		BW_Before_JM1[i++]=flag;										//0�ɹ���1ʧ�ܣ�2�ļ������ԣ�3���ȳ���

		if(flag==0)                 //0��ʾ�ɹ�
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

			BW_Before_JM1[i++]=(u8)(Rec_Doc_Num1&0xFF);   //�ļ���С�����Ӧ���ڴ洢��flash�е�ʱ��˳��洢����
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>8)&0xFF);   
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>16)&0xFF);   
			BW_Before_JM1[i++]=(u8)((Rec_Doc_Num1>>24)&0xFF);  
		}
		else												//���ļ�
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=F_FR_NA_1;
		sum+=F_FR_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=REQ;
		sum+=REQ;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------�������ݰ�����-----------*/
		BW_Before_JM1[i++]=0x02;
		sum+=0x02;
		/*-----------������ʶ-----------*/
		BW_Before_JM1[i++]=0x0a;
		sum+=0x0a;
		/*-----------�ļ�ID-----------*/	
		BW_Before_JM1[i++]=0x0;  //	ID
		sum+=0x0;		
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;			
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;	
		/*-----------���ݶκ�-----------*/	
		BW_Before_JM1[i++]=(u8)(segment_num_last&0xFF);   //�ļ���С�����Ӧ���ڴ洢��flash�е�ʱ��˳��洢����
		sum+=(u8)(segment_num_last&0xFF);	
		BW_Before_JM1[i++]=(u8)((segment_num_last>>8)&0xFF);   
		sum+=(u8)((segment_num_last>>8)&0xFF);	
		BW_Before_JM1[i++]=(u8)((segment_num_last>>16)&0xFF);   
		sum+=(u8)((segment_num_last>>16)&0xFF);	
		BW_Before_JM1[i++]=(u8)((segment_num_last>>24)&0xFF);  
		sum+=(u8)((segment_num_last>>24)&0xFF);	
		/*-----------���������----------*/
		BW_Before_JM1[i++]=Rec_Error_Flag1;
		sum+=Rec_Error_Flag1;
		Rec_Error_Flag1=0;							//��0����
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=2;															//�������ݰ�����2���ļ�����
		BW_Before_JM1[i++]=10;															//������ʶ10��д�ļ���������ȷ�ϼ���
		
		BW_Before_JM1[i++]=0;															//�ļ�ID��ȫΪ0�����ֽ�
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;	
		BW_Before_JM1[i++]=0;
		
		BW_Before_JM1[i++]=((u8)segment_num_last)&0xFF;							    //�κ�
		BW_Before_JM1[i++]=((u8)segment_num_last>>8)&0xFF;	
		BW_Before_JM1[i++]=((u8)segment_num_last>>16)&0xFF;	
		BW_Before_JM1[i++]=((u8)segment_num_last>>24)&0xFF;	
		
		BW_Before_JM1[i++]=Rec_Error_Flag1;										//0�ɹ���1ʧ�ܣ�2У��ʹ���3���ȳ���
		Rec_Error_Flag1=0;   //��0����	
	}
	return i;	
}

u8 Confirm_Update_BefJM(u8 ca,u8 reason,u8 SE_FLAG)
{
	u8 sum=0,i=0;
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=F_SR_NA_1;
		sum+=F_SR_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=reason;
		sum+=reason;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------��������CTYPE-----------*/
		BW_Before_JM1[i++]=SE_FLAG;
		sum+=SE_FLAG;

		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_RR_NA_1;
		sum+=C_RR_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x1;
		sum+=0x1;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=(u8)SN_Num1&0xFF;
		sum+=(u8)SN_Num1&0xFF;
		BW_Before_JM1[i++]=(u8)(SN_Num1>>8)&0xFF;
		sum+=(u8)(SN_Num1>>8)&0xFF;	
		/*-----------��С��ֵ����-----------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------���ֵ����-----------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=length;
		BW_Before_JM1[i++]=length;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_SR_NA_1;
		sum+=C_SR_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0x1;
		sum+=0x1;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��Ϣ�����ַ------------*/
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		BW_Before_JM1[i++]=0x0;
		sum+=0x0;
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=(u8)SN_Num1&0xFF;
		sum+=(u8)SN_Num1&0xFF;
		BW_Before_JM1[i++]=(u8)(SN_Num1>>8)&0xFF;
		sum+=(u8)(SN_Num1>>8)&0xFF;	
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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

u8 Read_All_Constparas_BefJM(u8 ca)						//��ȡȫ�����в���
{
	u8 sum=0,j=0,i=0;
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0xa;							//�����ֵ
		sum+=0xa;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------����������ʶ------------*/	
		BW_Before_JM1[i++]=0x01;							//����Ҫȷ���Ƿ��к�����0���޺�����1���к���
		sum+=0x01;	
		/*-----------��Ϣ�����ַ------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=0x0A;								//�ɱ䳤�Ƚṹ�޶��ʣ���Ҫ���ݷ��͵����ݸ���
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//��ֵ���ţ�������Ϊ0����ֵ����Ҫ
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x01;					//����������ʶ,�к���
	
		BW_Before_JM1[i++]=0x01;              //��Ϣ���ַ
		BW_Before_JM1[i++]=0x80;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Constpara_Tag[0];  //tag����
		BW_Before_JM1[i++]=Constpara_Len[0];  //���ݳ���
		for(j=0;j<Constpara_Len[0];j++)
		{
			BW_Before_JM1[i++]=Constpara1[j];   //����
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

u8 Read_All_Runparas_BefJM(u8 ca)					//��ȡȫ�����в���
{
	u8 sum=0,i=0,j=0,k=0;
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------֡���޶���------------*/
	  BW_Before_JM1[i++]=Runpara_Num;			      //ȫ�������в�����Runpara_Num��
		sum+=Runpara_Num;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------����������ʶ------------*/	
		BW_Before_JM1[i++]=0x01;								//�к���
		sum+=0x01;	
		/*-----------��Ϣ����------------*/
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
		
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=0x15;								//�ɱ䳤�Ƚṹ�޶��ʣ���Ҫ���ݷ��͵����ݸ���
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//��ֵ���ţ�������Ϊ0����ֵ����Ҫ
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x01;					//����������ʶ,�к���
		
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
	fix_send.fixpara[9].floatdata = (float)ProtectSet.para.delay_fast_off / 100;//����I�ζ�ֵ
  fix_send.fixpara[10].floatdata = (float)ProtectSet.para.delay_fast_off_time / 100;
	fix_send.fixpara[11].bytedata[0] = ProtectSet.para.delay_fast_off_voltage_lock;
	fix_send.fixpara[12].bytedata[0] = ProtectSet.para.delay_fast_off_direction_lock;
	
	fix_send.fixpara[13].bytedata[0] = ProtectSet.para.max_current_enable;
	fix_send.fixpara[14].floatdata = (float)ProtectSet.para.max_current / 100;//����II�ζ�ֵ
  fix_send.fixpara[15].floatdata = (float)ProtectSet.para.max_current_time / 100;
	fix_send.fixpara[16].bytedata[0] = ProtectSet.para.max_current_voltage_lock;
	fix_send.fixpara[17].bytedata[0] = ProtectSet.para.max_current_direction_lock;
	
	fix_send.fixpara[18].floatdata = (float)ProtectSet.para.max_current_after / 100;//��������ٶ�ֵ
  fix_send.fixpara[19].floatdata = (float)ProtectSet.para.max_current_after_time / 100;
	
	fix_send.fixpara[20].floatdata = (float)ProtectSet.para.max_load / 100;//�����ɸ澯��ֵ
  fix_send.fixpara[21].floatdata = (float)ProtectSet.para.max_load_time / 100;
	
	fix_send.fixpara[22].bytedata[0] = ProtectSet.para.zero_fast_off_enable;
	fix_send.fixpara[23].floatdata = (float)ProtectSet.para.zero_fast_off / 100;//����I�ζ�ֵ
  fix_send.fixpara[24].floatdata = (float)ProtectSet.para.zero_fast_off_time / 100;
	fix_send.fixpara[25].bytedata[0] = 0;
	
	fix_send.fixpara[26].floatdata = (float)ProtectSet.para.zero_max_current_after / 100;//�������ٶ�ֵ
  fix_send.fixpara[27].floatdata = (float)ProtectSet.para.zero_max_current_after_time / 100;
		
	fix_send.fixpara[28].bytedata[0] = ProtectSet.para.once_reon_enable;//�غ�բͶ��
	fix_send.fixpara[29].bytedata[0] = ProtectSet.para.reon_Un_enable;  //�غ�բ����ѹͶ��
	fix_send.fixpara[30].bytedata[0] = ProtectSet.para.reon_synchron_enable;//�غ�բ��ͬ��Ͷ��
	fix_send.fixpara[31].floatdata = (float)ProtectSet.para.once_reon_time / 100;//�غ�բʱ��
	
	fix_send.fixpara[32].bytedata[0] = 1;        //С�����ӵظ澯Ͷ��                      
	fix_send.fixpara[33].floatdata = (float)ProtectSet.para.zero_max_voltage / 100;//�����ѹ��ֵ
		
	fix_send.fixpara[34].bytedata[0] = ProtectSet.para.max_voltage_enable; //��ѹ����Ͷ��                   
	fix_send.fixpara[35].floatdata = (float)ProtectSet.para.max_voltage / 100;//��ѹ������ֵ
	fix_send.fixpara[36].floatdata = (float)ProtectSet.para.max_voltage_time / 100;//��ѹ������ʱ
		
	fix_send.fixpara[37].bytedata[0] = ProtectSet.para.max_freq_enable; //��Ƶ����Ͷ��                   
	fix_send.fixpara[38].floatdata = (float)ProtectSet.para.max_freq / 100;//��Ƶ������ֵ
	fix_send.fixpara[39].floatdata = (float)ProtectSet.para.max_freq_time / 100;//��Ƶ������ʱ
		
	fix_send.fixpara[40].bytedata[0] = ProtectSet.para.low_freq_enable; //��Ƶ����Ͷ��                   
	fix_send.fixpara[41].floatdata = (float)ProtectSet.para.low_freq / 100;//��Ƶ������ֵ
	fix_send.fixpara[42].floatdata = (float)ProtectSet.para.low_freq_time / 100;//��Ƶ������ʱ
		
	if(y_104==0)
	{
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------֡���޶���------------*/
	  BW_Before_JM1[i++]=Fixpara_Num;				     //ȫ����ֵFixpara_Num��
		sum+=Fixpara_Num;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------����������ʶ------------*/	
		BW_Before_JM1[i++]=0x0;									//��ȡ�궨ֵ����û�к�����
		sum+=0x0;	
		/*-----------��Ϣ����------------*/
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

		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=0x01;								//�ɱ䳤�Ƚṹ�޶��ʣ���Ҫ���ݷ��͵����ݸ���
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//��ֵ����		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x0;					//����������ʶ	
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

u8 Read_ConstParas_BefJM(u8 ca,u8 yw)      //���в�����ȡ
{
	u8 sum=0,j=0,k=0,i=0;
	if(y_104==0)
	{
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=const_value.params_num;
		sum+=const_value.params_num;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------����������ʶ------------*/	
		BW_Before_JM1[i++]=yw;
		sum+=yw;	
		/*-----------��Ϣ�����ַ------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=const_value.params_num;								//�ɱ䳤�Ƚṹ�޶��ʣ���Ҫ���ݷ��͵����ݸ���
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//��ֵ����		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=yw;					//����������ʶ	
		
		
		/*-----------��Ϣ�����ַ------------*/
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
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=run_value.params_num;
		sum+=run_value.params_num;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------����������ʶ------------*/	
		BW_Before_JM1[i++]=yw;
		sum+=yw;	
		/*-----------��Ϣ�����ַ------------*/
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

		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=run_value.params_num;								//�ɱ䳤�Ƚṹ�޶��ʣ���Ҫ���ݷ��͵����ݸ���
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//��ֵ����		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x01;					//����������ʶ	
		
		/*-----------��Ϣ�����ַ------------*/
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
	fix_send.fixpara[9].floatdata = (float)ProtectSet.para.delay_fast_off / 100;//����I�ζ�ֵ
  fix_send.fixpara[10].floatdata = (float)ProtectSet.para.delay_fast_off_time / 100;
	fix_send.fixpara[11].bytedata[0] = ProtectSet.para.delay_fast_off_voltage_lock;
	fix_send.fixpara[12].bytedata[0] = ProtectSet.para.delay_fast_off_direction_lock;
	
	fix_send.fixpara[13].bytedata[0] = ProtectSet.para.max_current_enable;
	fix_send.fixpara[14].floatdata = (float)ProtectSet.para.max_current / 100;//����II�ζ�ֵ
  fix_send.fixpara[15].floatdata = (float)ProtectSet.para.max_current_time / 100;
	fix_send.fixpara[16].bytedata[0] = ProtectSet.para.max_current_voltage_lock;
	fix_send.fixpara[17].bytedata[0] = ProtectSet.para.max_current_direction_lock;
	
	fix_send.fixpara[18].floatdata = (float)ProtectSet.para.max_current_after / 100;//��������ٶ�ֵ
  fix_send.fixpara[19].floatdata = (float)ProtectSet.para.max_current_after_time / 100;
	
	fix_send.fixpara[20].floatdata = (float)ProtectSet.para.max_load / 100;//�����ɸ澯��ֵ
  fix_send.fixpara[21].floatdata = (float)ProtectSet.para.max_load_time / 100;
	
	fix_send.fixpara[22].bytedata[0] = ProtectSet.para.zero_fast_off_enable;
	fix_send.fixpara[23].floatdata = (float)ProtectSet.para.zero_fast_off / 100;//����I�ζ�ֵ
  fix_send.fixpara[24].floatdata = (float)ProtectSet.para.zero_fast_off_time / 100;
	fix_send.fixpara[25].bytedata[0] = 0;
	
	fix_send.fixpara[26].floatdata = (float)ProtectSet.para.zero_max_current_after / 100;//�������ٶ�ֵ
  fix_send.fixpara[27].floatdata = (float)ProtectSet.para.zero_max_current_after_time / 100;
		
	fix_send.fixpara[28].bytedata[0] = ProtectSet.para.once_reon_enable;//�غ�բͶ��
	fix_send.fixpara[29].bytedata[0] = ProtectSet.para.reon_Un_enable;  //�غ�բ����ѹͶ��
	fix_send.fixpara[30].bytedata[0] = ProtectSet.para.reon_synchron_enable;//�غ�բ��ͬ��Ͷ��
	fix_send.fixpara[31].floatdata = (float)ProtectSet.para.once_reon_time / 100;//�غ�բʱ��
	
	fix_send.fixpara[32].bytedata[0] = 1;        //С�����ӵظ澯Ͷ��                      
	fix_send.fixpara[33].floatdata = (float)ProtectSet.para.zero_max_voltage / 100;//�����ѹ��ֵ
		
	fix_send.fixpara[34].bytedata[0] = ProtectSet.para.max_voltage_enable; //��ѹ����Ͷ��                   
	fix_send.fixpara[35].floatdata = (float)ProtectSet.para.max_voltage / 100;//��ѹ������ֵ
	fix_send.fixpara[36].floatdata = (float)ProtectSet.para.max_voltage_time / 100;//��ѹ������ʱ
		
	fix_send.fixpara[37].bytedata[0] = ProtectSet.para.max_freq_enable; //��Ƶ����Ͷ��                   
	fix_send.fixpara[38].floatdata = (float)ProtectSet.para.max_freq / 100;//��Ƶ������ֵ
	fix_send.fixpara[39].floatdata = (float)ProtectSet.para.max_freq_time / 100;//��Ƶ������ʱ
		
	fix_send.fixpara[40].bytedata[0] = ProtectSet.para.low_freq_enable; //��Ƶ����Ͷ��                   
	fix_send.fixpara[41].floatdata = (float)ProtectSet.para.low_freq / 100;//��Ƶ������ֵ
	fix_send.fixpara[42].floatdata = (float)ProtectSet.para.low_freq_time / 100;//��Ƶ������ʱ
		
	if(y_104==0)
	{
		/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_RS_NA_1;
		sum+=C_RS_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=fix_value.params_num;
		sum+=fix_value.params_num;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------����������ʶ------------*/	
		BW_Before_JM1[i++]=yw;
		sum+=yw;	
		/*-----------��Ϣ�����ַ------------*/
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
		/*-----------У����------------*/
		BW_Before_JM1[i++]=sum;	
		/*-----------֡β------------*/
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
		BW_Before_JM1[i++]=fix_value.params_num;								//�ɱ䳤�Ƚṹ�޶��ʣ���Ҫ���ݷ��͵����ݸ���
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//��ֵ����		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x0;					//����������ʶ	
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
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=0;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_WS_NA_1;
		sum+=C_WS_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=Decryption.DataBuffer[8];
		sum+=Decryption.DataBuffer[8];
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=ACTCON;
		sum+=ACTCON;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------����������ʶ------------*/	
		BW_Before_JM1[i++]=0x80;
		sum+=0x80;	
		/*-----------��Ϣ������------------*/		
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
		BW_Before_JM1[i++]=Decryption.DataBuffer[7];								//�ɱ䳤�Ƚṹ�޶��ʣ���Ҫ���ݷ��͵����ݸ���
		BW_Before_JM1[i++]=ACTCON;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//��ֵ����		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=0x80;					//����������ʶ	
		
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
			/*-----------��ͷ------------*/
		BW_Before_JM1[i++]=0x68;
		BW_Before_JM1[i++]=0xc;
		BW_Before_JM1[i++]=0xc;											//�����������¸�ֵ
		BW_Before_JM1[i++]=0x68;
		/*-----------������-----------*/
		BW_Before_JM1[i++]=ca;
		sum+=ca;
		/*-----------��ַ��------------*/
		BW_Before_JM1[i++]=ADDR&0xFF;
		sum+=(ADDR&0xFF);
		BW_Before_JM1[i++]=ADDR>>8;	
		sum+=(ADDR>>8);
		/*-----------���ͱ�ʶ------------*/
		BW_Before_JM1[i++]=C_WS_NA_1;
		sum+=C_WS_NA_1;
		/*-----------֡���޶���------------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------����ԭ��------------*/
		BW_Before_JM1[i++]=reason;
		sum+=reason;
		BW_Before_JM1[i++]=0;
		sum+=0;
		/*-----------ASDU��ַ------------*/
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		sum+=(Asdu_addr&0xFF);
		BW_Before_JM1[i++]=Asdu_addr>>8;
		sum+=(Asdu_addr>>8);
		/*-----------��ǰ��ֵ����-----------*/
		BW_Before_JM1[i++]=0;
		sum+=0;
		BW_Before_JM1[i++]=0;
		sum+=0;	
		/*-----------����������ʶ------------*/	
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
		BW_Before_JM1[i++]=0;								//�ɱ䳤�Ƚṹ�޶��ʣ���Ҫ���ݷ��͵����ݸ���
		BW_Before_JM1[i++]=reason;
		BW_Before_JM1[i++]=0;
		BW_Before_JM1[i++]=Asdu_addr&0xFF;
		BW_Before_JM1[i++]=Asdu_addr>>8;
		
		BW_Before_JM1[i++]=0;						//��ֵ����		
		BW_Before_JM1[i++]=0;		
		
		BW_Before_JM1[i++]=sign;					//����������ʶ	
	}

	return i;
}

/*-----------------������ܱ��ĵ�ͨ�÷����ļ���104��һ���ģ�������û������------------------*/
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
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x08);									//���ı�ʶ��֪���Ƿ���ȷ
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
		timer_enable=1;	  //ʹ�ܼ�ʱ��
		delay_time_101=0;	//ÿ�η��ͳ�ȥһ֡���Ϳ�ʼ��ʱ	
	}
}

u8 Check_List_Name1()
{
	u8 i=0,flag=1;
	if(List_Num11==8)								//˵���п�����COMTRADE
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
	else if(List_Num11==10)          //HISTORY/COң�ؼ�¼
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
		case 1:													//Comtrade�ļ�
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
		case 2:												//HISTORY/COң�ؼ�¼
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
	//&&(List_Name1[5]=='.')&&(List_Name1[6]=='B')&&(List_Name1[7]=='I')&&(List_Name1[8]=='N')) //�ж��ǲ���.bin�ļ�
	{
		flag=1;
	}
	else 
		Finish_Flag1=2;       //�ļ�����֧��
	return flag;
}

/****************************************************************************
*
*��    �ƣ�void Encryption_Handle(u8 receivedata)
*
*��    �ܣ����ܽ�������
*
*��ڲ���������1���ݣ�104��Լ������
*
*���ز�����
*
****************************************************************************/

void Encryption_Handle(u8 receivedata,u8 USARTxnumber)//����Ӵ��ڽ��յ������ݣ����밲ȫ����
{
	static u8 sum=0;
	switch(ProtocolResolveState)
	{
		case 0:  											//���յ��µ�����֡
			if(receivedata == 0xEB)			// �ж�����ͷ����վ����վ���͵������ַ�
			{
				if(USARTxnumber == 1)
				{
					if(Usart1RxReady == 0)		//δ�������
					{
						channel = 0;
						BackstageProtocol1.DataBuffer[0] = receivedata;//�����bak��������һ������ʵʱ������һ�����Ǳ��ݣ�channel=0��Ӧʵʱ����channel=1��Ӧ����
					}
					else if(Usart1bakRxReady == 0)
					{
						channel = 1;
						BackstageProtocol1bak.DataBuffer[0] = receivedata;
					}
			  }
				else if(USARTxnumber == 3)
				{
					if(Usart3RxReady == 0)		//δ�������
					{
						channel = 0;
						BackstageProtocol3.DataBuffer[0] = receivedata;//�����bak��������һ������ʵʱ������һ�����Ǳ��ݣ�channel=0��Ӧʵʱ����channel=1��Ӧ����
					}
					else if(Usart3bakRxReady == 0)
					{
						channel = 1;
						BackstageProtocol3bak.DataBuffer[0] = receivedata;
					}
				}
				sum=0;
				RxCounter_serial1=1;
				remain1=3;								//��ʣsan���ַ�û�н���
				ProtocolResolveState = 1;//���յ�һ���ַ�
			}
			else 
				ProtocolResolveState = 0;
		break;
		case 1:													      //֮ǰ�Ѿ����յ�����0xeb
			if(USARTxnumber == 1)
			{
				if(channel == 0)                    //����BackstageProtocol1�е�����
				{ 
					remain1--;			                    //ʣ��������1
					BackstageProtocol1.DataBuffer[RxCounter_serial1++] = receivedata;
					if(remain1==0)								      //����Ѿ����յ����ĸ��ַ���
					{
						if(BackstageProtocol1.DataBuffer[3]==0xEB) //��һ�����ݺ͵��ĸ����ݶ�Ϊ0xEB
						{
							Length_serial1 = ((u16)BackstageProtocol1.DataBuffer[1]<<8)|BackstageProtocol1.DataBuffer[2];
							remain1 = Length_serial1;
							ProtocolResolveState=2;
						}
						else													  //�����ԣ�׼�����½���
							ProtocolResolveState = 0;
					}
				}			
				else
				{
					remain1--;			                    //ʣ��������1
					BackstageProtocol1bak.DataBuffer[RxCounter_serial1++] = receivedata;
					if(remain1==0)								   
					{
						if(BackstageProtocol1.DataBuffer[3]==0xEB) //��һ�����ݺ͵��ĸ����ݶ�Ϊ0xEB
						{
							Length_serial1  =((u16)BackstageProtocol1bak.DataBuffer[1]<<8)|BackstageProtocol1bak.DataBuffer[2];
							remain1=Length_serial1;
							ProtocolResolveState=2;				
						}
						else													  //�����ԣ�׼�����½���
							ProtocolResolveState = 0;
					}
				}
		  }
			else if(USARTxnumber == 3)
			{
				if(channel == 0)                    //����BackstageProtocol1�е�����
				{ 
					remain1--;			                    //ʣ��������1
					BackstageProtocol3.DataBuffer[RxCounter_serial1++] = receivedata;
					if(remain1==0)								      //����Ѿ����յ����ĸ��ַ���
					{
						if(BackstageProtocol3.DataBuffer[3]==0xEB) //��һ�����ݺ͵��ĸ����ݶ�Ϊ0xEB
						{
							Length_serial1 = ((u16)BackstageProtocol3.DataBuffer[1]<<8)|BackstageProtocol3.DataBuffer[2];
							remain1 = Length_serial1;
							ProtocolResolveState=2;
						}
						else													  //�����ԣ�׼�����½���
							ProtocolResolveState = 0;
					}
				}			
				else
				{
					remain1--;			                    //ʣ��������1
					BackstageProtocol3bak.DataBuffer[RxCounter_serial1++] = receivedata;
					if(remain1==0)								   
					{
						if(BackstageProtocol3.DataBuffer[3]==0xEB) //��һ�����ݺ͵��ĸ����ݶ�Ϊ0xEB
						{
							Length_serial1  =((u16)BackstageProtocol3bak.DataBuffer[1]<<8)|BackstageProtocol3bak.DataBuffer[2];
							remain1=Length_serial1;
							ProtocolResolveState=2;				
						}
						else													  //�����ԣ�׼�����½���
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
			if(remain1==0) 																										//�Ѿ�������������
			{
				if(Length_serial1==(RxCounter_serial1-4))																//��֤�����Ƿ�һ��					
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
					if(sum==receivedata)																							//�鿴У����Ƿ���ȷ
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
					if(sum==receivedata)																							//�鿴У����Ƿ���ȷ
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
					if(sum==receivedata)																							//�鿴У����Ƿ���ȷ
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
					if(sum==receivedata)																							//�鿴У����Ƿ���ȷ
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
			if(receivedata==0xD7)																							//�鿴֡β�Ƿ���ȷ
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
								datdone_serial1=3;					//��ά���߼���
							else
								datdone_serial1=1;					//��վ����
						}
						else
							datdone_serial1=2;					//������
						if(BackstageProtocol1.DataBuffer[4]==0xFF)
							datdone_serial1=4;					//����
					}																										
					else
					{
						BackstageProtocol1bak.DataBuffer[RxCounter_serial1++] = receivedata;
							Usart1bakRxReady=1;
							if(BackstageProtocol1bak.DataBuffer[5]&0x08)
							{
								if(BackstageProtocol1bak.DataBuffer[5]&0xC0)
									datdone_serial1=3;					//��ά���߼���
								else
									datdone_serial1=1;					//��վ����
							}
							else
								datdone_serial1=2;					//������
							if(BackstageProtocol1bak.DataBuffer[4]==0xFF)
								datdone_serial1=4;					//����
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
								datdone_serial1=3;					//��ά���߼���
							else
								datdone_serial1=1;					//��վ����
						}
						else
							datdone_serial1=2;					//������
						if(BackstageProtocol3.DataBuffer[4]==0xFF)
							datdone_serial1=4;					//����
					}																										
					else
					{
						BackstageProtocol3bak.DataBuffer[RxCounter_serial1++] = receivedata;
							Usart3bakRxReady=1;
							if(BackstageProtocol3bak.DataBuffer[5]&0x08)
							{
								if(BackstageProtocol3bak.DataBuffer[5]&0xC0)
									datdone_serial1=3;					//��ά���߼���
								else
									datdone_serial1=1;					//��վ����
							}
							else
								datdone_serial1=2;					//������
							if(BackstageProtocol3bak.DataBuffer[4]==0xFF)
								datdone_serial1=4;					//����
					}
				}		
				isr_evt_set (0x0080, t_Task4); //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
		  }
      ProtocolResolveState = 0;			
		break;
		default:
			ProtocolResolveState=0;
			break;
	}
}

/*---------------------------------*/
//----------------���ܱ��Ĵ���
void ProtocolResolve1_JM(u8 USARTxChannel)	
{
	u16 Add=0,lc=0,i=0;	
	u8 Params_Num=0,sum=0,len=0,jm_ok=0,j=0,crc_yhf=0;
	u8 spi_flag=0,cd=0,ca=0;
  //���ж��ǲ��ǿɱ䳤��֡����
	if((Decryption.DataBuffer[0]==0x68)&&(Decryption.DataBuffer[3]==0x68)&&(Decryption.DataBuffer[1]==Decryption.DataBuffer[2])&&(Decryption.DataBuffer[BW_Len1-1]==0x16)) 	//˵����101����
	{
		delay_time_101=0;
		timer_enable=0;
		resend_num=0;
		len=Decryption.DataBuffer[1];      //������+��ַ��+ASDU
		for(i=0;i<len;i++) sum+=Decryption.DataBuffer[4+i];
		if(sum==Decryption.DataBuffer[BW_Len1-2])					//У���Ҳ�������
		{
			ADDR=(u16)Decryption.DataBuffer[6]<<8|Decryption.DataBuffer[5];   //��·��ַ
			Asdu_addr=(u16)Decryption.DataBuffer[12]<<8|Decryption.DataBuffer[11]; //ASDU��ַ
			if(Decryption.DataBuffer[4]&0x10)						//FCVλΪ1��˵��FCB��Ч������Ҫ�ж��ǵڼ��ν���
			{																											//���FCVλΪ0��˵����Ч����ô�Ͳ���Ҫ�������FCB�Ƿ��λ��Ҳ��Ϊ����ȷ��
				if(jm==0)																						//ֻ����һ��
				{
					if(Decryption.DataBuffer[4]&0x20)        //FCBλΪ1
						FCB_Flag_Main=1;
					else 
						FCB_Flag_Main=0;															//FCBλΪ0
					jm=1;
					jm_ok=1;				//���Ҫ���ϣ�֮ǰû��һֱû����������ж�
				}
				else	//�ڶ��ν���
				{
					if((Decryption.DataBuffer[4]&0x20)&&(FCB_Flag_Main==0))	//��վFCB��λ�ɹ�
					{
						FCB_Flag_Main=1;
						jm_ok=1;
					}
					else if(((Decryption.DataBuffer[4]&0x20)==0)&&(FCB_Flag_Main==1))	//��վFCB��λ�ɹ�
					{
						FCB_Flag_Main=0;
						jm_ok=1;																			//ok_flag=1˵�������ǵڶ��ν�����FCB��ת��ȷ
					}
					else 
					{
						jm_ok=2;																		//ok_flag=0˵�������ǵڶ��ν���������վ�ط�������֡
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
	if(jm_ok==1)												//������ȷ�����ұ�λ��ȷ
	{
		send_buffer_len=0;	//һ����λ��ȷ�Ͱ���һ֡�ı���ɾ��
		switch(Decryption.DataBuffer[7])     //���ͱ�ʶ����������Ǵ���������101��Լ������
		{
			case C_IC_NA_1:															//���ٻ� 
				ca=0x80;
				Common_MW(USARTxChannel,ca);
				if(call_num==0)                         //����ǵ�һ�ν��룬��ô��Ӧ��ʼ��֮��ĵ�һ�����ٻ�
				{
					Super_Sign[1]=1;
				}
				else																		//����ǵڶ��ν��룬�Ƿǳ�ʼ�����٣����Ա����
				{
					Super_Sign[4]=1;
				}						
				break;
			case C_CS_NA_1: 																		//��ʱ
				ca=0x80;
				Common_MW(USARTxChannel,ca);	
				if(Decryption.DataBuffer[9]==ACT)					//ʱ��ͬ��
				{
					TimeNow.msec    = ((u16)(Decryption.DataBuffer[15] | (u16)(Decryption.DataBuffer[16]<<8))%1000)&0x3FF;
					TimeNow.second  = ((u16)(Decryption.DataBuffer[15] | (u16)(Decryption.DataBuffer[16]<<8))/1000)&0x3F;  
					TimeNow.minute  = Decryption.DataBuffer[17]&0x3F;
					TimeNow.hour    = Decryption.DataBuffer[18]&0x1F;
					TimeNow.date    = Decryption.DataBuffer[19]&0x1F;
					TimeNow.month   = Decryption.DataBuffer[20]&0x0F;
					TimeNow.year    = Decryption.DataBuffer[21]&0x7F;
					correct_time();		                      
					Super_Sign[6]=1;                       //����ʱ��ͬ��ȷ�ϱ���
				}
				else if(Decryption.DataBuffer[9]==REQ)		//ʱ������
				{
					Super_Sign[6]=2;                       //����ʱ������ȷ�ϱ���
				}							
				break;
			case C_TS_NA_1:                             //��·��������
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
			case C_RP_NA_1:     																//��λ��������
				ca=0x80;
				Common_MW(USARTxChannel,ca);	
				Super_Sign[8]=1;									
				break;
			case C_CI_NA_1:     								 								//���������ٻ�	
				ca=0x80;
				Common_MW(USARTxChannel,ca);	
				Super_Sign[11]=1;			
				break;	
			case C_SC_NA_1:																			//����ң��
			case C_DC_NA_1:																			//˫���ң��						
																							            //һ�����룬������֤ǩ������ȷ��		
				if(Decryption.DataBuffer[9]==ACT)			//ѡ���ִ��
				{
					if(Decryption.DataBuffer[15]&0x80)   	//�ж�S/Eλ��0����1����SCO(������),DCO(˫����)�����λ����8λ��
					{
						switch(YY_Style1)
						{
							case 1:	//ǩ��
							case 4:	//ǩ��
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
				  }
					else		//ִ��
					{
						switch(YY_Style1)
						{
							case 3:	//ǩ��
							case 5:
							case 6:
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
				  }
			  }
					
				lc=BW_Len1+MsgSafty_Len1-1;												//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
				spi_flag=0;
				while(spi_flag!=3)															  //��֤ǩ������ȷ��
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															//֡ͷ
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:ǩ����Կ����
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
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)										//ǩ����ȷ
				{
					succ_flag_spi=0;
					control_jm.byte = Decryption.DataBuffer[15];
					Object_addr=(u16)Decryption.DataBuffer[14]<<8|Decryption.DataBuffer[13];
					if(Decryption.DataBuffer[9]==ACT)				//ѡ���ִ��
					{
						if(control_jm.bit.S_E == 1)   	//�ж�S/Eλ��0����1����SCO(������),DCO(˫����)�����λ����8λ��
						{
							if(yk_state==0 && Select101Addr==0)	//���ڳ�ʼ״̬������ѡ��
							{
								ca=0x80;
								Common_MW(USARTxChannel,ca);		
								if(Decryption.DataBuffer[7] == C_SC_NA_1)
								  Super_Sign[2]=1;	
								else if(Decryption.DataBuffer[7] == C_DC_NA_1)
									Super_Sign[2]=5;	
								Select101Addr=1;
								yk_state=1;	//ѡ��״̬
							}
							else
							{
								ca=0x81;
								Common_MW(USARTxChannel,ca);	
								Select101Addr=0;
								yk_state=0;	//�ָ�����ʼ״̬
							}
						}
						else if(control_jm.bit.S_E == 0)		//ң��ִ��
						{
							for(i=0;i<8;i++)
							{
								if(Msg_Safty1[6+i]!=Ter_Random1[i])	//��֤������Ƿ������Ƿ���ȥ��
								{
									Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//��֤�����ʧ��
									return;
								}
							}
							
							if(yk_state==1 && Select101Addr==1)		//ֻ�д���ѡ��״̬����ִ��
							{
								ca=0x80;
								Common_MW(USARTxChannel,ca);					//�̶�֡��ȷ��֡		
								if(Decryption.DataBuffer[7] == C_SC_NA_1)
								  Super_Sign[2]=2;	
								else if(Decryption.DataBuffer[7] == C_DC_NA_1)
									Super_Sign[2]=6;	
								yk_state=3;	//����ִ��״̬
							}
							else
							{
								ca=0x81;	//��ȷ��
								Common_MW(USARTxChannel,ca);	
								yk_state=0;	//�ָ�����ʼ״̬
								Select101Addr=0;
							}
						}
					}
					else if(Decryption.DataBuffer[9]==DEACT)  //��ֹ\��������
					{
						if(yk_state==1)	//ֻ�д���ѡ��״̬���ܳ���
						{
							ca=0x80;
							Common_MW(USARTxChannel,ca);					//�̶�֡��ȷ��֡	
              if(Decryption.DataBuffer[7] == C_SC_NA_1)
								Super_Sign[2]=3;	//������Ҫִ��ң��ȡ������					
							else if(Decryption.DataBuffer[7] == C_DC_NA_1)
								Super_Sign[2]=7;														
							yk_state=2;	        //���ڳ���
							Select101Addr=0;								
						}
						else
						{
							ca=0x81;	//��ȷ��
							Common_MW(USARTxChannel,ca);	
							yk_state=0;	//�ָ�����ʼ״̬
							Select101Addr=0;
						}
					}
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//��֤ǩ��ʧ��
				break;
			case F_SC_NA_1:												//�ٻ�Ŀ¼
	//				ca=0x80;
	//				Common_MW(USARTxChannel,ca);	
	//				ca=0xc3;
	//				if(Check_List_Name1())									//˵��Ŀ¼����
	//				{
	//					Send_len=Confirm_List_BefJM(ca,0);				 //0��ʾ�ɹ���1��ʾʧ��
	//					BW_Before_JM1[1]=Send_len-6;
	//					BW_Before_JM1[2]=Send_len-6;
	//					lc=16+4+Send_len;		
	//					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
	//					{
	//						succ_flag_spi=0;
	//						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
	//					}					
	//				}
	//				else
	//				{
	//					Send_len=Confirm_List_BefJM(ca,1);				 //0��ʾ�ɹ���1��ʾʧ��
	//					BW_Before_JM1[1]=Send_len-6;
	//					BW_Before_JM1[2]=Send_len-6;
	//					lc=16+4+Send_len;		
	//					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
	//					{
	//            succ_flag_spi=0;
	//						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
	//					}									
	//				}			
				break;
			case F_FR_NA_1:		  									//�ļ�����,�������ļ�Ŀ¼�ٻ�����д�ļ������д�ļ����䣬��Ҫ����������ʶ����ʶ��
				if(Decryption.DataBuffer[16]==1)								//�ٻ��ļ�Ŀ¼
				{
					ca=0x80;
					Common_MW(USARTxChannel,ca);
					if(Decryption.DataBuffer[21]==0)								//Ĭ��Ŀ¼,COMTRADE
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
						List_Num11=Decryption.DataBuffer[21];							//�ٻ���Ŀ¼������
						for(i=0;i<List_Num11;i++)
						{
							List_Name11[i]=Decryption.DataBuffer[22+i];			//ASC����ʽ�洢��Ŀ¼��
						}					
					}
					if(Decryption.DataBuffer[22+List_Num11])					//��Ҫ����ļ��ڴ�ʱ�����
					{
						List_Name11[49]=1;
						for(i=0;i<7;i++)
						{
							//Start_Time1[i]=Decryption.DataBuffer[23+List_Num11+i]; //ʱ�������
							//End_Time1[i]=Decryption.DataBuffer[30+List_Num11+i];
						}				
					}
					else
						List_Name11[49]=0;																					//�����ļ�
					Super_Sign[9]=1;	
				}	
				else if(Decryption.DataBuffer[16]==3)					       	//���ļ�����
				{
					ca=0x80;
					Common_MW(USARTxChannel,ca);
					List_Num1=Decryption.DataBuffer[17];									//Ҫ��ȡ���ļ�������
					for(i=0;i<List_Num1;i++)													
					{
						List_Name1[i]=Decryption.DataBuffer[18+i];			   	//ASC����ʽ�洢�ļ�����������չ��������׺������.dat
					}
					Super_Sign[10]=1;
				}
				else if(Decryption.DataBuffer[16]==6)						//���ļ����ݴ���ȷ��
				{
					ca=0x80;																
					Common_MW(USARTxChannel,ca);					//�̶�֡��ȷ��֡				
					Super_Sign[10]=0;									
				}	
				else if((Decryption.DataBuffer[9]==ACT)&&(Decryption.DataBuffer[16]==7))						//д�ļ�����
				{			
					ca=0x80;																
					Common_MW(USARTxChannel,ca);					//�̶�֡��ȷ��֡
					List_Num1=Decryption.DataBuffer[17];									    //Ҫд����ļ����ĳ���
					for(i=0;i<List_Num1;i++)
					{
						List_Name1[i]=Decryption.DataBuffer[18+i];			  		 	//ASC����ʽ�洢�ļ���
					}	
					Rec_Doc_Num1 |= (((u32)Decryption.DataBuffer[22+List_Num1])&0x000000FF);		
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[23+List_Num1])<<8)&0x0000FF00);	
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[24+List_Num1])<<16)&0x00FF0000);		
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[25+List_Num1])<<24)&0xFF000000);					
					if(Rec_Doc_Num1>0x80000)	Finish_Flag1=3;											 			//���ȼ�飬���ܴ���512k
					lc=BW_Len1+MsgSafty_Len1-1;												//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
					spi_flag=0;
					while(spi_flag!=3)															//��֤ǩ������ȷ��
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);															//֡ͷ
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;
						SPI2_ReadWrite(0x08);
						crc_yhf^=0x08;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:ǩ����Կ����
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
						SSN_H();											   //����Ƭѡ
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi) 																		
					{
						succ_flag_spi=0;
						Super_Sign[10]=3;
						Rec_num_101=0;	
					}
					else		
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//��֤ǩ��ʧ��
				}	
				else if((Decryption.DataBuffer[9]==ACT)&&(Decryption.DataBuffer[16]==9))						//д�ļ�����
				{
					Rec_Error_Flag1=0;//ÿ�ν�������Ҫ�������0����Ȼ����ϴ��д���һֱ����
					Receive_Doc_101();	//ʵ��ʹ��																													//ÿ�ν��ܵ���վ��д�ļ����ݶ������
					ca=0x80;																
					Common_MW(USARTxChannel,ca);					//�̶�֡��ȷ��֡

					if((Decryption.DataBuffer[25]==0)||(Rec_Error_Flag1))//�޺������߳����ˣ��������Ҫ�ϱ�����Ȼ��վ������֪������
					{
						ulFlieLength=Decryption.DataBuffer[1]-23;
						Super_Sign[10]=5;					
					}
				}				
		    //��Ҫ��Ŀ¼���в�ѯ������flash�ڴ�Ӧ���ȴ�ü���Ŀ¼��COMTRADE(��Ŀ¼�´����¼���ļ�)��ӦASCIIΪ43 4F 4D 54 52 41 44 45	,HISTORY/SOE,HISTORY/CO,HISTORY/EXV,HISTORY/FIXPT�ȵ�	
		    //�鵽Ŀ¼�󣬽���Ŀ¼�µ��ļ������͵���վ		
				break;	
			case F_SR_NA_1:												//�������
				if(Decryption.DataBuffer[9]==ACT)
				{
					if(Decryption.DataBuffer[15]&0x80)							  //�ж�S/E��1����������
					{
						switch(YY_Style1)
						{
							case 0:	//ǩ��
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
						ca=0x80;
						Common_MW(USARTxChannel,ca);
						lc=BW_Len1+MsgSafty_Len1-1;											//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
						spi_flag=0;
						while(spi_flag!=3)															//��֤ǩ������ȷ��
						{
							SSN_L();	
							SPI2_ReadWrite(0x55);													//֡ͷ
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;
							SPI2_ReadWrite(0x08);
							crc_yhf^=0x08;
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;
							SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);	//asKID:ǩ����Կ����
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
							SSN_H();											   //����Ƭѡ
							spi_flag=Ter_WriteRead_Spi1();
						}
						if(succ_flag_spi)
						{
							succ_flag_spi=0;
							Super_Sign[10]=6;
              Rec_num_101=0;								
						}
						else
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);		//��֤ǩ��ʧ��
					}
					else																								//0��������
					{
						ca=0x80;																
						Common_MW(USARTxChannel,ca);				        	//�̶�֡��ȷ��֡	
						Super_Sign[10]=7;						
					}
				}
				else if(Decryption.DataBuffer[9]==DEACT)					      //��������
				{
					ca=0x80;																
					Common_MW(USARTxChannel,ca);					          //�̶�֡��ȷ��֡	
					Super_Sign[10]=8;							
				}	
				break;
			case C_RR_NA_1:                     	                  //����ǰ��ֵ��
				ca=0x80;																
				Common_MW(USARTxChannel,ca);					              //�̶�֡��ȷ��֡
				Super_Sign[12]=1;	
				break;	
			case C_SR_NA_1:                     	                  //�л���ֵ��
				switch(YY_Style1)
				{
					case 0:	//ǩ��
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
						return;
					default:
						break;
				}
				ca=0x80;																
				Common_MW(USARTxChannel,ca);						
				lc=BW_Len1+MsgSafty_Len1-1;												    //��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
				spi_flag=0;
				while(spi_flag!=3)															      //��֤ǩ������ȷ��
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															  //֡ͷ
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:ǩ����Կ�������Դ����ж����ĸ���վ֤��
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
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 																
				{
					succ_flag_spi=0;
					SN_Num1=Decryption.DataBuffer[15] | (Decryption.DataBuffer[16]<<8);
					Super_Sign[12]=2;	
				}
				else			
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//��֤ǩ��ʧ��
				break;	
			case C_RS_NA_1:                     											//�������Ͷ�ֵ,��ȡ����ʱ����ֵ��������
//			SN_Num1=Decryption.DataBuffer[13] | (Decryption.DataBuffer[14]<<8);	//����Ҫ��ȡ��ֵ�����ˣ���ǰ���л����߶�ȡ�Ķ�ֵ���ž����������Ƕ�д����ʱ�õĶ�ֵ�����ĸ�
				ca=0x80;																
				Common_MW(USARTxChannel,ca);											  //�̶�֡��ȷ��֡		

				Params_total = 0;
				fix_value.params_num = 0;
				run_value.params_num = 0;
				const_value.params_num = 0;
			
				if((Decryption.DataBuffer[8]&0x7F)==0)					        //��ȡȫ�������Ͷ�ֵ
				{
					Super_Sign[12]=3;
				}
				else																							      //��ȡ��������Ͷ�ֵ
				{
					for(i=0;i<(Decryption.DataBuffer[8]&0x7F);i++)
					{
						if((Decryption.DataBuffer[16+2*i]==0x82)&&(Decryption.DataBuffer[15+2*i]>=0x20))     //��ֵ
						{
							fix_value.params_addr[fix_value.params_num++]=Decryption.DataBuffer[15+2*i];//ֻȡ�͵�ַ
							Params_total++;
						}
						else if((Decryption.DataBuffer[16+2*i]==0x80)&&(Decryption.DataBuffer[15+2*i]>=0x20))							
						{
							run_value.params_addr[run_value.params_num++]=Decryption.DataBuffer[15+2*i];//ֻȡ�͵�ַ
							Params_total++;
						}
						else if((Decryption.DataBuffer[16+2*i]==0x80)&&(Decryption.DataBuffer[15+2*i]<0x20))
						{
							const_value.params_addr[const_value.params_num++]=Decryption.DataBuffer[15+2*i];//ֻȡ�͵�ַ
							Params_total++;	
						}
					}
					Super_Sign[12]=4;
				}		
				break;	
			case C_WS_NA_1:                     								//д�����Ͷ�ֵ
				if((Decryption.DataBuffer[9]==0x06)&&((Decryption.DataBuffer[15]&0x80)==0x80))	//����Ԥ��
				{
					switch(YY_Style1)
					{
						case 0:	//ǩ��
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
							return;
						default:
							break;
					}
				}
				else if((Decryption.DataBuffer[9]==0x06)&&((Decryption.DataBuffer[15]&0xC0)==0))   //�����̻�
				{
					switch(YY_Style1)
					{
						case 1:	//ǩ��
						case 2:	//ǩ��
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
							return;
						default:
							break;
					}
				}
					
				ca=0x80;																
				Common_MW(USARTxChannel,ca);	
				//ȫ������֤ǩ��
				lc=BW_Len1+MsgSafty_Len1-1;												//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
				spi_flag=0;
				while(spi_flag!=3)															  //��֤ǩ������ȷ��
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);													  //֡ͷ
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);		//asKID:ǩ����Կ����
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
					SSN_H();											    //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi)										//ǩ����ȷ
				{
					succ_flag_spi=0;
					if((Decryption.DataBuffer[9]==ACT)&&((Decryption.DataBuffer[15]&0x80)==0x80))	//����Ԥ��
					{
            //����Ԥ�ã�����һ�£��̻�����Ҫ��ȡ����Ҫ
						Params_Num=Decryption.DataBuffer[8]&0x7F;																		//ҪԤ�ü�������
						
						for(i=0;i<Params_Num;i++)
						{
							Add=(u16)Decryption.DataBuffer[17+4*i+cd]<<8|Decryption.DataBuffer[16+4*i+cd];
							if(Add<0x8020)                             //���в�����Ӧ�ò��������޸Ĺ��в���
							{
								Add-=0x8000;														 //�жϲ�������
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
							else if((Add>=0x8020)&&(Add<=0x821F))										//���в���
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
							else if((Add>=0x8220)&&(Add<=0x85EF))										//��ֵ
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
					else if((Decryption.DataBuffer[9]==ACT)&&((Decryption.DataBuffer[15]&0xC0)==0))   //�����̻�
					{
//          SN_Num1=Decryption.DataBuffer[13] | (Decryption.DataBuffer[14]<<8);
            //�̻�һ�£�������ֵ����洢
						for(i=0;i<8;i++)
						{
							if(Msg_Safty1[i]!=Ter_Random1[i])	//��֤������Ƿ������Ƿ���ȥ��,Ӧ������Ϊ3��û��ʱ�䣬ֱ�Ӵ��������ʼ
							{
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//��֤ǩ��ʧ��
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
                    ProtectSet.para.delay_fast_off = (u16)(fix_send.fixpara[9].floatdata * 100);//����I�ζ�ֵ
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
                    ProtectSet.para.max_current = (u16)(fix_send.fixpara[14].floatdata * 100);//����II�ζ�ֵ
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
                    ProtectSet.para.max_current_after = (u16)(fix_send.fixpara[18].floatdata * 100);//��������ٶ�ֵ
										break;
									case 19:
                    ProtectSet.para.max_current_after_time = (u16)(fix_send.fixpara[19].floatdata * 100);
										break;
									case 20:
                    ProtectSet.para.max_load = (u16)(fix_send.fixpara[20].floatdata * 100);//�����ɸ澯��ֵ
										break;
									case 21:
                    ProtectSet.para.max_load_time = (u16)(fix_send.fixpara[21].floatdata * 100);
										break;										
	                case 22:
                    ProtectSet.para.zero_fast_off_enable = fix_send.fixpara[22].bytedata[0];
										break;
									case 23:
                    ProtectSet.para.zero_fast_off = (u16)(fix_send.fixpara[23].floatdata * 100);//��������ٶ�ֵ
										break;
									case 24:
                    ProtectSet.para.zero_fast_off_time = (u16)(fix_send.fixpara[24].floatdata * 100);//�����ɸ澯��ֵ
										break;	
	                case 25: //����I�ξ��������
                    
										break;	
									case 26:
                    ProtectSet.para.zero_max_current_after = (u16)(fix_send.fixpara[26].floatdata * 100);//�������ٶ�ֵ
										break;
									case 27:
                    ProtectSet.para.zero_max_current_after_time = (u16)(fix_send.fixpara[27].floatdata * 100);
										break;	
	                case 28:
                    ProtectSet.para.once_reon_enable = fix_send.fixpara[28].bytedata[0];//�غ�բͶ��
										break;	
	                case 29:
                    ProtectSet.para.reon_Un_enable = fix_send.fixpara[29].bytedata[0];//�غ�բ����ѹͶ��
										break;	
	                case 30:
                    ProtectSet.para.reon_synchron_enable = fix_send.fixpara[30].bytedata[0];//�غ�բ��ͬ��Ͷ��
										break;
									case 31:
                    ProtectSet.para.once_reon_time = (u16)(fix_send.fixpara[31].floatdata * 100);//�غ�բʱ��
										break;									
		              case 32: //С�����ӵظ澯Ͷ��
                    
										break;
									case 33:
                    ProtectSet.para.zero_max_voltage = (u16)(fix_send.fixpara[33].floatdata * 100);//�����ѹ��ֵ
										break;
	               	case 34:
                    ProtectSet.para.max_voltage_enable = fix_send.fixpara[34].bytedata[0];//��ѹ����Ͷ��     
										break;
									case 35:
                    ProtectSet.para.max_voltage = (u16)(fix_send.fixpara[35].floatdata * 100);//��ѹ������ֵ
										break;
									case 36:
                    ProtectSet.para.max_voltage_time = (u16)(fix_send.fixpara[36].floatdata * 100);//��ѹ������ʱ
										break;
								  case 37:
                    ProtectSet.para.max_freq_enable = fix_send.fixpara[37].bytedata[0];//��Ƶ����Ͷ��       
										break;
									case 38:
                    ProtectSet.para.max_freq = (u16)(fix_send.fixpara[38].floatdata * 100);//��Ƶ������ֵ
										break;
									case 39:
                    ProtectSet.para.max_freq_time = (u16)(fix_send.fixpara[39].floatdata * 100);//��Ƶ������ʱ
										break;
									case 40:
                    ProtectSet.para.low_freq_enable = fix_send.fixpara[40].bytedata[0];//��Ƶ����Ͷ��
										break;
									case 41:
                    ProtectSet.para.low_freq = (u16)(fix_send.fixpara[41].floatdata * 100);//��Ƶ������ֵ
										break;
									case 42:
                    ProtectSet.para.low_freq_time = (u16)(fix_send.fixpara[42].floatdata * 100);//��Ƶ������ʱ
										break;
									default:	
							      break;	
								}									
							}
							Fixpara_State[i]=0;
						}
						Super_Sign[12]=8;		
					}
//						else if((Decryption.DataBuffer[9]==ACT)&&(Decryption.DataBuffer[15]>>6&0x01))  //ȡ��Ԥ��
//						{
//								if(succ_flag_spi)
//							{
//								succ_flag_spi=0;
//                SN_Num1=Decryption.DataBuffer[13] | (Decryption.DataBuffer[14]<<8);
//	              //ȡ��Ԥ�ã�����Ӧ���������
//								Super_Sign[12]=9;
//							}								
//						}
					else if((Decryption.DataBuffer[9]==DEACT)&&((Decryption.DataBuffer[15]&0x80)==0))       //��ֹ,ȡ��
					{
//          SN_Num1=Decryption.DataBuffer[13] | (Decryption.DataBuffer[14]<<8);
	          //�̻�һ�£�������ֵ����洢
						for(i=0;i<10;i++)							//������в���Ԥ��
						{
							Constpara_State[i]=0;
							Constpara_Tag[i]=0;
							Constpara_Len[i]=0;
						}
						for(i=0;i<32;i++)							//������в���Ԥ��
						{
							Runpara_State[i]=0;
							Runpara_Tag[i]=0;
							Runpara_Len[i]=0;
						}
						for(i=0;i<64;i++)							//�����ֵԤ��
						{
							Fixpara_State[i]=0;
							Fixpara_Tag[i]=0;
							Fixpara_Len[i]=0;
						}
						Super_Sign[12]=10;										
					}										
				}		
				else					
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//��֤ǩ��ʧ��
				break;		
		}
	}
	else if(jm_ok==2)	//������ȷ���յ���λ����ȷ
	{
		ca=0x80;
		Common_MW(USARTxChannel,ca);
		for(i=0;i<send_buffer_len;i++)
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],send_buffer[i]); //��һ֡�ı���
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
	/*---------------------------------��ʼ������------------------------------*/
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
				Send_len=YX_BefJM(11+YX_Number,ca);							//����ң�ű���
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
				Send_len=YC_BefJM(11+YC_Number*5,ca); 						//����ң�ⱨ��
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
				Send_len=Conclude_GeneralCall_BefJM(0x0C,ca);	//�������ٽ�������
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
					Super_Sign[1]=0;		
					call_num=1;	//�´����ٻ��Ͳ���������	
				}	
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[1]=0;
				}
			}
	/*---------------------------------ң��------------------------------*/
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
				Send_len=Ctrl_General_BefJm(C_SC_NA_1,0x0C,ca,ACTCON);                   //����ң��ѡ��ȷ�ϱ���
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				spi_flag=0;
				while(spi_flag!=3)															//��ȡ�ն������
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);														  //֡ͷ
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									//������ʼ�������ն���������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
					spi_flag=0;
					while(spi_flag!=3)															//
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														  //֡ͷ
						for(i=0;i<4;i++)															//����
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															//��վ�����
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															//��վ���������
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);															//Ӧ������
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);													//���ĳ���
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												//��������
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);															//��������ȸ��ֽ�
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);															//��������ȵ��ֽ�
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															//�����
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //����Ƭѡ
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
				  Send_len=Ctrl_General_BefJm(C_SC_NA_1,0x0C,ca,ACTCON);               //����ң��ִ��ȷ�ϱ���
				else if(Super_Sign[2]==6)
					Send_len=Ctrl_General_BefJm(C_DC_NA_1,0x0C,ca,ACTCON);               //����ң��ִ��ȷ�ϱ���
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									//������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
				  Send_len=Ctrl_General_BefJm(C_SC_NA_1,0x0C,ca,DEACTCON);        //����ң��ִ��ȷ�ϱ���
				else if(Super_Sign[2]==7)
				  Send_len=Ctrl_General_BefJm(C_DC_NA_1,0x0C,ca,DEACTCON);        //����ң��ִ��ȷ�ϱ���
			  BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 								     	    //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len	
				spi_flag=0;
				crc_yhf=0;
				if(YY_00_WR_SPI1(lc,Send_len)) 				                //����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
					Super_Sign[2]=0;
					yk_state=0;	//����֮��ָ���ʼ״̬
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
					if((control_jm.bit.QCS_RCS==1) && (Select101Addr==1)) //������Ҫִ��ң��
					{
						if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
						{
							RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
							SwitchFlag = 1;
						}
					}
					else if((control_jm.bit.QCS_RCS==0) && (Select101Addr==1))
					{
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
						{
							RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
							SwitchFlag = 2;
						}
					}
					Send_len=Ctrl_General_BefJm(C_SC_NA_1,0x0C,ca,ACTTERM);          //����ң�ؽ�������
			  }		
        else if(Super_Sign[2]==8 && Object_addr == (0x6001+ Ctrl_offset))
				{
					if((control_jm.bit.QCS_RCS==2) && (Select101Addr==1)) //������Ҫִ��ң��
					{
						if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
						{
							RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
							SwitchFlag = 1;
						}
					}
					else if((control_jm.bit.QCS_RCS==1) && (Select101Addr==1))
					{
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
						{
							RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
							SwitchFlag = 2;
						}
					}
					Send_len=Ctrl_General_BefJm(C_DC_NA_1,0x0C,ca,ACTTERM);          //����ң�ؽ�������
			  }					
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len				
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���	
					Select101Addr=0;
					Super_Sign[2]=0;
					yk_state=0;	//�ָ���ʼ״̬
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[2]=0;
				}			
			}
	/*---------------------------------ң������-------------------------------------*/
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
				  Send_len=AutoSend_YX_TIME(10*yx_data_num+9,ca,yx_data_num); //���ɴ�ʱ���ң�����ͱ���
				else if(Super_Sign[3] == 2)
					Send_len=AutoSend_YX(3*yx_data_num+9,ca,yx_data_num);       //���ɲ���ʱ��ң�����ͱ���
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 				  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len	
				spi_flag=0;
				crc_yhf=0;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					if(Super_Sign[3] == 2)
					  yx_data_flag=0;//ң�Ż����־λ��0
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���			
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[3]=0;
				}			
			}
	/*---------------------------------�ǳ�ʼ������------------------------------*/
			else if(Super_Sign[4]==1)    													//�ǳ�ʼ������ȷ�ϣ����Ա����
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
				Send_len=YX_BefJM(11+YX_Number,ca);							//����ң�ű���
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
				Send_len=YC_BefJM(11+YC_Number*5,ca); 						//����ң�ⱨ��
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
				Send_len=Conclude_GeneralCall_BefJM(0x0C,ca);	//�������ٽ�������
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���	
					Super_Sign[4]=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
					Super_Sign[4]=0;
				}			
			}
	/*----------------------------------ʱ��ͬ��/����----------------------------------*/
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
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
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
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
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
	/*----------------------------------ң������----------------------------------------*/
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
				Send_len=AutoSend_YC(7*yc_send.yc_num+9,ca);               //����ң�����ͱ���
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									//������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len	
				spi_flag=0;
				crc_yhf=0;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
					Super_Sign[7]=0;
					yc_send.yc_state=0;					//ң��״̬λ��0
					yc_send.yc_num=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
				}			
			}
	/*----------------------------------��λȷ��----------------------------------*/
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
				p_Reset=2;													//��λԭ��Զ����λ
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
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
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
	/*----------------------------------Ŀ¼�ٻ�----------------------------------*/
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
				if(Check_List_Name1())									//˵��Ŀ¼����
				{
					Send_len=Confirm_List_BefJM(ca,0);				 //0��ʾ�ɹ���1��ʾʧ��
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���	
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
					Send_len=Confirm_List_BefJM(ca,1);				 //0��ʾ�ɹ���1��ʾʧ��
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���	
						Super_Sign[9]=0;
					}
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);
						Super_Sign[9]=0;						
					}
				}				
			}
	/*----------------------------------�ļ�����----------------------------------*/
			else if(Super_Sign[10]==1)    //���ļ�����ȷ��
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
					Send_len=Confirm_ReadDoc_BefJM(ca,0);												//��ȡ�ļ�����ȷ�ϣ��ļ�������С�����ȵ�									
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
					Send_len=Confirm_ReadDoc_BefJM(ca,1);												//��ȡ�ļ�����ȷ�ϣ��ļ�������С�����ȵ�									
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
						Super_Sign_bak[10]=1;
					}	
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[10]=0;							
					}				
				}			
			}
			else if(Super_Sign[10]==2)								//���ļ�����
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
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
					Super_Sign[10]=2;	
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[10]=0;						
				}
			}
			else if(Super_Sign[10]==3)										//д�ļ�����ȷ��
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
				if(Check_WriteFile_Name()&&Finish_Flag1==0)			//�ļ�����ȷ
				{
					Send_len=Confirm_WriteDoc_BefJM(ca,0);	
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
						Super_Sign[10]=0;	
					}	
					else
					{
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
						Super_Sign[10]=0;							
					}				
				}			
			}
	//		else if(Super_Sign[10]==4)							//�������ݣ����Ա����
	//		{
	//			ca=0x80;																
	//			Common_MW(USARTxChannel,ca);					//�̶�֡��ȷ��֡	
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
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
	/*----------------------------------����----------------------------------*/		
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
				while(spi_flag!=3)															//��ȡ�ն������
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);														  //֡ͷ
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									//������ʼ�������ն���������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len	
					spi_flag=0;				
					while(spi_flag!=3)															//
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														  //֡ͷ
						for(i=0;i<4;i++)															//����
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															//��վ�����
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															//��վ���������
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);															//Ӧ������
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);													//���ĳ���
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												//��������
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);															//��������ȸ��ֽ�
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);															//��������ȵ��ֽ�
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															//�����
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //����Ƭѡ
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
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //���ټ��ܱ���	
//******************�����������ݳ���ʱ����Ϣ****************************************************
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
					    SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10); 	// �������ֽ���
							break;
						}
					}
//******************************************************************************
					//����ժҪֵ
					for(i=0;i<8;i++)
					{
						if(Msg_Safty1[6+i]!=Ter_Random1[i])	//��֤������Ƿ������Ƿ���ȥ��
						{
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//��֤�����ʧ��
							Super_Sign[10]=0;
							return;
						}
					}
					data_reset();
					GetBinFileMd5(decrypt);
					//��֤ժҪֵ
					lc=16+6+8+64;												//��֤�������ݳ��ȣ�ժҪֵ+ʱ����Ϣ����+�����+ǩ��ֵ
					spi_flag=0;
					while(spi_flag!=3)															//��֤ǩ������ȷ��
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);															//֡ͷ
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;
						SPI2_ReadWrite(0x08);
						crc_yhf^=0x08;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(Msg_Safty_sj[78]);				//asKID:ǩ����Կ����
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
						SSN_H();											   //����Ƭѡ
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)										//ǩ����ȷ
					{
						succ_flag_spi=0;
						RecoverKZQBackupFlag =1 ;		      //˵�������ļ�������ϣ����Խ���������	
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
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
	/*----------------------------------�����ٻ�----------------------------------*/
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
				Send_len=Confirm_Enegy_BefJM(0x0C,ca); 							//���ɵ��������ٻ�ȷ�ϱ���
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;						 									    //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //���ټ��ܱ���
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
				Send_len=Confirm_Enegy_Pluse_BefJM(0x33,ca);							//���ɵ������ݱ���
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
				Send_len=Conclude_Enegy_BefJM(0x0C,ca);	//���ɵ����ٻ���������
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
					Super_Sign[11]=0;
				}
				else
				{
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);	
					Super_Sign[11]=0;					
				}				
			}
	/*----------------------------------������д----------------------------------*/	
			else if(Super_Sign[12]==1)								//��ȡ��ֵ��
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
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
			else if(Super_Sign[12]==2)							//�л���ֵ��
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
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
			else if(Super_Sign[12]==3)							//��ȡȫ�������Ͷ�ֵ
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
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
			else if(Super_Sign[12]==4)													//��ȡ���ֲ����Ͷ�ֵ
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
				
        if(fix_value.params_num)             //��ֵ����
				{
					if(run_value.params_num||const_value.params_num)
						Send_len=Read_FixvalueParas_BefJM(0,1);	
					else
					{
						params_flag=1;
						Send_len=Read_FixvalueParas_BefJM(0,0);	
					}
				}
				else if(run_value.params_num)             //���в���
				{
					if(const_value.params_num)
						Send_len=Read_RunParas_BefJM(0,1);
					else
					{
						params_flag=1;
						Send_len=Read_RunParas_BefJM(0,0);
					}	
				}
				else if(const_value.params_num)            //���в���
				{
					params_flag=1;
					Send_len=Read_ConstParas_BefJM(0,0);		
				}
					
				BW_Before_JM1[1]=Send_len-6;
				BW_Before_JM1[2]=Send_len-6;							
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
			else if(Super_Sign[12]==7)												//����Ԥ��
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
				while(spi_flag!=3)															//��ȡ�ն������
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);														  //֡ͷ
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									//������ʼ�������ն���������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len	
					spi_flag=0;				
					while(spi_flag!=3)															//
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														  //֡ͷ
						for(i=0;i<4;i++)															//����
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															//��վ�����
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															//��վ���������
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);															//Ӧ������
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);													//���ĳ���
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												//��������
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);															//��������ȸ��ֽ�
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);															//��������ȵ��ֽ�
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															//�����
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //����Ƭѡ
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
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
	//			Send_len=Act_Set_BefJM(ca,0x07,0xC0);		          //ȡ��Ԥ��
	//			lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
				Send_len=Act_Set_BefJM(ca,0x09,0x40);		          //ȡ��Ԥ��	
					BW_Before_JM1[1]=Send_len-6;
					BW_Before_JM1[2]=Send_len-6;							
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
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
	/*----------------------------------��·����----------------------------------*/
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
//----------------���Ĵ���
void ProtocolResolve1_MW(u8 USARTxChannel)	
{
	u16 lc=0;	
	u8 Send_len=0,ca=0;
	if((zz_ok==1)&&(wg_ok==1))
	{
		ADDR= Decryption.DataBuffer[2]+(Decryption.DataBuffer[3]<<8);
		switch(Decryption.DataBuffer[1])
		{
	/*-------------------------------��ʼ��----------------------------------*/	
			case 0x42:													//��������
				ca=0x80;
				Common_MW(USARTxChannel,ca);					
				break;
			case 0x49:													//������·״̬����վΪ����վ
				flag_succ=0;											//ÿ�γ�ʼ����Ҫ��0����Ȼ�ڶ��γ�ʼ����������ʼ��������Ӧ
				Super_Sign[0]=1;
				ca=0x8B;
				Common_MW(USARTxChannel,ca);			//��·״̬���
				break;
			case 0x40:													//��λԶ����·
				Super_Sign[0]=1;
				ca=0x80;
				Common_MW(USARTxChannel,ca);			//��·��λȷ��
				flag_succ=0;
				os_dly_wait (30);
				ca=0xC9;
				Common_MW(USARTxChannel,ca);			//������·״̬���ն�Ϊ����վ	
				break;
			case 0x0B:												  //��·״̬��ã���վ�ظ�
				Super_Sign[0]=1;
				call_num=0;
				yk_state=0;
				flag_succ=0;
				jm=0;
				ca=0xC0;
				Common_MW(USARTxChannel,ca);			//��λԶ����·����λ��վ	
				break;
			case 0x00:													//��·��λȷ�ϣ���վȷ�ϣ���ʼ�����������漴Ϊ�ȴ��ٻ�	
				if(flag_succ==0)												//δ��ɾͽ��룬���������
				{
					FCB_Flag_Slave=0;								//�����Ϊ���ն��״�ʹ��FCVλ��FCBλ��0��ʼ��Ч�����濪ʼ��ת
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
					delay_time_101=0;												    //ÿ�ν�������4���������Լ�1
					send_buffer_len=0;													//���յ���վ��ȷ��֡�Ͱ���һ֡�ı���ɾ��
					timer_enable=0;
					resend_num=0;
	/*-------------------------------��ʼ�����ٻ�----------------------------------*/	
	/*------������Բ�����active_upload��������Ϊ�ǳ�ʼ�������޷������------*/
	/*----����Ϊ�˱�֤��active_upload�����в����ͱ�ģ��⼸��ѡ��Ҫ�ӽ�ȥ----*/
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
	/*-------------------------------ң��----------------------------------*/			
	/*------------------ң�ص����ȼ�Ҳ�Ƚϸߣ������Դ��-------------------*/					
					else if(Super_Sign[2]==2)
					{	
	//ִ�н���֮������ң�ؽ�������
						Super_Sign[2]=4;
					}
					else if(Super_Sign[2]==6)
					{	
	//ִ�н���֮������ң�ؽ�������
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
	/*-------------------------------�ǳ�ʼ�����ٻ�----------------------------------*/	
					else if(Super_Sign[4]==1)
					{
						if(Super_Sign_bak[4]==1)	//˵��ִ�й���
						{
							Super_Sign_bak[4]=0;
							Super_Sign[4]=2;
						}	
					}
					else if(Super_Sign[4]==2)
					{
						if(Super_Sign_bak[4]==2)	//˵��ִ�й���
						{
							Super_Sign_bak[4]=0;
							Super_Sign[4]=3;
						}
					}
					else if(Super_Sign[4]==3)
					{
						if(Super_Sign_bak[4]==3)	//˵��ִ�й���
						{
							Super_Sign_bak[4]=0;
							Super_Sign[4]=4;
						}
					}
					
	/*-------------------------------��λ----------------------------------*/
					else if(Super_Sign[8]==2)								//��λȷ�ϵõ��ظ�֮���ٽ��и�λ
					{
					  __set_FAULTMASK(1);
						NVIC_SystemReset();			
					}
	/*-------------------------------�ļ�����----------------------------------*/
					else if(Super_Sign[10]==1)
					{
						if(Super_Sign_bak[10]==1)	//˵��ִ�й���
						{
							Super_Sign_bak[10]=0;
							Super_Sign[10]=2;
						}										
					}
	/*-------------------------------����----------------------------------*/
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

void IEC_104_Resolve_JM(u8 USARTxChannel)					//�����spi���յ������ݣ�������Ҫ���ܵĶ���I֡���ݣ�U�����ݺ�S�����ݲ�����
{
	u16 Add=0,lc=0,i=0;	
	u8 Send_len=0,Params_Num=0,ok_flag=0,j=0,crc_yhf=0,sum=0;
	u8 spi_flag=0;
	u8 cd=0;

	if((Decryption.DataBuffer[0]==0x68)&&(Decryption.DataBuffer[1]>4)&&(Decryption.DataBuffer[1]<=253)&&((Decryption.DataBuffer[2]&1)==0)&&((Decryption.DataBuffer[4]&1)==0))//I��
	{
		Send_NumBak=((((u16)Decryption.DataBuffer[3])<<8)&0xFF00)|(Decryption.DataBuffer[2]&0xFF);		
		Send_NumBak>>=1;																//���������ķ������кţ�������Ϊ�Լ����͵Ĵ���
	  //�����ʵ����Ҫ�����ǲ���Ҫȥ�ж������ȷ����������վ��Ϊ�Լ����͵ĸ������ն�ֻ��Ҫ�������к�
		Recv_NumBak=((((u16)Decryption.DataBuffer[5])<<8)&0xFF00)|(Decryption.DataBuffer[4]&0xFF);			//���������Ľ������кţ�������Ϊ�Լ����յĴ���
		Recv_NumBak>>=1;
		//���Send_Num-Recv_NumBak>12��ֹͣ���ͣ���ʱBackstageProtocol0�д�ŵ��ǵ�ǰ�յ��ı���
		Recv_Num++; 																										//�������ռ�1������ʵ�ʽ��յ���I֡���ݴ���
		
		cd=Send_Num-Recv_NumBak;	//����δ��ȷ�ϵ��������������Ӧ����С�ڵ���queen_sum��
		if(cd>=12)
		{
			diff_flag=1;	//˵��δȷ��֡���ﵽkֵ���ޣ����ٷ���
		}
		else if(cd==0)	//ȫ��ȷ�ϣ�û��δȷ��֡
		{
			diff_flag=0;
			ok_flag=1;   												//�����I֡������һ
		}
		else
		{
			diff_flag=0;
			ok_flag=1;   												//�����I֡������һ
		}
		cd=0;
		Asdu_addr=(u16)Decryption.DataBuffer[11]<<8|Decryption.DataBuffer[10]; //ASDU��ַ
		Informat_addr[0]=Decryption.DataBuffer[12];	  //��Ϣ��ַ
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
			case C_IC_NA_1:																			//���ٻ� 
				if(call_num1==0)
				{
					Send_len=GeneralCall_BefJM(0x0C,0);							//�������ټ����
					BW_Before_JM1[1]=Send_len-2;
					lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
					if(YY_00_WR_SPI1(lc,Send_len)) 
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //���ټ��ܱ���
						Send_Num++;
					}	
					else
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);		
		//���淢������ң�ź�ң�ⱨ�ģ�����Ҫ����ң�ź�ң�ⱨ��
					Send_len=YX_BefJM(13+YX_Number,0);								//����ң�ű���
					BW_Before_JM1[1]=Send_len-2;
					lc=16+4+Send_len;
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
						Send_Num++;
					}	
					else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);						
					Send_len=YC_BefJM(13+YC_Number*3,0); 						//����ң�ⱨ��
					BW_Before_JM1[1]=Send_len-2;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
						Send_Num++;
					}	
					else
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);						
		//�������ٻ�
					Send_len=Conclude_GeneralCall_BefJM(0x0C,0);	//�������ٽ�������
					BW_Before_JM1[1]=Send_len-2;
					lc=16+4+Send_len;		
					if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
					{
						succ_flag_spi=0;
						Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
						Send_Num++;
						call_num1=1;	//�´β��ٽ�������ж����
					}	
					else
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9109);						
				}
				else
				{
					Super_Sign[4]=1;
				}
				break;
			case C_CS_NA_1: 																		//��ʱ
				if(Decryption.DataBuffer[8]==ACT)					//ʱ��ͬ��
				{
					TimeNow.msec    = ((u16)(Decryption.DataBuffer[15] | (u16)(Decryption.DataBuffer[16]<<8))%1000)&0x3FF;
					TimeNow.second  = ((u16)(Decryption.DataBuffer[15] | (u16)(Decryption.DataBuffer[16]<<8))/1000)&0x3F;  
					TimeNow.minute  = Decryption.DataBuffer[17]&0x3F;
					TimeNow.hour    = Decryption.DataBuffer[18]&0x1F;
					TimeNow.date    = Decryption.DataBuffer[19]&0x1F;
					TimeNow.month   = Decryption.DataBuffer[20]&0x0F;
					TimeNow.year    = Decryption.DataBuffer[21]&0x7F;
					correct_time();
	//����ʱ��ͬ��ȷ�ϱ���
					Super_Sign[6]=1;
				}
				else if(Decryption.DataBuffer[8]==REQ)		//ʱ������
				{
	//����ʱ������ȷ�ϱ���
					Super_Sign[6]=2;				
				}
				break;
			case C_RP_NA_1:     																//��λ��������
				Super_Sign[8]=1;
				break;
			case C_CI_NA_1:     								 								//���������ٻ�	
				Super_Sign[11]=1;			
				break;	

			case C_SC_NA_1:																			//˫��ң�ظĳɵ���ң��
			case C_DC_NA_1:																			//˫���ң��
			  if(Decryption.DataBuffer[8]==ACT)   //�ж�S/Eλ��0����1����SCO(������),DCO(˫����)�����λ����8λ����
				{
					if(Decryption.DataBuffer[15]&0x80)	//ң��ѡ��
					{
						switch(YY_Style1)
						{
							case 1:	//ǩ��
							case 4:	//ǩ��
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
					}
					else	//ң��ִ��
					{
						switch(YY_Style1)
						{
							case 3:	//ǩ��
							case 5:
							case 6:
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}			
					}
				}
				
				lc=BW_Len1+MsgSafty_Len1-1;												//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
				spi_flag=0;
				while(spi_flag!=3)															//��֤ǩ������ȷ��
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															//֡ͷ
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:ǩ����Կ����
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
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					control_jm.byte=Decryption.DataBuffer[15];
					if(Decryption.DataBuffer[8]==ACT)  // ����
					{
						if(control_jm.bit.S_E == 1)      //�ж�S/Eλ��0����1����SCO(������),DCO(˫����)�����λ����8λ��
						{
							if(yk_state==0 && Select101Addr==0)	//��ʼ״̬
							{
								Select101Addr=1;
								Send_len=Ctrl_General_BefJm(Decryption.DataBuffer[6],0x0C,0,ACTCON);                  //����ң��ѡ��ȷ�ϱ���
								BW_Before_JM1[1]=Send_len-2;							
								spi_flag=0;
								while(spi_flag!=3)															//��ȡ�ն������
								{
									SSN_L();	
									SPI2_ReadWrite(0x55);														  //֡ͷ
									for(i=0;i<6;i++)
									{
										SPI2_ReadWrite(Get_Ter_Random[i]);	
										crc_yhf^=Get_Ter_Random[i];
									}
									crc_yhf=~crc_yhf;
									SPI2_ReadWrite(crc_yhf);	
									crc_yhf=0;
									SSN_H();											   //����Ƭѡ
									spi_flag=Ter_WriteRead_Spi1();
								}	
								if(succ_flag_spi) 
								{
									succ_flag_spi=0;
									for(i=0;i<8;i++)
										Ter_Random1[i]=Spi_Recv1[i];
									lc=16+8+4+Send_len;						 									//������ʼ�������ն���������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
									spi_flag=0;								
									while(spi_flag!=3)															//
									{
										SSN_L();	
										SPI2_ReadWrite(0x55);														  //֡ͷ
										for(i=0;i<4;i++)															//����
										{
											SPI2_ReadWrite(Send_JM[i]);	
											crc_yhf^=Send_JM[i];
										}
										SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
										crc_yhf^=(u8)(lc>>8)&0xFF;
										SPI2_ReadWrite((u8)lc&0xFF);
										crc_yhf^=(u8)lc&0xFF;
										for(i=0;i<8;i++) 															//��վ�����
										{
											SPI2_ReadWrite(Zz_Random1[i]);
											crc_yhf^=Zz_Random1[i];
										}	
										for(i=0;i<8;i++) 															//��վ���������
										{
											SPI2_ReadWrite(~Zz_Random1[i]);
											crc_yhf^=(~Zz_Random1[i]);
										}	
										SPI2_ReadWrite(0x02);															//Ӧ������
										crc_yhf^=0x02;
										SPI2_ReadWrite(Send_len);													//���ĳ���
										crc_yhf^=Send_len;
										for(i=0;i<Send_len;i++)												//��������
										{
											SPI2_ReadWrite(BW_Before_JM1[i]);	
											crc_yhf^=BW_Before_JM1[i];
										}
										SPI2_ReadWrite(0x00);															//��������ȸ��ֽ�
										crc_yhf^=0x00;		
										SPI2_ReadWrite(0x08);															//��������ȵ��ֽ�
										crc_yhf^=0x08;		
										for(i=0;i<8;i++)															//�����
										{
											SPI2_ReadWrite(Ter_Random1[i]);	
											crc_yhf^=Ter_Random1[i];
										}
										crc_yhf=~crc_yhf;
										SPI2_ReadWrite(crc_yhf);	
										crc_yhf=0;
										SSN_H();											   //����Ƭѡ
										spi_flag=Ter_WriteRead_Spi1();
									}	
									if(succ_flag_spi) 
									{
										succ_flag_spi=0;
										Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);
										Send_Num++;
										yk_state=1;	//ѡ��״̬
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
								yk_state=0;	//�ָ���ʼ״̬
							}
						}
						else	if(control_jm.bit.S_E == 0)		//ң��ִ��
						{
							for(i=0;i<8;i++)
							{
								if(Msg_Safty1[6+i]!=Ter_Random1[i])	//��֤������Ƿ������Ƿ���ȥ��
								{
									Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//��֤�����ʧ��
									return;
								}
							}
							if(yk_state==1 && Select101Addr==1)
							{
								Send_len=Ctrl_General_BefJm(Decryption.DataBuffer[6],0x0C,0,ACTCON);  //����ң��ִ��ȷ�ϱ���
								BW_Before_JM1[1]=Send_len-2;
								lc=16+4+Send_len;						 									         //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
								spi_flag=0;
								crc_yhf=0;
								if(YY_00_WR_SPI1(lc,Send_len)) 				                 //����ң�ű��ĳɹ�
								{
									succ_flag_spi=0;
									Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);   //����ң�ż��ܱ���
									Send_Num++;
									yk_state=3;	//ң��ִ�У�ִ�н���֮������ң�ؽ�������
									
									if(Informat_addr[0] == (0x01+ Ctrl_offset))
									{
										if(Decryption.DataBuffer[6] == C_SC_NA_1)          // ����ң������
									  {
											if((control_jm.bit.QCS_RCS==1) && (Select101Addr==1))
											{
												if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
												{
													RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
													SwitchFlag = 1;
												}
											}
											else if((control_jm.bit.QCS_RCS==0) && (Select101Addr==1))
											{
												if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
												{
													RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
													SwitchFlag = 2;
												}
											}
									  }
										else if(Decryption.DataBuffer[6] == C_DC_NA_1)     // ˫��ң������
									  {
											if((control_jm.bit.QCS_RCS==2) && (Select101Addr==1))
											{
												if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
												{
													RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
													SwitchFlag = 1;
												}
											}
											else if((control_jm.bit.QCS_RCS==1) && (Select101Addr==1))
											{
												if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
												{
													RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
													SwitchFlag = 2;
												}
											}
									  }
								  }										
									Send_len=Ctrl_General_BefJm(Decryption.DataBuffer[6],0x0C,0,ACTTERM);         //����ң�ؽ�������
									BW_Before_JM1[1]=Send_len-2;
									lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
									spi_flag=0;
									crc_yhf=0;	
									if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
									{
										succ_flag_spi=0;
										Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
										Send_Num++;
										Select101Addr=0;
										yk_state=0;	//ң��ִ�н���
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
					else if(Decryption.DataBuffer[8]==DEACT)  //������Ҫִ��ң��ȡ��������ң�س�������û�������
					{
						if(yk_state==1)
						{
							Select101Addr=0;	
							Send_len=Ctrl_General_BefJm(Decryption.DataBuffer[6],0x0C,0,DEACTCON);             //����ң��ִ��ȷ�ϱ���
							BW_Before_JM1[1]=Send_len-2;
							lc=16+4+Send_len;						 									//������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len	
							spi_flag=0;
							crc_yhf=0;
							if(YY_00_WR_SPI1(lc,Send_len)) 				//����ң�ű��ĳɹ�
							{
								succ_flag_spi=0;
								Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);			//��ǩʧ��
				break;
	//		case F_SC_NA_1:												//�ٻ�Ŀ¼,���ﲻһ�����ٻ�Ŀ¼��Ҳ�п������ļ�����
	//			break;
			case F_FR_NA_1:		  									//�ļ�����,�������ļ�Ŀ¼�ٻ�����д�ļ������д�ļ����䣬��Ҫ����������ʶ����ʶ��
				if(Decryption.DataBuffer[16]==1)								//�ٻ��ļ�Ŀ¼
				{
					if(Decryption.DataBuffer[21]==0)								//Ĭ��Ŀ¼,COMTRADE
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
						List_Num1=Decryption.DataBuffer[21];							//�ٻ���Ŀ¼������
						for(i=0;i<List_Num1;i++)
						{
							List_Name1[i]=Decryption.DataBuffer[22+i];			//ASC����ʽ�洢��Ŀ¼��
						}				
					}
					if(Decryption.DataBuffer[22+List_Num1])					//��Ҫ����ļ��ڴ�ʱ�����
					{
						List_Name1[49]=1;
						for(i=0;i<7;i++)
						{
//							Start_Time[i]=Decryption.DataBuffer[23+List_Num+i]; //ʱ�������
//							End_Time[i]=Decryption.DataBuffer[30+List_Num+i];
						}				
					}
					else
						List_Name1[49]=0;
					Super_Sign[9]=1;
				}
				else if(Decryption.DataBuffer[16]==3)					       	//���ļ�����
				{
					List_Num1=Decryption.DataBuffer[17];									//Ҫ��ȡ���ļ�������
					for(i=0;i<List_Num1;i++)													
					{
						List_Name1[i]=Decryption.DataBuffer[18+i];			   	//ASC����ʽ�洢�ļ�����������չ��������׺������.dat
					}
					Super_Sign[10]=1;
				}
				else if(Decryption.DataBuffer[16]==6)						//���ļ����ݴ���ȷ��
				{
					Super_Sign[10]=0;	
				}
				else if((Decryption.DataBuffer[8]==0x06)&&(Decryption.DataBuffer[16]==7))					//д�ļ�����
				{
					List_Num1=Decryption.DataBuffer[17];									    //Ҫд����ļ����ĳ���
					for(i=0;i<List_Num1;i++)
					{
						List_Name1[i]=Decryption.DataBuffer[18+i];			  		 	//ASC����ʽ�洢�ļ���
					}	
					Rec_Doc_Num1	|= (((u32)Decryption.DataBuffer[22+List_Num1])&0x000000FF);		
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[23+List_Num1])<<8)&0x0000FF00);	
					Rec_Doc_Num1	|= ((((u32)Decryption.DataBuffer[24+List_Num1])<<16)&0x00FF0000);		
					Rec_Doc_Num1 |= ((((u32)Decryption.DataBuffer[25+List_Num1])<<24)&0xFF000000);	
					if(Rec_Doc_Num1>0x80000)	Finish_Flag1=3;											 			//���ȼ�飬���ܴ���512k
				  //if(Check_List_Name(List_Name)) 	Finish_Flag=2;//�ļ�����飬��д�ļ��У���Ҫ�Ƕ�ȡ��д�ļ�Ӧ��ֻ����������Ż��õ�
					//д�ļ�ȷ�ϼ���	
					lc=BW_Len1+MsgSafty_Len1-1;												//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
					spi_flag=0;
					while(spi_flag!=3)															//��֤ǩ������ȷ��
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);															//֡ͷ
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;
						SPI2_ReadWrite(0x08);
						crc_yhf^=0x08;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:ǩ����Կ����
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
						SSN_H();											   //����Ƭѡ
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi) 																
					{
						succ_flag_spi=0;
						Super_Sign[10]=3;
					}
					else
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);			//��ǩʧ��	
				}
				else if((Decryption.DataBuffer[8]==0x05)&&(Decryption.DataBuffer[16]==9)) //д�ļ�����
				{
					sum=0;
					Rec_Error_Flag1=0;
					Receive_Doc_101();			//ʵ��ʹ��																	//ÿ�ν��ܵ���վ��д�ļ����ݶ������
					for(i=0;i<(Decryption.DataBuffer[1]-25);i++)												//�����ν������ݴ洢����
					{
						ucaFlashBuf[i]=Decryption.DataBuffer[26+i];												//ÿ�ν��յ������ȴ浽pbuffer[]��ȥ
						sum+=Decryption.DataBuffer[26+i];
					}
					if(sum!=Decryption.DataBuffer[Decryption.DataBuffer[1]+1])		//���У��ֵ���ԣ������־λ��һ
					{
						Rec_Error_Flag1=2;
					}
					Super_Sign[10]=4;	//ʵ��ʹ��
					if((Decryption.DataBuffer[25]==0)||(Rec_Error_Flag1))   //�޺������߳���
					{
						ulFlieLength=Decryption.DataBuffer[1]-25;
						Super_Sign[10]=5;	
					}
				}
				break;	
			case F_SR_NA_1:												//�������
				if(Decryption.DataBuffer[8]==0x06)
				{
					if(Decryption.DataBuffer[15]&0x80)							//�ж�S/E��1����������
					{
						switch(YY_Style1)
						{
							case 0:	//ǩ��
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
								return;
							default:
								break;
						}
						
						lc=BW_Len1+MsgSafty_Len1-1;												//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
						spi_flag=0;
						while(spi_flag!=3)															//��֤ǩ������ȷ��
						{
							SSN_L();	
							SPI2_ReadWrite(0x55);															//֡ͷ
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;
							SPI2_ReadWrite(0x08);
							crc_yhf^=0x08;
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;
							SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:ǩ����Կ����
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
							SSN_H();											   //����Ƭѡ
							spi_flag=Ter_WriteRead_Spi1();
						}	
						if(succ_flag_spi) 																		
						{
							succ_flag_spi=0;
							Super_Sign[10]=6;
						}
						else
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);			//��ǩʧ��
					}
					else																									//0��������
					{
						Super_Sign[10]=7;										
					}
				}
				else if(Decryption.DataBuffer[8]==0x08)					//��������
				{
					Super_Sign[10]=8;   							
				}		
				break;
			case C_RR_NA_1:                     	//����ǰ��ֵ��
				Super_Sign[12]=1;		
				break;	
			case C_SR_NA_1:                     	            //�л���ֵ��
				switch(YY_Style1)
				{
					case 0:	//ǩ��
						Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
						return;
					default:
						break;
				}
						
				lc=BW_Len1+MsgSafty_Len1-1;											//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
				spi_flag=0;
				while(spi_flag!=3)															//��֤ǩ������ȷ��
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															//֡ͷ
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:ǩ����Կ����
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
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 																
				{
					succ_flag_spi=0;
					SN_Num1=(u16)Decryption.DataBuffer[15]&0xFF;
					SN_Num1|=(u16)Decryption.DataBuffer[16]<<8&0xFF00;
					Super_Sign[12]=2;	
	//��Ҫ�л���SN_Num��
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//��֤ǩ��ʧ��
				break;	
			case C_RS_NA_1:                     											//�������Ͷ�ֵ
	//			SN_Num=(u16)Decryption.DataBuffer[12]&0xFF;
	//			SN_Num=(u16)Decryption.DataBuffer[13]<<8&0xff00;	
				Params_total = 0;
				fix_value.params_num = 0;
				run_value.params_num = 0;
				const_value.params_num = 0;
				
				if((Decryption.DataBuffer[7]&0x7F)==0)								//��ȡȫ������
				{
					Super_Sign[12]=3;	
				}
				else
				{	
					for(i=0;i<(Decryption.DataBuffer[7]&0x7F);i++)
					{
						if((Decryption.DataBuffer[15+3*i]==0x82)&&(Decryption.DataBuffer[14+3*i]>=0x20))     //��ֵ����
						{
							fix_value.params_addr[fix_value.params_num++]=Decryption.DataBuffer[14+3*i];//ֻȡ�͵�ַ
							Params_total++;
						}
						else if((Decryption.DataBuffer[15+3*i]==0x80)&&(Decryption.DataBuffer[14+3*i]>=0x20))	//���в���			
						{
							run_value.params_addr[run_value.params_num++]=Decryption.DataBuffer[14+3*i];//ֻȡ�͵�ַ
							Params_total++;
						}
						else if((Decryption.DataBuffer[15+3*i]==0x80)&&(Decryption.DataBuffer[14+3*i]<0x20))  //���в���
						{
							const_value.params_addr[const_value.params_num++]=Decryption.DataBuffer[14+3*i];//ֻȡ�͵�ַ
							Params_total++;
						}
					}
					Super_Sign[12]=4;	
				}
				break;	
			case C_WS_NA_1:                     											//д�����Ͷ�ֵ
				if((Decryption.DataBuffer[8]==0x06)&&(Decryption.DataBuffer[14]&0x80))	//����Ԥ��
				{	
					switch(YY_Style1)
					{
						case 0:	//ǩ��
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
							return;
						default:
							break;
					}
				}
				else if((Decryption.DataBuffer[8]==0x06)&&((Decryption.DataBuffer[14]&0xc0)==0))   //�����̻�
				{
					switch(YY_Style1)
					{
						case 1:	//ǩ��
						case 2:	//ǩ��
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9101);
							return;
						default:
							break;
					}
				}
					
				lc=BW_Len1+MsgSafty_Len1-1;												//��֤�������ݳ��ȣ����ĳ���+ʱ����Ϣ����+ǩ��ֵ
				spi_flag=0;
				while(spi_flag!=3)															//��֤ǩ������ȷ��
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);															//֡ͷ
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;
					SPI2_ReadWrite(0x08);
					crc_yhf^=0x08;
					SPI2_ReadWrite(0x00);
					crc_yhf^=0x00;
					SPI2_ReadWrite(Msg_Safty1[MsgSafty_Len1-1]);				//asKID:ǩ����Կ����
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
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 																		
				{
					succ_flag_spi=0;
					if((Decryption.DataBuffer[8]==0x06)&&(Decryption.DataBuffer[14]&0x80))	//����Ԥ��
					{
	//����Ԥ�ã�����һ�£��̻�����Ҫ��ȡ����Ҫ
						Params_Num=Decryption.DataBuffer[7]&0x7F;																		//ҪԤ�ü�������
	//						SN_Num=(u16)Decryption.DataBuffer[13]<<8|Decryption.DataBuffer[12]; //����
						for(i=0;i<Params_Num;i++)
						{
							Add=(u16)Decryption.DataBuffer[16+5*i+cd]<<8|Decryption.DataBuffer[15+5*i+cd];
							if(Add>0x8000 && Add<0x8020)               //���в���
							{
								Add-=0x8000;														 //�жϲ�������
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
							else if((Add>=0x8020)&&(Add<=0x821F))										//���в���
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
							else if((Add>=0x8220)&&(Add<=0x85EF))										//��ֵ
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
					else if((Decryption.DataBuffer[8]==0x06)&&((Decryption.DataBuffer[14]&0xc0)==0))   //�����̻�
					{
	//						SN_Num=(u16)Decryption.DataBuffer[15]&0xFF;
	//						SN_Num=(u16)Decryption.DataBuffer[16]<<8&0xff00;
		//�̻�һ�£�������ֵ����洢
						for(i=0;i<8;i++)
						{
							if(Msg_Safty1[i]!=Ter_Random1[i])	//��֤������Ƿ������Ƿ���ȥ��
							{
								Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//��֤�����ʧ��
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
                    ProtectSet.para.delay_fast_off = (u16)(fix_send.fixpara[9].floatdata * 100);//����I�ζ�ֵ
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
                    ProtectSet.para.max_current = (u16)(fix_send.fixpara[14].floatdata * 100);//����II�ζ�ֵ
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
                    ProtectSet.para.max_current_after = (u16)(fix_send.fixpara[18].floatdata * 100);//��������ٶ�ֵ
										break;
									case 19:
                    ProtectSet.para.max_current_after_time = (u16)(fix_send.fixpara[19].floatdata * 100);
										break;
									case 20:
                    ProtectSet.para.max_load = (u16)(fix_send.fixpara[20].floatdata * 100);//�����ɸ澯��ֵ
										break;
									case 21:
                    ProtectSet.para.max_load_time = (u16)(fix_send.fixpara[21].floatdata * 100);
										break;										
	                case 22:
                    ProtectSet.para.zero_fast_off_enable = fix_send.fixpara[22].bytedata[0];
										break;
									case 23:
                    ProtectSet.para.zero_fast_off = (u16)(fix_send.fixpara[23].floatdata * 100);//��������ٶ�ֵ
										break;
									case 24:
                    ProtectSet.para.zero_fast_off_time = (u16)(fix_send.fixpara[24].floatdata * 100);//�����ɸ澯��ֵ
										break;	
	                case 25: //����I�ξ��������
                    
										break;	
									case 26:
                    ProtectSet.para.zero_max_current_after = (u16)(fix_send.fixpara[26].floatdata * 100);//�������ٶ�ֵ
										break;
									case 27:
                    ProtectSet.para.zero_max_current_after_time = (u16)(fix_send.fixpara[27].floatdata * 100);
										break;	
	                case 28:
                    ProtectSet.para.once_reon_enable = fix_send.fixpara[28].bytedata[0];//�غ�բͶ��
										break;	
	                case 29:
                    ProtectSet.para.reon_Un_enable = fix_send.fixpara[29].bytedata[0];//�غ�բ����ѹͶ��
										break;	
	                case 30:
                    ProtectSet.para.reon_synchron_enable = fix_send.fixpara[30].bytedata[0];//�غ�բ��ͬ��Ͷ��
										break;
									case 31:
                    ProtectSet.para.once_reon_time = (u16)(fix_send.fixpara[31].floatdata * 100);//�غ�բʱ��
										break;									
		              case 32: //С�����ӵظ澯Ͷ��
                    
										break;
									case 33:
                    ProtectSet.para.zero_max_voltage = (u16)(fix_send.fixpara[33].floatdata * 100);//�����ѹ��ֵ
										break;
	               	case 34:
                    ProtectSet.para.max_voltage_enable = fix_send.fixpara[34].bytedata[0];//��ѹ����Ͷ��     
										break;
									case 35:
                    ProtectSet.para.max_voltage = (u16)(fix_send.fixpara[35].floatdata * 100);//��ѹ������ֵ
										break;
									case 36:
                    ProtectSet.para.max_voltage_time = (u16)(fix_send.fixpara[36].floatdata * 100);//��ѹ������ʱ
										break;
								  case 37:
                    ProtectSet.para.max_freq_enable = fix_send.fixpara[37].bytedata[0];//��Ƶ����Ͷ��       
										break;
									case 38:
                    ProtectSet.para.max_freq = (u16)(fix_send.fixpara[38].floatdata * 100);//��Ƶ������ֵ
										break;
									case 39:
                    ProtectSet.para.max_freq_time = (u16)(fix_send.fixpara[39].floatdata * 100);//��Ƶ������ʱ
										break;
									case 40:
                    ProtectSet.para.low_freq_enable = fix_send.fixpara[40].bytedata[0];//��Ƶ����Ͷ��
										break;
									case 41:
                    ProtectSet.para.low_freq = (u16)(fix_send.fixpara[41].floatdata * 100);//��Ƶ������ֵ
										break;
									case 42:
                    ProtectSet.para.low_freq_time = (u16)(fix_send.fixpara[42].floatdata * 100);//��Ƶ������ʱ
										break;
									default:	
							      break;
							  }
                Fixpara_State[i]=0;		
						  }							
						}
						Super_Sign[12]=8;			
					}
	//				else if((Decryption.DataBuffer[8]==0x06)&&(Decryption.DataBuffer[14]>>6&0x01))  //ȡ��Ԥ��
	//				{
	//					if(succ_flag_spi)
	//					{
	//						succ_flag_spi=0;
	//						SN_Num=(u16)Decryption.DataBuffer[15]&0xFF;
	//						SN_Num=(u16)Decryption.DataBuffer[16]<<8&0xff00;
	//	//�̻�һ�£�������ֵ����洢
	//						Super_Sign[12]=9;									
	//					}								
	//				}
					else if((Decryption.DataBuffer[8]==0x08)&&((Decryption.DataBuffer[14]&0x80)==0))       //��ֹ
					{
	//							SN_Num=(u16)Decryption.DataBuffer[15]&0xFF;
	//							SN_Num=(u16)Decryption.DataBuffer[16]<<8&0xff00;
	//�̻�һ�£�������ֵ����洢
						for(i=0;i<10;i++)							//������в���Ԥ��
						{
							Constpara_State[i]=0;
							Constpara_Tag[i]=0;
							Constpara_Len[i]=0;
						}
						for(i=0;i<32;i++)							//������в���Ԥ��
						{
							Runpara_State[i]=0;
							Runpara_Tag[i]=0;
							Runpara_Len[i]=0;
						}
						for(i=0;i<64;i++)							//�����ֵԤ��
						{
							Fixpara_State[i]=0;
							Fixpara_Tag[i]=0;
							Fixpara_Len[i]=0;
						}
						Super_Sign[12]=10;																	
					}			
				}
				else
					Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);					//��֤ǩ��ʧ��
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
	if((zz_ok==1)&&(wg_ok==1))	//if((yw_ok==1)&&(zz_ok_104==1)&&(wg_ok_104==1))�Ƿ���Ҫ
	{
		if(datdone == 2)     //U��
		{
			if(Decryption.DataBuffer[2]==0x7)	  				 //�������ֻ������վ����
			{
				Recv_Num=0;        
				Send_Num=0;			
				Recv_NumBak=0;			
				Send_NumBak=0;		//ÿ�γ�ʼ���Ѽ�����0	
				diff_flag=0;	
				call_num1=0;	//����ʱ��0�������ٴ�����
				yk_state=0;
				IEC104_Answer_U(0x0B,USARTxChannel);  							 //����ȷ��  
	//��ʼ����������
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
			else if(Decryption.DataBuffer[2]==0x13)			 //ֹͣ����
			{
				IEC104_Answer_U(0x23,USARTxChannel);               //ֹͣȷ��		
			}
			else if(Decryption.DataBuffer[2]==0x43) 		 //��������
			{			
				IEC104_Answer_U(0x83,USARTxChannel);							 //����ȷ��
			}
		}
		else if(datdone == 1)//S��
		{
			Recv_NumBak=((((u16)Decryption.DataBuffer[5])<<8)&0xFF00)|(Decryption.DataBuffer[4]&0xFF);			//���������Ľ������кţ�������Ϊ�Լ����յĴ���
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
		if((Send_Num-Recv_NumBak)>=12)	diff_flag=1;//��������Ƿ�ֹ�Է��޻�Ӧ
		if(diff_flag==1)	//�����6��δȷ�Ͼ���ͣ����
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
			else if(Super_Sign[10]==6)                     //����
			{
				Send_len=Confirm_Update_BefJM(0,ACTCON,0x80); 
        Super_Sign[10]=0;				
				BW_Before_JM1[1]=Send_len-2;		
				spi_flag=0;
				
				while(spi_flag!=3)															//��ȡ�ն������
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);													//֡ͷ
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											                //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									  //������ʼ�������ն���������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len	
					spi_flag=0;			
					while(spi_flag!=3)															
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														//֡ͷ
						for(i=0;i<4;i++)															  //����
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);								//�ܳ���
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															  //��վ�����
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															  //��վ���������
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);														//Ӧ������
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);												//���ĳ���
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												  //��������
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);														//��������ȸ��ֽ�
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);														//��������ȵ��ֽ�
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															  //�����
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //����Ƭѡ
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
				lc=16+4+Send_len;						 									  //������ʼ�������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len		
				if(YY_00_WR_SPI1(lc,Send_len)) 
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //���ܱ���
					Send_Num++;
//*************************ʵ��ʹ��*****************************************************************
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
					    SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10); 	// �������ֽ���
							break;
						}
					}
//***************************************************************************************************
					//����ժҪֵ
					//��֤ժҪֵ
					for(i=0;i<8;i++)
					{
						if(Msg_Safty1[6+i]!=Ter_Random1[i])	//��֤������Ƿ������Ƿ���ȥ��
						{
							Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);					//��֤�����ʧ��
							Super_Sign[10]=0;
							return;
						}
					}
					
					data_reset();
					GetBinFileMd5(decrypt);
					lc=16+6+8+64;												//��֤�������ݳ��ȣ�ժҪֵ+ʱ����Ϣ����+�����+ǩ��ֵ
					spi_flag=0;
					while(spi_flag!=3)															//��֤ǩ������ȷ��
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);															//֡ͷ
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;
						SPI2_ReadWrite(0x08);
						crc_yhf^=0x08;
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						SPI2_ReadWrite(Msg_Safty_sj[78]);				//asKID:ǩ����Կ����
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
						SSN_H();											   //����Ƭѡ
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)										//ǩ����ȷ
					{
						succ_flag_spi=0;
						RecoverKZQBackupFlag =1 ;		//˵�������ļ�������ϣ����Խ���������	
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
				while(spi_flag!=3)															//��ȡ�ն������
				{
					SSN_L();	
					SPI2_ReadWrite(0x55);														  //֡ͷ
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Get_Ter_Random[i]);	
						crc_yhf^=Get_Ter_Random[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);	
					crc_yhf=0;
					SSN_H();											   //����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<8;i++)
						Ter_Random1[i]=Spi_Recv1[i];
					lc=16+8+4+Send_len;						 									//������ʼ�������ն���������������ݣ�Ӧ�����ͣ����ĳ��ȣ��ն����������    /Send_len	
					spi_flag=0;			
					while(spi_flag!=3)															//
					{
						SSN_L();	
						SPI2_ReadWrite(0x55);														  //֡ͷ
						for(i=0;i<4;i++)															//����
						{
							SPI2_ReadWrite(Send_JM[i]);	
							crc_yhf^=Send_JM[i];
						}
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
						crc_yhf^=(u8)(lc>>8)&0xFF;
						SPI2_ReadWrite((u8)lc&0xFF);
						crc_yhf^=(u8)lc&0xFF;
						for(i=0;i<8;i++) 															//��վ�����
						{
							SPI2_ReadWrite(Zz_Random1[i]);
							crc_yhf^=Zz_Random1[i];
						}	
						for(i=0;i<8;i++) 															//��վ���������
						{
							SPI2_ReadWrite(~Zz_Random1[i]);
							crc_yhf^=(~Zz_Random1[i]);
						}	
						SPI2_ReadWrite(0x02);															//Ӧ������
						crc_yhf^=0x02;
						SPI2_ReadWrite(Send_len);													//���ĳ���
						crc_yhf^=Send_len;
						for(i=0;i<Send_len;i++)												//��������
						{
							SPI2_ReadWrite(BW_Before_JM1[i]);	
							crc_yhf^=BW_Before_JM1[i];
						}
						SPI2_ReadWrite(0x00);															//��������ȸ��ֽ�
						crc_yhf^=0x00;		
						SPI2_ReadWrite(0x08);															//��������ȵ��ֽ�
						crc_yhf^=0x08;		
						for(i=0;i<8;i++)															//�����
						{
							SPI2_ReadWrite(Ter_Random1[i]);	
							crc_yhf^=Ter_Random1[i];
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);	
						crc_yhf=0;
						SSN_H();											   //����Ƭѡ
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
				  Send_len=AutoSend_YX_TIME(11*yx_data_num+10,0,yx_data_num); //���ɴ�ʱ���ң�����ͱ���
					Super_Sign[3] = 2;
				}
				else if(Super_Sign[3]==2)
				{
					Send_len=AutoSend_YX(4*yx_data_num+10,0,yx_data_num);       //���ɲ���ʱ��ң�����ͱ���
					yx_data_flag = 0;
					Super_Sign[3]=0;
				}
				else if(Super_Sign[4]==1)                           //�ǳ�ʼ������
				{
					Send_len=GeneralCall_BefJM(0x0C,0);	 			    //�������ټ����
					Super_Sign[4]=2;	
				}
				else if(Super_Sign[4]==2)
				{
					Send_len=YX_BefJM(13+YX_Number,0);						//����ң�ű���
					Super_Sign[4]=3;
				}
				else if(Super_Sign[4]==3)
				{
					Send_len=YC_BefJM(13+YC_Number*3,0); 	        //����ң�ⱨ��
					Super_Sign[4]=4;
				}				
				else if(Super_Sign[4]==4)
				{
					Send_len=Conclude_GeneralCall_BefJM(0x0C,0);	//�������ٽ�������
					Super_Sign[4]=0;
				}				
				else if(Super_Sign[6]==1)                 //ʱ��ͬ��������
				{
					Send_len=Cal_Time_BefJM(0x12,0);    //����ʱ��ͬ��ȷ�ϱ���
					Super_Sign[6]=0;
					Callend_Flag=1;
				}
				else if(Super_Sign[6]==2)
				{
					Send_len=Read_Time_BefJM(0x12,0);   //����ʱ������ȷ�ϱ���
					Super_Sign[6]=0;
				}		
				else if(Super_Sign[7]==1)
				{
					Send_len=AutoSend_YC(6*yc_send.yc_num+10,0); 		    //���ɱ仯ң�ⱨ��
					Super_Sign[7]=0;
					yc_send.yc_state=0;					      //ң��״̬λ��0
					yc_send.yc_num=0;			
				}
				else if(Super_Sign[8]==1)           //��λ����
				{
					Send_len=Confirm_Reset_BefJM(0x0C,0);
					Super_Sign[8]=2;	
					p_Reset=2;													//��λԭ��Զ����λ
					while(1)
					{
						if(Flash_Writeflag == 0)
						{
					    SPI_Flash_Write((u8*)&p_Reset,FLASH_RESETYY_ADDR,2);
							break;
						}
					}			
				}
				else if(Super_Sign[9]==1)  //�ļ�����
				{
			//��Ҫ��Ŀ¼���в�ѯ������flash�ڴ�Ӧ���ȴ�ü���Ŀ¼��COMTRADE(��Ŀ¼�´����¼���ļ�)��ӦASCIIΪ43 4F 4D 54 52 41 44 45	,HISTORY/SOE,HISTORY/CO,HISTORY/EXV,HISTORY/FIXPT�ȵ�	
			//�鵽Ŀ¼�󣬽���Ŀ¼�µ��ļ������͵���վ				
	//			  if(Check_List_Name())									//˵��Ŀ¼����
	//				{
	//					Send_len=Confirm_List_Bef_JM(0);				 //0��ʾ�ɹ���1��ʾʧ��
	//				}
	//				else                                                         
	//				{
	//					Send_len=Confirm_List_Bef_JM(1);				 //0��ʾ�ɹ���1��ʾʧ��
	//				}
					Super_Sign[9]=0;
				}
				else if(Super_Sign[10]==1)
				{
//					if(Check_File_Name())
//					{
//						Send_len=Confirm_ReadDoc_Bef_JM(0);												//��ȡ�ļ�����ȷ�ϣ��ļ�������С�����ȵ�
//						Super_Sign[10]=2;					
//					}
//					else
//					{
//						Send_len=Confirm_ReadDoc_Bef_JM(1);												//��ȡ�ļ�����ȷ�ϣ��ļ�������С�����ȵ�
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
				else if(Super_Sign[11]==1)                    //�����ٻ�
				{
					Send_len=Confirm_Enegy_BefJM(0x0C,0); 			 //���ɵ��������ٻ�ȷ�ϱ���
					Super_Sign[11]=2;
				}
				else if(Super_Sign[11]==2)
				{
					Send_len=Confirm_Enegy_Pluse_BefJM(0x33,0);	 //���ɵ������ݱ���
					Super_Sign[11]=3;
				}		
				else if(Super_Sign[11]==3)
				{
					Send_len=Conclude_Enegy_BefJM(0x0C,0);	    //���ɵ����ٻ���������
					Super_Sign[11]=0;
				}
				else if(Super_Sign[12]==1)                   //������д
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
					if(fix_value.params_num)             //��ֵ����
					{
						if(run_value.params_num||const_value.params_num)
							Send_len=Read_FixvalueParas_BefJM(0,1);	
						else
						{
							params_flag=1;
							Send_len=Read_FixvalueParas_BefJM(0,0);	
						}
					}
					else if(run_value.params_num)             //���в���
					{
						if(const_value.params_num)
							Send_len=Read_RunParas_BefJM(0,1);
						else
						{
							params_flag=1;
							Send_len=Read_RunParas_BefJM(0,0);
						}	
					}
					else if(const_value.params_num)            //���в���
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
					Send_len=Act_Set_BefJM(0,0x09,0x40);		          //ȡ��Ԥ��
					Super_Sign[12]=0;
				}
				BW_Before_JM1[1]=Send_len-2;
				lc=16+4+Send_len;		
				if(YY_00_WR_SPI1(lc,Send_len)) 				        //���ܱ��ĳɹ�
				{
					succ_flag_spi=0;
					Common_Style(Spi_Length1,USARTxChannel,Spi_Recv1);  //����ң�ż��ܱ���
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
	if(datdone_serial1==1)                         //���ܱ��ģ���Ҫ����
	{
		if((zz_ok==1)&&(wg_ok==1))
		{
			lc=16+Length_serial1-2;											//��Ҫ���ܵ����ݳ���
			spi_flag=0;
			while(spi_flag!=3)
			{
				SSN_L();	
				SPI2_ReadWrite(0x55);														//֡ͷ
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
				crc_yhf=0;					//����Ҫ
				SSN_H();											   //����Ƭѡ
				spi_flag=Ter_WriteRead_Spi1();
			}	
			if(succ_flag_spi)	
			{
				succ_flag_spi=0;
													/*������Щ���ʣ�����֮������Щ��������ȥ���𣬻��ǵ�����ֻ����*/
				YY_Style1= Spi_Recv1[0];																						//Ӧ������
				if(YY_Style1<0x20)			//���ܱ���ֻ��0~8��0x1f
				{
					if(YY_Style1==0x08) 	//û��ԭʼ����
					{
						MsgSafty_Len1=(((u16)Spi_Recv1[1])<<8|Spi_Recv1[2]);		//��ȫ��Ϣ�峤��
						for(i=0;i<MsgSafty_Len1;i++)
							Msg_Safty_sj[i]=Spi_Recv1[3+i];										////��֤ժҪֵ����					
					}
					else
					{
						BW_Len1 = Spi_Recv1[1];																						//���ĳ���
						for(i=0;i<BW_Len1;i++) 
							Decryption.DataBuffer[i]=Spi_Recv1[2+i]; 								//�������ݣ���Ҫ����
						MsgSafty_Len1=(((u16)Spi_Recv1[2+BW_Len1])<<8|Spi_Recv1[3+BW_Len1]);		//��ȫ��Ϣ�峤��
						for(i=0;i<MsgSafty_Len1;i++)
							Msg_Safty1[i]=Spi_Recv1[4+BW_Len1+i];															//��ȫ��Ϣ������
//						switch(YY_Style1)
//						{
//							case 1:	//ǩ��
//								if(MsgSafty_Len1<64)	//˵��û��ǩ��
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
//									return;
//								}
//								break;
//							case 3:	//�����+ǩ��
//								if(MsgSafty_Len1<60)	//˵��û��ǩ��
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
//									return;
//								}
//								else if((MsgSafty_Len1>65)&&(MsgSafty_Len1<70))//˵��û�������
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);
//									return;
//								}
//								break;
//							case 5:	//ʱ��+ǩ��
//								if(MsgSafty_Len1<60)	//˵��û��ǩ��
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
//									return;
//								}	
//								else if((MsgSafty_Len1>65)&&(MsgSafty_Len1<70))//˵��û��ʱ��
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9105);
//									return;
//								}
//								break;
//							case 7:	//ʱ��+�����+ǩ��
//								if(MsgSafty_Len1<64)//˵��û��ǩ��
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9102);
//									return;
//								}
//								else if((MsgSafty_Len1>65)&&(MsgSafty_Len1<72))//	˵��û�������
//								{
//									Re_error_msg(USARTxChannel,0x01,0x1F,0x9104);
//									return;
//								}
//								else if((MsgSafty_Len1>72)&&(MsgSafty_Len1<78))	//˵��û��ʱ��
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
										ProtocolResolve1_JM(USARTxChannel);	//������������
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
								ProtocolResolve1_JM(USARTxChannel);	//������������	
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
	else if(datdone_serial1==2)                    //�����ܱ���
	{
		switch(BackstageProtocol1.DataBuffer[6])
		{
			case 0x00:																//˵��Ϊ�̶�֡����ʽ����
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
						if(((BackstageProtocol1.DataBuffer[10]&0x3)==1)&&((BackstageProtocol1.DataBuffer[12]&1)==0)) datdone=1;  //S�� 
						else if(((BackstageProtocol1.DataBuffer[10]&0x3)==3)&&((BackstageProtocol1.DataBuffer[12]&1)==0)) datdone=2;//U��
						for(i=0;i<Length_serial1-6;i++)  Decryption.DataBuffer[i]=BackstageProtocol1.DataBuffer[8+i];
						y_104=1;
						IEC_104_Resolve_MW(USARTxChannel);
					}						
				}
				break;
			case 0x20:
				for(i=0;i<8;i++)   
			    Wg_Random1[i]=BackstageProtocol1.DataBuffer[9+i];				  //�洢���������
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //����Ƭѡ
					SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Wg_ca1[i]);
						crc_yhf^=Wg_ca1[i];	
					}
					for(i=0;i<8;i++)    												   //�������������
					{
						SPI2_ReadWrite(Wg_Random1[i]);
						crc_yhf^=Wg_Random1[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//����Ҫ
					SSN_H();																		   //����Ƭѡ
				  spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Ter_Req_WgZz1(Spi_Length1+1,USARTxChannel,0x06,0x21);   //����0x01Ϊǩ����Կ��ʶ�����ǲ�֪����0x01����0x06���������ײ�һ��
				}
				break;
			case 0x22:				//���ض��ն���֤�������Ӧ
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //��ȫ��Ϣ����
				if(num<60)	//û��ǩ��
				{
					Ter_Re_WgZz1(USARTxChannel,0x9090,0x23); 
					return;
				}
				for(i=0;i<64;i++)
					Wg_Sign1[i]=BackstageProtocol1.DataBuffer[9+i]; //��������ǩ�����
					//spi�Ĳ������һ��
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //����Ƭѡ
					SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Wg_ca2[i]);
						crc_yhf^=Wg_ca2[i];	
					}
					for(i=0;i<64;i++)    												   //��������ǩ��
					{
						SPI2_ReadWrite(Wg_Sign1[i]);
						crc_yhf^=Wg_Sign1[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//����Ҫ
					SSN_H();																		   //����Ƭѡ
				  spi_flag=Ter_WriteRead_Spi1();
				}
//				if(num<64)	Ter_Re_WgZz1(USARTxChannel,0x9090,0x23);   //
				if(succ_flag_spi)									//Ӧ���ǲ��ܳɹ����Ҫ����״̬
				{
					wg_ok=1;						//������λ
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
/*--------------------------֤�������-------------------------------------------------*/
/*---------------------------------------------------------------------------------------*/				
			case 0x30:     //֤�鹤�����ն���֤
				num_flag++;
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //��ȫ��Ϣ����
				if(BackstageProtocol1.DataBuffer[10]>BackstageProtocol1.DataBuffer[11])									//��֡�����ڵ�ǰ֡��ţ�˵�����к���
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
				else		//��֡�����ڵ�ǰ֡��ţ����һ֡
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
						SSN_L();																		   //����Ƭѡ
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x1E);
						crc_yhf^=0x1E;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//�ܳ���
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
						crc_yhf=0;					//����Ҫ
						SSN_H();						//����Ƭѡ
						spi_flag=Ter_WriteRead_Spi1();
					}
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						spi_flag=0;
						while(spi_flag!=3)															//��ȡ�ն������
						{
							SSN_L();	
							SPI2_ReadWrite(0x55);														  //֡ͷ
							for(i=0;i<6;i++)
							{
								SPI2_ReadWrite(Get_Ter_Random[i]);	
								crc_yhf^=Get_Ter_Random[i];
							}
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);	
							crc_yhf=0;					//����Ҫ
							SSN_H();											   //����Ƭѡ
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
					SSN_L();																		   //����Ƭѡ
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
					crc_yhf=0;					//����Ҫ
					SSN_H();						//����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					yw_ok=1;//��ά��֤��λ
					succ_flag_spi=0;
					FZ_Reback(USARTxChannel,0x33,0x9000);
				}
				else 
				{
					succ_flag_spi=0;
					FZ_Reback(USARTxChannel,0x33,0x9090);
				}
				break;
			case 0x34:   				//��ȡ�ն���Կ�汾
				if(yw_ok==1)
				{
					spi_flag=0;
					while(spi_flag!=3)
					{
						SSN_L();																		   //����Ƭѡ
						SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
						for(i=0;i<6;i++)
						{
							SPI2_ReadWrite(Sec_ca1[i]);
							crc_yhf^=Sec_ca1[i];	
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//����Ҫ
						SSN_H();																		   //����Ƭѡ
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						key_ver=Spi_Recv1[0];     //��Կ�汾
						spi_flag=0;
						while(spi_flag!=3)															//��ȡ�ն������
						{
							SSN_L();	
							SPI2_ReadWrite(0x55);														  //֡ͷ
							for(i=0;i<6;i++)
							{
								SPI2_ReadWrite(Get_Ter_Random[i]);	
								crc_yhf^=Get_Ter_Random[i];
							}
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);	
							crc_yhf=0;					//����Ҫ
							SSN_H();											   //����Ƭѡ
							spi_flag=Ter_WriteRead_Spi1();
						}	
						if(succ_flag_spi) 
						{
							succ_flag_spi=0;
							for(i=0;i<8;i++)
								Ter_Random2[i]=Spi_Recv1[i];      //�ն������
							Send_Ver(USARTxChannel,key_ver,Ter_Random2);
						}
					}			
				}
				else
					Re_error_msg(USARTxChannel,0x40,0x35,0x9108);
				break;
			case 0x36:					//��ȡ�ն����кţ����ǰ�ȫоƬ���к�
				Send_Ter_ID(USARTxChannel);
				break;
			case 0x38:  				//��ȡ��ȫоƬ���к�
				spi_flag=0;
				while(spi_flag!=3)
				{
					SSN_L();																		   //����Ƭѡ
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
					crc_yhf=0;					//����Ҫ
					SSN_H();						//����Ƭѡ
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
				while(spi_flag!=3)											//ȡ��Կ
				{
					SSN_L();																		   //����Ƭѡ
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
					crc_yhf=0;					//����Ҫ
					SSN_H();						//����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi) 
				{
					succ_flag_spi=0;
					for(i=0;i<64;i++)
						pubkey[i]=Spi_Recv1[i];
					lc=8+8+16+1+2+64;  		//֤��ID���ն�оƬ���������ʼ������Ӧ�����ͣ����ĳ��ȣ����ģ���Կ��
					spi_flag=0;
					while(spi_flag!=3)											//ȡ��Կ
					{
						SSN_L();																		   //����Ƭѡ
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x26);
						crc_yhf^=0x26;	
						SPI2_ReadWrite(0x62);
						crc_yhf^=0x62;	
						SPI2_ReadWrite(0x02);
						crc_yhf^=0x02;	
						SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
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
						crc_yhf=0;					//����Ҫ
						SSN_H();						//����Ƭѡ
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
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //��ȫ��Ϣ����
				lc=8+8+16+num-2;  		//֤��ID���ն�оƬ���������ʼ����������ǩ�����ĺ�MAC
				while(spi_flag!=3)										
				{
					SSN_L();																		   //����Ƭѡ
					SPI2_ReadWrite(0x55);	
					SPI2_ReadWrite(0x80);
					crc_yhf^=0x80;	
					SPI2_ReadWrite(0x2C);
					crc_yhf^=0x2C;	
					SPI2_ReadWrite(0x62);
					crc_yhf^=0x62;	
					SPI2_ReadWrite(0x02);
					crc_yhf^=0x02;	
					SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
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
					SSN_H();						//����Ƭѡ
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
					if(BackstageProtocol1.DataBuffer[9]>BackstageProtocol1.DataBuffer[10])									//��֡�����ڵ�ǰ֡��ţ�˵�����к���
					{
						FZ_Reback(0,0x45,0x9000);
					}
					else
					{
						spi_flag=0;
						while(spi_flag!=3)						
						{
							SSN_L();																		   //����Ƭѡ
							SPI2_ReadWrite(0x55);	
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite(0x0A);
							crc_yhf^=0x0A;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//�ܳ���
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
							SSN_H();						//����Ƭѡ
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
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //��ȫ��Ϣ����
				idx=BackstageProtocol1.DataBuffer[9];     																							//֤���ʶ
				if(BackstageProtocol1.DataBuffer[10]>BackstageProtocol1.DataBuffer[11])									//��֡�����ڵ�ǰ֡��ţ�˵�����к���
				{
					for(i=0;i<num-3;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-3);
					FZ_Reback(USARTxChannel,0x45,0x9000);
				}
				else		//��֡�����ڵ�ǰ֡��ţ����һ֡
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
						SSN_L();																		   //����Ƭѡ
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
						SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//�ܳ���
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
						crc_yhf=0;					//����Ҫ
						SSN_H();						//����Ƭѡ
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
			case 0x40:						//��ʼ֤���д����
				spi_flag=0;
				while(spi_flag!=3)												 //ȡ��Կ
				{
					SSN_L();																		   //����Ƭѡ
					SPI2_ReadWrite(0x55);	
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(Read_Cer_len[i]);
						crc_yhf^=Read_Cer_len[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//����Ҫ
					SSN_H();						//����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					num=((u16)Spi_Recv1[0]<<8)|(Spi_Recv1[1]&0xFF);
					spi_flag=0;
					while(spi_flag!=3)											//ȡ��Կ
					{
						SSN_L();																		   //����Ƭѡ
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
						SPI2_ReadWrite((u8)(num>>8)&0xFF);									//�ܳ���
						crc_yhf^=(u8)(num>>8)&0xFF;
						SPI2_ReadWrite((u8)num&0xFF);
						crc_yhf^=(u8)num&0xFF;
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//����Ҫ
						SSN_H();						//����Ƭѡ
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
						while(spi_flag!=3)											//ȡ��Կ
						{
							SSN_L();																		   //����Ƭѡ
							SPI2_ReadWrite(0x55);	
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite(0x24);
							crc_yhf^=0x24;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;	
							SPI2_ReadWrite((u8)(num>>8)&0xFF);									//�ܳ���
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
							crc_yhf=0;					//����Ҫ
							SSN_H();						//����Ƭѡ
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
			case 0x42:								//�����ն�֤��
				frame_num=0;
				spi_flag=0;
				while(spi_flag!=3)											//
				{
					SSN_L();																		   //����Ƭѡ
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
					crc_yhf=0;					//����Ҫ
					SSN_H();						//����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Cer_len=Spi_Length1;    //֤����ܳ���
					for(i=0;i<Spi_Length1;i++)
					{
						Cer_data[i]=Spi_Recv1[i];
					}
					frame_num=1;
					frame_total=Cer_len/200;
					if((Cer_len-frame_total*200)>0)
						frame_total+=1;				//��֡��		
					if(frame_total==0)   	  //֤�鳤��С��200
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
						SSN_L();																		   //����Ƭѡ
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x1C);
						crc_yhf^=0x1C;	
						SPI2_ReadWrite(0x02);
						crc_yhf^=0x02;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x00);									//�ܳ���
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
						crc_yhf=0;					//����Ҫ
						SSN_H();						//����Ƭѡ
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
/*----------------֤������߱��Ľ���-----------------*/
/*-----------------------------------------------------------------------*/
/*----------------������վ����-----------------*/

			case 0x50:
				for(i=0;i<8;i++)   Zz_Random1[i]=BackstageProtocol1.DataBuffer[9+i];				  //�洢��վ�����
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //����Ƭѡ
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
					crc_yhf=0;					//����Ҫ
					SSN_H();						//����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Ter_Req_WgZz1(Spi_Length1+1,USARTxChannel,0x06,0x51);   //����0x01Ϊǩ����Կ��ʶ�����ǲ�֪����0x01����0x06���������ײ�һ��
				}
				SSN_H();
				break;
			case 0x52:
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //��ȫ��Ϣ����
				if(num<60)	//û��ǩ��
				{
					Ter_Re_WgZz1(USARTxChannel,0x9090,0x53); 
					return;
				}
				for(i=0;i<65;i++)
					Wg_Sign1[i]=BackstageProtocol1.DataBuffer[9+i]; //������վǩ�����
					//spi�Ĳ������һ��
				spi_flag=0;
			  while(spi_flag!=3)
				{
					SSN_L();																		   //����Ƭѡ
					SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
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
					for(i=0;i<64;i++)    												   //������վǩ��
					{
						SPI2_ReadWrite(Wg_Sign1[i]);
						crc_yhf^=Wg_Sign1[i];
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//����Ҫ
					SSN_H();																		   //����Ƭѡ
				  spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)									//Ӧ���ǲ��ܳɹ����Ҫ����״̬
				{
					zz_ok=1;					//��վ��֤��λ
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
					SSN_L();																		   //����Ƭѡ
					SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(ID_Spi[i]);
						crc_yhf^=ID_Spi[i];	
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//����Ҫ
					SSN_H();																		   //����Ƭѡ
				  spi_flag=Ter_WriteRead_Spi1();
				}	
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Ter_Req_WgZz1(Spi_Length1,USARTxChannel,0x06,0x55);   //����0x01Ϊǩ����Կ��ʶ�����ǲ�֪����0x01����0x06���������ײ�һ��
				}				
				break;
			case 0x60:
				if((zz_ok==1)&&(wg_ok==1))
				{
					spi_flag=0;
					while(spi_flag!=3)
					{
						SSN_L();																		   //����Ƭѡ
						SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
						for(i=0;i<6;i++)
						{
							SPI2_ReadWrite(Sec_ca1[i]);
							crc_yhf^=Sec_ca1[i];	
						}
						crc_yhf=~crc_yhf;
						SPI2_ReadWrite(crc_yhf);
						crc_yhf=0;					//����Ҫ
						SSN_H();							//����Ƭѡ
						spi_flag=Ter_WriteRead_Spi1();
					}	
					if(succ_flag_spi)
					{
						succ_flag_spi=0;
						key_ver=Spi_Recv1[0];
						spi_flag =0;				//��ʱ��ʱ����san��ѭ��
						while(spi_flag!=3)
						{
							SSN_L();																		   //����Ƭѡ
							SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
							for(i=0;i<6;i++)
							{
								SPI2_ReadWrite(Sec_ca2[i]);
								crc_yhf^=Sec_ca2[i];	
							}
							crc_yhf=~crc_yhf;
							SPI2_ReadWrite(crc_yhf);		
							crc_yhf=0;					//����Ҫ						
							SSN_H();																		   //����Ƭѡ
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
							Ter_Req_WgZz1(0x09,USARTxChannel,0x06,0x61);   	//����0x01Ϊǩ����Կ��ʶ�����ǲ�֪����0x01����0x06���������ײ�һ��
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
					num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //��ȫ��Ϣ����
					if(num<190)
					{
						Ter_Re_WgZz1(USARTxChannel,0x9091,0x63);
						return;
					}
					spi_flag=0;
					while(spi_flag!=3)
					{
						SSN_L();																		   //����Ƭѡ
						SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;						
						SPI2_ReadWrite(0x1C);
						crc_yhf^=0x1C;			
						if(BackstageProtocol1.DataBuffer[9]>0)  //��Կ�汾����0Ϊ����
						{
							SPI2_ReadWrite(0x00);
							crc_yhf^=0x00;										
						}
						else if(BackstageProtocol1.DataBuffer[9]==0)//����0Ϊ�ָ�
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
						crc_yhf=0;					//����Ҫ
						SSN_H();																		   //����Ƭѡ
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
					num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //��ȫ��Ϣ����
					idx=BackstageProtocol1.DataBuffer[9];     																							//֤���ʶ
					if(BackstageProtocol1.DataBuffer[10]>BackstageProtocol1.DataBuffer[11])									//��֡�����ڵ�ǰ֡��ţ�˵�����к���
					{
						for(i=0;i<num-3;i++)
						{
							Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
						}
						nu+=(num-3);
					}
					else		//��֡�����ڵ�ǰ֡��ţ����һ֡
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
							SSN_L();																		   //����Ƭѡ
							SPI2_ReadWrite(0x55);	
							SPI2_ReadWrite(0x80);
							crc_yhf^=0x80;	
							SPI2_ReadWrite(0x2C);
							crc_yhf^=0x2C;	
							SPI2_ReadWrite(0x60);
							crc_yhf^=0x60;	
							SPI2_ReadWrite(0x01);
							crc_yhf^=0x01;
							
							SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
							crc_yhf^=(u8)(lc>>8)&0xFF;
							SPI2_ReadWrite((u8)lc&0xFF);
							crc_yhf^=(u8)lc&0xFF;
							
							for(i=0;i<8;i++) 															//��վ�����
							{
								SPI2_ReadWrite(Zz_Random1[i]);
								crc_yhf^=Zz_Random1[i];
							}	
							for(i=0;i<8;i++) 															//��վ���������
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
							crc_yhf=0;					//����Ҫ
							SSN_H();						//����Ƭѡ
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
								SSN_L();																		   //����Ƭѡ
								SPI2_ReadWrite(0x55);	
								SPI2_ReadWrite(0x80);
								crc_yhf^=0x80;	
								SPI2_ReadWrite(0x08);
								crc_yhf^=0x08;	
								SPI2_ReadWrite(0x00);
								crc_yhf^=0x00;	
								SPI2_ReadWrite(Cer_data[nu-1]);
								crc_yhf^=Cer_data[nu-1];	
								
								SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
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
								crc_yhf=0;					//����Ҫ
								SSN_H();						//����Ƭѡ
								spi_flag=Ter_WriteRead_Spi1();
							}
							if(succ_flag_spi)
							{
								succ_flag_spi=0;
								lc=nu-79;
								spi_flag=0;
								while(spi_flag!=3)											//
								{
									SSN_L();																		   //����Ƭѡ
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
									
									SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
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
									crc_yhf=0;					//����Ҫ
									SSN_H();						//����Ƭѡ
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
				num=((u16)BackstageProtocol1.DataBuffer[7]<<8)|(BackstageProtocol1.DataBuffer[8]&0xFF); //��ȫ��Ϣ����
				idx=BackstageProtocol1.DataBuffer[9];     																							//֤���ʶ
				if(BackstageProtocol1.DataBuffer[10]>BackstageProtocol1.DataBuffer[11])									//��֡�����ڵ�ǰ֡��ţ�˵�����к���
				{
					for(i=0;i<num-3;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-3);
				}
				else		//��֡�����ڵ�ǰ֡��ţ����һ֡
				{
					for(i=0;i<num-3;i++)
					{
						Cer_data[nu+i]=BackstageProtocol1.DataBuffer[12+i];
					}
					nu+=(num-3);
					spi_flag=0;
					while(spi_flag!=3)											//
					{
						SSN_L();																		   //����Ƭѡ
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x24);
						crc_yhf^=0x24;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;
						
						SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//�ܳ���
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
						crc_yhf=0;					//����Ҫ
						SSN_H();						//����Ƭѡ
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
			case 0x74:     		//��վ��ȡ�ն�֤��
				spi_flag=0;
				while(spi_flag!=3)											//
				{
					SSN_L();																		   //����Ƭѡ
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
					crc_yhf=0;					//����Ҫ
					SSN_H();						//����Ƭѡ
					spi_flag=Ter_WriteRead_Spi1();
				}
				if(succ_flag_spi)
				{
					succ_flag_spi=0;
					Cer_len=Spi_Length1;    //֤����ܳ���
					for(i=0;i<Spi_Length1;i++)
					{
						Cer_data[i]=Spi_Recv1[i];
					}
					frame_total=Cer_len/200;
					if((Cer_len-frame_total*200)>0)
						frame_total+=1;				//��֡��		
					if(frame_total==0)   	  //֤�鳤��С��200
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
		lc=8+8+16+Length_serial1-2;											//��Ҫ���ܵ����ݳ���
		spi_flag=0;
		while(spi_flag!=3)										
		{
			SSN_L();																		   //����Ƭѡ
			SPI2_ReadWrite(0x55);	
			SPI2_ReadWrite(0x80);
			crc_yhf^=0x80;	
			SPI2_ReadWrite(0x2C);
			crc_yhf^=0x2C;	
			SPI2_ReadWrite(0x62);
			crc_yhf^=0x62;	
			SPI2_ReadWrite(0x02);
			crc_yhf^=0x02;	
			SPI2_ReadWrite((u8)(lc>>8)&0xFF);									//�ܳ���
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
			crc_yhf=0;					//����Ҫ
			SSN_H();						//����Ƭѡ
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
				if(Spi_Recv1[3]>Spi_Recv1[4])									//��֡�����ڵ�ǰ֡��ţ�˵�����к���
				{
					FZ_Reback(USARTxChannel,0x45,0x9000);
				}
				else
				{
					spi_flag=0;
					while(spi_flag!=3)						
					{
						SSN_L();																		   //����Ƭѡ
						SPI2_ReadWrite(0x55);	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite(0x0A);
						crc_yhf^=0x0A;	
						SPI2_ReadWrite(0x00);
						crc_yhf^=0x00;	
						SPI2_ReadWrite(0x80);
						crc_yhf^=0x80;	
						SPI2_ReadWrite((u8)(nu>>8)&0xFF);									//�ܳ���
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
						crc_yhf=0;					//����Ҫ
						SSN_H();						//����Ƭѡ
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
					SSN_L();																		   //����Ƭѡ
					SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
					for(i=0;i<8;i++)
					{
						SPI2_ReadWrite(ID_Spi[i]);
						crc_yhf^=ID_Spi[i];	
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//����Ҫ
					SSN_H();																		   //����Ƭѡ
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
					SSN_L();																		   //����Ƭѡ
					SPI2_ReadWrite(0x55);													     //�Ƿ���Ҫ��������û��ʹ��֡ͷ��У��
					for(i=0;i<6;i++)
					{
						SPI2_ReadWrite(Sec_ca1[i]);
						crc_yhf^=Sec_ca1[i];	
					}
					crc_yhf=~crc_yhf;
					SPI2_ReadWrite(crc_yhf);
					crc_yhf=0;					//����Ҫ
					SSN_H();																		   //����Ƭѡ
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

void Send_Again(u8 USARTxChannel)	//�ط�
{
	u16 i=0;
	resend_num++;
	if(resend_num>2)	  //֮ǰ�Ѿ��ط������ˣ���û�гɹ�����һ��Ҫ����
	{
		resend_num=0;	    //������0
		delay_time_101=0;	//������0
		timer_enable=0;	  //��ʱ���ر�
		Common_MW(USARTxChannel,0xC9);			//������·״̬���ն�Ϊ����վ	
		flag_succ=0;
		return;
	}
	for(i=0;i<send_buffer_len;i++)
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],send_buffer[i]); //��һ֡�ı���
	delay_time_101=0;
	timer_enable=1;
}


