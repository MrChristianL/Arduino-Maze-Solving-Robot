import java.util.*;

enum Direction
{
  Top(1),
  Bottom(2),
  Right(4),
  Left(8);
  
  public final int value;
  
  private Direction(int value)
  {
    this.value = value;
  }
}

class Obstacle
{
  float xOffset = 40;
  float yOffset = 40;
  int index = 0;


  float placeholderX = 0;
  float placeholderY = 0;
  Set<Direction> directions = new HashSet<Direction>(); 

  Obstacle(int xPlace, int yPlace, Set<Direction> directionFacing)
  {
    placeholderX = xPlace;
    placeholderY = yPlace;
    directions = directionFacing;
    index++;
  }

  void display()
  {
      for (Direction direction : directions)
      {
        stroke(255);
        strokeWeight(4);
        switch (direction)
        {
          case Top:
            {
              PVector l = new PVector (placeholderX, placeholderY);
              line(l.x - xOffset, l.y - yOffset, l.x + xOffset, l.y - yOffset);
            }
            break;
          case Bottom:
            {
              PVector l = new PVector (placeholderX, placeholderY);
              //BACK
              line(l.x - xOffset, l.y + yOffset, l.x + xOffset, l.y + yOffset);
            }
            break;
          case Left:
            {
              PVector l = new PVector (placeholderX, placeholderY);
              line(l.x - xOffset, l.y - yOffset, l.x - xOffset, l.y + yOffset);
            }
            break;
          case Right:
            {
              PVector l = new PVector (placeholderX, placeholderY);
              line(l.x + xOffset, l.y - yOffset, l.x + xOffset, l.y + yOffset);
            }  
            break;
        }
      }
    }
  }
