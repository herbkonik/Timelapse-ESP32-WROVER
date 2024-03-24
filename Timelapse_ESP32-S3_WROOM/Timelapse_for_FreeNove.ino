/**********************************************************************
  Code to take a pic every x secounds and save it to the SD Card.
  The sketch is made for ESP32S3-WROOM Board / OV2640 camera,based at 
  the sample code which you can download at www.freenove.com and should 
  work at similar Boards to (untested). 
  Without changes it should NOT work at AI Thinker and other Boards with 
  a limited amount of pins, because i add a signal for an external flash
  at GPO21.

  Auther      : Herbert Kozuschnik (based at freenove example)
  Last Update : 2024/03/24
**********************************************************************/

#include "esp_camera.h"
#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"
#include "sd_read_write.h"

constexpr uint8_t FlashPin = 21;
constexpr uint16_t take_pic_every_millis = 3000;

void setup() {
  pinMode(FlashPin, OUTPUT);
  sdmmcInit();
  createDir(SD_MMC, "/camera");
  listDir(SD_MMC, "/camera", 0);
  if (cameraSetup() == 0) {
    return;
  }
}

void loop() {
  delay(take_pic_every_millis);
  digitalWrite(FlashPin, HIGH);
  delay(5);
  camera_fb_t* fb = NULL;
  fb = esp_camera_fb_get();
  delay(5);
  digitalWrite(FlashPin, LOW);

  if (fb != NULL) {
    int photo_index = readFileNum(SD_MMC, "/camera");
    if (photo_index != -1) {
      String path = "/camera/" + String(photo_index) + ".jpg";
      writejpg(SD_MMC, path.c_str(), fb->buf, fb->len);
    }
    esp_camera_fb_return(fb);
  }
}

int cameraSetup(void) {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Limit the frame size when PSRAM is not available
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    //   Serial.printf("Camera init failed with error 0x%x", err);
    return 0;
  }

  sensor_t* s = esp_camera_sensor_get();
  // initial sensors are flipped horizontal / vertically and colors are a bit saturated
  s->set_vflip(s, 1);       // flip it vertically   ( 0 or 1 )
  s->set_hmirror(s, 1);     // flip it horizontal   ( 0 or 1 )
  s->set_brightness(s, 1);  // set brightness       ( 0 to 2 )
  s->set_saturation(s, 0);  // set saturation       ( 0 to 2 )

  // Serial.println("Camera configuration complete!");
  return 1;
}
