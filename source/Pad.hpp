#include <3ds.h>
#include "PadRing.hpp"
#include "CirclePad.hpp"
#include "Remapper.hpp"
#define IOHIDPAD *(vu16*)0x1EC46000

class Pad
{
    public:
        void Initialize();
        void SetPadRing(PadRing *ring) { m_ring = ring; };
        PadRing *GetPadRing() {return m_ring; };
        void SetTimer();
        Handle *GetTimer() { return &m_timer; };
        void Sampling(u32 rcpr);
        void ReadFromIO(PadEntry *entry, uint32_t *raw, CirclePadEntry circlepad);
        Handle *GetEvent() {return &m_event; };
    private:
        uint8_t m_isinitialized = 0;
        uint32_t m_latestkeys = 0;
        Handle m_timer;
        PadRing *m_ring = nullptr;
        Handle m_event;
        CirclePad m_circlepad;
        Remapper m_remapper;
};  