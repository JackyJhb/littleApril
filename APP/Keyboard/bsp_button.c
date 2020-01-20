/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ģ��
*	�ļ����� : bsp_button.c
*	��    �� : V2.0
*	˵    �� : ʵ�ְ����ļ�⣬��������˲����ƣ����Լ�������¼���
*				(1) ��������
*				(2) ��������
*				(3) ������
*				(4) ����ʱ�Զ�����
*				(5) ��ϼ�
*
*********************************************************************************************************
*/

#include "stm32f4xx.h"
#include <stdio.h>

#include "bsp_button.h"
#include "../PhotoElectricIsolation/bsp_gpio.h"

static BUTTON_T s_BtnOK;		                                        /* ȷ�� �� */
static BUTTON_T s_BtnCancel;                                       	    /* ȡ�� �� */
static BUTTON_T s_BtnLeft;	                                            /* ��     �� */
static BUTTON_T s_BtnRight;	                                            /* ��     �� */
static BUTTON_T s_BtnUp;		                                        /* ��     �� */
static BUTTON_T s_BtnDown;		                                        /* ��     �� */

static KEY_FIFO_T s_Key;		                                        /* ����FIFO����,�ṹ�� */

static void bsp_InitButtonVar(void);
static void bsp_InitButtonHard(void);
static void bsp_DetectButton(BUTTON_T *_pBtn);

/*
	���庯���жϰ����Ƿ��£�����ֵ1 ��ʾ���£�0��ʾδ����
*/

