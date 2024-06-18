#include <neslib.h>
#include <nesdoug.h>
#include <stdint.h>
#include <ines.h>
#include <fixed_point.h>

using namespace fixedpoint_literals;

MAPPER_USE_VERTICAL_MIRRORING;

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30

//#define FIXED(float_literal) (uint16_t)(float_literal * 256.0)
//#define SFIXED(float_literal) (int16_t)(float_literal * 256.0)
//#define HIGH_BYTE(a) *((uint8_t*)&a+1)
bool player_dead = false;
uint8_t paused = 0;
uint8_t paused_down = 0;

uint8_t pad_sum;

uint8_t random(){
  set_rand(rand8() + (pad_sum << 8));

  return rand8();
}

typedef struct Block {
  uint8_t ypos = 0;
  uint8_t xpos = 144; // (2 + col) * 16
  uint8_t col = 12;
  bool shouldExist = false;
} Block;

void write_to_collisions(Block block);

uint8_t jumped;

uint8_t collision;
uint8_t floor_collision;

int8_t eject_x;
int8_t eject_y;

void player_movement();
void block_movement(Block* block);
void bg_collision();
void block_collision(Block* block);
void update_pause();

FixedPoint<12, 4, false> x_pos = 0;
FixedPoint<12, 4, false> y_pos = 0;
uint8_t player_dir;



FixedPoint<12, 4> x_vel = 0;
FixedPoint<12, 4> y_vel = 0;

uint8_t block_x = 112;
uint8_t block_y = 0;

uint8_t on_ground;
uint8_t squished;

uint16_t scroll_wait;
uint8_t scrolling;
uint8_t scroll_timer;
uint16_t y_scroll;

// constexpr uint8_t starting_c_map[240] =
// {
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
//   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
// };
uint8_t c_map[240];
// uint8_t get_normal_column(){
//   uint8_t r1 = (random()%6);
//   uint8_t r2 = (random()%6);
//   return r1 + r2 + 1;
// }
 
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
// uint8_t columns_old[12] = {
//   206,206,206,206,206,206,
//   206,206,206,206,206,206
// };
uint8_t cols_to_change[16] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

// uint8_t getIndexOfEmptyCollider(uint8_t column){
//   for(uint8_t i = 0; i < 15; i++){

//   }
// }
uint8_t frames_since_last_spawn = 200;
Block blocks[16];

bool columnOk(uint8_t col){
  for(Block b : blocks){
    if(b.shouldExist && b.col==col && b.ypos < 17){
      return false;
    }
  }
  return true;
}

void spawnBlock(){
  Block new_block;
  do{
    new_block.col = random()%12;
  }while(/*columns[new_block.col]>207 || */!columnOk(new_block.col)); //Only matters if spawning blocks rapidly
  new_block.xpos = (new_block.col + 2) << 4;
  new_block.shouldExist = true;
  for(uint8_t i = 0; i < 16; i++){ // Magic Number: 16 Length of Blocks
    if(!blocks[i].shouldExist){
      new_block.ypos += (uint8_t)(y_scroll&255);
      new_block.ypos %=240;
      blocks[i] = new_block;
      break;
    }
  }
}


