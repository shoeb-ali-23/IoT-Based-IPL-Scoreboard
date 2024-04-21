#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C

#define batLOGO_HEIGHT   14
#define batLOGO_WIDTH    14
#define ballLOGO_HEIGHT   16
#define ballLOGO_WIDTH    16


static const unsigned char PROGMEM logo_bmp[] = {
0b01100000,	0b00000000,
0b01110000,	0b00000000,
0b00111000,	0b00000000,
0b00011111,	0b00000000,
0b00001111,	0b10000000,
0b00001111,	0b11000000,
0b00001111,	0b11100000,
0b00000111,	0b11110000,
0b00000011,	0b11111000,
0b00000001,	0b11111100,
0b00000000,	0b11111100,
0b00000000,	0b01111110,
0b00000000,	0b00111100,
0b00000000,	0b00000000,
0b00000000,	0b00000000,
0b00000000,	0b00000000,
0b00000011,	0b11000000,
0b00001111,	0b11110000,
0b00011111,	0b10011000,
0b00111111,	0b00111100,
0b00111110,	0b01110100,
0b01111100,	0b11100110,
0b01111001,	0b11001110,
0b01110011,	0b10011110,
0b01100111,	0b00111110,
0b00101110,	0b01111100,
0b00111100,	0b11111100,
0b00011001,	0b11111000,
0b00001111,	0b11110000,
0b00000011,	0b11000000,
0b00000000,	0b00000000,
0b00000000,	0b00000000
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


const char* targetSeriesID = "76ae85e2-88e5-4e99-83e4-5f352108aebc";  
const char *host = "api.thingspeak.com";
const int httpPort = 80;  
const char* url1 = "/apps/thinghttp/send_request?api_key=M4JG9NK73EXNWJ1I"; 
HTTPClient http; 
int scrollTextLenth = 300;  // scroll text length in px

unsigned long previousMillis = 0;   // Variable to store the previous time
unsigned long previousMillis2 = 0;
unsigned long currentMillis;
const long interval = 5000;
bool scrollTask = false;
static int16_t scrollStep = 2;
static int16_t scrollPos = SCREEN_WIDTH;


String name ; 
String status; 
String venue; 
String scoresz; 
int scoreSize; 
String Team1; 
String Team2;
String short_Team1; 
String short_Team2; 
JsonObject scoreObject1; 
int runs1; 
int wickets1; 
String overs1; 
String inning1;
JsonObject TeamObject1; 
String Team1name;      
String Team1Shortname; 
int Short_team_index1; 
String inning1_team_name; 
String inning1_short_team_name; 
JsonObject scoreObject2;
int runs2; 
int wickets2;
String overs2; 
String inning2; 
JsonObject TeamObject2;
String Team2name;      
String Team2Shortname; 
int Short_team_index2; 
String inning2_team_name; 
String inning2_short_team_name;
int statusLen; 

String Response_data;
DeserializationError error;

JsonObject root;
JsonArray data;
bool getData = true;
JsonDocument doc; 
void setup() 
{
  Serial.begin(9600);
  WiFi.begin("SSID", "PASSWORD");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
    Serial.println("STARTED");  
  display.clearDisplay();

  
}


void loop() {
  if(getData == true){
    doc = post_parse_data();
    error = deserializeJson(doc, Response_data);
    root = doc.as<JsonObject>();
    data = root["data"].as<JsonArray>();
    getData = false;
  }

    for (JsonObject match : data) {
      if (match["series_id"] == targetSeriesID && match["matchEnded"] == false && match["matchStarted"] == true) {
          name = match["name"].as<String>();        
          status = match["status"].as<String>();    
          venue = match["venue"].as<String>();
          scoresz = match["score"].as<String>();
          scoreSize = scoresz.length();
          Team1 = match["teams"][0].as<String>();
          Team2 = match["teams"][1].as<String>();          
          short_Team1 = getTeamShortName(Team1);
          short_Team2 = getTeamShortName(Team2);

          scoreObject1 = match["score"][0].as<JsonObject>();
          runs1 = scoreObject1["r"].as<int>();  
          wickets1 = scoreObject1["w"].as<int>();  
          overs1 = scoreObject1["o"].as<String>();  
          inning1 = scoreObject1["inning"].as<String>();
          TeamObject1 = match["teamInfo"][0].as<JsonObject>();
          Team1name      = TeamObject1["name"].as<String>();
          Team1Shortname = TeamObject1["shortname"].as<String>();
          Short_team_index1 = inning1.indexOf("Inning 1");
          inning1_team_name = inning1.substring(0,Short_team_index1);
          inning1_short_team_name = getTeamShortName(inning1_team_name);

          scoreObject2 = match["score"][1].as<JsonObject>();
          runs2 = scoreObject2["r"].as<int>();  
          wickets2 = scoreObject2["w"].as<int>(); 
          overs2 = scoreObject2["o"].as<String>();  
          inning2 = scoreObject2["inning"].as<String>();
          TeamObject2 = match["teamInfo"][1].as<JsonObject>();      
          Team2name      = TeamObject1["name"].as<String>();
          Team2Shortname = TeamObject1["shortname"].as<String>();
          Short_team_index2 = inning2.indexOf("Inning 1");
          inning2_team_name = inning2.substring(0,Short_team_index2);
          inning2_short_team_name = getTeamShortName(inning2_team_name);
          
          statusLen = status.length();
      }
    }  


currentMillis = millis(); 

while(currentMillis - previousMillis <= 10000 && currentMillis - previousMillis > 0 && scrollTask == false){
          currentMillis = millis(); 

          if(runs2==0){   
          display.clearDisplay();  
          display.setTextColor(WHITE);   
          display.setTextSize(1);
          display.setCursor(18, 5); 
          display.print(inning1_short_team_name);
          display.setCursor(18, 21); 

            if(inning1_short_team_name == short_Team1){
               display.print(short_Team2);
            }
            if(inning1_short_team_name == short_Team2){
               display.print(short_Team1);
            }                        
          
          display.setTextSize(2);
          display.setCursor(46,1); 
          display.print(runs1);
          display.setCursor(76,1); 
          display.print("/");
          display.setCursor(89,1 );      
          display.print(wickets1);
          display.setCursor(46,16); 
          display.print(overs1);
          showicon(1, 1, logo_bmp, 16, 32);
          display.display();                           
          }

          else{                
          display.clearDisplay();  
          display.setTextColor(WHITE);   
          display.setTextSize(1);
          display.setCursor(18, 5); 
          display.print(inning2_short_team_name);
          display.setCursor(18, 21); 
          display.print(inning1_short_team_name);
          display.setCursor(95,21); 
          display.print("(");
          display.print(runs1);
          display.print(")");
          display.setTextSize(2);
          display.setCursor(46,1 ); 
          display.print(runs2);
          display.print("/");
          display.print(wickets2);
          display.setCursor(46,16); 
          display.print(overs2);
          showicon(1, 1, logo_bmp, 16, 32);
          display.display();                 
          }
          if(currentMillis - previousMillis > 9800){
            display.clearDisplay();
            break;
          }
}



while(currentMillis - previousMillis < 12000 && currentMillis - previousMillis > 10000 && scrollTask == false) {
          currentMillis = millis();      
          if(runs2==0){    
              display.setTextColor(WHITE);   
              display.setTextSize(1);
              display.setCursor(1, 1); 
              display.print(inning1_short_team_name);
              display.setCursor(102, 1); 
              display.print(inning2_short_team_name);
              display.setTextSize(1);
              display.setCursor(25, 1); 
              display.print(runs1);
              display.print("/");
              display.print(wickets1);
              display.setCursor(75, 1); 
              display.print(overs1);  
              display.setCursor(63, 1); 
              display.print("|");    
              display.display();
          }

          else{        
          display.setTextColor(WHITE);   
          display.setTextSize(1);
          display.setCursor(1, 1); 
          display.print(inning2_short_team_name);
          display.setCursor(102, 1); 
          display.print(inning1_short_team_name);
          display.setTextSize(1);
          display.setCursor(25, 1); 
          display.print(runs2);
          display.print("/");
          display.print(wickets2);
          display.setCursor(75, 1); 
          display.print(overs2);
          display.setCursor(63, 1); 
          display.print("|");    
          display.display();
          }
          if(currentMillis - previousMillis >= 11500){
            scrollTask = true;
            break;
          }        
        }

while(currentMillis - previousMillis >= 11000 && currentMillis - previousMillis <= 20000 && scrollTask == true) {
       currentMillis = millis(); 
       if(currentMillis - previousMillis2 >=38 && scrollTask == true) {          
          display.setTextSize(1);      
          display.setTextColor(SSD1306_BLACK); 
          display.setCursor(scrollPos, 24); 
          display.println(status);

          scrollPos -= scrollStep;
          
          if (scrollPos < - statusLen*7) 
             {
              scrollPos = SCREEN_WIDTH;
            }

          display.setTextSize(1);      
          display.setTextColor(SSD1306_WHITE); 
          display.setCursor(scrollPos, 24); 
          display.println(status);
          display.display(); 

          previousMillis2 = currentMillis;
         }          
        if(currentMillis - previousMillis >= 20000){
            scrollPos = SCREEN_WIDTH;
            getData = true;
            scrollTask = false;
            previousMillis = currentMillis;
            break;
          }
} 
}



String sendRequest(const char *host, const int httpPort,const char* url1){
    http.begin(host,httpPort,url1); 
    int httpCode = http.GET();
    String data = http.getString();
    return data;
}

String getTeamShortName(String teamName) {
  if (teamName.isEmpty()) {
    return "";
  }

  String shortName = "";
  char prevChar = ' ';

  for (int i = 0; i < teamName.length(); i++) {
    char currentChar = teamName.charAt(i);

    if (isupper(currentChar)==true && !isalpha(prevChar)) {
      shortName += currentChar;
    }
    prevChar = currentChar;
  }

  return shortName;
}



void showicon(int X, int Y, const unsigned char *logo_bmp, int LOGO_WIDTH, int LOGO_HEIGHT)
{
  display.drawBitmap(X, Y, logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}




String post_parse_data(){
  Response_data = sendRequest(host, httpPort, url1);
  return Response_data;
}


