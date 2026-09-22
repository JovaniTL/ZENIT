#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

// Credenciales de Red
const char* ssid = "Red-Zenit";
const char* password = "[PASSWORD]";

// Credenciales de Autenticación Básica
const char* http_username = "zenit";
const char* http_password = "[PASSWORD]";

// Pines de Control (Relevadores)
const int PIN_ELEVATE = 26;
const int PIN_LOWER   = 27;

// Tiempos y Calibración
const unsigned long TOTAL_LIFT_TIME_MS = 66000; 
unsigned long actionStartMillis = 0;
unsigned long currentActiveTime = 0;
    
enum SystemState { STOPPED, ELEVATING, LOWERING };
SystemState currentState = STOPPED;

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);

    pinMode(PIN_ELEVATE, OUTPUT);
    pinMode(PIN_LOWER, OUTPUT);
    digitalWrite(PIN_ELEVATE, LOW);
    digitalWrite(PIN_LOWER, LOW);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado a WiFi. IP: " + WiFi.localIP().toString());

    // Endpoint API para control del actuador
    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/api/control", [](AsyncWebServerRequest *request, JsonVariant &json) {
        if (!request->authenticate(http_username, http_password)) {
            return request->requestAuthentication();
        }

        JsonObject jsonObj = json.as<JsonObject>();
        String command = jsonObj["command"].as<String>();

        StaticJsonDocument<200> responseDoc;

        if (command == "ELEVATE") {
            if (currentState != ELEVATING) {
                digitalWrite(PIN_LOWER, LOW);
                digitalWrite(PIN_ELEVATE, HIGH);
                currentState = ELEVATING;
                actionStartMillis = millis();
                responseDoc["status"] = "Elevando";
            }
        } else if (command == "LOWER") {
            if (currentState != LOWERING) {
                digitalWrite(PIN_ELEVATE, LOW);
                digitalWrite(PIN_LOWER, HIGH);
                currentState = LOWERING;
                actionStartMillis = millis();
                responseDoc["status"] = "Bajando";
            }
        } else if (command == "STOP") {
            digitalWrite(PIN_ELEVATE, LOW);
            digitalWrite(PIN_LOWER, LOW);
            currentState = STOPPED;
            responseDoc["status"] = "Detenido (Paro de Emergencia)";
        } else {
            request->send(400, "application/json", "{\"error\":\"Comando invalido\"}");
            return;
        }

        // Estimación básica de porcentaje (Gemelo Digital)
        float elevationPercentage = (float)currentActiveTime / TOTAL_LIFT_TIME_MS * 100.0;
        if (elevationPercentage > 100.0) elevationPercentage = 100.0;
        responseDoc["elevation"] = elevationPercentage;

        String responseString;
        serializeJson(responseDoc, responseString);
        request->send(200, "application/json", responseString);
    });

    server.addHandler(handler);
    server.begin();
}

void loop() {
    // El servidor asíncrono no requiere código bloqueante dentro de loop()
    if (currentState == ELEVATING || currentState == LOWERING) {
        currentActiveTime = millis() - actionStartMillis;
    }
}