#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configurazione Access Point (L'ESP creerà questa rete)
const char* ssid = "MSP432_Radar";
const char* pass = "12345678";

ESP8266WebServer server(80);
String radarData = "0,-1"; // Dati ricevuti dalla MSP432 via Seriale

const char* HTML_PAGE = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>IoT Radar MSP432</title>
    <style>
        body { background: #000; color: #0f0; font-family: 'Courier New', monospace; margin: 0; overflow: hidden; }
        #distanceBox { position: absolute; top: 10px; right: 20px; font-size: 24px; border: 1px solid #0f0; padding: 10px; }
        canvas { display: block; margin: auto; }
    </style>
</head>
<body>
    <div id="distanceBox">DIST: <span id="dist">--</span> cm</div>
    <canvas id="radarCanvas"></canvas>

    <script>
        const canvas = document.getElementById('radarCanvas');
        const ctx = canvas.getContext('2d');
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;

        function draw() {
            fetch('/data').then(r => r.text()).then(data => {
                const [angle, dist] = data.split(',');
                document.getElementById('dist').textContent = (dist == -1) ? "---" : dist;

                const centerX = canvas.width / 2;
                const centerY = canvas.height * 0.9;
                const radius = canvas.height * 0.8;

                // Sfumatura radar (scia)
                ctx.fillStyle = 'rgba(0, 0, 0, 0.05)';
                ctx.fillRect(0, 0, canvas.width, canvas.height);

                // Disegno raggio
                const rad = (angle - 180) * Math.PI / 180;
                ctx.beginPath();
                ctx.strokeStyle = '#0f0';
                ctx.lineWidth = 2;
                ctx.moveTo(centerX, centerY);
                ctx.lineTo(centerX + radius * Math.cos(rad), centerY + radius * Math.sin(rad));
                ctx.stroke();

                // Disegno oggetto
                if(dist > 0 && dist < 400) {
                    const rObj = (dist / 400) * radius;
                    ctx.beginPath();
                    ctx.fillStyle = 'red';
                    ctx.arc(centerX + rObj * Math.cos(rad), centerY + rObj * Math.sin(rad), 5, 0, Math.PI*2);
                    ctx.fill();
                }
            });
        }
        setInterval(draw, 50); // Aggiornamento rapido
    </script>
</body>
</html>
)rawliteral";

void setup() {
  // Nota: La MSP432 deve usare lo stesso Baud Rate
  Serial.begin(9600);
  
  // Creiamo una rete WiFi dedicata (Access Point)
  WiFi.softAP(ssid, pass);
  
  Serial.println("\nRadar Server Avviato");
  Serial.print("IP del Radar: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() { server.send(200, "text/html", HTML_PAGE); });
  server.on("/data", []() { server.send(200, "text/plain", radarData); });
  server.begin();
}

void loop() {
  server.handleClient();
  if (Serial.available()) {
    radarData = Serial.readStringUntil('\n');
  }
}