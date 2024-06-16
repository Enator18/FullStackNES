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
bool player_dead = false;

uint8_t pad_sum;

uint8_t random(){
  set_rand(rand8() + pad_sum);

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

uint16_t x_pos;
uint16_t y_pos;



int16_t x_vel = 0;
int16_t y_vel = 0;

uint8_t block_x = 112;
uint8_t block_y = 0;

uint8_t on_ground;
uint8_t squished;

constexpr uint8_t starting_c_map[240] =
{
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
uint8_t c_map[240];
 
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

bool playerIntersect(uint8_t blockx, uint8_t blocky){
  // within this function, we are the block
  if(blockx + 15 < uint8_t(x_pos >> 8) ){
    return false; // we are too far to the right of the player
  }
  if(blockx > uint8_t(x_pos >> 8) + 7){
    return false; // we are too far to the left of the player
  }
  if(blocky + 15 < uint8_t(y_pos >> 8)){
    return false;//our bottom left corner is above the player's top left corner
  }
  if(blocky > uint8_t(y_pos >> 8) + 15){
    return false; // our top left corner is below the player's bottom left corner
  }
  return true;
}

void spawnBlock(){
  Block new_block;
  do{
    new_block.col = random()%12;
  }while(columns[new_block.col]>207);
  new_block.xpos = (new_block.col + 2) << 4;
  new_block.shouldExist = true;
  for(uint8_t i = 0; i < 16; i++){ // Magic Number: 16 Length of Blocks
    if(!blocks[i].shouldExist){
      blocks[i] = new_block;
      break;
    }
  }
}


void run_game(){
  x_pos = FIXED(124);
  y_pos = FIXED(208);
  x_vel = 0;
  y_vel = 0;
  frames_since_last_spawn = 200; // a nice value. Anything over 25 (the number of frames between spawns) works
  player_dead = false;
  for(uint8_t i = 0; i < 240; i++){
    c_map[i] = starting_c_map[i];
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
    ppu_wait_nmi();

    frames_since_last_spawn++;
    
    if(frames_since_last_spawn>25){
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
      player_dead = true;
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
        multi_vram_buffer_vert(tiles, sizeof(tiles),
                            NTADR_A((cols_to_change[i] << 1) + 4, (columns[cols_to_change[i]] >> 3) + 3));
        tiles[0] = 4;
        tiles[1] = 5;
        multi_vram_buffer_vert(tiles, sizeof(tiles),
                      NTADR_A((cols_to_change[i] << 1) + 5, (columns[cols_to_change[i]] >> 3) + 3));
        cols_to_change[i] = 12;
    }
    oam_clear();
    oam_spr((uint8_t)(x_pos >> 8), (uint8_t)((y_pos >> 8) - 1), 0x01, 0x00);
    for(uint8_t i = 0; i < 16; i++){ //Magic Number 16: length of blocks
      if(blocks[i].shouldExist){
        oam_meta_spr(blocks[i].xpos, blocks[i].ypos - 1, block_sprite);
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
}
int main(void)
{
  static const uint8_t palette[16]={ 0x0f, 0x07, 0x37, 0x17 };
  static const uint8_t palette_sp[16] = { 0x0f, 0x30, 0x0f, 0x00, 0x0f, 0x07, 0x37, 0x17 };

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

void player_movement()
{
    //Player Input
    char pad = pad_poll(0);

    pad_sum += pad;
    
    if(!player_dead)
    {
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
    }

    //Gravity
    if (y_vel < SFIXED(5))
    {
      y_vel += pad & PAD_A ? FIXED(0.25) : FIXED(0.75);
    }
    else
    {
      y_vel = SFIXED(5);
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
      *((uint8_t*)&x_pos) = 0;
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
            x_pos -= ((uint16_t)eject_x) << 8;
            *((uint8_t*)&x_pos) = 0;
            x_vel = 0;

            break;
          }
        }
      }
    }


    y_pos += y_vel;

    bg_collision();
    if (collision)
    {
      y_pos -= ((uint16_t)eject_y) << 8;
      *((uint8_t*)&y_pos) = 224;
      y_vel = 0;
    }
}

void block_movement(Block* block){
  uint8_t col_to_change = 12;
  if(block->shouldExist){
    block->ypos+=4;
    if(block->ypos>=columns[block->col]){
      block->shouldExist=false;
      c_map[(block->col) + (columns[block->col]) + 3] = 1;
      columns[block->col] -= 16;
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
        y_pos -= ((uint16_t)eject_y) << 8;
        *((uint8_t*)&y_pos) = 224;
        y_vel = FIXED(4);
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

void block_collision(Block* block)
{
  collision = 0;
  eject_x = 0;
  eject_y = 0;

  uint8_t left = (uint8_t)(x_pos >> 8);
  uint8_t up = (uint8_t)(y_pos >> 8);
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