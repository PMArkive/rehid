#pragma once
#include <3ds.h>
#include "Touch.hpp"
#include "Pad.hpp"
extern "C"
{
    #include "mythread.h"
    #include "codec.h"
}
class Hid
{
    public:
        void CreateAndMapMemoryBlock();
        void CreateRingsOnSharedmemoryBlock();
        void StartThreadsForSampling();
        void TakeOverIRRSTIfRequired();
        void InitializePad();
        void EnteringSleepMode();
        void ExitingSleepMode();
        void IsShellOpened(bool opened) {m_shellisopen = opened; };
        Pad *GetPad() {return &m_pad; };
        Touch *GetTouch() {return &m_touch; };
        Handle *GetSharedMemHandle() { return &m_sharedmemhandle; };
        uint8_t *ExitThread() { return &m_exitthread; };
        LightLock *GetSleepLock() { return &m_sleeplock; };
        Handle dummyhandles[4] = {0};
    private:
        Handle m_sharedmemhandle;
        void *m_addr = nullptr;
        PadRing *m_padring = nullptr;
        TouchRing *m_touchring = nullptr;
        Pad m_pad;
        Touch m_touch;
        bool m_shellisopen = true;
        MyThread m_samplingthread;
        bool m_samplingthreadstarted = false;
        uint8_t m_exitthread = 0;
        LightLock m_sleeplock;
};