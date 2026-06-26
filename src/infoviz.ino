#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// credenciales WIFI
const char* ssid = "NOMBRE WIFI";
const char* password = "CLAVE";

WebServer server(80);

int valor_electrocucion_mortandad = 0; // defecto hasta elegir país

// --- funciones base ---

//Nivel electrocución en base a magnitud de mortandad
void electro_BUM(int escalar){
  if(escalar < 0) escalar = 0;
  if(escalar > 20) escalar = 20;

  if(escalar == 0){
    Serial.println("Electrocucion cancelada...");
    while(valor_electrocucion_mortandad > 0){
      digitalWrite(4, LOW);
      delay(100);
      digitalWrite(4, HIGH);
      delay(100);
      valor_electrocucion_mortandad--;
    }
    return;
  }

  Serial.print("Preparando electrocucion nivel ");
  Serial.println(escalar);

  while(valor_electrocucion_mortandad < escalar){
    digitalWrite(2, LOW);
    delay(100);
    digitalWrite(2, HIGH);
    delay(100);
    valor_electrocucion_mortandad++;
  }

  while(valor_electrocucion_mortandad > escalar){
    digitalWrite(4, LOW);
    delay(100);
    digitalWrite(4, HIGH);
    delay(100);
    valor_electrocucion_mortandad--;
  }

  Serial.print("Nivel actual: ");
  Serial.println(valor_electrocucion_mortandad);
}
// Calculo de velocidad del corazón para el delay, en base a la magnitud
void beat_corazon(int velocidad){
  if(velocidad == 0){
    Serial.println("Sin Bocina...");
    return;
  }
  Serial.print("Bocina de mortandad rapidez ");
  Serial.println(velocidad);
  digitalWrite(16, HIGH);
  delay(20);
  digitalWrite(16, LOW);
  delay(1000 / velocidad);
}

// --- Manejadores del Servidor Web ---

// Página Principal (HTML + JS)
void handleRoot() {
  // HTML embebido. He puesto unos países de ejemplo ordenados de mayor a menor promedio.
  // Deberás rellenar/reemplazar el <select> con los datos ordenados de tu CSV mapeados del 1 al 20.
  String html = "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Control de Mortandad</title>";
  html += "<style>body{font-family:Arial,sans-serif; text-align:center; padding:20px; background:#f4f4f4;} select{padding:10px; font-size:16px; margin:20px; width:80%; max-width:400px;}</style>";
  html += "</head><body>";
  html += "<h2>Selecciona un País</h2>";
  html += "<p>Los países están ordenados de mayor a menor tasa de mortandad (Escala 1-20)</p>";

  html += "<select id='pais' onchange='enviarEscala()'>";
  html += "<option value='0'>-- Ninguno (Desactivar) --</option>";

  File archivo = LittleFS.open("/paises_esp32.csv", "r");

  if (archivo) {

    archivo.readStringUntil('\n'); // omite encabezado

    while (archivo.available()) {

      String linea = archivo.readStringUntil('\n');
      linea.trim();

      if (linea.length() == 0)
        continue;

      int coma = linea.lastIndexOf(',');

      if (coma == -1)
        continue;

      String nombre = linea.substring(0, coma);
      String escala = linea.substring(coma + 1);

      nombre.trim();
      escala.trim();

      if (nombre.startsWith("\""))
        nombre.remove(0, 1);

      if (nombre.endsWith("\""))
        nombre.remove(nombre.length() - 1);

      html += "<option value='";
      html += escala;
      html += "'>";
      html += nombre;
      html += " - Nivel ";
      html += escala;
      html += "</option>";
    }

    archivo.close();

  } else {

    html += "<option value='0'>Error leyendo paises_esp32.csv</option>";

  }

  html += "</select>";

  html += "<h3 id='status'>Nivel actual: 0</h3>";

  html += "<script>";
  html += "function enviarEscala() {";
  html += "  var val = document.getElementById('pais').value;";
  html += "  fetch('/set?escalar=' + val)";
  html += "  .then(response => response.text())";
  html += "  .then(data => { document.getElementById('status').innerText = 'Nivel actual: ' + val; });";
  html += "}";
  html += "</script>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Endpoint para recibir el entero (?escalar=X)
void handleSet() {
  if (server.hasArg("escalar")) {
    int nuevoEscalar = server.arg("escalar").toInt();
    server.send(200, "text/plain", String(nuevoEscalar)); // Devuelve el entero solicitado
    
    // Ejecuta lógica con los relés
    electro_BUM(nuevoEscalar);
  } else {
    server.send(400, "text/plain", "Falta el parametro 'escalar'");
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(16, OUTPUT);

  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(16, LOW);

  if (!LittleFS.begin(true)) {
      Serial.println("Error montando LittleFS");
      return;
  }
  
  File archivo = LittleFS.open("/paises_esp32.csv", "r");

  if (!archivo) {
      Serial.println("No se pudo abrir el CSV");
      return;
  }
  archivo.close();

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado con éxito. IP del ESP32: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  digitalWrite(2, HIGH); //asegura que entre manipulaciones no queden prendidos los relés....
  digitalWrite(4, HIGH);
  server.handleClient(); // Mantiene el servidor web escuchando peticiones
  
  if (valor_electrocucion_mortandad > 0) {
    beat_corazon(valor_electrocucion_mortandad);  
  }
}