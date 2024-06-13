#include <neslib.h>
#include <nesdoug.h>
#include <stdint.h>
#include <ines.h>

MAPPER_USE_HORIZONTAL_MIRRORING;

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30

#define FIXED(float_literal) (uint16_t)(float_literal * 256.0)
#define SFIXED(float_literal) (int16_t)(float_literal * 256.0)
#define HIGH_BYTE(a) *((uint8_t*)&a+1)



typedef struct Block {
  uint8_t ypos = 0;
  uint8_t xpos = 140;
  uint8_t col = 7;
  bool shouldExist = true;
} Block;

uint8_t jumped;

uint8_t collision;
uint8_t floor_collision;

uint8_t eject_x;
uint8_t eject_y;

void player_movement();
void block_movement(Block* block);
void bg_collision();

uint16_t x_pos = FIXED(128);
uint16_t y_pos = FIXED(208);



int16_t x_vel = 0;
int16_t y_vel = 0;

uint8_t block_x = 112;
uint8_t block_y = 0;

uint8_t on_ground;

uint8_t c_map[240] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
 
uint8_t block_sprite[9] = 
{
  0, 0, 0x02, 0x01,
  8, 0, 0x04, 0x01,
  128
};

uint8_t columns[12] = {
  207,207,207,207,207,207,
  207,207,207,207,207,207
};

Block active_block;

int main(void)
{
  const unsigned char palette[4]={ 0x37, 0x0f, 0x17, 0x07 };
  static const char palette_sp[8] = { 0x0f, 0x30, 0x1c, 0x2c, 0x0f, 0x0f, 0x17, 0x07 };

  //Draw Background and set PPU Settings
  ppu_off();
  pal_bg(palette);
  pal_spr(palette_sp);

  oam_size(1);

  ppu_on_all();


  while (1)
  {
    ppu_wait_nmi();
    
    player_movement();

    block_movement(&active_block);

    oam_clear();
    oam_spr((uint8_t)(x_pos >> 8), (uint8_t)((y_pos >> 8) - 1), 0x01, 0x00);
    oam_meta_spr(active_block.xpos, active_block.ypos - 1, block_sprite);
  }
}

void player_movement()
{
    //Player Input
    char pad = pad_poll(0);
    if (pad & PAD_LEFT)
    {
      x_vel -= FIXED(0.25);
    }
    else if (pad & PAD_RIGHT)
    {
      x_vel += FIXED(0.25);
    }

    else if (x_vel > 0)
    {
      x_vel -= FIXED(0.25);
    }

    else if (x_vel < 0)
    {
      x_vel += FIXED(0.25);
    }

    if (on_ground)
    {
      if (pad & PAD_A)
      {
        if (!jumped)
        {
          on_ground = 0;
          jumped = 1;
          y_vel = SFIXED(-7);
        }
      }
      else
      {
        jumped = 0;
      }
    }

    //Gravity
    if (y_vel < SFIXED(4))
    {
      y_vel += pad & PAD_A ? FIXED(0.25) : FIXED(0.75);
    }

    int16_t max_speed = pad & PAD_B ? SFIXED(3) : SFIXED(2);

    //Cap Velocity
    if (x_vel > max_speed)
    {
      x_vel = max_speed;
    }

    if (x_vel < -max_speed)
    {
      x_vel = -max_speed;
    }

    //Move with Collisions
    x_pos += x_vel;

    bg_collision();
    if (collision)
    {
      x_pos -= ((uint16_t)eject_x) << 8;
      x_vel = 0;
    }


    y_pos += y_vel;

    bg_collision();
    if (collision)
    {
      y_pos -= ((uint16_t)eject_y) << 8;
      y_vel = 0;
    }
}

void block_movement(Block* block)
{
  if(block->shouldExist){
    block->ypos+=4;
    if(block->ypos>=columns[block->col]){
      columns[block->col] -= 16;
      block->shouldExist=false;
    }
  }else{
    active_block = Block();
  }
}

void bg_collision()
{
  collision = 0;
  on_ground = 0;
  eject_x = 0;
  eject_y = 0;

  uint8_t left = (uint8_t)(x_pos >> 8);
  uint8_t up = (uint8_t)(y_pos >> 8);
  uint8_t right = left + 7;
  uint8_t down = up + 15;

  if (up >= 0xf0)
  {
    return;
  }

  if (c_map[(left >> 4) + (up & 0xf0)])
  {
    ++collision;
    eject_x = (left & 0x0f) - 16;
    eject_y = (up & 0x0f) - 16;
  }

  if (c_map[(right >> 4) + (up & 0xf0)])
  {
    ++collision;
    eject_x = (right & 0x0f) + 1;
    eject_y = (up & 0x0f) - 16;
  }

  if (down >= 0xf0)
  {
    return;
  }

  if (c_map[(left >> 4) + (down & 0xf0)])
  {
    ++collision;
    ++on_ground;
    eject_x = (left & 0x0f) - 16;
    eject_y = (down & 0x0f) + 1;
  }

  if (c_map[(right >> 4) + (down & 0xf0)])
  {
    ++collision;
    ++on_ground;
    eject_x = (right & 0x0f) + 1;
    eject_y = (down & 0x0f) + 1;
  }
}