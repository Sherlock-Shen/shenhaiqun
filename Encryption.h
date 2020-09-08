/*************************************************************
*�ļ����ƣ�Encryption.h
*�������ڣ�2018��11��16��
*������ڣ�
*��    �ߣ�shq
**************************************************************/
#include "global.h"
#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H_

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define RL(x, y) (((x) << (y)) | ((x) >> (32 - (y))))
#define FF(a, b, c, d, x, s, ac) a = b + (RL((a + F(b, c, d) + x + ac), s))
#define GG(a, b, c, d, x, s, ac) a = b + (RL((a + G(b, c, d) + x + ac), s))
#define HH(a, b, c, d, x, s, ac) a = b + (RL((a + H(b, c, d) + x + ac), s))
#define II(a, b, c, d, x, s, ac) a = b + (RL((a + I(b, c, d) + x + ac), s))
#define PP(x) (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24)

/********************���ͱ�ʶ**********************/

//���ӷ���Ĺ�����Ϣ
#define M_SP_NA_1 1    	  // ������Ϣ
#define M_DP_NA_1 3     	// ˫����Ϣ
//#define M_ST_NA_1 5     // ��λ����Ϣ
//#define M_BO_NA_1 7     // 32λ��
#define M_ME_NA_1 9       // ����ֵ����һ��ֵ
#define M_ME_NB_1 11      // ����ֵ����Ȼ�ֵ
#define M_ME_NC_1 13      // ����ֵ���̸�����
//#define M_IT_NA_1 15    // �ۼ���
//#define M_PS_NA_1 20    // ��״̬����ĵ�����Ϣ
//#define M_ME_ND_1 21    //����Ʒ�������Ĺ�һ������ֵ
#define M_SP_TB_1 30      //��ʱ�굥����Ϣ
#define M_DP_TB_1 31      //��ʱ��˫����Ϣ
#define M_FT_NA_1 42      //�����¼���Ϣ
#define M_IT_NB_1 206     //�ۼ���,�̸�����
#define M_IT_TC_1 207     //��ʱ���ۼ���,�̸�����

// ��վ����վ �ڿ��Ʒ���Ĺ�����Ϣ
#define C_SC_NA_1	45		// ����ң������
#define C_DC_NA_1	46		// ˫��ң������
#define C_RC_NA_1	47		// ��������
#define C_SE_NA_1	48		// �趨�����һ��ֵ
#define C_SE_NB_1	49		// �趨�����Ȼ�ֵ
#define C_SE_NC_1	50		// �趨����̸�����
#define C_BO_NA_1	51		// 32λ��λ��

#define C_SC_TB_1	58		// ��ʱ�굥��ң������
#define C_DC_TB_1	59		// ��ʱ��˫��ң������
#define C_RC_TB_1	60		// ��ʱ����������
#define C_SE_TA_1	61		// ��ʱ���趨�����һ��ֵ
#define C_SE_TB_1	62		// ��ʱ���趨�����Ȼ�ֵ
#define C_SE_TC_1	63		// ��ʱ���趨����̸�����
#define C_BO_TB_1	64		// ��ʱ��32λ��λ��



//�ڼ��ӷ����ϵͳ����
#define M_EI_NA_1	70		// ��ʼ������

// ��վ����վ �ڿ��Ʒ����ϵͳ��Ϣ
#define C_IC_NA_1	100					// �ٻ�����
#define C_CI_NA_1	101					// ���������ٻ�����
#define C_RD_NA_1	102					// ����������
#define C_CS_NA_1	103					// ʱ��ͬ������
#define C_TS_NA_1	104					// ��������
#define C_RP_NA_1	105					// ��λ��������
#define C_CD_NA_1	106					// ��ʱ�������
#define C_EI_NA_1	70					// ��ʼ������
// �ڿ��Ʒ���Ĳ���
#define P_ME_NA_1	110					// װ�ز�������
#define P_AC_NA_1	113					// �������


//�ļ�����
#define F_SC_NA_1	122					// �ٻ�Ŀ¼��ѡ���ļ�
#define F_SG_NA_1	126					// Ŀ¼,�ļ�
#define F_FR_NA_1	210					// �ļ�����
#define F_SR_NA_1	211					// �������

