//Christian Leonard
//March 9, 2020
//HN400

//NOTES:
//----
//Walter may require a few attempts to start the 'Transcribe' process, so if it doesn't work, unplug and try it again

//'ConcurrentModificationException', as far as I can tell, means it is trying to draw something that isn't there (such as unplugging Walter too early or if it gets stuck in a 'default movement cycle') 
//it's nothing major to worry about and will still draw a maze
//-----

import processing.serial.*; // imports library for serial communication
import java.awt.event.KeyEvent; // imports library for reading the data from the serial port
import java.io.IOException;
Serial myPort; // defines Object Serial

int columns, rows;
int scale = 80; // 400/80 = 5 rows and 5 columns
int posX = 360; //Starting position of the vehicle on the screen (bottom right)
int posY = 440;

String movementEntered = null;
float myVal;
String orientation = "forward";

int nl = 10; //detects a new line in the serial monitor

ArrayList<Obstacle> wallDetected;

PFont Font; //to type in the start and finish lines

void setup()
{
  size(800, 800);
  wallDetected = new ArrayList<Obstacle>();
  smooth(10);
  int w = 400; //size of the grid
  int h = 400;
  columns = w/scale; //set number of columns and rows
  rows = h/scale;
  Font = createFont("Serif", 24); //type of font

  myPort = new Serial(this, "COM4", 9600); // starts the serial communication
  myPort.bufferUntil(nl); // reads the data from the serial port up to the character '.'.
  vehicleSpace();
  frameRate(30);
}

void draw()
{
  background(200);
  translate(width/4, height/4);
  mazeGrid();
  goalLines();
  borderWalls();
  if (movementEntered != null)
  {
    vehicleSpace();
  }
  
  for (Obstacle barrier : wallDetected)
  {
    barrier.display();
  }
}

void mazeGrid() //light grey grid to define the possible cells of the 5x5 maze -- these will be drawn over if there are walls detected there
{
  stroke(160);
  strokeWeight(1);
  noFill();
  for (int x = 0; x < columns; x++)
  {
    for (int y = 0; y < rows; y++)
    {
      rect(x*scale, y*scale, scale, scale);
    }
  }
}

void borderWalls()
{
  fill(0);
  stroke(100);
  strokeWeight(5);
  line(0, 0, 0, 400); //left line
  line(80, 0, 400, 0); //top line
  line(400, 0, 400, 400); //right line
  line(0, 400, 320, 400); //bottom line
}

void goalLines()
{
  //starting line
  fill(0);
  noStroke();
  strokeWeight(2);
  textSize(14);
  text("START", 340, 418);

  stroke(0, 255, 0);
  strokeWeight(5);
  line(320, 400, 400, 400);

  //finish line
  fill(0);
  noStroke();
  strokeWeight(2);
  textSize(14);
  text("FINISH", 18, -8);

  stroke(128, 212, 255);
  strokeWeight(5);
  line(0, 0, 80, 0);
  text("Frame Rate = " + frameRate, -35, 420);
}

// --- Everything below this point is dependent on the vehicle's movement and the serial info ---

