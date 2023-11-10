#include "widget.h"
#include "ui_widget.h"

#include <strmif.h>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    CoInitialize(NULL);

    //枚举设备
    auto hr = MFCreateAttributes(&attributes, 1);
    if (FAILED(hr))
    {
        appMsg("create attributes error!");
        return;
    }
    hr = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    if (FAILED(hr))
    {
        appMsg("set guid error!");
        return;
    }
    vieww = nullptr;
    on_btnRefresh_clicked();
    isrun = true;
    exitFlag = true;
    receiver = new std::thread(&Widget::CamCapThread, this);
}

Widget::~Widget()
{
    isrun = false;
    delete ui;
}

void Widget::NV12ToI420(uint8_t* src_y, uint8_t* src_uv,
    uint8_t* dst_yuv,uint32_t width,uint32_t height) {
    // src_y --> dst_y memcpy
    for (int i = 0; i < (width* height); ++i) {
        dst_yuv[i] = src_y[i];
    }

    // src_uv --> dst_u,dst_v
    int index = 0;
    for (int i = 0; i < (width* height/2); i+=2) {
        dst_yuv[index + width* height] = src_uv[i];
        index++;
    }
    for (int i = 1; i < (width* height/2); i+=2) {
        dst_yuv[index + width* height] = src_uv[i];
        index++;
    }
}

std::string Widget::ws2s(const std::wstring& ws)
{
    char* strLocale = setlocale(LC_ALL, "");
    const wchar_t* wchSrc = ws.c_str();

    size_t nDestSize = 0;
    wcstombs_s(&nDestSize, NULL, 0, wchSrc, 0);
    nDestSize += 1;
    char* chDest = new char[nDestSize];
    memset(chDest, 0, nDestSize);
    wcstombs_s(NULL, chDest, nDestSize, wchSrc, nDestSize);
    std::string strResult = chDest;
    delete[] chDest;
    setlocale(LC_ALL, strLocale);
    return strResult;
}

void Widget::CamCapThread()
{
    while(isrun)
    {
        if(exitFlag){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            //解除引用
//            if(attributes){
//                attributes->Release();
//                attributes = nullptr;
//            }
//            if(source){
//                source->Release();
//                source = nullptr;
//            }
//            if(reader){
//                reader->Release();
//                reader = nullptr;
//            }
            continue;
        }
        IMFSample* sample;
        auto hr = reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &index, &flag, &timestamp, &sample);
        if (FAILED(hr))
        {
            appMsg("read sample error!");
            on_btnClose_clicked();
            on_btnRefresh_clicked();
            continue;
        }
        if (sample)
        {
            DWORD count = 0;
            BYTE* data;
            DWORD len;
            IMFMediaBuffer* buffer = NULL;
            sample->GetBufferCount(&count);
            for (int i = 0; i < count; i++)
            {
                sample->GetBufferByIndex(i, &buffer);
                if (buffer)
                {
                    hr = buffer->Lock(&data, NULL, &len);
                    if (FAILED(hr))
                    {
                        qDebug()<<("buffer lock error!");
                    }
                    //此处获取到图像数据，判断subtype后对数据进行处理
//                    if (IsEqualGUID(subtype, MFVideoFormat_I420))
//                    {
//                        //显示(data,len)
//                        qDebug()<<__func__<<"i420";
//                    }
//                    else if (IsEqualGUID(subtype, MFVideoFormat_YUY2))
//                    {
//                        //显示(data,len)
//                        qDebug()<<__func__<<"YUY2";
//                    }else if (IsEqualGUID(subtype, MFVideoFormat_YVYU))
//                    {
//                        //显示(data,len)
//                        qDebug()<<__func__<<"YVYU";
//                    }else if (IsEqualGUID(subtype, MFVideoFormat_MJPG))
//                    {
//                        //显示(data,len)
//                        qDebug()<<__func__<<"MJPG";
//                    }else
                    if (IsEqualGUID(subtype, MFVideoFormat_NV12))
                    {
                        //显示(data,len)
                        NV12ToI420(data,data+width*height, out_data,width,height);
                        m_pD3DRender->UpdateImage(dwIndex,out_data, width, height);
                        m_pD3DRender->BeginRender(dwIndex, 0);
                        m_pD3DRender->DrawImage(dwIndex);
                        m_pD3DRender->EndRender();
                    } else {
                        char dataRect[100] = { 0 };
                        sprintf(dataRect,
                            "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
                            (int)subtype.Data1, (int)subtype.Data2, (int)subtype.Data3,
                            static_cast<int>(subtype.Data4[0]),
                            (int)subtype.Data4[1],
                            (int)subtype.Data4[2],
                            (int)subtype.Data4[3],
                            (int)subtype.Data4[4],
                            (int)subtype.Data4[5],
                            (int)subtype.Data4[6],
                            (int)subtype.Data4[7]);
                        appMsg("type:" + QString(dataRect));
                    }
//#if 1
//                    NV12ToI420(data,data+width*height, out_data,width,height);
//                    fwrite(out_data,len,1,outFile);
//#else
                    fwrite(data,len,1,outFile);
//#endif

                    hr = buffer->Unlock();
                    if (FAILED(hr))
                    {
                        qDebug()<<("buffer unlock error!");
                    }
                    buffer->Release();
                }
            }
            sample->Release();
        }
    }
}
void Widget::appMsg(QString msg)
{
    ui->textEdit->append(msg);
    qDebug()<<__func__<<msg;
}

