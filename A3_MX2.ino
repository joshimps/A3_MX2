#include <stdio.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int currentSelection;

void setup()
{
    lcd.begin(16, 2);
    Serial.begin(9600);
}

void loop()
{
    mainMenu();
}

void mainMenu()
{ 
    currentSelection = 0;
    int analogValue = 0;

    lcd.clear();
    lcd.print("13533880");
    lcd.setCursor(0, 1);
    lcd.print("Main Menu");

    while (true)
    {
        delay(100);
        analogValue = analogRead(A0);

        if (analogValue <= 600 && analogValue >= 400 && currentSelection == 3)
        {
            delay(200);
            currentSelection = 1;
            setMenuSelection();
        }

        else if (analogValue <= 600 && analogValue >= 400)
        {
            delay(200);
            currentSelection++;
            setMenuSelection();
        }

        else if (analogValue >= 800 && analogValue <= 1000)
        {
            delay(200);
            modeChosen();
        }
    }
}

void setMenuSelection()
{
    if (currentSelection == 1)
    {
        lcd.setCursor(0, 1);
        lcd.print("Control        ");
        
    }

    else if (currentSelection == 2)
    {
        lcd.setCursor(0, 1);
        lcd.print("Sweep         ");
    }

    else if (currentSelection == 3)
    {
        lcd.setCursor(0, 1);
        lcd.print("Wall Follow");
    }
}

void modeChosen()
{
    printMessage("CMD_START");
    if (currentSelection == 1)
    {
        
        control();
    }

    else if (currentSelection == 2)
    {
        sweepMenu();
    }

    else if (currentSelection == 3)
    {
        wallFollow();
    }
}

void control()
{
    int analogValue = 0;

    while (true)
    {
        analogValue = analogRead(A0);

        if (analogValue <= 200 && analogValue >= 0) // Right
        {
            delay(50);
            printMessage("CMD_ACT_ROT_1_1");
        }

        else if (analogValue <= 800 && analogValue >= 600) // Left
        {
            delay(50);
            printMessage("CMD_ACT_ROT_0_1");
        }

        else if (analogValue <= 400 && analogValue >= 200) // Up
        {
            delay(200);
            printMessage("CMD_ACT_LAT_1_0.1");
            delay(200);
        }

        else if (analogValue <= 600 && analogValue >= 400) // Down
        {
            delay(200);
            printMessage("CMD_ACT_LAT_0_0.1");
            delay(200);
        }

        else if (analogValue <= 1000 && analogValue >= 800) // Select
        {
            delay(50);
            mainMenu();
        }
    }
}

void sweepMenu()
{
    int analogValue = 0;
   
    while (true)
    {
        delay(100);
        analogValue = analogRead(A0);

        if (analogValue <= 400 && analogValue >= 200) // Up
        {
            delay(200);
            sweep();
        }

        else if (analogValue <= 1000 && analogValue >= 800) // Select
        {
            delay(200);
            mainMenu();
        }
    }
}

String sweep()
{
    int closestObjectAngle = 0;
    String closestObjectDistance = "10";
    String tempObjectDistance;
    String angleString;

    //Find close to where the closest distance is

    for (int i = 360; i >= 0; i = i - 45)
    {
        // Create String
        angleString = "CMD_SEN_ROT_" + String(i);
        printMessage(angleString);
        delay(10);
        printMessage("CMD_SEN_IR");
        delay(10);
        tempObjectDistance = Serial.readString();
        delay(10);

        if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
        {
            closestObjectDistance = tempObjectDistance;
            closestObjectAngle = i;
        }
    }

    if(closestObjectDistance == "10")
    {
        return "10";
    }

    //Reset for next movement
    printMessage("CMD_SEN_ROT_0");
    closestObjectDistance = "10";
    //Find actual closest distance

    for(int j = 45 + closestObjectAngle; j >= closestObjectAngle -45; j = j-5)
    {
        angleString = "CMD_SEN_ROT_" + String(j);
        printMessage(angleString);
        delay(10);
        printMessage("CMD_SEN_IR");
        delay(10);
        tempObjectDistance = Serial.readString();
        delay(10);
        if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
        {
            closestObjectDistance = tempObjectDistance;
            
            if(j < 0)
            {
                closestObjectAngle = 360 + j;
            }

            else if(j >= 0)
            {
                closestObjectAngle = j;
            }
        }
    }

    printMessage("CMD_SEN_ROT_0");
    closestObjectDistance = "10";
    
    for(int k = (8 + closestObjectAngle); k >= (closestObjectAngle - 8); k--)
    {
        angleString = "CMD_SEN_ROT_" + String(k);
        printMessage(angleString);
        

            tempObjectDistance = Serial.readString();
        
        if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
        {
            closestObjectDistance = tempObjectDistance;
            
            if(k < 0)
            {
                closestObjectAngle = 360 + k;
            }

            else if(k >= 0)
            {
                closestObjectAngle = k;
            }
        }
        
    }

    delay(50);
    printMessage("CMD_SEN_ROT_0");
    rotateRobot(closestObjectAngle);

    return closestObjectDistance;
}

void wallFollow()
{
    moveIntoRange(); 
    moveTo2m(); 
    moveAcrossWall(); 
}

