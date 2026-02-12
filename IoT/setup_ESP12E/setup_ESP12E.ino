#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// access point configuration (The ESP will create this network)
const char* ssid = "MSP432_Radar";
const char* pass = "12345678";

ESP8266WebServer server(80);
String radarData = "0,-1"; // data received from MSP via serial (format: "angle,distance")

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

        function draw(){
            // fetch data from the ESP8266 server endpoint
            fetch('/data').then(r => r.text()).then(data => {
                const [angle, dist] = data.split(',');
                document.getElementById('dist').textContent = (dist == -1) ? "---" : dist;

                const centerX = canvas.width / 2;
                const centerY = canvas.height * 0.9;
                const radius = canvas.height * 0.8;

                // radar trail effect (semi-transparent overlay to create motion blur)
                ctx.fillStyle = 'rgba(0, 0, 0, 0.05)';
                ctx.fillRect(0, 0, canvas.width, canvas.height);

                // convert angle to radians for trigonometric functions
                const rad = angle * Math.PI / 180;

                // draw the scanning line
                ctx.beginPath();
                ctx.strokeStyle = '#0f0';
                ctx.lineWidth = 2;
                ctx.moveTo(centerX, centerY);

                /* * coordinate logic: 
                 * X positive (Math.cos) -> moves right to left
                 * Y negative (-Math.sin) -> points upwards on the screen
                 */
                ctx.lineTo(centerX + radius * Math.cos(rad), centerY - radius * Math.sin(rad));
                ctx.stroke();

                // draw detected object
                if(dist > 0 && dist < 400){
                    const rObj = (dist / 400) * radius; // map physical distance to canvas radius
                    ctx.beginPath();
                    ctx.fillStyle = 'red';
                    ctx.arc(centerX + rObj * Math.cos(rad), centerY - rObj * Math.sin(rad), 5, 0, Math.PI*2);
                    ctx.fill();
                }
            });
        }
        setInterval(draw, 50); // refresh rate for the radar UI
    </script>
</body>
</html>
)rawliteral";

void setup(){
  // the MSP must use the same Baud Rate (9600)
  Serial.begin(9600);
  
  // create a dedicated wifi network
  WiFi.softAP(ssid, pass);
  
  Serial.println("\nRadar Server Started");
  Serial.print("Radar IP Address: ");
  Serial.println(WiFi.softAPIP());

  // define server routes
  server.on("/", []() { server.send(200, "text/html", HTML_PAGE); });      // main Web Page
  server.on("/data", []() { server.send(200, "text/plain", radarData); }); // API endpoint for radar data
  server.begin();
}

void loop(){
  server.handleClient(); // handle incoming HTTP requests
  
  // check for incoming serial data from MSP432
  if (Serial.available()){
    radarData = Serial.readStringUntil('\n');
  }
}