#ifndef encode_h
#define encode_h
#include "codes.h"
#include <M5Stack.h>

enum class Code {
  SHORT,
  LONG,
  SPACE,
  DELIMITER,
};

Code charToCode(char c) {
  if (c == '.') {
    return Code::SHORT;
  } else if (c == '-') {
    return Code::LONG;
  }
}

int encodeCharToMorseCode(char character, Code* codePtr, size_t& length) {
  char upper = toupper(character);
  if (upper >= 'A' && upper <= 'Z') {
    int index = upper - 'A';
    length = strlen(morseCodes[index]);
    const char* morse = reinterpret_cast<const char*>(pgm_read_dword(&morseCodes[index]));
    for (int j = 0; morse[j] != '\0'; j++) {
      codePtr[j] = charToCode(morse[j]);
    }
    return 0;
  }
  
  if (character >= '0' && character <= '9') {
    int index = character - '0' + 26;
    length = strlen(morseCodes[index]);
    const char* morse = reinterpret_cast<const char*>(pgm_read_dword(&morseCodes[index]));
    for (int j = 0; morse[j] != '\0'; j++) {
      codePtr[j] = charToCode(morse[j]);
    }
    return 0;
  }

  if (character == ' ') {
    length = 1;
    codePtr[0] = Code::SPACE;
    return 0;
  }

  int index = -1;
  for (uint32_t j = 0; j < sizeof(specialChars) - 1; j++) {
    if (character == specialChars[j]) {
      index = j;
      break;
    }
  }
  if (index >= 0 && index < 17) {
    length = strlen(morseSpecialChars[index]);
    const char* morse = reinterpret_cast<const char*>(pgm_read_dword(&morseSpecialChars[index]));
    for (int j = 0; morse[j] != '\0'; j++) {
      codePtr[j] = charToCode(morse[j]);
    }
    return 0;
  }

  return 1;
}

void encodeMessageToMorseAndEnqueue(QueueHandle_t& queue, const char* message) {
  for (int i = 0; message[i] != '\0'; i++) {
    size_t len = 0;
    Code* codePtr = (Code*)malloc(sizeof(Code) * MORSE_CODE_MAX_SIZE);
    int result = encodeCharToMorseCode(message[i], codePtr, len);
    for (int j = 0; j < len; j++) {
      int ret = xQueueSend(queue, &codePtr[j], 0);
    }
    free(codePtr);
    Code delimiter = Code::DELIMITER;
    int ret = xQueueSend(queue, &delimiter, 0);
  }
}

#endif
