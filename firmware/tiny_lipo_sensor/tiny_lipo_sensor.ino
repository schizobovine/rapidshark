/*
 * tiny_lipo_sensor.ino
 *
 * Fart out battery stats over some kind of serial.
 *
 */

#include <Arduino.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

const uint8_t NUM_CELLS = 3;
const uint8_t CELL_PINS[NUM_CELLS] = {
  A0, // cell 1
  A1, // cell 2
  A2  // cell 3
};

const uint8_t EN_DIV  = 3; // PA3
const uint8_t PIN_CLK = 4; // PA4 aka SCLK aka SCL
const uint8_t PIN_DAT = 5; // PA5 aka MISO aka DO
const uint8_t PIN_EN  = 6; // PA6 aka MOSI aka DI aka SDA
const uint8_t LED = 8; // PA6 aka MOSI aka DI aka SDA

//
// Voltage divider calculations
//
//              R1 + R2
// Vin = Vout * -------
//                R2
//
// R1 = 4K7  (1%)
//    = 22K1 (1%)
//    = 36K  (1%)
// R2 = 10K  (1%, same for all)
//

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Pre-calculated arrays of voltage divider multipliers, using some dirty fixed
// point math to avoid floating point. These times the 
const uint32_t VDIV[NUM_CELLS] = {
  5000L * ( 4900L + 10000L) / 10000L,
  5000L * (22100L + 10000L) / 10000L,
  5000L * (36000L + 10000L) / 10000L
};

// Number of bits of ADC resolution, used to divide reading after applying
// voltage divider multiplier (...which is a bit of an oxymoron now that I read
// it...).
const uint8_t ADC_NUM_BITS = 10;

// Readings in ADC ticks
volatile uint16_t readings_raw[NUM_CELLS] = {
  0, 0, 0
};

// Do ADC -> millivolt conversion with enough precision before dropping back to
// 16bit ints
volatile uint32_t reading_temp = 0;

// Readings in millivolts as 16-bit unsigned big-endian integers,
// double-buffered to allow us to serve results while computing the next set.
volatile uint16_t readings_buff[NUM_CELLS] = {
  0, 0, 0
};

// Size of the above buffer in BYTES
//const uint8_t BUFF_SZ = sizeof(uint16_t) * NUM_CELLS;
const uint8_t BUFF_SZ = 6;

// Buffer position
volatile uint8_t pos = 0;

// In SPI transaction
volatile boolean in_transaction = false;

// Max position in buffer by byte (not number), so we can transfer the MSB and
// LSB of the 16 bit results separately.
//const uint8_t BUFF_POS_MAX = NUM_CELLS * 2;

// Which of the two buffers is ready for reading by the master?
//volatile uint8_t curr_buff = 0;

// Where in that buffer the IRQ is reading from?
//volatile uint8_t buff_read_pos = BUFF_POS_MAX;

// Needed to prevent writer from swapping buffers while the other one is still
// being read in a SPI transaction.
//volatile boolean in_transaction = false;

// Software UARt for sharting data out
//SoftwareSerial serial(SERIAL_RX, SERIAL_TX);

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

// Do a full cylce of enabling voltage dividers & reading the ADC.
void do_read_voltages() {

  // Enable dividers by flipping Q4 (N-MOS) gate HIGH, thereby flipping the
  // gates for Q1, Q2 and Q3 (P-MOS) LOW, opening the connections to the cell
  // terminals.
  digitalWrite(EN_DIV, HIGH);

  // Wait a tick! The ADC needs time to stabilize.
  delay(10);

  // Do the needful
  readings_raw[0] = analogRead(CELL_PINS[0]);
  readings_raw[1] = analogRead(CELL_PINS[1]);
  readings_raw[2] = analogRead(CELL_PINS[2]);

  // Disable dividers by doing the reverse of the whole dance above. In
  // theory, you can take it back to tristate here, since the hardwired
  // "default" should be the safe/power-saving version that keeps all the
  // P-MOSFETs closed.
  digitalWrite(EN_DIV, LOW);

}

