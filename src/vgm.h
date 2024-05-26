#ifndef VGM_INC
#define VGM_INC

typedef struct {
    FILE* fp1;
    FILE* fp2;
    bool rec_started;
    const char* prompt;
    int vgmpos;
    unsigned char vgmdat[1024];
} vgm_sound_rec_t;

extern vgm_sound_rec_t vgm_rec;

bool vgm_start_rec(vgm_sound_rec_t* rec, const char* filename);
void vgm_stop_rec(vgm_sound_rec_t* rec);
void vgm_log_sound(vgm_sound_rec_t* rec);

#endif


