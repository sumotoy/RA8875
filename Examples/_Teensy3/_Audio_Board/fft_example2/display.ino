
uint8_t createBar(uint16_t origX,uint16_t origY){

  //the real bar height
  uint16_t barHeight = origY + (_segHeight * _segments) + (_segments * 2);
  if (barHeight >= tft.height()) return 0;//siamo fuori misura, exit
  //with vertical bars the Y origin starts from the bottom
  uint16_t newOriginY = (tft.height()-1)-barHeight;
  uint8_t rc,gc,bc;//container for colors
  bc = 1;//blue always 0
  for (int i=0;i<_segments;i++){
    gc = map(i,0,_segments-1,100,255);//calculate green
    rc = map(i,_segments-1,0,200,255);//calculate red
    tft.fillRect(origX,newOriginY+((_segHeight*i)+(i*2)),_barWidth,_segHeight,tft.Color565(bc,gc,rc));//bar
  }
  return barHeight;
}

void createBars(uint16_t vbands,uint16_t origX,uint16_t origY){
  uint8_t spacer = 2;
  if (bandRange == 4) spacer = 1;
  uint16_t barsWidth = origX + (_barWidth*vbands)+(vbands*spacer);

  if (barsWidth >= tft.width()) return;//too wide! exit
  //if (barsWidth >= tft.width()) Serial.println(barsWidth,DEC);
  tft.fillScreen(RA8875_BLACK);
  //tft.changeMode(GRAPHIC);
  tft.displayOn(false);
  tft.useLayers(true);//turn on layers
  tft.writeTo(L1);//write colored bars to layer 1
  for (int i=0;i<vbands;i++){//create the bars
    if (!createBar(origX+(_barWidth*i)+(i*spacer),origY)) return;
  }
  
  //tft.changeMode(TEXT);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(0,253);
  tft.print("<100");
  //tft.changeMode(GRAPHIC);
  tft.writeTo(L2);//from this point we write on layer 2
  tft.layerEffect(AND);//apply AND effect between layer 1 and 2
  tft.fillRect(0,251,tft.width()-1,tft.height()-1-251,0xFFFF);//bar
  tft.displayOn(true);
}

void updateBar(uint8_t val,uint16_t origX,uint16_t origY){

  uint16_t barHeight = origY + (_segHeight * _segments) + (_segments * 2);
  uint16_t newOriginY = (tft.height()-1)-barHeight;
  uint16_t currentBarH = map(val,0,255,_segHeight,(barHeight-origY));
  if (val < 255) tft.fillRect(origX,newOriginY,_barWidth,(barHeight-origY)-currentBarH,0x0000);//bar negative
  tft.fillRect(origX,(barHeight-newOriginY)-(currentBarH),_barWidth,currentBarH,0xFFFF);//bar positive

}

void updateBars(uint16_t vbands,uint16_t origX,uint16_t origY){
  uint8_t spacer = 2;
  if (bandRange == 4) spacer = 1;
  for (int i=0;i<vbands;i++){//update the bars
    updateBar(fftdata[i],origX+(_barWidth*i)+(i*spacer),origY);
  }
}


/*
static inline uint8_t u8fromfloat_trick(float x)
{
    union { float f; uint32_t i; } u;
    u.f = 32768.0f + x * (255.0f / 256.0f);
    return (uint8_t)u.i;
}
*/