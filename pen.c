#include "pen.h"

void InitPen(Pen* pen) {
  pen->active = 1;
  pen->x = 0.0;
  pen->y = 0.0;
  pen->alpha = 0.0;
  pen->rgb[0] = 0;
  pen->rgb[1] = 0;
  pen->rgb[2] = 0;
}

double getInRadians(int a) {
  return (double)(a * PI / 180.0);
}

void rotatePen(Pen* pen, Instruction sig, int dalpha) {
  double da = getInRadians(dalpha);
  switch (sig) {
    case LEFT:
      pen->alpha -= da;
      break;
    case RIGHT:
      pen->alpha += da;
      break;
    default:
      perror("Bad usage of rotatePen !");
  }
}

void movePen(Pen* pen, int value) {
  pen->x += (double)(value*cos(pen->alpha));
  pen->y += (double)(value*sin(pen->alpha));
}

void changePenColor(Pen* pen, int index, int value) {
  // Change
  pen->rgb[index] = value;
  // Check Limits
  while (pen->rgb[index]>255) pen->rgb[index]-=255;
  if (pen->rgb[index]<0) pen->rgb[index] = 0;
}
