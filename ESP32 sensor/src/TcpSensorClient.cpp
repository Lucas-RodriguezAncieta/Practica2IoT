#include "TcpSensorClient.h"

#include <cstring>

#include "SensorProtocol.h"

namespace {
constexpr unsigned long kRegistrationTimeoutMs = 2000UL;
}

TcpSensorClient::TcpSensorClient(const char* serverIp, uint16_t serverPort,
                                 unsigned long reconnectIntervalMs,
                                 unsigned long connectTimeoutMs)
    : serverIp_(serverIp),
      serverPort_(serverPort),
      reconnectIntervalMs_(reconnectIntervalMs),
      connectTimeoutMs_(connectTimeoutMs) {}

void TcpSensorClient::update(unsigned long nowMs, bool wifiConnected) {
  if (!wifiConnected) {
    disconnect();
    return;
  }

  if (client_.connected()) {
    processIncoming();
    if (awaitingRegistration_ &&
        nowMs - registrationSentAtMs_ >= kRegistrationTimeoutMs) {
      Serial.println("[SENSOR] Registro TCP sin respuesta");
      disconnect();
    }
    return;
  }

  if (registered_ || awaitingRegistration_) {
    Serial.println("[SENSOR] TCP desconectado");
    disconnect();
  }
  if (nowMs - lastAttemptAtMs_ < reconnectIntervalMs_) {
    return;
  }

  lastAttemptAtMs_ = nowMs;
  Serial.print("[SENSOR] Conectando al servidor ");
  Serial.print(serverIp_);
  Serial.print(":");
  Serial.println(serverPort_);
  if (!client_.connect(serverIp_, serverPort_, connectTimeoutMs_)) {
    Serial.println("[SENSOR] Conexion TCP no disponible");
    return;
  }

  Serial.println("[SENSOR] TCP conectado");
  awaitingRegistration_ = true;
  registrationSentAtMs_ = nowMs;
  sendLine("REGISTER SENSOR");
}

bool TcpSensorClient::isRegistered() {
  return registered_ && client_.connected();
}

void TcpSensorClient::sendMeasurement(const Measurement& measurement) {
  if (!isRegistered()) {
    return;
  }
  if (measurement.status != MeasurementStatus::VALID) {
    char message[] = "INVALID";
    if (sendLine(message)) {
      Serial.println("[SENSOR] Enviado: INVALID");
    }
    return;
  }

  char message[32];
  if (buildMeasurementLine(measurement, message, sizeof(message)) &&
      sendLine(message)) {
    Serial.print("[SENSOR] Enviado: ");
    Serial.println(message);
  }
}

void TcpSensorClient::processIncoming() {
  while (client_.available() > 0) {
    const char character = static_cast<char>(client_.read());
    if (discardingOversizeLine_) {
      if (character == '\n') {
        discardingOversizeLine_ = false;
        Serial.println("[SENSOR] Respuesta TCP demasiado larga descartada");
      }
      continue;
    }
    if (character == '\n') {
      lineBuffer_[lineLength_] = '\0';
      if (lineLength_ > 0 && lineBuffer_[lineLength_ - 1] == '\r') {
        lineBuffer_[--lineLength_] = '\0';
      }
      processLine(lineBuffer_);
      lineLength_ = 0;
      continue;
    }
    if (lineLength_ >= kMaximumLineBytes) {
      lineLength_ = 0;
      discardingOversizeLine_ = true;
      continue;
    }
    lineBuffer_[lineLength_++] = character;
  }
}

void TcpSensorClient::processLine(const char* line) {
  if (awaitingRegistration_ && std::strcmp(line, "OK") == 0) {
    awaitingRegistration_ = false;
    registered_ = true;
    Serial.println("[SENSOR] Registro aceptado");
    return;
  }
  Serial.print("[SENSOR] Respuesta TCP: ");
  Serial.println(line);
  if (awaitingRegistration_ && std::strncmp(line, "ERROR ", 6) == 0) {
    disconnect();
  }
}

bool TcpSensorClient::sendLine(const char* line) {
  if (!client_.connected()) {
    return false;
  }
  const size_t lineLength = std::strlen(line);
  if (client_.print(line) != lineLength || client_.print('\n') != 1) {
    Serial.println("[SENSOR] Error al enviar por TCP");
    disconnect();
    return false;
  }
  return true;
}

void TcpSensorClient::disconnect() {
  client_.stop();
  registered_ = false;
  awaitingRegistration_ = false;
  lineLength_ = 0;
  discardingOversizeLine_ = false;
}