void moveIntoRange()
{
    String distanceFromWall;
    sweep();

    printMessage("CMD_SEN_IR");
    distanceFromWall = Serial.readString();

    while(distanceFromWall.charAt(0) == 'N')
    {
        printMessage("CMD_ACT_LAT_1_3");
        sweep();
        printMessage("CMD_SEN_IR");
        distanceFromWall = Serial.readString();
    }
}

void moveTo2m()
{
    String distanceFromWall;
    float distanceToTravel;

    while(true)
    {
        printMessage("CMD_SEN_IR");
        distanceFromWall = Serial.readString();

        if(distanceFromWall.toFloat() > 2.1 && distanceFromWall.charAt(0)!='N')
        {
            distanceToTravel = distanceFromWall.toFloat() - 2;
            printMessage("CMD_ACT_LAT_1_" + String(distanceToTravel));
            delay(50);
        }

        else if(distanceFromWall.toFloat() < 1.9 && distanceFromWall.charAt(0)!='N')
        {
            distanceToTravel = 2 - distanceFromWall.toFloat();
            printMessage("CMD_ACT_LAT_0_" + String(distanceToTravel));
            delay(50);
        }

        else if(distanceFromWall.charAt(0)=='N')
        {
            printMessage("CMD_ACT_LAT_1_3");
            delay(50);
            rotateRobot(270);
            reallign();
            moveTo2m();
            rotateRobot(90);
        }

        else
        {
            break;
        }
    }
}

void moveAcrossWall()
{
    String distanceFromNextWall;

    //Turn to face next wall
    while(true)
    {
        rotateRobot(90);
        moveTo2m();
    }
   
}

void rotateRobot(int angleToRotate)
{
    String sensorAngle;
    String sensorAngleCounter;

    int counter1 = 90;
    int counter2 = 20;
    int counter3 = 5;
    int counter4 = 1;


    printMessage("CMD_SEN_ROT_" + String(angleToRotate)) ;
    printMessage("CMD_SEN_CHECK");
    sensorAngle = Serial.readString();

    while(sensorAngle.toInt()>90)
    {   
        //Rotate robot one degree CCW
        printMessage("CMD_ACT_ROT_0_90");

        //Rotate sensor one degree CW to counteract robot spin
        angleToRotate = angleToRotate-counter1;
        sensorAngleCounter = "CMD_SEN_ROT_" + String(angleToRotate);
        printMessage(sensorAngleCounter);
        //Get current sensor angle to check later if = 0;
        printMessage("CMD_SEN_CHECK");
        sensorAngle = Serial.readString(); 

        if(sensorAngle.toInt()<100)
        {
            break;
        }

    }

    while(sensorAngle.toInt()>20)
    {   
        //Rotate robot one degree CCW
        printMessage("CMD_ACT_ROT_0_20");

        //Rotate sensor one degree CW to counteract robot spin
        angleToRotate = angleToRotate-counter2;
        sensorAngleCounter = "CMD_SEN_ROT_" + String(angleToRotate);
        printMessage(sensorAngleCounter);
        //Get current sensor angle to check later if = 0;
        printMessage("CMD_SEN_CHECK");
        sensorAngle = Serial.readString(); 

        if(sensorAngle.toInt()<=20)
        {
            break;
        }

    }

    while(sensorAngle.toInt() > 5)
    {
        //Rotate robot one degree CCW
        printMessage("CMD_ACT_ROT_0_5");

        //Rotate sensor one degree CW to counteract robot spin
        angleToRotate = angleToRotate - counter3;
        sensorAngleCounter = "CMD_SEN_ROT_" + String(angleToRotate);
        printMessage(sensorAngleCounter);
        //Get current sensor angle to check later if = 0;
        printMessage("CMD_SEN_CHECK");
        sensorAngle = Serial.readString();

         if(sensorAngle.toInt()<=5)
        {
            break;
        }

    }

    while(sensorAngle.toInt() > 0)
    {
        //Rotate robot one degree CCW
        printMessage("CMD_ACT_ROT_0_1");

        //Rotate sensor one degree CW to counteract robot spin
        angleToRotate = angleToRotate - counter4;
        sensorAngleCounter = "CMD_SEN_ROT_" + String(angleToRotate);
        printMessage(sensorAngleCounter);
        //Get current sensor angle to check later if = 0;
        printMessage("CMD_SEN_CHECK");
        sensorAngle = Serial.readString();

    }

    
}

void reallign()
{
    int closestObjectAngle = 0;

    String closestObjectDistance = "10";
    String tempObjectDistance;
    String angleString;

    for (int i = 5; i >= -5; i = i-5)
    {
        // Create String
        angleString = "CMD_SEN_ROT_" + String(i);
        printMessage(angleString);
        delay(10);
        printMessage("CMD_SEN_IR");
        delay(10);
        tempObjectDistance = Serial.readString();
        delay(10);

        if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
        {
            closestObjectDistance = tempObjectDistance;
            closestObjectAngle = i;
        }
    }

    rotateRobot(closestObjectAngle);
}


void printMessage(String message)
{
    Serial.print(message);
    Serial.write(13);
    Serial.write(10);
}
