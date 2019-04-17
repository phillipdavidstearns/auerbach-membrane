int rows=4;
int cols=4;
float gridPosX;
float gridPosY;
float gridW=300;
float gridH=300;
float paddingX=10;
float paddingY=10;
float radius=90;
boolean callibrate = true;

void setup() {
  size(500, 500);
  gridPosX=width/2;
  gridPosY=height/2;
  strokeWeight(0.5);
  rectMode(CENTER);
  textAlign(CENTER, CENTER);
  drawGrid();
}

void draw() {
}

void keyPressed(){
  switch(key){
    case 't':
    callibrate = !callibrate;
    break;
  }
  drawGrid();
}

void drawGrid() {
  background(255);
  float gridSpacingX = gridW/(cols-1);
  float gridSpacingY = gridH/(rows-1);
  float gridOffsetX = gridPosX-(gridW/2);
  float gridOffsetY = gridPosY-(gridH/2);
  fill(255);
  rect(gridPosX, gridPosY, gridW+gridSpacingX+paddingX, gridH+gridSpacingY+paddingY);
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      // horizontal line
      //line(x*gridSpacingX+gridOffsetX, 0, x*gridSpacingX+gridOffsetX, width);
      // vertical line(
      //line(0, y*gridSpacingY+gridOffsetY, height, y*gridSpacingY+gridOffsetY);
      fill(255);
      circle(x*gridSpacingX+gridOffsetX, y*gridSpacingY+gridOffsetY, radius);
      fill(0);
      drawLabels(x+y*cols, x*gridSpacingX+gridOffsetX, y*gridSpacingY+gridOffsetY);
    }
  }
}

void drawLabels(int _index, float _x, float _y) {
  String label = new String();

  if (callibrate) {
    switch(_index) {
    case 0:
      label = "Motor 1\nCCW\n1 deg";
      break;
    case 1:
      label = "Motor 1\nCW\n1 deg";
      break;
    case 2:
      label = "Motor 2\nCCW\n1 deg";
      break;
    case 3:
      label = "Motor 2\nCW\n1 deg";
      break;
    case 4:
    label = "Set OPEN\nTo Current\nPosition";
      break;
    case 5:
    label = "Reset\nOPEN\nPosition";
      break;
    case 6:
    label = "Set CLOSE\nTo Current\nPosition";
      break;
    case 7:
    label = "Reset\nCLOSE\nPosition";
      break;
    case 8:
    label="Go to\nOPEN\nPosition";
      break;
    case 9:
    label="Go to\nCLOSE\nPosition";
      break;
    case 10:
    label="Close by\n10 deg";
      break;
    case 11:
    label="Open by\n10 deg";
      break;
    case 12:
    label="(no function)";
      break;
    case 13:
    label="(no function)";
      break;
    case 14:
    label="(no function)";
      break;
    case 15:
    label="Exit\nCALIBRATE\nEnter\nRUN Mode";
      break;
    default:
      break;
    }
  } else {
    switch(_index) {
    case 0:
      label = "OPEN";
      break;
    case 1:
      label = "CLOSE";
      break;
    case 2:
      label = "STARTUP";
      break;
    case 3:
      label = "CALIBRATE";
      break;
    }
  }
  fill(0);
  text(label, _x, _y);
}