extern unsigned short InputGPIO[];
uint8_t IsKeyDownOK(void) 		{if (InputGPIO[7] & 0x08) return 1; return 0;}
uint8_t IsKeyDownCancel(void) 	{if (InputGPIO[7] & 0x04) return 1; return 0;}
uint8_t IsKeyDownUp(void) 		{if (InputGPIO[7] & 0x20) return 1; return 0;}
uint8_t IsKeyDownDown(void) 	{if (InputGPIO[7] & 0x10) return 1; return 0;}
uint8_t IsKeyDownLeft(void) 	{if (InputGPIO[7] & 0x02) return 1; return 0;}
uint8_t IsKeyDownRight(void) 	{if (InputGPIO[7] & 0x01) return 1; return 0;}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButton
*	����˵��: ��ʼ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitButton(void)
{
	bsp_InitButtonVar();		/* ��ʼ���������� */
	bsp_InitButtonHard();		/* ��ʼ������Ӳ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_PutKey
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    �Σ�_KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_Key.Buf[s_Key.Write] = _KeyCode;

	if (++s_Key.Write  >= KEY_FIFO_SIZE)
	{
		s_Key.Write = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;
	if (s_Key.Read == s_Key.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_Key.Buf[s_Key.Read];
		if (++s_Key.Read >= KEY_FIFO_SIZE)
		{
			s_Key.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyState
*	����˵��: ��ȡ������״̬
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t bsp_KeyState(uint8_t _ucKeyID)
{
	uint8_t ucState = 0;
	switch (_ucKeyID)
	{
		case KID_OK:
			ucState = s_BtnOK.State;
			break;
		case KID_CANCEL:
			ucState = s_BtnCancel.State;
			break;
		case KID_UP:
			ucState = s_BtnUp.State;
			break;
		case KID_DOWN:
			ucState = s_BtnUp.State;
			break;
		case KID_LEFT:
			ucState = s_BtnLeft.State;
			break;
		case KID_RIGHT:
			ucState = s_BtnRight.State;
			break;
	}
	return ucState;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButtonHard
*	����˵��: ��ʼ������Ӳ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitButtonHard(void)
{

}
	
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButtonVar
*	����˵��: ��ʼ����������
*	��    �Σ�strName : ���������ַ���
*			  strDate : ���̷�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitButtonVar(void)
{
    /* �԰���FIFO��дָ������ */
	s_Key.Read = 0;
	s_Key.Write = 0;
	/* ��ʼ��OK����������֧�ְ��¡����𡢳��� */
	s_BtnOK.IsKeyDownFunc = IsKeyDownOK;							/* �жϰ������µĺ��� */
	s_BtnOK.FilterTime = BUTTON_FILTER_TIME;						/* �����˲�ʱ�� */
	s_BtnOK.LongTime = BUTTON_LONG_TIME;							/* ����ʱ�� */
	s_BtnOK.Count = s_BtnOK.FilterTime / 2;							/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnOK.State = 0;												/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnOK.KeyCodeDown = KEY_DOWN_OK;								/* �������µļ�ֵ���� */
	s_BtnOK.KeyCodeUp   = KEY_UP_OK;								/* ��������ļ�ֵ���� */
	s_BtnOK.KeyCodeLong = KEY_LONG_OK;								/* �������������µļ�ֵ���� */
	s_BtnOK.RepeatSpeed = 0;										/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnOK.RepeatCount = 0;										/* ���������� */	
	/* ��ʼ��Left����������֧�ְ��� */
	s_BtnLeft.IsKeyDownFunc = IsKeyDownLeft;						/* �жϰ������µĺ��� */
	s_BtnLeft.FilterTime = BUTTON_FILTER_TIME;						/* �����˲�ʱ�� */
	s_BtnLeft.LongTime = BUTTON_LONG_TIME;							/* ����ʱ�� */
	s_BtnLeft.Count = s_BtnLeft.FilterTime / 2;						/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnLeft.State = 0;											/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnLeft.KeyCodeDown = KEY_DOWN_LEFT;							/* �������µļ�ֵ���� */
	s_BtnLeft.KeyCodeUp = KEY_UP_LEFT;								/* ��������ļ�ֵ���룬0��ʾ����� */
	s_BtnLeft.KeyCodeLong = KEY_LONG_LEFT;							/* �������������µļ�ֵ���룬0��ʾ����� */
	s_BtnLeft.RepeatSpeed = 0;										/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnLeft.RepeatCount = 0;										/* ���������� */
	/* ��ʼ��Right����������֧�ְ��� */
	s_BtnRight.IsKeyDownFunc = IsKeyDownRight;						/* �жϰ������µĺ��� */
	s_BtnRight.FilterTime = BUTTON_FILTER_TIME;						/* �����˲�ʱ�� */
	s_BtnRight.LongTime = BUTTON_LONG_TIME;							/* ����ʱ��, 0��ʾ����� */
	s_BtnRight.Count = s_BtnRight.FilterTime / 2;					/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnRight.State = 0;																	/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnRight.KeyCodeDown = KEY_DOWN_RIGHT;				/* �������µļ�ֵ���� */
	s_BtnRight.KeyCodeUp   = KEY_UP_RIGHT;							/* ��������ļ�ֵ���� */
	s_BtnRight.KeyCodeLong = KEY_LONG_RIGHT;					/* �������������µļ�ֵ���� */
	s_BtnRight.RepeatSpeed = 2;													/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnRight.RepeatCount = 0;														/* ���������� */	
	/* ��ʼ��UP����������֧�ְ��� */
	s_BtnUp.IsKeyDownFunc = IsKeyDownUp;								/* �жϰ������µĺ��� */
	s_BtnUp.FilterTime = BUTTON_FILTER_TIME;						/* �����˲�ʱ�� */
	s_BtnUp.LongTime = BUTTON_LONG_TIME;							/* ����ʱ�� */
	s_BtnUp.Count = s_BtnUp.FilterTime / 2;									/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnUp.State = 0;																		/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnUp.KeyCodeDown = KEY_DOWN_UP;							/* �������µļ�ֵ���� */
	s_BtnUp.KeyCodeUp   = KEY_UP_UP;									/* ��������ļ�ֵ���룬0��ʾ����� */
	s_BtnUp.KeyCodeLong = KEY_LONG_UP;								/* �������������µļ�ֵ���룬0��ʾ����� */
	s_BtnUp.RepeatSpeed = 2;														/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnUp.RepeatCount = 0;															/* ���������� */
	/* ��ʼ��DOWN����������֧�ְ��¡�����������10ms�� */
	s_BtnDown.IsKeyDownFunc = IsKeyDownDown;					/* �жϰ������µĺ��� */
	s_BtnDown.FilterTime = BUTTON_FILTER_TIME;					/* �����˲�ʱ�� */
	s_BtnDown.LongTime = BUTTON_LONG_TIME;					/* ����ʱ�� */
	s_BtnDown.Count = s_BtnDown.FilterTime / 2;						/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnDown.State = 0;																	/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnDown.KeyCodeDown = KEY_DOWN_DOWN;				/* �������µļ�ֵ���� */
	s_BtnDown.KeyCodeUp = KEY_UP_DOWN;							/* ��������ļ�ֵ���룬0��ʾ����� */
	s_BtnDown.KeyCodeLong = KEY_LONG_DOWN;					/* �������������µļ�ֵ���룬0��ʾ����� */
	s_BtnDown.RepeatSpeed = 2;													/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnDown.RepeatCount = 0;													/* ���������� */
	/* ��ʼ��DOWN����������֧�ְ��¡�����������10ms�� */
	s_BtnCancel.IsKeyDownFunc = IsKeyDownCancel;				/* �жϰ������µĺ��� */
	s_BtnCancel.FilterTime = BUTTON_FILTER_TIME;				/* �����˲�ʱ�� */
	s_BtnCancel.LongTime = BUTTON_LONG_TIME;					/* ����ʱ�� */
	s_BtnCancel.Count = s_BtnCancel.FilterTime / 2;					/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnCancel.State = 0;																/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnCancel.KeyCodeDown = KEY_DOWN_CANCEL;		/* �������µļ�ֵ���� */
	s_BtnCancel.KeyCodeUp = KEY_UP_CANCEL;					/* ��������ļ�ֵ���룬0��ʾ����� */
	s_BtnCancel.KeyCodeLong = KEY_LONG_CANCEL;			/* �������������µļ�ֵ���룬0��ʾ����� */
	s_BtnCancel.RepeatSpeed = 2;													/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnCancel.RepeatCount = 0;													/* ���������� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DetectButton
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    �Σ������ṹ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_DetectButton(BUTTON_T *_pBtn)
{
	/* ���û�г�ʼ�������������򱨴�
	if (_pBtn->IsKeyDownFunc == 0)
	{
		printf("Fault : DetectButton(), _pBtn->IsKeyDownFunc undefine");
	}
	*/

	if (_pBtn->IsKeyDownFunc())
	{
		if (_pBtn->Count < _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count < 2 * _pBtn->FilterTime)
		{
			_pBtn->Count++;
		}
		else
		{
			if (_pBtn->State == 0)
			{
				_pBtn->State = 1;
				/* ���Ͱ�ť���µ���Ϣ */
				if (_pBtn->KeyCodeDown > 0)
				{
					/* ��ֵ���밴��FIFO */
					bsp_PutKey(_pBtn->KeyCodeDown);
				}
			}
			if (_pBtn->LongTime > 0)
			{
				if (_pBtn->LongCount < _pBtn->LongTime)
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if (++_pBtn->LongCount == _pBtn->LongTime)
					{
						/* ��ֵ���밴��FIFO */
						bsp_PutKey(_pBtn->KeyCodeLong);						
					}
				}
				else
				{
					if (_pBtn->RepeatSpeed > 0)
					{
						if (++_pBtn->RepeatCount >= _pBtn->RepeatSpeed)
						{
							_pBtn->RepeatCount = 0;
							/* ��������ÿ��10ms����1������ */
							bsp_PutKey(_pBtn->KeyCodeDown);														
						}
					}
				}
			}
		}
	}
	else
	{
		if(_pBtn->Count > _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count != 0)
		{
			_pBtn->Count--;
		}
		else
		{
			if (_pBtn->State == 1)
			{
				_pBtn->State = 0;
				/* ���Ͱ�ť�������Ϣ */
				if (_pBtn->KeyCodeUp > 0)
				{
					/* ��ֵ���밴��FIFO */
					bsp_PutKey(_pBtn->KeyCodeUp);			
				}
			}
		}

		_pBtn->LongCount = 0;
		_pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyPro
*	����˵��: ������а�����������״̬�����뱻�����Եĵ��á�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_KeyPro(void)
{
	bsp_DetectButton(&s_BtnOK);												/* OK �� */
	bsp_DetectButton(&s_BtnCancel);										/* Cancel �� */
	bsp_DetectButton(&s_BtnUp);												/* UP �� */
	bsp_DetectButton(&s_BtnDown);											/* DOWN �� */
	bsp_DetectButton(&s_BtnLeft);												/* Left �� */
	bsp_DetectButton(&s_BtnRight);											/* Right �� */
}