void serialEvent(Serial myPort) //pulls data from the serial port in the form of strings that are converted to float numbers
{
  movementEntered = myPort.readStringUntil(nl);
  //movementEntered = movementEntered.substring(0, movementEntered.length() - 1);
  //movementNum= parseInt(movementEntered);
  myVal = float(movementEntered);
  int movementInt = (int)myVal;
  //given the serial value, move accordingly
  Set<Direction> directions = new HashSet<Direction>();
  
  switch (movementInt)
  {
    case 10:
    case 11:
    case 12:
      // detected a obstical
      if (orientation == "forward")
      {
         switch(movementInt)
         {
             case 10:
               directions.add(Direction.Top);
               break;
             case 11:
               directions.add(Direction.Left);
               break;
             case 12:
               directions.add(Direction.Right);
               break;
             default:
               break;
         }
      }
      else if (orientation == "backward")
      {
         switch(movementInt)
         {
             case 10:
               
               directions.add(Direction.Bottom);
               break;
             case 11:
               directions.add(Direction.Right);
               break;
             case 12:
               directions.add(Direction.Left);
               break;
             default:
               break;
         }
      }
      else if (orientation == "left")
      {
         switch(movementInt)
         {
             case 10:
               
               directions.add(Direction.Left);
               break;
             case 11:
               directions.add(Direction.Bottom);
               break;
             case 12:
               directions.add(Direction.Top);
               break;
             default:
               break;
         }
      }
      else if (orientation == "right")
      {
         switch(movementInt)
         {
             case 10:
               
               directions.add(Direction.Right);
               break;
             case 11:
               directions.add(Direction.Top);
               break;
             case 12:
               directions.add(Direction.Bottom);
               break;
             default:
               break;
         }
      }
    
    default:
      break;
  }

  //if told to move forward from the direction you are currently facing, which way is that now?
  if (myVal == 2) //DRIVING FORWARD
  {
    if (orientation == "forward")
    {
      orientation = "forward";
    } else if (orientation == "right")
    {
      orientation = "right";
    } else if (orientation == "backward")
    {
      orientation = "backward";
    } else if (orientation == "left")
    {
      orientation = "left";
    }
  }

  //if told to move backward from the direction you are currently facing, which way is that now?
  if (myVal == 1) //DRIVING BACKWARD
  {
    if (orientation == "forward")
    {
      orientation = "backward";
    } else if (orientation == "right")
    {
      orientation = "left";
    } else if (orientation == "backward")
    {
      orientation = "forward";
    } else if (orientation == "left")
    {
      orientation = "right";
    }
  }

  //given the order to turn right, which way are you currently facing to turn to the right of where you are now?
  if (myVal == 4) //TURNING RIGHT
  {
    if (orientation == "forward")
    {
      orientation = "right";
    } else if (orientation == "right")
    {
      orientation = "backward";
    } else if (orientation == "backward")
    {
      orientation = "left";
    } else if (orientation == "left")
    {
      orientation = "forward";
    }
  }

  //given the order to turn left, which way are you currently facing to turn to the left of where you are now?
  if (myVal == 3) //TURNING LEFT
  {
    if (orientation == "forward")
    {
      orientation = "left";
    } else if (orientation == "left")
    {
      orientation = "backward";
    } else if (orientation == "backward")
    {
      orientation = "right";
    } else if (orientation == "right")
    {
      orientation = "forward";
    }
  }

  if (directions.size() > 0)//wall detection
  {
    wallDetected.add(new Obstacle(posX, posY, directions));
  }
  
  //given the order to move forward, which way are you currently facing to move in that direction?
  if (myVal == 99) //GO IN THE DIRECTION CURRENTLY FACING
  {
    if (orientation == "forward" && ((posY > 80) || (posX == 40 && posY == 40))) //if facing forward, move forward
    {
      posY -= 80;
    }
    if (orientation == "backward" && ((posY < 360) || (posX == 360 && posY == 360))) //if facing backward, move back
    {
      posY += 80;
    }
    if (orientation == "right" && (posX < 360))
    {
      posX += 80;
    }
    if (orientation == "left" && (posX > 40))
    {
      posX -= 80;
    }
  }
}

void vehicleSpace()
{
  //vehicle starts facing North
  stroke(0);
  strokeWeight(1);
  fill(255, 0, 0); //red car
  ellipse(posX, posY, 25, 25);
  strokeWeight(3);

  //a line out the front of the circle on the maze to indicate which way the circle is orientated when moving
  if (orientation == "forward")
  {
    line(posX, posY -12.5, posX, posY -20);
  }
  if (orientation == "backward")
  {
    line(posX, posY +12.5, posX, posY +20);
  }
  if (orientation == "left")
  {
    line(posX - 12.5, posY, posX - 20, posY);
  }
  if (orientation == "right")
  {
    line(posX + 12.5, posY, posX + 20, posY);
  }
}
