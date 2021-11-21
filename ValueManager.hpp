#pragma once
#include "LiquidCrystal.h"

/* Degree character */
#define DEGREE_CHAR byte(0)

/* Definition of a '°' character */
const byte degreeChar[8] = {
  B00100,
  B01010,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
};

/* A way to retrieve labels and value boxes positions */
struct Position{ uint8_t x, y; };

/* Value storage, LCD printing management */
template <typename T>
class Value
{
	
public:
  Value(LiquidCrystal& lcd, const char* label, const Position& labelPos, const uint8_t& valueSize, const char* suffix = "", const bool& degree = false, const float& initValue = T(0))
    : m_lcd(lcd), m_label(label), m_labelPos(labelPos), m_valuePos({labelPos.x + strlen(label), labelPos.y}), m_valueSize(valueSize), m_suffix(suffix), m_degree(degree), value(initValue)
  {}

  void writeLabel()
  {
    cursorToLabel();
    m_lcd.print(m_label);
  }

  void writeValue()
  {
    cursorToValue();

    // Clearing the box
    char* clearMask = (char*)malloc(sizeof(char) * (m_valueSize + 1));
    memset(clearMask, ' ', sizeof(char) * m_valueSize);
    clearMask[m_valueSize] = 0; // String-end identificator
    m_lcd.print(clearMask);
    free(clearMask);

    cursorToValue();

    // Writing new data
    m_lcd.print(value);
    if(m_degree)
      m_lcd.write(DEGREE_CHAR);
    m_lcd.print(m_suffix);
  }

private:
  void cursorToLabel()
  {
    m_lcd.setCursor(m_labelPos.x, m_labelPos.y);
  }

  void cursorToValue()
  {
    m_lcd.setCursor(m_valuePos.x, m_valuePos.y);
  }

private:
  LiquidCrystal& m_lcd;
  const char* m_label;
  const Position m_labelPos;
  const Position m_valuePos;
  const uint8_t m_valueSize;
  const char* m_suffix = "";
  const bool m_degree;

public:
  T value;
};

void CreateLCDDegree(LiquidCrystal& lcd)
{
  lcd.createChar(DEGREE_CHAR, degreeChar);
}