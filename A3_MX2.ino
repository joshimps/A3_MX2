#include <stdio.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int currentSelection = 0;

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

    while (true)
    {
        delay(100);
        analogValue = analogRead(A0);

        if(currentSelection == 0)
        {
            lcd.clear();
            lcd.print("13533880");
            lcd.setCursor(0, 1);
            lcd.print("Main Menu");
        }

        else if (analogValue <= 600 && analogValue >= 400 && currentSelection == 3)
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
        lcd.print("Wall Follow");
    }

    else if (currentSelection == 2)
    {
        lcd.setCursor(0, 1);
        lcd.print("Control        ");
    }

    else if (currentSelection == 3)
    {
        lcd.setCursor(0, 1);
        lcd.print("Sweep         ");
    }
}

void modeChosen()
{
    printMessage("CMD_START");
    if (currentSelection == 1)
    {
        wallFollow();
    }

    else if (currentSelection == 2)
    {
        control();
    }

    else if (currentSelection == 3)
    {
        sweepMenu();
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

    for (int i = 360; i > -1; i = i - 10)
    {
        // Create String
        angleString = "CMD_SEN_ROT_" + String(i);
        printMessage(angleString);
        printMessage("CMD_SEN_IR");
        tempObjectDistance = Serial.readString();

        if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
        {
            closestObjectDistance = tempObjectDistance;
            closestObjectAngle = i;
        }
    }

    for (int l = 0; l < closestObjectAngle; l++)
    {
        printMessage("CMD_ACT_ROT_0_1");
    }

    if(closestObjectDistance = "10")
    {
        return "10";
    }

    //Reset for next movement

    closestObjectDistance = "10";
    printMessage("CMD_SEN_ROT_0");

    //Find actual closest distance

    for(int j = 10; j > -11; j--)
    {
        angleString = "CMD_SEN_ROT_" + String(j);
        printMessage(angleString);
        printMessage("CMD_SEN_IR");
        tempObjectDistance = Serial.readString();
        if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
        {
            closestObjectDistance = tempObjectDistance;
            closestObjectAngle = j;
        }
    }

    for (int m = 0; m < closestObjectAngle; m++)
    {
        printMessage("CMD_ACT_ROT_0_1");
    }

    printMessage("CMD_SEN_ROT_0");

    return closestObjectDistance;
}


void wallFollowMenu()
{
    int analogValue = 0;
    analogValue = analogRead(A0);

    if (analogValue <= 400 && analogValue >= 200) // Up
    {
        //Stop the robot
    }

    else if (analogValue <= 1000 && analogValue >= 800) // Select
    {
        delay(200);
        mainMenu();
    }
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

        delay(100);

        if(distanceFromWall.toFloat() > 2.1 && distanceFromWall.charAt(0)!='N')
        {
            distanceToTravel = distanceFromWall.toFloat() - 2;
            printMessage("CMD_ACT_LAT_1_" + String(distanceToTravel));
        }

        else if(distanceFromWall.toFloat() < 1.9 && distanceFromWall.charAt(0)!='N')
        {
            distanceToTravel = 2 - distanceFromWall.toFloat();
            printMessage("CMD_ACT_LAT_0_" + String(distanceToTravel));
        }

        else
        {
            printMessage("CMD_SEN_ROT_0");
            break;
        } 
    }
}

void moveAcrossWall()
{
    String distanceFromNextWall;

    //Turn to face next wall

    printMessage("CMD_ACT_ROT_1_270");
    printMessage("CMD_SEN_ROT_0");

    //Take actions to get to 2m from next wall

    while(true)
    {
        printMessage("CMD_SEN_IR");
        distanceFromNextWall = Serial.readString();

        delay(100);

        if(distanceFromNextWall.charAt(0)=='N')
        {
            printMessage("CMD_ACT_LAT_1_1");
            printMessage("CMD_ACT_ROT_1_90");
            printMessage("CMD_SEN_ROT_0");

            closeWallSweep();
            moveTo2m();

            printMessage("CMD_ACT_ROT_0_90");
            printMessage("CMD_SEN_ROT_0");

        }    

        else
        {
            moveTo2m();
        }
        
    }
}

void closeWallSweep()
{
    int closestObjectAngle = 0;

    String closestObjectDistance = "10";
    String tempObjectDistance;
    String angleString;

     for(int j = 10; j > -11; j--)
    {
        angleString = "CMD_SEN_ROT_" + String(j);
        printMessage(angleString);
        printMessage("CMD_SEN_IR");
        tempObjectDistance = Serial.readString();
        if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
        {
            closestObjectDistance = tempObjectDistance;
            closestObjectAngle = j;
        }
    }

    for (int m = 0; m < closestObjectAngle; m++)
    {
        printMessage("CMD_ACT_ROT_0_1");
    }

    printMessage("CMD_SEN_ROT_0");
}


void printMessage(String message)
{
    Serial.print(message);
    Serial.write(13);
    Serial.write(10);
}
