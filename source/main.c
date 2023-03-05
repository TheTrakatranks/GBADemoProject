#include <tonc.h>

#include "gfx/ball.h"
#include "gfx/grotto.h"

OBJ_ATTR objBuffer[128];

void BGSetup()
{
    //Charger les tiles + palette dans la VRAM (tilemem pour les tiles en charblocks et pal_bg_mem pour la palette BG)
    memcpy16(&tile_mem[0][0], grottoTiles, grottoTilesLen);
    memcpy16(pal_bg_mem, grottoPal, grottoPalLen);

    //Charger la map dans la VRAM en se_mem (Screenblock entries)
    memcpy16(&se_mem[10][0], grottoMap, grottoMapLen);
    //Charger la map dans la VRAM en se_mem (Screenblock entries)
    memcpy16(&se_mem[20][0], grottoMap, grottoMapLen);

    // Set up the background control registers at charclock base 0 and ScreenEntry
    REG_BG0CNT = BG_CBB(0) | BG_SBB(10) | BG_4BPP | BG_REG_32x32;
    REG_BG1CNT = BG_CBB(0) | BG_SBB(20) | BG_4BPP | BG_REG_32x32;

    for (int i = 0; i < 1024; i++)
    {
        se_mem[20][i] += 0x2000;
    }

    
}



//---------BALL INIT--------

int velx = 5;
int vely = 10;
int posx = 50;
int posy = 50;
int s_off = 4; // 8/2

OBJ_ATTR *ballObj;

void initBall(){
    //Copy Sprites and Palettes to VRAM
    memcpy16(MEM_VRAM_OBJ, ballTiles, ballTilesLen);
    memcpy16(MEM_PAL_OBJ, ballPal, ballPalLen);

    oam_init(objBuffer, 128); //init empty

    //init object for the ball
    ballObj = &objBuffer[1];
    obj_set_attr(   ballObj,
                    ATTR0_SQUARE | ATTR0_8BPP,
                    ATTR1_SIZE_8,
                    ATTR2_PALBANK(0) | ATTR2_ID(0)
    );
    obj_set_pos(ballObj, posx - s_off, posy - s_off);
}

//--------!BALL INIT--------

int main(void)
{
    irq_init(NULL);
    irq_enable(IRQ_VBLANK);

    // Initialize Display control
    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ_1D ;


    BGSetup();
    initBall();

    int ctr = 0;

    // Main loop
    while(1) {
        ctr++;
        vid_vsync();
        key_poll();

        posx += velx;
        posy += vely;
        
        if(posx < 0 + s_off){
            posx = 0 + s_off;
            velx *= -1;
        } 
        else if(posx > 240 - s_off){
            posx = 240 - s_off;
            velx *= -1;
        } 

        if(posy < 0 + s_off){
            posy = 0 + s_off;
            vely *= -1;
        } 
        else if(posy > 160 - s_off){
            posy = 160 - s_off;
            vely *= -1;
        } 

        REG_BG1HOFS = sin_lut[ctr%514] / 30;
        REG_BG1VOFS = ctr / 1;

        ballObj->attr2 = ATTR2_BUILD(0,0,0);
        obj_set_pos(ballObj, posx - s_off, posy - s_off);

        // Copy updated object attributes to OAM
        oam_copy(oam_mem, ballObj, 1);
    }
    return 0;
}
