#define USI_PIN_CL PA4
#define USI_PIN_DO PA5
#define USI_PIN_DI PA6
#define USI_BUF_SZ 18

// Stores incoming data until next transfer
uint8_t buff_in = 0;

uint8_t buff_out[USI_BUF_SZ];
size_t buff_pos = 0;

void spi_init_slave() {

  // Enable SPI/3-wire mode with external clocking on postive edge
  USICR |= (1<<USIOIE) | (1<<USIWM0) | (1<<USICS1);

  // Set pin modes
  pinMode(USI_PIN_CL, INPUT_PULLUP);
  pinMode(USI_PIN_DO, OUTPUT);
  pinMode(USI_PIN_DI, INPUT_PULLUP);

}

uint8_t spi_transfer_slave(uint8_t data_out) {
  uint8_t data_in = 0;

  // Load data to send
  USIDR = data;

  // Clear counter overflow flag
  USISR |= (1<<USIOIF);

  // Fast busy wait until transfer is complete (huh)
  asm volatile (
      "L%=SlaveSPITransfer_loop:"
        "in   %[data_in], USISR"          "\n\t"
        "sbrs %[data_in], USIOIF"         "\n\t"
        "rjmp L%=SlaveSPITransfer_loop"   "\n\t"
        "in   %[data_in], USIDR"          "\n\t"
  );

  return data_in;
}

// Interrupt handler for USI counter overflow
ISR(USI_OVF_vect) {

  // Clear counter overflow flag
  USISR |= (1<<USIOIF);

  // Move next byte into USI data register
  USIDR = buff_out[buff_pos];

  // Increment ringbuffer position
  buff_pos = (buff_pos + 1) % USI_BUF_SZ;

  // Store contents of USI buffer
  // TODO does this need to happen if we're reading in data? If so the pin can
  // be repurposed for SS...
  //spi_buff = USIBR;

}
