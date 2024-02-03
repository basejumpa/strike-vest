//////////////////////////////////////////////////////////////////////////////
/// Strike Vest
///
/// Strike Vest © 2024 by Alexander Mann-Wahrenberg (basejumpa)
/// is licensed under [CC BY-SA 4.0].
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Dependencies to standard C++ libaries
///////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
/// Dependencies (external) (outside from project)
///////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebSrv.h>

///////////////////////////////////////////////////////////////////////////////
/// Dependencies (inside the same project)
///////////////////////////////////////////////////////////////////////////////

#include "scrolltext.h"

///////////////////////////////////////////////////////////////////////////////
/// Configuration (public)
///////////////////////////////////////////////////////////////////////////////

struct cfg_strike_vest_t {
    String ssid;                        ///< WiFi SSID
    String passphrase;                  ///< WiFi passphrase
    unsigned long updateInterval;       ///< Display interval in milliseconds
};

const cfg_strike_vest_t cfg = {
    "StrikeVest",    ///< ssid
    "helau",         ///< WiFi password
    2500             ///< // Update interval in milliseconds
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
#define CFG_PRV_SUBNET_MASK       255, 255, 255, 0
#define CFG_PRV_OWN_URL           "http://4.3.2.1"
#define CFG_PRV_WEB_SERVER_PORT   80
#define CFG_PRV_DNS_TTL           3600
#define CFG_PRV_DNS_PORT          53
#define CFG_PRV_DNS_DOMAIN_NAME   "*"

/**
 * Private objects
*/
static DNSServer dnsServer; ///< Create a Domain Name System (DNS) Server
static AsyncWebServer webServer(CFG_PRV_WEB_SERVER_PORT); ///< Create Web Server
static unsigned long states_lastUpdateTime = 0;         ///< state variable

/**
 * Create the routes for creating a captive portal.
*/
static void addRoutesForCaptivePortal(){
  webServer.on("/connecttest.txt", [](AsyncWebServerRequest *request) {
    request->redirect("http://logout.net");
  });  // windows 11 captive portal workaround
  webServer.on("/wpad.dat", [](AsyncWebServerRequest *request) {
    request->send(404);
  });  // Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

  // Background responses: Probably not all are Required, but some are. Others might speed things up?
  // A Tier (commonly used by modern systems)
  webServer.on("/generate_204", [](AsyncWebServerRequest *request) {
    request->redirect(CFG_PRV_OWN_URL);
  });  // android captive portal redirect
  webServer.on("/redirect", [](AsyncWebServerRequest *request) {
    request->redirect(CFG_PRV_OWN_URL);
  });  // microsoft redirect
  webServer.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) {
    request->redirect(CFG_PRV_OWN_URL);
  });  // apple call home
  webServer.on("/canonical.html", [](AsyncWebServerRequest *request) {
    request->redirect(CFG_PRV_OWN_URL);
  });  // firefox captive portal call home
  webServer.on("/success.txt", [](AsyncWebServerRequest *request) {
    request->send(200);
  });  // firefox captive portal call home
  webServer.on("/ncsi.txt", [](AsyncWebServerRequest *request) {
    request->redirect(CFG_PRV_OWN_URL);
  });  // windows call home
  webServer.on("/favicon.ico", [](AsyncWebServerRequest *request) {
    request->send(404);
  });  // webpage icon
} // addRoutesForCaptivePortal


/**
 * Entry point setup
 *
 * Called one time after system init before first call of loop()
*/
void setup(void)
{
  /// Set up a visible hotspot w/o password
  WiFi.mode(CFG_PRV_WIFI_MODE);
  /// Configure the soft access point with a specific IP and subnet mask. Gateway is the ESP32 itself
  WiFi.softAPConfig(IPAddress(CFG_PRV_OWN_IP), IPAddress(CFG_PRV_OWN_IP), IPAddress(CFG_PRV_SUBNET_MASK));
  WiFi.softAP(cfg.ssid, cfg.passphrase);

  /// Set up the DNS server
  dnsServer.setTTL(CFG_PRV_DNS_TTL);
  dnsServer.start(CFG_PRV_DNS_PORT, CFG_PRV_DNS_DOMAIN_NAME, IPAddress(CFG_PRV_OWN_IP));

  // addRoutesForCaptivePortal();

  /// Root webpage

  scrollText_setup();

  webServer.begin(); ///< Start webserver

} // setup


/**
 * Entry point loop
 *
 * Being called continuously in an endless loop
*/
void loop(void) {
  if (millis() - states_lastUpdateTime >= cfg.updateInterval) {
    // Remove the oldest entry from the list
    // Update the last update time
    states_lastUpdateTime = millis();
  }

   dnsServer.processNextRequest(); ///< To be called about every 10ms

   scrollText_loop();
} // loop

///////////////////////////////////////////////////////////////////////////////
/// EOF
///////////////////////////////////////////////////////////////////////////////
