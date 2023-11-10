#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <exception>
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <iostream>

#include <locale>
#include <QDebug>
#include <thread>
#include "D3DVideoRenderInterface.h"
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private slots:
    void on_btnRefresh_clicked();

    void on_btnOpen_clicked();

    void on_btnClose_clicked();

private:
    std::string ws2s(const std::wstring& ws);

    void appMsg(QString msg);
    void CamCapThread();
    void NV12ToI420(uint8_t* src_y, uint8_t* src_uv,
        uint8_t* dst_yuv,uint32_t width,uint32_t height);
private:
    Ui::Widget *ui;

    IMFAttributes* attributes = NULL;
    IMFMediaSource* source = NULL;
    IMFSourceReader* reader = NULL;
    IMFActivate** devices = NULL;
    IMFMediaType* mediaType = NULL;
    UINT32 width;
    UINT32 height;
    GUID   subtype;
    UINT32 count = 0;
    DWORD index = 0;
    DWORD flag = 0;
    LONGLONG timestamp = 0;
    std::thread* receiver;
    bool isrun ;
    bool exitFlag = false;
    unsigned char* out_data = nullptr;
    FILE* outFile = NULL;
    long long dwIndex;
    ID3DRenderInterface* m_pD3DRender = nullptr;
    QWidget* vieww;
    int cam_save_index = 0;
};
#endif // WIDGET_H
