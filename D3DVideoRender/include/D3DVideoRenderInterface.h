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
		Upper_Down_Flip = 2, //图像上下颠倒
		Left_Right_Flip = 3 //图像左右颠倒
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

	// CreateBackBuffer创建后备帧缓冲。
	// 输入参数：
	// hWnd:			视频渲染窗口句柄。
	// lWidth:			后备缓冲区宽度，单位：像素。
	// lHeight:			后备缓冲区高度，单位：像素。
	// dwBufferCount:   后备缓冲区个数。
	
	// 输出参数：无。
	virtual bool CreateBackBuffer ( HWND hWnd, unsigned int lWidth, unsigned int lHeight, DWORD dwBufferCount = 2 ) = 0 ;

	// CreateImageBuffer创建视频图像缓冲区。
	// 输入参数：
	// lWidth:  视频图像缓冲区宽度，单位：像素。
	// lHeight: 视频图像缓冲区高度，单位：像素。
	// rect:	视频图像显示区域。
	// cs:      视频图像颜色空间。
	// 输出参数： 
	// dwIndex: 视频图像缓冲区索引。
	virtual bool CreateImageBuffer (long long *dwIndex, unsigned int lWidth, unsigned int lHeight, COLOR_SPACE cs, RECT *rect,long long mdwIndex) = 0 ;

	// DestroyImageBuffer销毁视频图像缓冲区。
	// 输入参数：
	// dwIndex: 视频图像缓冲区索引。
	virtual bool DestroyImageBuffer (long long dwIndex ) = 0 ;

	// CreateOverlayText创建叠加文本, 支持中文。
	// 输入参数：
	// strText: 叠加文本内容。
	// logfont: 叠加文本字体。

	// 输出参数：
	// dwIndex: 叠加文本索引。
	virtual bool CreateOverlayText (long long *dwIndex, const char *strText, LOGFONT *logfont ) = 0 ;

	// DestroyOverlayText销毁叠加文本。
	// 输入参数：
	// dwIndex: 叠加文本索引。
	virtual bool DestroyOverlayText (long long dwIndex ) = 0 ;

	// 输入参数：
	// dwIndex: 叠加文本索引。

	// 输出参数：
	// lWidth: 叠加文本的实际宽度，单位：像素。
	// lHeight: 叠加文本的实际高度，单位：像素。
	virtual bool GetOverlayTextWH (long long dwIndex, long *lWidth, long *lHeight ) = 0 ;

	// CreateDynamicFont创建内容动态变化的数字、英文字符文本，主要用来显示变化的数字，不支持中文。
	// 输入参数：
	// strFongName: 字体名称。
	// dwHeight: 字体高度。
	virtual bool CreateDynamicFont ( const char* strFontName, DWORD dwHeight ) = 0 ;

	// DestroyOverlayText销毁动态文本。
	virtual bool DestroyDynamicFont ( void ) = 0 ;

	// ColorFill填充视频缓冲区。
	// 输入参数：
	// dwIndex: 视频缓冲区索引。
	// r g b  : RGB COLOR。
	virtual bool ColorFill (long long dwIndex, BYTE r, BYTE g, BYTE b ) = 0 ;

	// HandleDeviceLost处理D3D设备丢失
	virtual int HandleDeviceLost( void ) = 0 ;

	// UpdateImage更新视频缓冲区内容。
	// 输入参数：
	// lIndex: 视频缓冲区索引。
	// pData: 更新数据缓冲区指针。
	virtual bool UpdateImage (long long dwIndex, BYTE *pData,int width, int height) = 0 ;

	virtual bool UpdateImage(long long dwIndex, BYTE *pYData, BYTE *pUData, BYTE *pVData, int width, int height) = 0;

	// BeginRender开始D3D渲染。
	// 输入参数：
	// bClearBackBuffer: 是否清空后备缓冲区(不清空后备缓冲区可以提高性能)。
	// dwBKColor: 清空后备缓冲区为指定颜色。[0Xargb]
	virtual bool BeginRender ( bool bClearBackBuffer, /*DWORD dwBKColor*/float*RGBA = 0 ) = 0 ;

	// DrawImage渲染视频图像。
	// 输入参数：
	// dwIndex: 视频缓冲区索引。
	// bFlip:  是否上下反转视频图像。
	// rect:   视频显示区域。
	//isGrayBackground 绘制背景
	//sValue 强度值
	//RGBA 必须是一个float[4]或者NULL
	virtual bool DrawImage (long long dwIndex, GEOMETRIC_TRANSFORMATION Transformation = GENERAL_Flip,ROTATE_TRANSFORMATION ROTATE =  R0, int isGrayBackground = 0,float sValue = 0.0, RECT *rect = 0,float*RGBA=0) = 0 ;


	// DrawText渲染叠加文本。
	// 输入参数：
	// dwIndex: 文本索引。
	// dwColor: 文本颜色。[0Xargb]
	// rect: 文本显示区域。
	virtual bool DrawOverlayText (long long dwIndext, DWORD dwColor, RECT *rect ) = 0 ;

	// DrawDynamicText渲染动态文本。
	// 输入参数：
	// x,y: 动态文本渲染坐标(左上角)。
	// dwColor: 动态文本颜色。[0Xargb]
	// strText: 动态文本。
	virtual bool DrawDynamicText ( long x, long y, DWORD dwColor, const char* strText ) = 0 ;
	
	// EndRender结束D3D渲染。
	virtual bool EndRender ( void ) = 0 ;

	//显示区域
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
