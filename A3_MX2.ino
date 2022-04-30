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
    lcd.clear();
    lcd.print("13533880");
    lcd.setCursor(0, 1);
    lcd.print("Main Menu");

    currentSelection = 0;
    int analogValue = 0;

    while (true)
    {
        delay(100);
        analogValue = analogRead(A0);

        if (analogValue <= 600 && analogValue >= 400 && currentSelection == 2)
        {
            delay(200);
            currentSelection = 0;
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
    if (currentSelection == 0)
    {
        lcd.setCursor(0, 1);
        lcd.print("Wall Follow");
    }

    else if (currentSelection == 1)
    {
        lcd.setCursor(0, 1);
        lcd.print("Control        ");
    }

    else if (currentSelection == 2)
    {
        lcd.setCursor(0, 1);
        lcd.print("Sweep         ");
    }
}

void modeChosen()
{
    printMessage("CMD_START");
    if (currentSelection == 0)
    {
        wallFollow();
    }

    else if (currentSelection == 1)
    {
        control();
    }

    else if (currentSelection == 2)
    {
        sweep();
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

void sweep()
{
    int analogValue = 0;
    int closestObjectAngleWide = 0;

    int closestObjectAngleFineStart;
    int closestObjectAngleFineEnd;
    int closestObjectAngleFine;

    String closestObjectDistance = "5";
    String tempObjectDistance;
    String angleString;

    while (true)
    {
        delay(100);
        analogValue = analogRead(A0);

        if (analogValue <= 400 && analogValue >= 200) // Up
        {
            for (int i = 360; i > -1; i = i - 10)
            {
                // Create String
                angleString = "CMD_SEN_ROT_" + String(i);
                printMessage(angleString);
                delay(50);
                printMessage("CMD_SEN_IR");
                delay(50);
                tempObjectDistance = Serial.readString();

                if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
                {
                    closestObjectDistance = tempObjectDistance;
                    closestObjectAngleWide = i;
                }

            }

            // Quadrant 1

            if (closestObjectAngleWide >= 0 && closestObjectAngleWide <= 30)
            {
                closestObjectAngleFineStart = 30;
                closestObjectAngleFineEnd = 0;
            }

            else if (closestObjectAngleWide > 30 && closestObjectAngleWide <= 60)
            {
                closestObjectAngleFineStart = 60;
                closestObjectAngleFineEnd = 30;
            }

            else if (closestObjectAngleWide > 60 && closestObjectAngleWide <= 90)
            {
                closestObjectAngleFineStart = 90;
                closestObjectAngleFineEnd = 60;
            }

            else if (closestObjectAngleWide > 90 && closestObjectAngleWide <= 120)
            {
                closestObjectAngleFineStart = 120;
                closestObjectAngleFineEnd = 90;
            }

            else if (closestObjectAngleWide > 120 && closestObjectAngleWide <= 150)
            {
                closestObjectAngleFineStart = 150;
                closestObjectAngleFineEnd = 120;
            }

            else if (closestObjectAngleWide > 150 && closestObjectAngleWide <= 180)
            {
                closestObjectAngleFineStart = 180;
                closestObjectAngleFineEnd = 150;
            }

            else if (closestObjectAngleWide > 180 && closestObjectAngleWide <= 210)
            {
                closestObjectAngleFineStart = 210;
                closestObjectAngleFineEnd = 180;
            }

            else if (closestObjectAngleWide > 210 && closestObjectAngleWide <= 240)
            {
                closestObjectAngleFineStart = 240;
                closestObjectAngleFineEnd = 210;
            }

            else if (closestObjectAngleWide > 240 && closestObjectAngleWide <= 270)
            {
                closestObjectAngleFineStart = 270;
                closestObjectAngleFineEnd = 240;
            }

            else if (closestObjectAngleWide > 270 && closestObjectAngleWide <= 300)
            {
                closestObjectAngleFineStart = 300;
                closestObjectAngleFineEnd = 270;
            }

            else if (closestObjectAngleWide > 300 && closestObjectAngleWide <= 330)
            {
                closestObjectAngleFineStart = 330;
                closestObjectAngleFineEnd = 300;
            }

            else if (closestObjectAngleWide > 330 && closestObjectAngleWide <= 360)
            {
                closestObjectAngleFineStart = 360;
                closestObjectAngleFineEnd = 330;
            }
    
            closestObjectDistance = "5";
            
            for (int j = closestObjectAngleFineStart; j >= closestObjectAngleFineEnd; j--)
            {
                
                angleString = "CMD_SEN_ROT_" + String(j);
                printMessage(angleString);
                delay(50);
                printMessage("CMD_SEN_IR");
                delay(50);
                tempObjectDistance = Serial.readString();

                if ((closestObjectDistance.toFloat()) > (tempObjectDistance.toFloat()) && tempObjectDistance.charAt(0) != 'N')
                {
                    closestObjectDistance = tempObjectDistance;
                    closestObjectAngleFine = j;
                    lcd.setCursor(0,1);
                    lcd.print("              ");
                    lcd.setCursor(0,1);
                    lcd.print(j);
                }
            }

            for(int l = 0; l < closestObjectAngleFine; l++)
            {
                printMessage("CMD_ACT_ROT_0_1");
            }
        }

        else if (analogValue <= 1000 && analogValue >= 800) // Select
        {
            delay(200);
            mainMenu();
        }
    }
}

void wallFollow()
{
}

void printMessage(String message)
{
    Serial.print(message);
    Serial.write(13);
    Serial.write(10);
}

void recieveMessage()
{
}