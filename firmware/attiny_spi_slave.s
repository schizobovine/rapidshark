//The following code demonstrates how to use the USI module as a SPI Slave:
init:
  ldi r16,(1<<USIWM0)|(1<<USICS1)
  out USICR,r16
// ...
SlaveSPITransfer:
  out USIDR,r16
  ldi r16,(1<<USIOIF)
  out USISR,r16
SlaveSPITransfer_loop:
  in r16, USISR
  sbrs r16, USIOIF
  rjmp SlaveSPITransfer_loop
  in r16,USIDR
  ret


// Enable SPI/3-wire mode with external clocking on postive edge
USICR |= (1<<USIOIE) | (1<<USIWM0) | (1<<USICS1)

// Load data to send
USIDR = data;

// Clear counter overflow flag
USISR |= (1<<USIOIF)

USICR Control Register
  7 USISIE
  6 USIOIE
  5 USIWM1
  4 USIWM0
  3 USICS1
  2 USICS0
  1 USICLK
  0 USITC

USISR Status Register
  7 USISIF Start Cond Intr Flag
  6 USIOIF Counter Overflow Intr Flag
  5 USIPF Stop condition
  4 USIDC Data output collision
  3 USICNT3 -+-- Counter value
  2 USICNT2  |
  1 USICNT1  |
  0 USICNT0 -+

USIDR Data Register
USIBR Buffer Register
