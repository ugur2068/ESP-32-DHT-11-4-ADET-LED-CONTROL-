#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

const char* ssid = "UGUR";
const char* password = "12345678";

#define DHTPIN 27
#define DHTTYPE    DHT11

DHT dht(DHTPIN, DHTTYPE);
int ledPins[] = {2, 3, 4, 5};
bool ledStates[] = {false, false, false, false};

AsyncWebServer server(80);

String readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <i class="fas fa-lightbulb" style="color:#f0e68c;"></i> 
    <span class="dht-labels">LED 1</span>
    <span id="ledState1">%LEDSTATE1%</span>
    <button onclick="toggleLED(1)">Toggle LED 1</button>
  </p>
  <p>
    <i class="fas fa-lightbulb" style="color:#f0e68c;"></i> 
    <span class="dht-labels">LED 2</span>
    <span id="ledState2">%LEDSTATE2%</span>
    <button onclick="toggleLED(2)">Toggle LED 2</button>
  </p>
  <p>
    <i class="fas fa-lightbulb" style="color:#f0e68c;"></i> 
    <span class="dht-labels">LED 3</span>
    <span id="ledState3">%LEDSTATE3%</span>
    <button onclick="toggleLED(3)">Toggle LED 3</button>
  </p>
  <p>
    <i class="fas fa-lightbulb" style="color:#f0e68c;"></i> 
    <span class="dht-labels">LED 4</span>
    <span id="ledState4">%LEDSTATE4%</span>
    <button onclick="toggleLED(4)">Toggle LED 4</button>
  </p>
</body>
<script>
function toggleLED(ledNum) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/toggle?led=" + ledNum, true);
  xhr.send();
}

function updateData() {
  var xhr1 = new XMLHttpRequest();
  var xhr2 = new XMLHttpRequest();

  xhr1.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };

  xhr2.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };

  xhr1.open("GET", "/temperature", true);
  xhr2.open("GET", "/humidity", true);

  xhr1.send();
  xhr2.send();
}

function updateLEDState() {
  var xhr3 = new XMLHttpRequest();
  var xhr4 = new XMLHttpRequest();
  var xhr5 = new XMLHttpRequest();
  var xhr6 = new XMLHttpRequest();

  xhr3.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ledState1").innerHTML = this.responseText;
    }
  };

  xhr4.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ledState2").innerHTML = this.responseText;
    }
  };

  xhr5.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ledState3").innerHTML = this.responseText;
    }
  };

  xhr6.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ledState4").innerHTML = this.responseText;
    }
  };

  xhr3.open("GET", "/ledState?led=1", true);
  xhr4.open("GET", "/ledState?led=2", true);
  xhr5.open("GET", "/ledState?led=3", true);
  xhr6.open("GET", "/ledState?led=4", true);

  xhr3.send();
  xhr4.send();
  xhr5.send();
  xhr6.send();
}

setInterval(updateData, 10000); // Her 10 saniyede bir sıcaklık ve nem verilerini güncelle
setInterval(updateLEDState, 1000); // Her 1 saniyede bir LED durumunu güncelle
</script>
</html>)rawliteral";

String processor(const String& var) {
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  else if(var == "LEDSTATE1"){
    return ledStates[0] ? "On" : "Off";
  }
  else if(var == "LEDSTATE2"){
    return ledStates[1] ? "On" : "Off";
  }
  else if(var == "LEDSTATE3"){
    return ledStates[2] ? "On" : "Off";
  }
  else if(var == "LEDSTATE4"){
    return ledStates[3] ? "On" : "Off";
  }
  return String();
}

void setup(){
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());

  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    int ledNum = request->arg("led").toInt();
    if (ledNum >= 1 && ledNum <= 4) {
      int index = ledNum - 1;
      ledStates[index] = !ledStates[index];
      digitalWrite(ledPins[index], ledStates[index] ? HIGH : LOW);
      request->send(200, "text/plain", ledStates[index] ? "1" : "0");
    }
  });
  server.on("/ledState", HTTP_GET, [](AsyncWebServerRequest *request){
    int ledNum = request->arg("led").toInt();
    if (ledNum >= 1 && ledNum <= 4) {
      int index = ledNum - 1;
      request->send_P(200, "text/plain", ledStates[index] ? "1" : "0");
    }
  });
  server.begin();
}

void loop() {
  // Ana döngüde ekstra bir işlem yapmanıza gerek yok.
}

