#include <stdio.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup()
{
    lcd.begin(16, 2);
    lcd.print("13533880");
    lcd.setCursor(0, 1);
    lcd.print("Main Menu");
    Serial.begin(9600);
}

void loop()
{
    mainMenu();
}

void mainMenu()
{
    int currentSelection = 0;
    int analogValue = 0;

    while (true)
    {
        analogValue = analogRead(A0);

        if (analogValue <= 600 && analogValue >= 400 && currentSelection == 2)
        {
            delay(200);
            currentSelection = 0;
            setMenuSelection(currentSelection);
        }

        else if (analogValue <= 600 && analogValue >= 400)
        {
            delay(200);
            currentSelection++;
            setMenuSelection(currentSelection);
        }

        else if(analogValue >= 800 && analogValue <= 900)
        {
            delay(200);
            modeChosen(currentSelection);
        }
    }
}

void setMenuSelection(int currentSelection)
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

void modeChosen(int currentSelection)
{
    printMessage("CMD_START");
    if(currentSelection == 0)
    {
        wallFollow();
    }

    else if(currentSelection == 1)
    {
        control();
    }

    else if(currentSelection == 2)
    {
        sweep();
    }
}

void control()
{
    int analogValue = 0;

    while(true)
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
    int closestObjectAngle = 0;
    String closestObjectDistance = "0";
    String tempObjectDistance = "0";
    String angleString;

    while(true)
    {
        analogValue = analogRead(A0);

        if(analogValue <= 400 && analogValue >= 200) // Up
        {
            delay(200);
            for(int i = 360; i>0; i--)
            {
                //Create String
                angleString = "CMD_SEN_ROT_" + i;
                printMessage(angleString);
                printMessage("CMD_SEN_IR");
                tempObjectDistance = Serial.readString();
                
                if(i == 0)
                {
                    closestObjectDistance = tempObjectDistance;
                }

                else if(closestObjectDistance>tempObjectDistance)
                {
                    closestObjectDistance = tempObjectDistance;
                    closestObjectAngle = i;
                }
            }

            closestObjectAngle = 360 - closestObjectAngle ;
            angleString = "CMD_ACT_ROT_0_" + closestObjectAngle;
            printMessage(angleString);
        }

        else if (analogValue <= 1000 && analogValue >= 800) // Select
        {
            delay(200);
            lcd.setCursor(0,1);
            lcd.print(closestObjectDistance);
            delay(50);
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