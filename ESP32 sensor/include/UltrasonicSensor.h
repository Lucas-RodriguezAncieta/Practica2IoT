#pragma once

#include <Arduino.h>

/** Resultado de un intento de medicion del HC-SR04. */
enum class MeasurementStatus {
  VALID,
  TIMEOUT,
  OUT_OF_RANGE,
};

/** Distancia sin clasificar y el estado que determina si puede utilizarse. */
struct Measurement {
  MeasurementStatus status;
  float distanceCm;
};

/**
 * Encapsula el disparo y la lectura de un HC-SR04.
 *
 * Los pines y el rango valido pertenecen a cada instancia para permitir su
 * reutilizacion con otros sensores o configuraciones.
 */
class UltrasonicSensor {
 public:
  UltrasonicSensor(uint8_t triggerPin, uint8_t echoPin, float minimumCm,
                   float maximumCm);

  /** Configura los pines y deja TRIG en nivel bajo. */
  void begin();

  /** Realiza un pulso de medida y devuelve su resultado sin clasificarlo. */
  Measurement measure();

  /** Convierte y valida una duracion de eco; expuesto para pruebas unitarias. */
  Measurement fromDurationUs(unsigned long durationUs) const;

 private:
  uint8_t triggerPin_;
  uint8_t echoPin_;
  float minimumCm_;
  float maximumCm_;
};