void run_game(){
  x_pos = 124.0_u12_4;
  y_pos = 208.0_u12_4;
  // x_pos = 5.588;
  x_vel = 0;
  y_vel = 0;
  scroll_wait = 0;
  scrolling = 0;
  y_scroll = 0;
  set_scroll_y(y_scroll);
  frames_since_last_spawn = 200; // a nice value. Anything over 25 (the number of frames between spawns) works
  player_dead = false;
  y_scroll = 0;
  for(uint8_t i = 0; i < 224; i++){
    c_map[i] = 1 - (((i%16)>1)&((i%16)<14));
    // c_map[i] = starting_c_map[i];
  }
  for(uint8_t i = 224; i < 240; i++){
    c_map[i] = 1;
  }
  for(uint8_t i = 0; i < 16; i++){
    cols_to_change[i]=12;
    blocks[i]=Block();
  }
  for(uint8_t i = 0; i < 12; i++){
    columns[i] = 207;
  }
  //   vram_adr(NTADR_A(0,0));
  // vram_fill(0,128);
  constexpr uint8_t top_row[24] = {
    2,4,
    2,4,
    2,4,
    2,4,
    2,4,
    2,4,
    2,4,
    2,4,
    2,4,
    2,4,
    2,4,
    2,4,
  };
    constexpr uint8_t bottom_row[24] = {
    3,5,
    3,5,
    3,5,
    3,5,
    3,5,
    3,5,
    3,5,
    3,5,
    3,5,
    3,5,
    3,5,
    3,5,
  };
  set_vram_buffer();
  multi_vram_buffer_horz(top_row, sizeof(top_row),
                    NTADR_A(4, 28));
  multi_vram_buffer_horz(bottom_row, sizeof(bottom_row),
                    NTADR_A(4, 29));
  while(!((pad_poll(0)&PAD_START))){
    ppu_wait_nmi();
  }
  seed_rng();

  while (!player_dead)
  {
    update_pause();
    ppu_wait_nmi();
    if(!paused)
    {
      if (scrolling)
      {
        scroll_timer++;

        if (scroll_timer >= 18)
        {
          y_scroll = sub_scroll_y(1, y_scroll);

          set_scroll_y(y_scroll);

          scroll_timer = 0;
        }
      }
      else
      {
        scroll_wait++;
        if (scroll_wait > 864)
        {
          scrolling = 1;
        }
      }

      frames_since_last_spawn++;
      
      if(frames_since_last_spawn>=24){
        spawnBlock();
        frames_since_last_spawn = 0;
      }
      // *sq1_pitch_low = rand8();
      // *sq1_vol = (rand8() & 0b11000000) + 0b00111111;
      //wait

      
      //move dude
      player_movement();

      squished = 0;
      //move stuff
      for(uint8_t i = 0; i < 16; i++){ //Magic Number 16: Length of blocks
        if(blocks[i].shouldExist){
          block_movement(&(blocks[i]));
        }
      }

      if (squished)
      {
        // player_dead = true;
      }

      //hardware hell
      set_vram_buffer();
      for(uint8_t i = 0; i < 16; i++){
        if(cols_to_change[i]==12){
          break;
        }
          uint8_t tiles[2] = {
            2,3
          };
          uint16_t ppuaddr1, ppuaddr2;
          // if(columns[cols_to_change[i]] < 32){
            ppuaddr1 = NTADR_A((cols_to_change[i] << 1) + 4, (columns[cols_to_change[i]] >> 3) + 3);
            ppuaddr2 = NTADR_A((cols_to_change[i] << 1) + 5, (columns[cols_to_change[i]] >> 3) + 3);
          // }else{
          //   ppuaddr1 = NTADR_B((cols_to_change[i] << 1) + 4, (columns[cols_to_change[i]] >> 3) + 3);
          //   ppuaddr2 = NTADR_B((cols_to_change[i] << 1) + 5, (columns[cols_to_change[i]] >> 3) + 3);
          // }
          multi_vram_buffer_vert(tiles, sizeof(tiles),
                              ppuaddr1);
          tiles[0] = 4;
          tiles[1] = 5;
          multi_vram_buffer_vert(tiles, sizeof(tiles),
                        ppuaddr2);
          cols_to_change[i] = 12;
      }
      oam_clear();
      oam_spr(x_pos.as_i(), (y_pos.as_i() - 1 - (y_scroll&255)) - ((y_scroll>255) << 4), 0x01, player_dir);
      for(uint8_t i = 0; i < 16; i++){ //Magic Number 16: length of blocks
        if(blocks[i].shouldExist){
          oam_meta_spr(blocks[i].xpos, blocks[i].ypos - 1 - (y_scroll&255) - ((y_scroll>255) << 4), block_sprite);
        }
      }
    }
  }
  uint8_t blank_row[24] = {
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
  };
  oam_clear();
  for(uint8_t i = 0; i < 28; i++){
    set_vram_buffer();
    multi_vram_buffer_horz(blank_row, sizeof(blank_row),
                      NTADR_A(4, i));
    ppu_wait_nmi();
  }
  //Death animation here
  y_scroll = 0;
}
int main(void)
{
  static const uint8_t palette[4]={ 0x0f, 0x07, 0x37, 0x17 };
  static const uint8_t palette_sp[8] = { 0x0f, 0x30, 0x0f, 0x00, 0x0f, 0x07, 0x37, 0x17 };

  //Draw Background and set PPU Settings
  ppu_off();
  pal_bg(palette);
  pal_spr(palette_sp);

  oam_size(1);

  vram_adr(get_at_addr(0,0,0));
  vram_fill(0,64);

  ppu_on_all();
  // *apu_enable = 1;
  // *sq1_vol = 0x3f;
  // *sq1_pitch_low = 0xc9;
  // *sq1_pitch_high = 0;
  while(1){
    run_game();
  }

}

