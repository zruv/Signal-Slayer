#ifndef RADIO_SETTINGS_H
#define RADIO_SETTINGS_H

#include <RF24.h>

extern RF24 radio;

bool configureNrf();
bool setupRadio();

#endif // RADIO_SETTINGS_H
