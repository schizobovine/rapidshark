#ifndef DEBUAG_H
#define DEBUAG_H

#define SERIAL_DEBUG 0

#if SERIAL_DEBUG
#define LOG(msg) Serial.println(msg)
#else
#define LOG(msg)
#endif

#endif
