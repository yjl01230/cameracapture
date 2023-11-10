#pragma once
#include <windows.h>
class ID3DRenderInterface
{
public:
	enum COLOR_SPACE
	{
		CS_YV12		= 0, // YVU420
		CS_I420		= 1, // YUV420
		CS_YUY2		= 2, // YUYV
		CS_UYVY		= 3, // UYVY 
		CS_NV12		= 4, // NV12
		CS_RGB24	= 5, // rgb rgb ...
		CS_BGR24	= 6, // bgr bgr ...
		CS_RGB16	= 7, // R5G6B5
		CS_RGB15	= 8, // X1R5G5B5
		CS_RGB32	= 9, // bgra bgra ...
		CS_UNKNOWN  = 0xffffffff
	} ;

	enum GEOMETRIC_TRANSFORMATION
	{
		GENERAL_Flip = 1,
		Upper_Down_Flip = 2, //ͼ�����µߵ�
		Left_Right_Flip = 3 //ͼ�����ҵߵ�
	} ;

	enum ROTATE_TRANSFORMATION
	{
		R0   = 0,
		R90  = 90,
		R180 = 180,
		R270 = 270
	};
	enum DEVICE_STATUS
	{
		M_D3DERR_DEVICELOST =-1,
		M_D3D_OK,
		M_D3DERR_DEVICENOTRESET,
		M_D3DERR_ERROR,
		M_D3D_UNKOWN

	};

public:

	// CreateBackBuffer������֡���塣
	// ���������
	// hWnd:			��Ƶ��Ⱦ���ھ����
	// lWidth:			�󱸻�������ȣ���λ�����ء�
	// lHeight:			�󱸻������߶ȣ���λ�����ء�
	// dwBufferCount:   �󱸻�����������
	
	// ����������ޡ�
	virtual bool CreateBackBuffer ( HWND hWnd, unsigned int lWidth, unsigned int lHeight, DWORD dwBufferCount = 2 ) = 0 ;

	// CreateImageBuffer������Ƶͼ�񻺳�����
	// ���������
	// lWidth:  ��Ƶͼ�񻺳�����ȣ���λ�����ء�
	// lHeight: ��Ƶͼ�񻺳����߶ȣ���λ�����ء�
	// rect:	��Ƶͼ����ʾ����
	// cs:      ��Ƶͼ����ɫ�ռ䡣
	// ��������� 
	// dwIndex: ��Ƶͼ�񻺳���������
	virtual bool CreateImageBuffer (long long *dwIndex, unsigned int lWidth, unsigned int lHeight, COLOR_SPACE cs, RECT *rect,long long mdwIndex) = 0 ;

	// DestroyImageBuffer������Ƶͼ�񻺳�����
	// ���������
	// dwIndex: ��Ƶͼ�񻺳���������
	virtual bool DestroyImageBuffer (long long dwIndex ) = 0 ;

	// CreateOverlayText���������ı�, ֧�����ġ�
	// ���������
	// strText: �����ı����ݡ�
	// logfont: �����ı����塣

	// ���������
	// dwIndex: �����ı�������
	virtual bool CreateOverlayText (long long *dwIndex, const char *strText, LOGFONT *logfont ) = 0 ;

	// DestroyOverlayText���ٵ����ı���
	// ���������
	// dwIndex: �����ı�������
	virtual bool DestroyOverlayText (long long dwIndex ) = 0 ;

	// ���������
	// dwIndex: �����ı�������

	// ���������
	// lWidth: �����ı���ʵ�ʿ�ȣ���λ�����ء�
	// lHeight: �����ı���ʵ�ʸ߶ȣ���λ�����ء�
	virtual bool GetOverlayTextWH (long long dwIndex, long *lWidth, long *lHeight ) = 0 ;

	// CreateDynamicFont�������ݶ�̬�仯�����֡�Ӣ���ַ��ı�����Ҫ������ʾ�仯�����֣���֧�����ġ�
	// ���������
	// strFongName: �������ơ�
	// dwHeight: ����߶ȡ�
	virtual bool CreateDynamicFont ( const char* strFontName, DWORD dwHeight ) = 0 ;

	// DestroyOverlayText���ٶ�̬�ı���
	virtual bool DestroyDynamicFont ( void ) = 0 ;

	// ColorFill�����Ƶ��������
	// ���������
	// dwIndex: ��Ƶ������������
	// r g b  : RGB COLOR��
	virtual bool ColorFill (long long dwIndex, BYTE r, BYTE g, BYTE b ) = 0 ;

	// HandleDeviceLost����D3D�豸��ʧ
	virtual int HandleDeviceLost( void ) = 0 ;

	// UpdateImage������Ƶ���������ݡ�
	// ���������
	// lIndex: ��Ƶ������������
	// pData: �������ݻ�����ָ�롣
	virtual bool UpdateImage (long long dwIndex, BYTE *pData,int width, int height) = 0 ;

	virtual bool UpdateImage(long long dwIndex, BYTE *pYData, BYTE *pUData, BYTE *pVData, int width, int height) = 0;

	// BeginRender��ʼD3D��Ⱦ��
	// ���������
	// bClearBackBuffer: �Ƿ���պ󱸻�����(����պ󱸻����������������)��
	// dwBKColor: ��պ󱸻�����Ϊָ����ɫ��[0Xargb]
	virtual bool BeginRender ( bool bClearBackBuffer, /*DWORD dwBKColor*/float*RGBA = 0 ) = 0 ;

	// DrawImage��Ⱦ��Ƶͼ��
	// ���������
	// dwIndex: ��Ƶ������������
	// bFlip:  �Ƿ����·�ת��Ƶͼ��
	// rect:   ��Ƶ��ʾ����
	//isGrayBackground ���Ʊ���
	//sValue ǿ��ֵ
	//RGBA ������һ��float[4]����NULL
	virtual bool DrawImage (long long dwIndex, GEOMETRIC_TRANSFORMATION Transformation = GENERAL_Flip,ROTATE_TRANSFORMATION ROTATE =  R0, int isGrayBackground = 0,float sValue = 0.0, RECT *rect = 0,float*RGBA=0) = 0 ;


	// DrawText��Ⱦ�����ı���
	// ���������
	// dwIndex: �ı�������
	// dwColor: �ı���ɫ��[0Xargb]
	// rect: �ı���ʾ����
	virtual bool DrawOverlayText (long long dwIndext, DWORD dwColor, RECT *rect ) = 0 ;

	// DrawDynamicText��Ⱦ��̬�ı���
	// ���������
	// x,y: ��̬�ı���Ⱦ����(���Ͻ�)��
	// dwColor: ��̬�ı���ɫ��[0Xargb]
	// strText: ��̬�ı���
	virtual bool DrawDynamicText ( long x, long y, DWORD dwColor, const char* strText ) = 0 ;
	
	// EndRender����D3D��Ⱦ��
	virtual bool EndRender ( void ) = 0 ;

	//��ʾ����
	virtual bool SetViewPort(RECT *rect) = 0;
	virtual void ResetDevices(int lWidth, int lHeight) = 0;
} ;


#if defined(__cplusplus)
extern "C" 
{
#endif

namespace D3D_VIDEO_RENDER
{
	ID3DRenderInterface* D3D_Video_Render_Create (void*hwnd, int streamId) ;
	void D3D_Video_Render_Destroy (void*pp, void*hwnd, int streamId) ;
}

#if defined(__cplusplus)
}
#endif
