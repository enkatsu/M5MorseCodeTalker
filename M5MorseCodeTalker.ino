#include <M5Stack.h>
#include "MorseEncoder.h"

const uint16_t wpm = 15;
const uint16_t unitTime = 1200 / wpm;
const uint32_t CODE_QUEUE_LENGTH = 128;
QueueHandle_t codeQueue;

void morseTask(void* arg) {
  Code code;
  while (1) {
    BaseType_t ret = xQueueReceive(codeQueue, &code, portMAX_DELAY);
    if (ret) {
      switch (code) {
        case Code::SHORT:
          M5.Speaker.tone(440, unitTime * 1);
          delay(unitTime * 1);
          M5.Speaker.mute();
          delay(unitTime * 1);
          break;
        case Code::LONG:
          M5.Speaker.tone(440, unitTime * 3);
          delay(unitTime * 3);
          M5.Speaker.mute();
          delay(unitTime * 1);
          break;
        case Code::SPACE:
          delay(unitTime * 7);
          break;
        case Code::DELIMITER:
          delay(unitTime * 3);
          break;
        default:
          break;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  codeQueue = xQueueCreate(CODE_QUEUE_LENGTH, sizeof(Code));
  xTaskCreatePinnedToCore(morseTask, "morseTask", 4096, NULL, 1, NULL, 0);
}

void loop() {
  M5.update();

  if (M5.BtnA.wasReleased()) {
    encodeMessageToMorseAndEnqueue(codeQueue, "hello");
  }
}
