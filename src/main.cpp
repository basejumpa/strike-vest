//////////////////////////////////////////////////////////////////////////////
/// Strike Vest
///
/// Strike Vest © 2024 by Alexander Mann-Wahrenberg (basejumpa)
/// is licensed under [CC BY-SA 4.0].
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Dependencies (external)
///////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>

#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebSrv.h>

#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>

///////////////////////////////////////////////////////////////////////////////
/// Configuration (public)
///////////////////////////////////////////////////////////////////////////////

struct cfg_strike_vest_t {
    String ssid;
};

const cfg_strike_vest_t cfg = {
    "StrikeVest"    ///< ssid
};

///////////////////////////////////////////////////////////////////////////////
/// Interface (public)
///////////////////////////////////////////////////////////////////////////////
void setup(void);
void loop(void);

///////////////////////////////////////////////////////////////////////////////
/// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Configuration depending on public configuration
*/

/**
 * Configuration (private)
*/
#define CFG_PRV_WIFI_MODE         WIFI_MODE_AP
#define CFG_PRV_OWN_IP            4,3,2,1
#define CFG_PRV_WEB_SERVER_PORT   80
#define CFG_PRV_DNS_TTL           3600
#define CFG_PRV_DNS_PORT          53
#define CFG_PRV_DNS_DOMAIN_NAME   "*"

/**
 * Objects
*/
static DNSServer dnsServer; ///< Create a Domain Name System (DNS) Server
static AsyncWebServer webServer(CFG_PRV_WEB_SERVER_PORT); ///< Create Web Server

static std::vector<std::string> userInputList;  ///< Vector to store user input

/**
 * Entry point setup
 *
 * Called one time after system init before first call of loop()
*/
void setup(void)
{
  /// Set up a visible hotspot w/o password
  WiFi.mode(CFG_PRV_WIFI_MODE);
  WiFi.softAP(cfg.ssid);

  /// Set up the DNS server
  dnsServer.setTTL(CFG_PRV_DNS_TTL);
  dnsServer.start(CFG_PRV_DNS_PORT, CFG_PRV_DNS_DOMAIN_NAME, IPAddress(CFG_PRV_OWN_IP));

  /// Root webpage
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "";
    html += "<html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "</head><body>";
    html += "Welcome on the StrikeVest";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  webServer.begin(); ///< Start webserver
} // setup


/**
 * Entry point loop
 *
 * Being called continuously in an endless loop
*/
void loop(void) {
   dnsServer.processNextRequest(); ///< To be called about every 10ms
} // loop

///////////////////////////////////////////////////////////////////////////////
/// EOF
///////////////////////////////////////////////////////////////////////////////
