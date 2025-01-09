// Llibreria lcd i2c

// Comandes del LCD
#define LCD_CMD  0x00
#define LCD_DATA 0x01

// Configuració de bits de control
#define ENABLE_BIT 0x04
#define BACKLIGHT  0x08

void lcd_send(uint8_t value, uint8_t mode);
void lcd_command(uint8_t command);
void lcd_init();
void lcd_print(const char *str);
void lcd_setCursor(uint8_t col, uint8_t row);
void lcd_clear();

// Funcions per al LCD
void lcd_send(uint8_t value, uint8_t mode) {
    uint8_t highNibble = (value & 0xF0) | BACKLIGHT | mode;
    uint8_t lowNibble = ((value << 4) & 0xF0) | BACKLIGHT | mode;

    Wire.beginTransmission(LCD_ADDR);
    Wire.write(highNibble | ENABLE_BIT);
    Wire.write(highNibble & ~ENABLE_BIT);
    Wire.write(lowNibble | ENABLE_BIT);
    Wire.write(lowNibble & ~ENABLE_BIT);
    Wire.endTransmission();
    delayMicroseconds(50); // Espera perquè el LCD processi
}

void lcd_command(uint8_t command) {
    lcd_send(command, LCD_CMD);
}

void lcd_init() {
    delay(50); // Espera perquè el LCD estigui llest
    lcd_command(0x03);
    delay(5);
    lcd_command(0x03);
    delayMicroseconds(150);
    lcd_command(0x03);
    lcd_command(0x02); // Mode de 4 bits

    // Configura el display: 2 files, 5x8 caràcters
    lcd_command(0x28);
    lcd_command(0x0C); // Display activat, cursor amagat
    lcd_command(0x06); // Incrementa el cursor
    lcd_command(0x01); // Neteja el display
    delay(5);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_send(*str++, LCD_DATA);
    }
}

void lcd_setCursor(uint8_t col, uint8_t row) {
    static const uint8_t rowOffsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_command(0x80 | (col + rowOffsets[row]));
}

void lcd_clear() {
    lcd_send(0x01, 0); // Instrucció per esborrar la pantalla
    delayMicroseconds(2000); // Retard necessari (aproximadament 2ms) per completar l'esborrat
}