void Widget::on_btnRefresh_clicked()
{
    qDebug()<<__func__;
    int c = ui->comboBox->count();
    for(int i = 0;i < c;i++)
        ui->comboBox->removeItem(0);

    auto hr = MFEnumDeviceSources(attributes, &devices, &count);
    if (FAILED(hr))
    {
        appMsg("enum device error!");
        return;
    }
    appMsg("find dev:"+QString::number(count));
    if (count < 1)
    {
        appMsg("can not find any video device!");
        return;
    }
    for(int i = 0 ; i < count;i++){
        WCHAR *ppszName = NULL;
        hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,&ppszName,NULL);
        QString name = ws2s(ppszName).c_str();
        appMsg("add dev:"+name);
        ui->comboBox->addItem(name);
    }


}


void Widget::on_btnOpen_clicked()
{
    int index = ui->comboBox->currentIndex();
    qDebug()<<__func__<<index;
    //根据第一个设备创建MediaSource
    auto hr = MFCreateDeviceSource(devices[index], &source);
    if (FAILED(hr))
    {
        appMsg("create device source error!"+QString::number(hr,16));
        return;
    }
    //创建SourceReader
     hr = MFCreateSourceReaderFromMediaSource(source, NULL, &reader);
    if (FAILED(hr))
    {
        appMsg("create source reader error!");
        return;
    }


    IMFMediaType    *pType = NULL;
    for (DWORD i = 0; ; i++)
    {
        qDebug()<<__func__;
        hr = reader->GetNativeMediaType(
            (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            i,
            &pType
            );

        if (FAILED(hr)) {
            break;
            qDebug()<<__func__<<"error"<<hr;
        }

        AM_MEDIA_TYPE*  mformt = NULL;
        pType->GetRepresentation(FORMAT_MFVideoFormat , (LPVOID*)&mformt);
        MFVIDEOFORMAT* z = (MFVIDEOFORMAT*)mformt->pbFormat;
        unsigned int format = z->surfaceInfo.Format;
        char type[5] = {0};
        type[0] = format & 0xff;
        type[1] = (format >> 8) & 0xff;
        type[2] = (format >> 16) & 0xff;
        type[3] = (format >> 24) & 0xff;
        qDebug()<<__func__<<type<<z->videoInfo.dwWidth<<z->videoInfo.dwHeight<<z->videoInfo.FramesPerSecond.Numerator<<z->videoInfo.FramesPerSecond.Denominator;
        int w = z->videoInfo.dwWidth;
        int h = z->videoInfo.dwHeight;
        pType->FreeRepresentation(FORMAT_MFVideoFormat ,(LPVOID)mformt);
        pType->GetGUID(MF_MT_SUBTYPE, &subtype);
        char dataRect[100] = { 0 };
        sprintf(dataRect,
            "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
            (int)subtype.Data1, (int)subtype.Data2, (int)subtype.Data3,
            static_cast<int>(subtype.Data4[0]),
            (int)subtype.Data4[1],
            (int)subtype.Data4[2],
            (int)subtype.Data4[3],
            (int)subtype.Data4[4],
            (int)subtype.Data4[5],
            (int)subtype.Data4[6],
            (int)subtype.Data4[7]);
        appMsg("WxH:"+QString::number(w)+"x"+QString::number(h) + ",type:" + QString(dataRect));
        if(subtype == MFVideoFormat_NV12){
            // 设置采集的分辨率
            int wsel = ui->lineEdit->text().toInt();
            if(w == wsel)
            {
                break;
            }else{
                pType->Release();
                pType = nullptr;
            }
        }
    }

    if(pType){
        mediaType = pType;
        hr = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM,nullptr, mediaType);
        if (FAILED(hr))
        {
            appMsg("get media type error!");
            return;
        }
    }else{
        hr = reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &mediaType);
        if (FAILED(hr))
        {
            appMsg("get media type error!");
            return;
        }
    }


    //获取当前默认视频格式(作为最简单例子不展示设置视频格式)

    //获取图像格式
    hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
    if (FAILED(hr))
    {
        appMsg("get subtype error!");
        return;
    }
    //获取分辨率
    hr = MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &width, &height);
    if (FAILED(hr))
    {
        appMsg("get frame size error!");
        return;
    }
    appMsg(QString("cap:")+QString::number(width)+","+QString::number(height));
    //开始采集（同步）
    QString file = QString("%1_%2_%3.yuv").arg(width).arg(height).arg(cam_save_index);
    cam_save_index ++;
    outFile = fopen(file.toLocal8Bit(), "wb+");
    appMsg(QString("save file:")+file);
    out_data = (unsigned char*)malloc(width* height * 3 >> 1);
    if(!vieww){
        vieww = new QWidget();
        vieww->resize(640,480);
    }
    vieww->show();
    m_pD3DRender = D3D_VIDEO_RENDER::D3D_Video_Render_Create(0, 0);
    m_pD3DRender->CreateBackBuffer((HWND)vieww->winId(), width, height, 2);
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    m_pD3DRender->CreateImageBuffer(&dwIndex, width, height, ID3DRenderInterface::COLOR_SPACE::CS_I420, &rect, 0);
    qDebug()<<__func__<<"finish";
    exitFlag = false;
}

#include <QEventLoop>
#include <QTimer>
void Widget::on_btnClose_clicked()
{
    exitFlag = true;
    QEventLoop eventloop;
    QTimer::singleShot(500, &eventloop, SLOT(quit()));
    eventloop.exec();
    if(source){
        source->Release();
        source = nullptr;
    }
    if(reader){
        reader->Release();
        reader = nullptr;
    }
    if(out_data){
        free(out_data);
        out_data = nullptr;
    }
    if(outFile){
        fclose(outFile);
        outFile = NULL;
    }
    if(vieww){
        vieww->close();
    }
}

