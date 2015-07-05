#include <stdint.h>


static const uint8_t image_data_null_0x20[1] = {//SPACE (only w needed)
    0x00
};
static const tImage null_0x20 = { image_data_null_0x20, 6};



static const tChar null_array[96] = {
  // character: ' '
  {0x20, &null_0x20},
};


const tFont null = { 1, null_array,6,8,0 };