void update_pause(){
  uint8_t pad = pad_poll(0);
  if(!paused_down){
    paused = (paused ^ (pad & PAD_SELECT));
  }
  paused_down = (pad & PAD_SELECT);
}

void player_movement()
{
    //Player Input
    char pad = pad_poll(0);

    pad_sum += pad;
    
    if(!player_dead)
    {
      if (pad & PAD_LEFT)
      {
        x_vel -= 0.25_12_4;
        player_dir = 64;
      }
      else if (pad & PAD_RIGHT)
      {
        x_vel += 0.25_12_4;
        player_dir = 0;
      }

      else if (x_vel > 0)
      {
        x_vel -= 0.25_12_4;
      }

      else if (x_vel < 0)
      {
        x_vel += 0.25_12_4;
      }

      if (on_ground)
      {
        if (pad & PAD_A)
        {
          if (!jumped)
          {
            on_ground = 0;
            jumped = 1;
            y_vel = -7.0_12_4;
          }
        }
        else
        {
          jumped = 0;
        }
      }
    }

    //Gravity
    if (y_vel < 5.0_12_4)
    {
      y_vel += pad & PAD_A ? 0.25_12_4 : 0.75_12_4;
    }
    else
    {
      y_vel = 5.0_12_4;
    }

    FixedPoint<12, 4> max_speed = pad & PAD_B ? 3.0_12_4 : 2.0_12_4;

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
      x_pos -= eject_x;
      x_pos.set_f(0);
      x_vel = 0;
    }

    else
    {
      for (uint8_t i = 0; i < 16; i++)
      {
        if (blocks[i].shouldExist)
        {
          block_collision(&blocks[i]);
          if (collision)
          {
            x_pos -= eject_x;
            x_pos.set_f(0);
            x_vel = 0;

            break;
          }
        }
      }
    }


    y_pos += y_vel;
    y_pos %= 240.0_u12_4;

    bg_collision();
    if (collision)
    {
      y_pos -= eject_y;
      y_pos %= 240.0_u12_4;
      y_pos.set_f(12);
      y_vel = 0;
    }
}

void block_movement(Block* block){
  uint8_t col_to_change = 12;
  if(block->shouldExist){
    block->ypos+=4;
    block->ypos%=240;
    if(block->ypos >> 2 ==(columns[block->col] >> 2)+1){
      block->shouldExist=false;
      c_map[(block->col) + (columns[block->col]) + 3] = 1;
      columns[block->col] -= 16;
      if(columns[block->col]==15){ //207%16 DO NOT FORGET
        columns[block->col] -= 16;
      }
      col_to_change = block->col;
      for(uint8_t i = 0; i < 16; i++){
        if(cols_to_change[i]==12){
          cols_to_change[i]=col_to_change;
          break;
        }
      }
      // spawnBlock();
    }
    // else if (playerIntersect(block->xpos, block->ypos)) {
    //   player_dead = true;
    // }

    else
    {
      block_collision(block);

      if (collision)
      {
        y_pos -= eject_y;
      y_pos.set_f(12);
        y_vel = 4.25_12_4;
      }
    }
  }
}

void bg_collision()
{
  collision = 0;
  on_ground = 0;
  eject_x = 0;
  eject_y = 0;

  uint8_t left = x_pos.as_i();
  uint8_t up = y_pos.as_i();
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

void block_collision(Block* block)
{
  collision = 0;
  eject_x = 0;
  eject_y = 0;

  uint8_t left = x_pos.as_i();
  uint8_t up = y_pos.as_i();
  uint8_t right = left + 7;
  uint8_t down = up + 15;

  if ((left >> 4) - 2 == block->col && up > block->ypos && up < block->ypos + 16)
  {
    ++collision;
    eject_x = left - block->xpos - 16;
    eject_y = up - block->ypos - 16;

    if (on_ground)
    {
      ++squished;
    }
  }

  if ((right >> 4) - 2 == block->col && up > block->ypos && up < block->ypos + 16)
  {
    ++collision;
    eject_x = right - block->xpos + 1;
    eject_y = up - block->ypos - 16;

    if (on_ground)
    {
      ++squished;
    }
  }

  if ((left >> 4) - 2 == block->col && down > block->ypos && down < block->ypos + 16)
  {
    ++on_ground;
    ++collision;
    eject_x = left - block->xpos - 16;
    eject_y = down - block->ypos + 1;
  }

  if ((right >> 4) - 2 == block->col && down > block->ypos && down < block->ypos + 16)
  {
    ++on_ground;
    ++collision;
    eject_x = right - block->xpos + 1;
    eject_y = down - block->ypos + 1;
  }
}