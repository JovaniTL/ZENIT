# ⚡ ZÉNIT — Prototipo IoT de Control y Elevación Residencial

<p align="center">
  <b>Sistema embebido de control y monitoreo inteligente para la elevación preventiva de estructuras ante contingencias por inundación.</b>
</p>

---

## 📋 Descripción del Proyecto

**ZÉNIT** es un prototipo funcional a escala desarrollado de manera colaborativa en un equipo multidisciplinario. El sistema opera de forma local mediante un microcontrolador **ESP32** conectado a una red Wi-Fi propia, permitiendo el control automatizado de actuadores y relevadores mediante una **API REST HTTP asíncrona** con soporte JSON y autenticación básica.

---

## 🛠️ Tecnologías y Arquitectura

* **Microcontrolador:** ESP32
* **Firmware:** C++ (Arduino IDE)
* **Conectividad & Red:** WiFi, `ESPAsyncWebServer`, `AsyncTCP`
* **Procesamiento de Datos:** `ArduinoJson` (peticiones y respuestas en formato JSON)
* **Seguridad:** Autenticación HTTP básica (`HTTP Basic Authentication`)
* **Hardware de Control:** Relés conectados a los pines GPIO 26 (`PIN_ELEVATE`) y GPIO 27 (`PIN_LOWER`)

---

## 🔌 API REST Endpoints

### `POST /api/control`
Recibe un objeto JSON con el comando para operar el sistema de elevación. Requiere credenciales de autenticación básica.

* **Cuerpo de la petición (JSON):**
  ```json
  {
    "command": "ELEVATE"
  }
