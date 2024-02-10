#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebSrv.h>
#include <vector>

#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>

#define PIN 16


const char *predefinedEntries__umzug_bischem_umzug_kostheim[] = {
  "WiFI **  Streikender-47 **  VERBINDE DICH **",
  "Streik ** Streik ** Streik ++ ",
  "Wo bleibt der Zug?",
  "Immer zu spaet kommen ist doch zuverlaessig!",
  "Wann kommt der Zug? ** Kann nicht mehr lange dauern, die Schienen liegen schon"
  "Helau!",
  "Bischum, helau!",
  "Mehr Mett!",
  "Ohne Mett kein Streik!",
  "Fahrplan? -- Nein, Unverbindliche Abfahrtsempfehlung mit Gleisvorschlag --",
  "35 Stunden bei gleichem Mett!",
  "ICE Frankfurt -> Mainz: Klo kaputt, bitte einhalten.",
  "S8 faellt aus. Grund: Zu kalt.",
  "RB75 heute 2900 Minuten spaeter",
};

const char *predefinedEntries[] = {
  "WiFI ++  Streikender-47 ++  VERBINDE DICH ++",
  "Club-11-Party!! ++ Club-11-Party ++ Club-11-Party ++",
  "Helau! ++ Helau! ++",
  "Humba - Humba - Humba - Humba - ",
  "Der Meenzer Fassenacht ein 3faches Helau - Helau - Helau"
  "Die Schwarzen Husaren rocken ab ++ Schwarzen Husaren rocken ab ++",
  "Jammin'Cool ++ Jammin'Cool ++",
  "JUMP! ++ JUMP! ++ JUMP ++",
  "Are you ready to party?!",
  "Let's make some noise for Meenz!",
  "Put your hands up in the air!",
  "I wanna see you dance!",
  "Are you feeling the beat?!",
  "Who's ready to have the time of their lives?!",
  "Shout out to all the party animals in the house!",
  "Let's turn this place into a dancefloor!",
  "Are you ready to feel the bass?!",
  "Make some noise if you're having a good time!",
  "++ __ ++ __ ++ __ ++ __",
  "? ! ? !",
  ":-)   :-)  :-)  :-)  :-)  ",
  ":-()  :-()  :-()  :-()  :-()  ",
  "=>  =>  =>  =>  =>  "
};

#define NUM_ENTRIES (sizeof(predefinedEntries) / sizeof(predefinedEntries[0]))

#define tile_w (32)
#define tile_h  (8)

#define tiles_num_x (1)
#define tiles_num_y (1)

#define mw (tile_w * tiles_num_x)
#define mh (tile_h * tiles_num_y)
#define NUMMATRIX (mw * mh)

void scrollText(const char *text, int delayTime);
void sendTwitterRequest();

CRGB matrixleds[NUMMATRIX];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(
  matrixleds,   // reference to 1-dim array of type CRGB, each element is a LED.
  mw,           // matrix width
  mh,           // matrix height,
  tiles_num_x,  // number of tiles in x direction
  tiles_num_y,  // number of tiles in y direction
  0
    // layout of a single tile
    + NEO_MATRIX_TOP
    + NEO_MATRIX_LEFT
    + NEO_MATRIX_COLUMNS
    + NEO_MATRIX_ZIGZAG

    // layout of the tiles
    + NEO_TILE_TOP
    + NEO_TILE_LEFT
    + NEO_TILE_ROWS
  );

const char *ssid = "Streikender-";
const char *password = NULL;  // no password

#define MAX_CLIENTS 8   // ESP32 supports up to 10 but I have not tested it yet
#define WIFI_CHANNEL 6  // 2.4ghz channel 6 https://en.wikipedia.org/wiki/List_of_WLAN_channels#2.4_GHz_(802.11b/g/n/ax)

#define DNS_INTERVAL 30

const IPAddress localIP(4, 3, 2, 1);           // the IP address the web server, Samsung requires the IP to be in public space
const IPAddress gatewayIP(4, 3, 2, 1);         // IP address of the network should be the same as the local IP for captive portals
const IPAddress subnetMask(255, 255, 255, 0);  // no need to change: https://avinetworks.com/glossary/subnet-mask/

const String localIPURL = "http://4.3.2.1";  // a string version of the local IP with http, used for redirecting clients to your webpage

DNSServer dnsServer;
AsyncWebServer server(80);

std::vector<String> userInputList;  // Vector to store user input
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 2500;  // Update every 1000 milliseconds (1 second)




String scrollText_text = "";
int16_t scrollText_x = mw;
unsigned long scrollText_lastUpdateTime = 0;
const unsigned long scrollText_updateInterval = 50;
int scrollText_times_to_show = 0;
void scrollText_set(String text, int times);
void scrollText_loop();

void scrollText_set(String text, int times) {
  scrollText_text = text;
  scrollText_times_to_show = times;
}

bool scrollText_finished() {
  return (scrollText_times_to_show == 0);
}

void scrollText_update() {
  if (userInputList.size() > 1)
    userInputList.erase(userInputList.begin());

  scrollText_set(userInputList.front(), 1);
}

