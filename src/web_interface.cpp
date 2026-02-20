#include <WebServer.h>
#include "web_interface.h"
#include "definitions.h"
#include "deauth.h"

WebServer server(80);
int num_networks = 0;

String getEncryptionType(wifi_auth_mode_t encryptionType);

void redirect_root() {
  server.sendHeader("Location", "/");
  server.send(301);
}

void handle_root() {
  num_networks = WiFi.scanNetworks();
  
  String html = "<!DOCTYPE html><html lang=\"en\"><head>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<title>ESP32-Deauther</title>";
  html += "<style>";
  html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
  html += "body { font-family: 'Segoe UI', sans-serif; background: linear-gradient(135deg, #1a1a2e, #16213e); min-height: 100vh; color: #e0e0e0; padding: 15px; }";
  html += ".container { max-width: 800px; margin: 0 auto; }";
  html += "h1 { text-align: center; color: #00ff88; font-size: 2em; margin-bottom: 20px; }";
  html += "h2 { color: #00d4ff; margin-bottom: 10px; border-bottom: 2px solid #00d4ff; padding-bottom: 8px; }";
  html += ".card { background: rgba(255,255,255,0.05); border-radius: 12px; padding: 20px; margin-bottom: 20px; }";
  html += "table { width: 100%; border-collapse: collapse; margin-bottom: 15px; background: rgba(0,0,0,0.3); border-radius: 8px; }";
  html += "th, td { padding: 10px 8px; text-align: left; border-bottom: 1px solid rgba(255,255,255,0.1); }";
  html += "th { background: linear-gradient(90deg, #00d4ff, #00ff88); color: #1a1a2e; font-weight: bold; font-size: 0.9em; }";
  html += "tr:hover { background: rgba(0,212,255,0.1); }";
  html += ".btn { padding: 10px 20px; border: none; border-radius: 6px; cursor: pointer; font-size: 14px; font-weight: bold; text-transform: uppercase; }";
  html += ".btn-primary { background: linear-gradient(90deg, #00d4ff, #0099ff); color: white; }";
  html += ".btn-success { background: linear-gradient(90deg, #00ff88, #00cc6a); color: #1a1a2e; }";
  html += ".btn-danger { background: linear-gradient(90deg, #ff4757, #ff6b81); color: white; }";
  html += ".btn-warning { background: linear-gradient(90deg, #ffa502, #ff7f50); color: white; }";
  html += "input[type=checkbox] { width: 18px; height: 18px; cursor: pointer; }";
  html += ".stats { display: flex; gap: 15px; margin-bottom: 20px; }";
  html += ".stat-box { flex: 1; background: rgba(0,0,0,0.3); padding: 15px; border-radius: 8px; text-align: center; }";
  html += ".stat-value { font-size: 1.8em; font-weight: bold; color: #00ff88; }";
  html += ".stat-label { color: #888; font-size: 0.85em; }";
  html += ".selected-count { background: #00ff88; color: #1a1a2e; padding: 2px 8px; border-radius: 10px; font-size: 0.8em; }";
  html += "</style>";
  html += "<script>";
  html += "function selectAll(source) { checkboxes = document.querySelectorAll('.net-checkbox'); for(var i=0, n=checkboxes.length;i<n;i++) checkboxes[i].checked = source.checked; updateCount(); }";
  html += "function updateCount() { checked = document.querySelectorAll('.net-checkbox:checked').length; document.getElementById('selCount').innerText = checked; }";
  html += "</script>";
  html += "</head><body>";
  html += "<div class=\"container\">";
  html += "<h1>ESP32-Deauther</h1>";
  
  html += "<div class=\"stats\">";
  html += "<div class=\"stat-box\"><div class=\"stat-value\">" + String(num_networks) + "</div><div class=\"stat-label\">Networks</div></div>";
  html += "<div class=\"stat-box\"><div class=\"stat-value\">" + String(eliminated_stations) + "</div><div class=\"stat-label\">Packets</div></div>";
  html += "</div>";

  html += "<div class=\"card\"><h2>Select Networks</h2>";
  html += "<form method=\"post\" action=\"/attack\">";
  html += "<table><tr><th><input type=\"checkbox\" onchange=\"selectAll(this)\"></th><th>#</th><th>SSID</th><th>Ch</th><th>RSSI</th><th>Enc</th></tr>";
  for (int i = 0; i < num_networks; i++) {
    String enc = getEncryptionType(WiFi.encryptionType(i));
    html += "<tr><td><input type=\"checkbox\" name=\"net\" value=\"" + String(i) + "\" class=\"net-checkbox\" onchange=\"updateCount()\"></td>";
    html += "<td>" + String(i) + "</td><td>" + WiFi.SSID(i) + "</td><td>" + String(WiFi.channel(i)) + "</td><td>" + String(WiFi.RSSI(i)) + "</td><td>" + enc + "</td></tr>";
  }
  html += "</table>";
  html += "<p>Selected: <span id=\"selCount\" class=\"selected-count\">0</span></p></div>";

  html += "<div class=\"card\"><h2>Attack Options</h2>";
  html += "<button type=\"submit\" name=\"attack_type\" value=\"single\" class=\"btn btn-primary\">Attack Selected</button>";
  html += "<button type=\"submit\" name=\"attack_type\" value=\"multi\" class=\"btn btn-warning\" style=\"margin-left:10px;\">Attack ALL Networks</button>";
  html += "</form>";
  
  html += "<hr style=\"border-color:rgba(255,255,255,0.1);margin:20px 0;\">";
  
  html += "<form method=\"post\" action=\"/broadcast\">";
  html += "<button type=\"submit\" class=\"btn btn-danger\">Broadcast Attack (All Clients)</button>";
  html += "</form>";
  
  html += "<hr style=\"border-color:rgba(255,255,255,0.1);margin:20px 0;\">";
  
  html += "<form method=\"post\" action=\"/stop\">";
  html += "<button type=\"submit\" class=\"btn btn-success\">Stop All Attacks</button>";
  html += "</form></div></div></body></html>";

  server.send(200, "text/html", html);
}