//��������
#define C_SR_NA_1	200					// �л���ֵ��
#define C_RR_NA_1	201					// ����ֵ��
#define C_RS_NA_1	202					// �������Ͷ�ֵ
#define C_WS_NA_1	203					// д�����Ͷ�ֵ

// ����ԭ��
#define PER_CYC		1		  // ���ڡ�ѭ��
#define BACK		  2		  // ����ɨ��
#define SPONT		  3		  // ͻ��
#define INIT		  4		  // ��ʼ��
#define REQ			  5		  // ���������
#define ACT			  6		  // ����
#define ACTCON	  7		  // ����ȷ��
#define DEACT		  8		  // ֹͣ����
#define DEACTCON	9		  // ֹͣ����ȷ��
#define ACTTERM		10		// �������
#define RETREM		11		// Զ����������ķ�����Ϣ
#define RETLOC		12		// ������������ķ�����Ϣ
#define FILETRANS	13		// �ļ�����

#define INTROGEN	20		// ��Ӧ���ٻ�
#define INTRO1		21		// ��Ӧ��1���ٻ�
#define INTRO2		22		// ��Ӧ��2���ٻ�
#define INTRO3		23		// ��Ӧ��3���ٻ�
#define INTRO4		24		// ��Ӧ��4���ٻ�
#define INTRO5		25		// ��Ӧ��5���ٻ�
#define INTRO6		26		// ��Ӧ��6���ٻ�
#define INTRO7		27		// ��Ӧ��7���ٻ�
#define INTRO8		28		// ��Ӧ��8���ٻ�
#define INTRO9		29		// ��Ӧ��9���ٻ�
#define INTRO10		30		// ��Ӧ��10���ٻ�
#define INTRO11		31		// ��Ӧ��11���ٻ�
#define INTRO12		32		// ��Ӧ��12���ٻ�
#define INTRO13		33		// ��Ӧ��13���ٻ�
#define INTRO14		34		// ��Ӧ��14���ٻ�
#define INTRO15		35		// ��Ӧ��15���ٻ�
#define INTRO16		36		// ��Ӧ��16���ٻ�

#define REQCOGCN	37		// ��Ӧ���������ٻ�
#define REQCO1		38		// ��Ӧ��1��������ٻ�
#define REQCO2		39		// ��Ӧ��2��������ٻ�
#define REQCO3		40		// ��Ӧ��3��������ٻ�
#define REQCO4		41		// ��Ӧ��4��������ٻ�

//�޶���
#define QOI_GenCall	20		    //���������޶���
#define QRP_RstCall 1

//��Լ����
#define ASDU_TYPE_YX	  0x01	//ASDU���ͣ�01-YX֡
#define	ASDU_TYPE_YXBW	0x03	//ASDU���ͣ�03-YXBW֡
#define	ASDU_TYPE_YC	  0x09//ASDU���ͣ�09-YC֡



/*---����Ӧ�����ͱ��---*/
/*--����������Ϣ��ȫ��չ��--*/
/*---timeʱ�䣺6���ֽ�---*/
/*---random�������8���ֽ�---*/
/*---signǩ��:65���ֽ�---*/

#define Msg_None 0x00 //������
#define Msg_S    0x01 //������+ǩ��
#define Msg_R    0x02 //������+�����
#define Msg_RS   0x03 //������+�����+ǩ��
#define Msg_T    0x04 //������+ʱ��
#define Msg_TS   0x05 //������+ʱ��+ǩ��
#define Msg_TR   0x06 //������+ʱ��+�����
#define Msg_TRS  0x07 //������+ʱ��+�����+ǩ��
#define NMsg_TRS 0x08 //�ޱ���+ʱ��+�����+ǩ��
#define Reback   0x1f //ҵ��ȫ����������
/*--------��ȫ��չӦ������--------*/
#define Wg_Req    0x20 //���ض��ն˵���֤����
#define Ter_Wg    0x21 //�ն�ȷ�ϲ�����������֤
#define Wg_Ter    0x22 //���ض��ն˵���֤�������Ӧ
#define Ter_Tw    0x23 //�ն˷�����֤���

