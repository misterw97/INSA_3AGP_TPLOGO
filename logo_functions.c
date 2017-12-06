#include "logo_functions.h"

Program InitLogo() {
  return NULL;
}

Node* createNode(Instruction instruction, int value, Program subNode) {
  Node* node = (Node*)malloc(sizeof(Node));
  node->instruction = instruction;
  node->value = value;
  node->next = NULL;
  node->subNode = subNode;
  return node;
}

void freeLogo(Program* program) {
  if (*program == NULL)
    return;
  freeLogo(&((*program)->subNode));
  freeLogo(&((*program)->next));
  free(*program);
}

void addNode(Program* program, Node* node) {
  if (*program == NULL) {
    *program = node;
  } else {
    return addNode(&((*program)->next),node);
  }
}

int getColorIndex(int value)  {
  int color = (int)(value/10);
  color *= 10;
  return (value-color);
}

char* getColorName(int value)  {
  switch (getColorIndex(value)) {
    case 0:
      return "RED";
    case 1:
      return "GREEN";
    case 2:
      return "BLUE";
    default:
      return "UNKNOWN";
  }
}

int getColorValue(int value)  {
  return (int)(value/10);
}

void printLogo(Program program, int indent) {
  if (program == NULL)
    return;
  int i;
  for (i=0;i<indent;++i)
    printf("\t");
  switch (program->instruction) {
    case FORWARD:
      printf("FORWARD %d", program->value);
      break;
    case LEFT:
      printf("LEFT %d", program->value);
      break;
    case RIGHT:
      printf("RIGHT %d", program->value);
      break;
    case REPEAT:
      printf("REPEAT %d [\n", program->value);
      printLogo(program->subNode, indent+1);
      for (i=0;i<indent;++i)
        printf("\t");
      printf("]");
      break;
    case DCOLOR:
      printf("DELTA ");
    case COLOR:
      printf("COLOR %s %d",getColorName(program->value),getColorValue(program->value));
      break;
    case HIDE:
      printf("HIDE %d", program->value);
      break;
    default:
      printf("UNKNOWN %d", program->value);
  }
  printf("\n");
  printLogo(program->next, indent);
}

void writeSVGLine(FILE* svg, double x, double y, Pen pen) {
  fprintf(
    svg,
    "<line x1=\"%.3f\" y1=\"%.3f\" x2=\"%.3f\" y2=\"%.3f\" stroke=\"#%02X%02X%02X\" />\n",
    x,y,
    pen.x,pen.y,
    pen.rgb[0],pen.rgb[1],pen.rgb[2]
  );
}

void writeSVGInstruction(FILE* svg, Program program, Pen* pen) {
  if (program == NULL)
    return;
  double xOld, yOld;
  int i;
  switch (program->instruction) {
    case FORWARD:
      xOld = pen->x;
      yOld = pen->y;
      movePen(pen,program->value);
      if (pen->active) writeSVGLine(svg,xOld,yOld,*pen);
      break;
    case LEFT:
    case RIGHT:
      rotatePen(pen,program->instruction,program->value);
      break;
    case REPEAT:
      for (i=0;i<program->value;++i) {
        writeSVGInstruction(svg,program->subNode,pen);
      }
      break;
    case HIDE:
      pen->active = 1 - program->value;
      break;
    case COLOR:
      pen->rgb[getColorIndex(program->value)] = getColorValue(program->value);
      break;
    case DCOLOR:
      pen->rgb[getColorIndex(program->value)] += getColorValue(program->value);
      break;
    default:
      perror("Instruction non identifiée !");
  }
  writeSVGInstruction(svg,program->next,pen);
}

void defineCanvas(Program program, Pen* pen, double* initX, double* initY, double* sizeX, double* sizeY) {
  if (program == NULL)
    return;
  int i;
  switch (program->instruction) {
    case FORWARD:
      movePen(pen,program->value);
      *initX = min(*initX,pen->x);
      *initY = min(*initY,pen->y);
      *sizeX = max(*sizeX,pen->x);
      *sizeY = max(*sizeY,pen->y);
      break;
    case LEFT:
    case RIGHT:
      rotatePen(pen,program->instruction,program->value);
      break;
    case REPEAT:
      for (i=0;i<program->value;++i) {
        defineCanvas(program->subNode,pen,initX,initY,sizeX,sizeY);
      }
      break;
    default:
      break;
  }
  defineCanvas(program->next,pen,initX,initY,sizeX,sizeY);
}

void writeSVG(Program program, char* name) {
    FILE* svg = fopen(name,"w+");
    // write svg headers
    fprintf(svg,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    // TODO Calculer la taille du dessin ! (et centrer le Pen ?)
    Pen pen;
    InitPen(&pen);
    double x=0,y=0,sizex=0,sizey=0;
    defineCanvas(program,&pen,&x,&y,&sizex,&sizey);
    pen.x = -x;
    pen.y = -y;
    pen.alpha = 0.0;
    fprintf(svg,"<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\">\n",(int)(-x+sizex),(int)(-y+sizey));
    fprintf(svg,"<title>%s</title>\n",name);
    writeSVGInstruction(svg,program,&pen);
    fprintf(svg,"</svg>\n");
    fclose(svg);
}