void handle_attack() {
  String attack_type = server.arg("attack_type");

  String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><meta http-equiv=\"refresh\" content=\"3;url=/\"><title>Attack</title>";
  html += "<style>body{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#1a1a2e,#16213e);min-height:100vh;display:flex;justify-content:center;align-items:center;color:#e0e0e0;}.card{background:rgba(255,255,255,0.05);border-radius:20px;padding:40px;text-align:center;}.card h1{color:#00ff88;}</style></head><body>";
  html += "<div class=\"card\"><h1>Attack Started!</h1></div></body></html>";

  if (attack_type == "single") {
    String nets = server.arg("net");
    if (nets.length() > 0) {
      start_deauth(0, DEAUTH_TYPE_SINGLE, 1);
    }
  } else if (attack_type == "multi") {
    start_deauth(0, DEAUTH_TYPE_MULTI, 1);
  }
  
  server.send(200, "text/html", html);
}

void handle_broadcast() {
  String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Broadcast</title>";
  html += "<style>body{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#1a1a2e,#16213e);min-height:100vh;display:flex;justify-content:center;align-items:center;color:#e0e0e0;}.card{background:rgba(255,255,255,0.05);border-radius:20px;padding:40px;text-align:center;border:1px solid rgba(255,71,87,0.3);}.card h1{color:#ff4757;}</style></head><body>";
  html += "<div class=\"card\"><h1>Broadcast Attack!</h1></div></body></html>";

  start_deauth(0, DEAUTH_TYPE_ALL, 1);
  server.send(200, "text/html", html);
}

void handle_stop() {
  stop_deauth();
  redirect_root();
}

void start_web_interface() {
  server.on("/", handle_root);
  server.on("/attack", handle_attack);
  server.on("/broadcast", handle_broadcast);
  server.on("/stop", handle_stop);
  server.begin();
}

void web_interface_handle_client() {
  server.handleClient();
}

String getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "EAP";
    default: return "?";
  }
}
