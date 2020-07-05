#include <Keypad.h>
#include <LiquidCrystal.h>

// LCD

#define LCD_ROWS 2
#define LCD_COLS 16

#define MSG_ERR_R1      "     Error!     "
#define ERROR_VISIBILITY_TIMEOUT 2000

/* Alarm menu */

#define MSG_ALRM_R2    "a     C D      B"
#define MSG_NO_ALRM_R1 "   No alarms    "
#define MSG_NO_ALRM_R2 " #: Create new  "

#define MSG_EDIT_TOOLS "#   A  B  C  D  "
#define MSG_HOUR       " Hour(0-23)    #"
#define MSG_MINUTES    " Minutes(0-59) #"
#define MSG_DISPENSER  " Dispenser     #"
#define MSG_CONFIRM    "Sure? Yes:# No:C"

/* Week menu */

#define MSG_DAY_NUM     "Day number(1-7) "
#define MSG_WEEK_TOGGLE " A - toggle     "

/* Main menu */

#define MSG_MAIN_R1     "  A: Week days  "    
#define MSG_MAIN_R2     "  B: Alarms     "

// Keypad

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

// Alarms

#define ALARMS_PER_DAY 10
#define WEEK_LENGTH 7

int i, j; //variables for loops

/* 
 * --------------------------------------------- 
*/

//
// LCD
//

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

void printToLCD(int rowID, char message[]) 
{
  lcd.setCursor(0, rowID);
      
  for(i = 0; i < LCD_COLS; i++) 
  {
    lcd.write(message[i]);
    delay(1); //TinkerCad optimization
  }
}

void printErrorToLCD(char message[]) 
{
  printToLCD(0, MSG_ERR_R1);
  printToLCD(1, message);

  delay(ERROR_VISIBILITY_TIMEOUT);
}

//
// Keypad
//

