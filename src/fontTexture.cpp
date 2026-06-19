#include "../include/HammerEngine/HammerEngine.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../lib/stb_truetype.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"


// thanks to https://github.com/ZDev22/ZEngine/blob/main/src/ztext.h for the base implementation
// and he use this https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c as base implementation 

HammerFont::HammerFont(HammerEngine& eng, const char* fontPath) : engine(eng) {
    FILE* fontFile = fopen(fontPath, "rb");
    fseek(fontFile, 0, SEEK_END);
    unsigned int size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    unsigned char* fontBuffer = (unsigned char*)malloc(size);

    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    stbtt_fontinfo info;
    stbtt_InitFont(&info, fontBuffer, 0);

    stbtt_fontinfo* oldInfo = (stbtt_fontinfo*)malloc(fontSize * sizeof(stbtt_fontinfo));
    memcpy(oldInfo, fonts, fontSize * sizeof(stbtt_fontinfo));

    free(fonts);
    fonts = (stbtt_fontinfo*)malloc((fontSize + 1) * sizeof(stbtt_fontinfo));
    memcpy(fonts, oldInfo, fontSize * sizeof(stbtt_fontinfo));
    fonts[fontSize] = info;
    fontSize++;
}

HammerCustomTexture* HammerFont::createTextPtr(HammerEngine& engine, const char* word, unsigned char index, const unsigned int l_h, const unsigned int b_w, const unsigned int b_h) {
    unsigned char* bitmap = (unsigned char*)calloc(1, b_w * b_h);

    float scale = stbtt_ScaleForPixelHeight(&fonts[index], l_h);

    unsigned int x = 0;
    int ascent  = 0;
    int descent = 0;
    int lineGap = 0;

    stbtt_GetFontVMetrics(&fonts[index], &ascent, &descent, &lineGap);

    ascent = (int)((float)ascent * scale);
    descent = (int)((float)descent * scale);

    for (unsigned int i = 0; i < strlen(word); ++i) {
        /* char width */
        int ax = 0; int lsb = 0;
        stbtt_GetCodepointHMetrics(&fonts[index], word[i], &ax, &lsb);

        /* get bounding box */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&fonts[index], word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* render char */
        int byteOffset = x + (int)((float)lsb * scale) + ((ascent + c_y1) * b_w);
        stbtt_MakeCodepointBitmap(&fonts[index], bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);

        /* advance & kerning */
        x += (unsigned int)((float)ax * scale) + (int)((float)stbtt_GetCodepointKernAdvance(&fonts[index], word[i], word[i + 1]) * scale);
    }

    HammerCustomTexture* output = new HammerCustomTexture(engine, bitmap, b_w, b_h, HammerTextureFilter::Linear);

    //stbi_write_png("out.png", b_w, b_h, 1, bitmap, b_w); // for debug, you can enable it

    free(bitmap); 
    
    return output;
}