// Conversion from raw ADC values to millivolts. Written to a double-buffer to
// ensure consistent reads by the master.
void do_calc_voltages() {
  uint8_t i;

  // Figure out which buffer we need to write to, whichever is not the one
  // currently being read from. The switch between buffers is controlled by the
  // SPI transaction process, to ensure consistent reads.
  //other = (curr_buff + 1) % 2;

  // Calculate voltages
  for (i=0; i<NUM_CELLS; i++) {

    // Multiply by (R1+R2)/R1 * 1000
    //reading_temp = readings_raw[i] * VDIV[i];

    // Normalize by ADC precision & store
    //readings_buff[i] = reading_temp >> ADC_NUM_BITS;
    readings_buff[i] = readings_raw[i];

  }

}

// Busy wait based writing of buffers on bus. Helps that we can ignore the data
// from the master.
void put_buffers() {

  // Pre-put first byte
  USIDR = ((uint8_t*)readings_buff)[0];

  // Reset buff position
  pos = 1;

  // Put in transaction
  in_transaction = true;

  // Enable overflow interrupt
  USICR |= (1<<USIOIE);

  // Wait for transmission to finish
  while (in_transaction)
    ;

  // Disable overflow interrupt
  USICR &= ~(1<<USIOIE);


}

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

// Interrupt handler for USI overflow vector, which gets called when slave
// transmission of a byte is complete. When this is tripped, we move the next
// byte into the transmission register and (optionally, we're not doing this)
// read out whatever the master sent us. Additionally, we advance buffer
// position, optionally swapping buffers if the other is ready.
ISR(USI_OVF_vect) {

  // Clear USI counter overflow
  USISR |= (1<<USIOIF);

  // If not in a transaction, what are we doing here?!?
  if (!in_transaction)
    return;

  // Put next byte on bus
  if (pos < BUFF_SZ) {
    USIDR = ((uint8_t*)readings_buff)[pos];
    pos += 1;
  }
  
  // If done with transaction, unblock main
  if (pos == BUFF_SZ) {
    in_transaction = false;
  }
  // Figure out which byte is next for reading
  //if (buff_read_pos < BUFF_MAX_POS) {

  // Transmission complete, set transaction flag off and repoint buffer at
  // start of array.
  //} else {
  //  buff_read_pos = 0;
  //  in_transaction = false;
  //}
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

  //delay(200);

  // Initialize divider to be default off
  pinMode(EN_DIV, OUTPUT);
  digitalWrite(EN_DIV, LOW);

  // Setup pins for "SPI" output
  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DAT, OUTPUT);
  pinMode(PIN_EN,  INPUT_PULLUP);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Setup USI in 3-wire mode (though we'll be ignoring the input from the
  // master, since that's our enable line), with external clocking (slave mode)
  // on positive edges.
  USICR = (1<<USIWM0) | (1<<USICS1);

  readings_buff[0] = 1;
  readings_buff[1] = 2;
  readings_buff[2] = 3;

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Check chip select pin to see if we should do jack and/or shit
  //if (digitalRead(PIN_EN) == LOW) {

    digitalWrite(LED, HIGH);
    do_read_voltages();
    do_calc_voltages();
    //readings_buff[0] += 3;
    //readings_buff[1] += 3;
    //readings_buff[2] += 3;
    put_buffers();
    digitalWrite(LED, LOW);

    // Voltage calculations
    //for (uint8_t i=0; i<NUM_CELLS; i++) {
    //  voltages[i] = calc_voltage(i);
    //}

    //
    // Fart out data
    //
    // Wire protocol is pretty basic: cell number followed by two bytes of data
    // (raw 10bit ADC value plus 6bits padding), with 0xFF signalling end of
    // data.
    //

    //serial.begin(SERIAL_BAUD);
    //serial.write((uint8_t)NUM_CELLS);
    //for (uint8_t i=0; i<NUM_CELLS; i++) {
    //  serial.write((uint8_t)(i+1)); // Cell num
    //  serial.write((uint8_t)((readings[i] >> 0) & 0xFF));
    //  serial.write((uint8_t)((readings[i] >> 8) & 0xFF));
    //}
    //serial.write((uint8_t)0xFF);
    //serial.flush();
    //serial.end();

    //// Because I'm feeling sassy. And hate battery life (despite desiging this
    //// whole circuit around it (don't judge my hypocracy! :P)).
    //delay(100);

  //}

}

// vi: syntax=arduino