#define Zz_Req    0x50 //��վ���ն˵���֤����
#define Ter_Zz    0x51 //�ն�ȷ�ϲ�������վ��֤
#define Zz_Ter    0x52 //��վ���ն˵���֤�������Ӧ
#define Ter_Tz    0x53 //�ն˷�����֤���
#define Zz_Rnm    0x54 //��վ��ȡ�ն����к�
#define Ter_Rnm   0x55 //�ն˷������к�

#define Zz_Rse    0x60 //��վ��ȡ�ն���Կ�汾
#define Ter_Rse   0x61 //�ն˷�����Կ�汾
#define Zz_Upd    0x62 //��վִ��Զ����Կ����
#define Ter_Rup   0x63 //�ն˷��ظ��½��
#define Zz_Rre    0x64 //��վִ��Զ����Կ�ָ�
#define Ter_Rre   0x65 //�ն˷��ػָ����

#define Zz_Rz_L   0x50 //�ֳ����ն���֤Ӧ�����͵ͱ߽�
#define Zz_Rz_H   0x5f //�ֳ����ն���֤Ӧ�����͸߽߱�

#define Zz_My_L   0x60 //��վ���ն���Կ����Ӧ�����͵ͱ߽�
#define Zz_My_H   0x6f //��վ���ն���Կ����Ӧ�����͸߽߱�

#define Zz_Zs_L   0x70 //�ֳ����ն�֤�����Ӧ�����͵ͱ߽�
#define Zz_Zs_H   0x7f //�ֳ����ն�֤�����Ӧ�����͸߽߱�

#define YC_Number  30  //�������ң�����
#define YX_Number  48  //�������ң�Ÿ���

// �Ժ�̨˽��Э��ṹ��
struct BACKSTAGE_ENCRYPTION_TYPE1      				// 
{
	u8 DataBuffer[512];	// ���ݽ��ջ���
};

typedef union
{
	float floatdata;
	u8 bytedata[4];
	s32 longdata;
}FLOAT_BYTE;

struct YC_data
{
	FLOAT_BYTE yc_meas[YC_Number];
	u32 yc_state;
	u8  yc_num;
};
struct FIX_data
{
	FLOAT_BYTE fixpara[64];
};

struct RUN_data
{
	FLOAT_BYTE runpara[32];
};

extern u8 y_104;
extern u8 Index_1[8];
extern u8 Index_2[11];
extern u8 Index_3[10];
extern u8 Index_4[11];
extern u8 Index_5[13];
extern u8 Index_6[11];
extern u8 Index_7[15];
extern u8 Index_8[12];
extern u8 cpu_style[];
extern u8 frequence[];
extern u8 communcation_modle[];
extern char Constpara9[];

extern u8 zz_ok,wg_ok,yw_ok;	             //��վ�����ء���ά��֤��־λ
extern u8 In_Tag_Style[10];          //���в���TAG����
extern u8 In_Params_Len[10];			  //���в������ݳ���
extern u8 Wg_ca1[6];				//������֤оƬ��ȡ��1
extern u8 Wg_ca2[6];				//������֤оƬ��ȡ��2
extern u8 ID_Spi[8];
extern u8 Get_Ter_Random[6];
extern u8 Sec_ca1[6];
extern u8 Sec_ca2[6];	
extern u8 Recv_JM[4];
extern u8 Send_JM[4];
extern u8 ucaFlashBuf[1025];
extern u8 Super_Sign[15];
extern u8 Callend_Flag;
extern u8 index_which;    //������ʾ��ǰ��������ĸ�Ŀ¼
extern u8 renzheng_flag;
extern u32 ulFlieLength;

void Send_Again(u8 USARTxChannel);	//�ط�
u8 SOE_Send_PD(void);
u8 getfloat_yc_state(void);  // 20%�ı仯
void Active_Upload(u8 USARTxChannel);	
void Active_Upload1(u8 USARTxChannel);	
u8 Ter_WriteRead_Spi1(void);
void Encryption_Handle(u8 receivedata,u8 USARTxnumber);
void CommandProcess_101_Encryption(u8 USARTxChannel);
void CommandProcess_Encryption_Prepare(u8 USARTxnumber);
#endif

