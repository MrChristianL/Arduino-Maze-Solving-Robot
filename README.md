<!-- ABOUT THE PROJECT -->
<img src="https://github.com/MrChristianL/MrChristianL/blob/main/images/Walter.jpg" width="600">

## Meet Walter - The Wall Tracking Extraction Rover
Walter was my first exposure to the field of robotics during my time in my undergraduate program. Walter was initially thought of as a minimalistic approach to providing a robot capable of searching enclosed areas, such as caves, with an emphasis on search and rescue efforts. With a limited budget and timeframe, Walter was boiled down to the core tenets necessary to make progress toward this goal.
<br/><br/>
Instead of a cave, Walter uses a 5x5 maze as the enclosed area for this instance. Being my first venture into robotics and search algorithms, Walter employed the Left Hand Rule (LHR) method of exploring and solving the maze to make use of the limited storage available on the Arduino. The single ultrasonic sensor fastened to the front of the robot allows Walter to detect the presense of walls within the maze, and then respond accordingly. Walter *does not* memorize the maze environment. The robot's memory is wiped before each trial, remembering a new set of obstacles that need to be drawn each time. 

### Walter Solving The Maze Using LHR and Ultrasonic Sensors
<img src="https://github.com/MrChristianL/MrChristianL/blob/main/images/Solving.gif" width="500">

Note that the robot also makes small corrections to its placement within the maze to effectively execute turns and maneuver the maze. This is due to Walter's relative size within the maze, requiring a high level of precision when determining Walter's next actions within the maze. Walter's best efforts to correct his position to nearby obstacles presents itself in a sort of wiggle-type movement of microcorrections. 

### The Goal of Walter
Rather than having the goal of making it from start to finish, the main focus of the project was to have Walter be able to traverse the maze, remembering every obstacle encountered throughout the journey, and then be able to draw the layout of the maze from memory after completion. Though Walter had a definitive start and end position for the sake of this project, Walter was capable of navigate throughout the environment when placed anywhere.

### 2D Re-Creation of Maze Environment
<img src="https://github.com/MrChristianL/MrChristianL/blob/main/images/Drawing.gif" width="350">
Though the programming of Walter employs C++, the drawing of the obstacles as seen above occurs in Processing, utilizing the Arduino's onboard EEPROM functionality to recall the location of each obstacle in an array-like fashion. This memory would be purged each time, allowing Walter to start from scratch with each subsequent run of the maze.
<br/><br/>
