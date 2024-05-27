#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#include "b-em.h"

#include "vgm.h"

const char* vgn_name[260];
static int vgmsamples;

vgm_sound_rec_t vgm_rec = {
    NULL,   // temp fp
    NULL,   // dest fp
    false,  // rec_started
    "Record VGM to file"
};

bool vgm_start_rec(vgm_sound_rec_t* rec, const char* filename)
{
    if (rec->fp1)
        fclose(rec->fp1);
    if (rec->fp2)
        fclose(rec->fp2);
    vgmsamples = rec->vgmpos = 0;
    rec->fp1 = fopen("temp.vgm", "wb");
    rec->fp2 = fopen(filename, "wb");
    rec->rec_started = true;
    putc('V', rec->fp1);
    putc('g', rec->fp1);
    putc('m', rec->fp1);
    putc(' ', rec->fp1);
    /*We don't know file length yet so just store 0*/
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*Version number - 1.50*/
    putc(0x50, rec->fp1); putc(1, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*Clock speed - 4mhz*/
    putc(4000000 & 255, rec->fp1);
    putc(4000000 >> 8, rec->fp1);
    putc(4000000 >> 16, rec->fp1);
    putc(4000000 >> 24, rec->fp1);
    /*We don't have an FM chip*/
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*We don't have an GD3 tag*/
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*We don't know total samples*/
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*No looping*/
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*50hz. This is true even in NTSC mode as the sound log is always updated at 50hz*/
    putc(50, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*White noise feedback pattern & length*/
    putc(3, rec->fp1); putc(0, rec->fp1); putc(15, rec->fp1); putc(0, rec->fp1);
    /*We don't have an FM chip*/
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*We don't have an FM chip*/
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*Data offset*/
    putc(0xC, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    /*Reserved*/
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);
    putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1); putc(0, rec->fp1);

    return true;
}

void vgm_stop_rec(vgm_sound_rec_t* rec)
{
    int c, len;

    putc(0x66, rec->fp1);
    len = ftell(rec->fp1) - 4;			// relative to start of file
    fclose(rec->fp1);
    rec->fp1 = fopen("temp.vgm", "rb");

    for (c = 0; c < 4; c++)
        putc(getc(rec->fp1), rec->fp2);

    putc(len, rec->fp2);
    putc(len >> 8, rec->fp2);
    putc(len >> 16, rec->fp2);
    putc(len >> 24, rec->fp2);

    for (c = 0; c < 4; c++)
        (void)getc(rec->fp1);

    for (c = 0; c < 16; c++)
        putc(getc(rec->fp1), rec->fp2);

    putc(vgmsamples, rec->fp2);
    putc(vgmsamples >> 8, rec->fp2);
    putc(vgmsamples >> 16, rec->fp2);
    putc(vgmsamples >> 24, rec->fp2);

    for (c = 0; c < 4; c++)
        (void)getc(rec->fp1);

    while (!feof(rec->fp1))
    {
        putc(getc(rec->fp1), rec->fp2);
    }

    fclose(rec->fp2);
    fclose(rec->fp1);
    remove("temp.vgm");
    rec->rec_started = false;
}

void vgm_log_sound(vgm_sound_rec_t* rec)
{
    if (!rec->rec_started)
        return;

    if (rec->vgmpos)
        fwrite(rec->vgmdat, rec->vgmpos, 1, rec->fp1);
        
    putc(0x63, rec->fp1);
    vgmsamples += 882;
    rec->vgmpos = 0;
}
