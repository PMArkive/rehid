#include "TitleManager.hpp"
#include "smdh.hpp"

void Utils::TitleManager::PopulateTitleList() {
    u32 readcount = 0;
    u64 gamecardid = 0;
    FS_CardType type;
    Result ret = AM_GetTitleCount(MEDIATYPE_SD, &m_count);

    if (ret != 0)
        return;

    m_titles = new u64[m_count + 1];
    ret = AM_GetTitleList(&readcount, MEDIATYPE_SD, m_count, m_titles);

    if (ret != 0)
        return;

    if (R_SUCCEEDED(FSUSER_GetCardType(&type))) {
        if (type == CARD_CTR && R_SUCCEEDED(AM_GetTitleList(nullptr, MEDIATYPE_GAME_CARD, 1, &gamecardid))) {
            readcount++;
            m_titles[m_count] = gamecardid;
            m_gamecardid = gamecardid;
        }
    }

    m_count = readcount;
    return;
}

FS_Path fs_make_path_binary(const void* data, u32 size) {
    FS_Path path = {PATH_BINARY, size, data};
    return path;
}

void u16tou8(std::string &buf, const u16 *input, size_t bufsize) {
    char *data = new char[bufsize];
    ssize_t units = utf16_to_utf8((u8*)data, input, bufsize);

    if (units < 0)
        units = 0;

    data[units] = 0;
    buf = data;
    delete[] data;
}

void Utils::TitleManager::PopulateIcons(uint64_t *titles, uint32_t count) {
    static const u32 filepath[5] = {0x00000000, 0x00000000, 0x00000002, 0x6E6F6369, 0x00000000};
    Handle filehandle;
    SMDH smdh;
    std::vector<uint8_t> data;
    std::string descstr;

    for (uint32_t i = 0; i < count; i++) {
        data.clear();
        data.shrink_to_fit();
        descstr.clear();
        descstr.shrink_to_fit();
        u32 mediatype = titles[i] == m_gamecardid ? MEDIATYPE_GAME_CARD : MEDIATYPE_SD;
        u32 archivepath[4] = {(u32)(titles[i] & 0xFFFFFFFF), (u32)((titles[i] >> 32) & 0xFFFFFFFF), mediatype, 0x00000000};

        if (R_SUCCEEDED(FSUSER_OpenFileDirectly(&filehandle, ARCHIVE_SAVEDATA_AND_CONTENT,
                                                fs_make_path_binary(archivepath, sizeof(archivepath)), fs_make_path_binary(filepath, sizeof(filepath)), FS_OPEN_READ, 0))) {
            u32 bytesRead = 0;

            if (R_SUCCEEDED(FSFILE_Read(filehandle, &bytesRead, 0, &smdh, sizeof(SMDH))) && bytesRead == sizeof(SMDH)) {
                if (smdh.magic[0] == 'S' && smdh.magic[1] == 'M' && smdh.magic[2] == 'D' && smdh.magic[3] == 'H') {
                    u16tou8(descstr, smdh.titles[1].shortdescription, 0x80);

                    for (int i = 0; i < 0x480; i++)
                        data.push_back(smdh.smallicon[i]);

                    m_smdhvector.push_back(data);
                    m_descvector.push_back(descstr);
                    m_finaltitles.push_back(titles[i]);
                }
            }

            FSFILE_Close(filehandle);
        }
    }
}

void Utils::TitleManager::ConvertIconsToC2DImage(std::vector<C2D_Image> &images, std::vector<C3D_Tex*> &texs) {
    texs.resize(m_smdhvector.size());
    static const Tex3DS_SubTexture subt3x = {24, 24, 0.0f, 24 / 32.0f, 24 / 32.0f, 0.0f};

    for (int i = 0; i < m_smdhvector.size(); i++) {
        texs[i] = new C3D_Tex;
        C3D_TexInit(texs[i], 32, 32, GPU_RGB565);
        uint16_t* dest = (uint16_t*)texs[i]->data + (32 - 24) * 32;
        uint16_t* src  = (uint16_t*)m_smdhvector[i].data();

        for (int j = 0; j < 24; j += 8) {
            std::copy(src, src + 24 * 8, dest);
            src += 24 * 8;
            dest += 32 * 8;
        }

        images.push_back({texs[i], &subt3x});
    }
}

void Utils::TitleManager::FilterOutTWLAndUpdate() {
    for (int i = 0; i < m_count; i++) {
        if ((((m_titles[i] >> 32) & 0x8000) == 0) && (((m_titles[i] >> 32) & 0xE) == 0))
            m_filteredvector.push_back(m_titles[i]);
    }
}