/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ģ��    
*	�ļ����� : bsp_button.h
*	��    �� : V2.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

/* �����˲�ʱ��50ms, ��λ10ms
 ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
*/
#define BUTTON_FILTER_TIME 	5
#define BUTTON_LONG_TIME 	100		/* ����1�룬��Ϊ�����¼� */

/*
	ÿ��������Ӧ1��ȫ�ֵĽṹ�������
	���Ա������ʵ���˲��Ͷ��ְ���״̬�������
*/
typedef struct
{
	/* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
	unsigned char (*IsKeyDownFunc)(void); /* �������µ��жϺ���,1��ʾ���� */

	unsigned char Count;			/* �˲��������� */
	unsigned char FilterTime;		/* �˲�ʱ��(���255,��ʾ2550ms) */
	unsigned short LongCount;		/* ���������� */
	unsigned short LongTime;		/* �������³���ʱ��, 0��ʾ����ⳤ�� */
	unsigned char  State;			/* ������ǰ״̬�����»��ǵ��� */
	unsigned char KeyCodeUp;		/* ��������ļ�ֵ����, 0��ʾ����ⰴ������ */
	unsigned char KeyCodeDown;	/* �������µļ�ֵ����, 0��ʾ����ⰴ������ */
	unsigned char KeyCodeLong;	/* ���������ļ�ֵ����, 0��ʾ����ⳤ�� */
	unsigned char RepeatSpeed;	/* ������������ */
	unsigned char RepeatCount;	/* �������������� */
}BUTTON_T;

/* �����ֵ����
	�Ƽ�ʹ��enum, ����#define��ԭ��
	(1) ����������ֵ,�������˳��ʹ���뿴���������
	(2)	�������ɰ����Ǳ����ֵ�ظ���
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 ��ʾ�����¼� */

	KEY_DOWN_OK,			/* OK������ */
	KEY_UP_OK,			    /* OK������ */
	KEY_LONG_OK,			/* OK������ */

	KEY_DOWN_CANCEL,		/* CANCEL������ */
	KEY_UP_CANCEL,			/* CANCEL������ */
	KEY_LONG_CANCEL,		/* CANCEL������ */

	KEY_DOWN_UP,			/* Up������ */
	KEY_UP_UP,			    /* Up������ */
	KEY_LONG_UP,			/* Up������ */

	KEY_DOWN_DOWN,			/* Down������ */
	KEY_UP_DOWN,			/* Down������ */
	KEY_LONG_DOWN,			/* Down������ */

	KEY_DOWN_LEFT,			/* LEFT������ */
	KEY_UP_LEFT,			/* LEFT������ */
	KEY_LONG_LEFT,			/* LEFT������ */
	
	KEY_DOWN_RIGHT,	   
    KEY_UP_RIGHT,
    KEY_LONG_RIGHT
}KEY_ENUM;

/* ����ID */
enum
{
	KID_OK = 0,
	KID_CANCEL,
	KID_LEFT,
    KID_RIGHT,    
	KID_UP,
	KID_DOWN
};

/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE	20
typedef struct
{
	unsigned char Buf[KEY_FIFO_SIZE];		/* ��ֵ������ */
	unsigned char Read;					    /* ��������ָ�� */
	unsigned char Write;					/* ������дָ�� */
}KEY_FIFO_T;

/* ���ⲿ���õĺ������� */
void bsp_InitButton(void);
void bsp_PutKey(unsigned char _KeyCode);
unsigned char bsp_GetKey(void);
void bsp_KeyPro(void);
unsigned char bsp_KeyState(unsigned char _ucKeyID);
unsigned char IsKeyDownOK(void);
unsigned char IsKeyDownCancel(void);
unsigned char IsKeyDownLeft(void);
unsigned char IsKeyDownRight(void);
unsigned char IsKeyDownUp(void);
unsigned char IsKeyDownDown(void);
#endif