void scrollText_loop() {
  if (millis() - scrollText_lastUpdateTime >= scrollText_updateInterval) {

    if (scrollText_finished()) {
      scrollText_update();
    }

    // matrix->setTextColor(matrix->Color(255, 0, 0));      // red
    matrix->setTextColor(matrix->Color(255, 255, 255));  // white

    // Calculate text width to scroll the entire text
    uint16_t textWidth, textHeight;
    int16_t x1, y1;
    matrix->getTextBounds(scrollText_text, 0, 0, &x1, &y1, &textWidth, &textHeight);

    if (scrollText_x >= -textWidth) {
    } else {
      scrollText_x = mw;
      scrollText_times_to_show -= (scrollText_times_to_show > 0 ? 1 : 0);
    }
    matrix->fillScreen(matrix->Color(0, 0, 0));  // Clear the matrix
    matrix->setCursor(scrollText_x, 0);          // Set text starting position
    matrix->print(scrollText_text);              // Print the text at the current position
    matrix->show();                              // Show the updated matrix

    scrollText_x--;

    scrollText_lastUpdateTime = millis();
  }
}


void setup(void) {
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, PIN>(matrixleds, NUMMATRIX);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(40);

  // Set the WiFi mode to access point and station
  WiFi.mode(WIFI_MODE_AP);

  // Define the subnet mask for the WiFi network
  const IPAddress subnetMask(255, 255, 255, 0);

  // Configure the soft access point with a specific IP and subnet mask
  WiFi.softAPConfig(localIP, gatewayIP, subnetMask);

  // Start the soft access point with the given ssid, password, channel, max number of clients
  // Get the MAC address
  String macAddress = WiFi.macAddress();
  Serial.println("Full MAC Address: " + macAddress);

  // Get the last two characters
  String lastTwoCharacters = String("47");

  WiFi.softAP(ssid + lastTwoCharacters, password, WIFI_CHANNEL, 0, MAX_CLIENTS);

  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  dnsServer.setTTL(3600);
  dnsServer.start(53, "*", localIP);

  // Required
  server.on("/connecttest.txt", [](AsyncWebServerRequest *request) {
    request->redirect("http://logout.net");
  });  // windows 11 captive portal workaround
  server.on("/wpad.dat", [](AsyncWebServerRequest *request) {
    request->send(404);
  });  // Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

  // Background responses: Probably not all are Required, but some are. Others might speed things up?
  // A Tier (commonly used by modern systems)
  server.on("/generate_204", [](AsyncWebServerRequest *request) {
    request->redirect(localIPURL);
  });  // android captive portal redirect
  server.on("/redirect", [](AsyncWebServerRequest *request) {
    request->redirect(localIPURL);
  });  // microsoft redirect
  server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) {
    request->redirect(localIPURL);
  });  // apple call home
  server.on("/canonical.html", [](AsyncWebServerRequest *request) {
    request->redirect(localIPURL);
  });  // firefox captive portal call home
  server.on("/success.txt", [](AsyncWebServerRequest *request) {
    request->send(200);
  });  // firefox captive portal call home
  server.on("/ncsi.txt", [](AsyncWebServerRequest *request) {
    request->redirect(localIPURL);
  });  // windows call home

  // B Tier (uncommon)
  //  server.on("/chrome-variations/seed",[](AsyncWebServerRequest *request){request->send(200);}); //chrome captive portal call home
  //  server.on("/service/update2/json",[](AsyncWebServerRequest *request){request->send(200);}); //firefox?
  //  server.on("/chat",[](AsyncWebServerRequest *request){request->send(404);}); //No stop asking Whatsapp, there is no internet connection
  //  server.on("/startpage",[](AsyncWebServerRequest *request){request->redirect(localIPURL);});

  // return 404 to webpage icon
  server.on("/favicon.ico", [](AsyncWebServerRequest *request) {
    request->send(404);
  });  // webpage icon

  userInputList.push_back(predefinedEntries[0]);

  // Define the captive portal route
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "";
    html += "<html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "</head><body>";

    // Form with buttons to append predefined entries
    html += "<form action='/send' method='post'>";
    for (int i = 0; i < NUM_ENTRIES; ++i) {
      html += "<button name=\"button\" value=\"" + String(i) + "\" type=\"submit\">" + String(predefinedEntries[i]) + "</button><br>";
    }
    html += "</form>";

    // Display the current list of user inputs
    html += "<ul>";
    for (const String &input : userInputList) {
      html += "<li>" + input + "</li>";
    }
    html += "</ul>";

    // JavaScript to reload the page every 2 seconds
    html += "<script>setTimeout(function(){ location.reload(); }, 2000);</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Define the route to handle form submission
  server.on("/send", HTTP_POST, [](AsyncWebServerRequest *request) {
    // Check which button was pressed
    if (request->hasParam("button", true)) {
      // Append the corresponding predefined entry to the list
      int i = request->getParam("button", true)->value().toInt();
      userInputList.push_back(predefinedEntries[i]);
    }
    // Redirect back to the root after submission
    request->redirect("/");
  });

  // Start server
  server.begin();

#if 0
  matrix->clear();
  matrix->drawPixel(0,       0, CRGB( 255, 0,   0));    // red    links oben
  matrix->drawPixel(mw-1,    0, CRGB( 0,   0,   255));  // blue   rechts oben
  matrix->drawPixel(0,    mh-1, CRGB( 0,   255, 0));    // green  links unten
  matrix->drawPixel(mw-1, mh-1, CRGB( 255, 255, 0));    // yellow rechts unten
  matrix->show();
#endif // 0
}



void loop() {
  // Check if it's time to update the list
  if (millis() - lastUpdateTime >= updateInterval) {
    // Remove the oldest entry from the list
    // Update the last update time
    lastUpdateTime = millis();
  }

  dnsServer.processNextRequest();  // I call this atleast every 10ms in my other projects (can be higher but I haven't tested it for stability)
  delay(DNS_INTERVAL);             // seems to help with stability, if you are doing other things in the loop this may not be needed

  scrollText_loop();
}