char keys[KEYPAD_ROWS][KEYPAD_COLS] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[KEYPAD_ROWS] = {9, 8, 7, 6};
byte colPins[KEYPAD_COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

//
// Days of week
//

char weekLetters[] = {'M', 'T', 'W', 'H', 'F', 'S', 'U'};

int toggleDayIndex;

void toggleWeekDayEnable() 
{
   weekLetters[toggleDayIndex] = (isupper(weekLetters[toggleDayIndex])) 
     					 		 ? tolower(weekLetters[toggleDayIndex]) 
     							 : toupper(weekLetters[toggleDayIndex]);
}

//
// Alarms
//

class TAlarm 
{
  public:
    char *day, hour, minutes, dispenserId = -1;
};

TAlarm alarms[WEEK_LENGTH * ALARMS_PER_DAY];

int firstEmptyAlarmIndex() 
{
  for(i = 0; i < sizeof(alarms)/sizeof(*alarms); i++) 
  {
    if (alarms[i].dispenserId == -1) break;
  }
  return (i == sizeof(alarms)/sizeof(*alarms)) ? -1 : i;
}

int nextAlarmIndex(int currIndex) {
  for(i = currIndex + 1; i < sizeof(alarms)/sizeof(*alarms); i++) 
  {
    if (alarms[i].dispenserId != -1) break;
  }
  return (i == sizeof(alarms)/sizeof(*alarms)) ? -1 : i;
}

int prevAlarmIndex(int currIndex) {
  for(i = currIndex - 1; i >= 0; i--) 
  {
    if (alarms[i].dispenserId != -1) break;
  }
  return i;
}

//
// Menus
//

void (*menu)(char);
bool confirm;

int alarmIndex = -1;

/* Alarm edit submenu */

char formatted[LCD_COLS + 1];

enum awaitingValue {
  awaitingNothing = -1, awaitingDay, awaitingHour, awaitingMinutes, awaitingDispenser, awaitingDeletion
};

#define AV_CONFIRM 0
#define AV_AWAITING 1
#define AV_VALUE 2

char awaitVal[3] = {false, awaitingNothing, -1};

void awaitValReset() {
  awaitVal[AV_CONFIRM] = false, awaitVal[AV_AWAITING] = awaitingNothing, awaitVal[AV_VALUE] = -1; 
}

void setEditingNum(char *buffer, char key, int row) {
  if (*buffer > -1) {
    *buffer *= 10;
    *buffer += key - '0';
    editingSubMenu('#', row);
  }
  else {
    *buffer = key - '0';
  }
}

void editingSubMenu(char key, int row) {
  switch(key) {
    case 'A': awaitVal[AV_AWAITING] = awaitingDay; printToLCD(row, MSG_DAY_NUM);
      break;
    case '1': case '2': case '3': case '4': case '5': 
    case '6': case '7': case '8': case '9': case '0':
   	  setEditingNum(&awaitVal[AV_VALUE], key, row);
      if (awaitVal[AV_AWAITING] == awaitingDay) editingSubMenu('#', row);
    
      break;
    case 'B': awaitVal[AV_AWAITING] = awaitingHour; printToLCD(row, MSG_HOUR); break;
    case 'C':
      if (awaitVal[AV_CONFIRM]) {
        editingSubMenu('*');
      } 
      else {
        awaitVal[AV_AWAITING] = awaitingMinutes;
        printToLCD(row, MSG_MINUTES);
      }
    
      break;
    case 'D': awaitVal[AV_AWAITING] = awaitingDispenser; printToLCD(row, MSG_DISPENSER); break;
    case '#':
      if (awaitVal[AV_AWAITING] == awaitingNothing) {
        awaitVal[AV_AWAITING] = awaitingDeletion;
      }
      
      if (awaitVal[AV_CONFIRM]) {
        switch(awaitVal[AV_AWAITING]) {
          case awaitingDay: alarms[alarmIndex].day = &weekLetters[awaitVal[AV_VALUE]]; break;
          case awaitingHour: alarms[alarmIndex].hour = awaitVal[AV_VALUE]; break;
          case awaitingMinutes: alarms[alarmIndex].minutes = awaitVal[AV_VALUE]; break;
          case awaitingDeletion:
          case awaitingDispenser: alarms[alarmIndex].dispenserId = awaitVal[AV_VALUE]; break;
        }
        awaitValReset();
        editingSubMenu('*', row);
      }
      else {
        awaitVal[AV_CONFIRM] = true;
        printToLCD(row, MSG_CONFIRM);
      }
    
      break;
    case '*': awaitValReset(); menu = alarmMenu; menu('~'); break;
    case '~': printToLCD(row, MSG_EDIT_TOOLS); break;
  }
}

void editingSubMenu(char key) {
  editingSubMenu(key, 1);
}

/* Alarm menu */

void printAlarmMenu() {
  if (alarmIndex < 0) 
  {
    printToLCD(0, MSG_NO_ALRM_R1);
    printToLCD(1, MSG_NO_ALRM_R2);
  }
  else 
  {
    snprintf(formatted, LCD_COLS + 1, "%02i  %c  %02i:%02i %02i ", alarmIndex, *alarms[alarmIndex].day, alarms[alarmIndex].hour, alarms[alarmIndex].minutes, alarms[alarmIndex].dispenserId);
    printToLCD(0, formatted);
    
    snprintf(formatted, LCD_COLS + 1, "%c        C     %c", (prevAlarmIndex(alarmIndex) >= 0) ? 'A' : 'a' , (alarmIndex < nextAlarmIndex(alarmIndex)) ? 'B' : 'b');
    printToLCD(1, formatted);
  }
}

void alarmMenu(char key) {
  switch(key) {
    case 'A':
      if (prevAlarmIndex(alarmIndex) >= 0) {
        alarmIndex--; 
        printAlarmMenu(); 
      } 
    
      break; 
    case 'B': 
      if (alarmIndex < nextAlarmIndex(alarmIndex)) { 
        alarmIndex++; 
        printAlarmMenu(); 
      } 
    
      break;
    case 'C': menu = editingSubMenu; menu('~'); break;
    case '#': 
      alarmIndex = firstEmptyAlarmIndex();
      menu = editingSubMenu; 
      alarmMenu('~'); menu('~');
      break;
    case '*': alarmIndex = 0; menu = mainMenu; menu('~'); break;
    case '~': printAlarmMenu(); break;
  }
}

/* Week menu */

bool waitForDayNum;

void printWeekMenu() 
{
  sprintf(formatted," %c %c %c %c %c  %c %c ", weekLetters[0], weekLetters[1], weekLetters[2], weekLetters[3], weekLetters[4], weekLetters[5], weekLetters[6], weekLetters[7]);
  printToLCD(0, formatted);
  
  if (waitForDayNum) printToLCD(1, MSG_DAY_NUM);
  else if (confirm) printToLCD(1, MSG_CONFIRM);
  else printToLCD(1, MSG_WEEK_TOGGLE);
}



void weekMenu(char key) 
{  
  switch(key)
  {
    case 'A': waitForDayNum = true; weekMenu('~'); break;
    case 'C': confirm = false; weekMenu('~'); break;
    case '#': 
      if (confirm) 
      {
        toggleWeekDayEnable();
      	confirm = false;
      }  
      weekMenu('~');
    
      break;
    case '1': case '2': case '3': case '4': case '5': case '6':
    case '7': 
   	  if (waitForDayNum) 
      {
      	toggleDayIndex = (key - '0') - 1; 
        waitForDayNum = false; confirm = true; 
      } 
      weekMenu('~'); 
    
      break;
    case '*': menu = mainMenu; menu('~'); break;
    case '~': printWeekMenu();  break;
  }
}

/* Main menu */

void mainMenu(char key) 
{  
  switch(key)
  {
    case 'A': menu = weekMenu; weekMenu('~'); break;
    case 'B': menu = alarmMenu; alarmMenu('~'); break;
    default: printToLCD(0, MSG_MAIN_R1); printToLCD(1, MSG_MAIN_R2); break;
  }
}

//
// Main logic
//

void setup()
{
  lcd.begin(LCD_COLS, LCD_ROWS);

  menu = mainMenu;
  mainMenu('~');
}

char key;
void loop()
{
  key = keypad.getKey(); // getKey function is too quick, once you get the key, the second time you get it it will return null
  if (key != '\0') 
  {
    menu(key);
  }
  delay(1); //TinkerCad optimization
}
