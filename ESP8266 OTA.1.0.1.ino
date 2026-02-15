#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Updater.h>

ESP8266WebServer server(80);

const char* apSSID = "Sarjul";
const char* apPassword = "Sarjul123";

#define LED_PIN 2
bool flashing = false;

String buildPageHTML(const IPAddress &ip) {

  FSInfo fs_info;
  SPIFFS.info(fs_info);

  size_t total = fs_info.totalBytes;
  size_t used  = fs_info.usedBytes;
  size_t free  = total - used;

  size_t maxFirmware = ESP.getFreeSketchSpace();

  int pct = (int)((float)used / total * 100);

  String html = R"rawliteral(
<!doctype html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Sarjul OTA</title>
<style>
body{font-family:Arial;margin:0;background:#f1f3f6;color:#222}
.container{max-width:420px;margin:auto;padding:18px}
.card{background:#fff;padding:16px;border-radius:12px;box-shadow:0 2px 8px rgba(0,0,0,0.12);margin-bottom:18px}
h1{text-align:center;font-size:22px;margin-bottom:10px}
.info-text{text-align:center;font-size:14px;color:#444}
.barbox{background:#ddd;border-radius:6px;height:10px;overflow:hidden;margin-top:4px}
.usagebar{height:100%;background:#007bff;width:)rawliteral" + String(pct) + R"rawliteral(%;}
button{
 width:100%;padding:12px;margin-top:12px;font-size:16px;font-weight:600;border:none;
 border-radius:12px;cursor:pointer;background:linear-gradient(135deg,#6BFF8A,#22CC55);
 color:#0d3a12;box-shadow:0 4px 14px rgba(0,180,60,0.35);transition:0.25s ease;}
button.red{background:#ff7777;color:#520000;box-shadow:0 4px 14px rgba(255,0,0,0.35);}
button:disabled{opacity:0.6;transform:none;cursor:not-allowed;}
#status{margin-top:12px;padding:10px;background:#eef;border-radius:6px;text-align:center;font-size:14px}
#progress{width:100%;height:10px;background:#ddd;border-radius:6px;margin-top:10px}
#bar{height:100%;width:0%;background:#007bff;border-radius:6px}
.small{font-size:13px;color:#666}
</style>
</head>
<body>
<div class="container">

<div class="card">
<h1>Sarjul Device</h1>
<div class="info-text">IP: <b>)rawliteral" + ip.toString() + R"rawliteral(</b></div>
<div class="small">SPIFFS Storage:</div>
Total: )rawliteral" + String(total/1024) + R"rawliteral( KB<br>
Used: )rawliteral" + String(used/1024) + R"rawliteral( KB<br>
Free: )rawliteral" + String(free/1024) + R"rawliteral( KB<br>
<div class="barbox"><div class="usagebar"></div></div>
<div class="info-text">Used: )rawliteral" + String(pct) + R"rawliteral(%</div>
</div>

<div class="card">
<div class="small">Max firmware allowed (bytes): <b>)rawliteral" + String(maxFirmware) + R"rawliteral(</b></div><br>
<input type="file" id="firm" style="width:100%;margin-bottom:10px">
<button id="flashBtn" onclick="upload()">Upload & Flash</button>
<div id="status">Ready</div>
<div id="progress"><div id="bar"></div></div>
</div>

</div>

<script>
const maxfw = )rawliteral" + String(maxFirmware) + R"rawliteral(;
const firmInput = document.getElementById('firm');
const flashBtn = document.getElementById('flashBtn');
const status = document.getElementById('status');
const bar = document.getElementById('bar');

firmInput.onchange = function() {
  const f = this.files[0];
  if(!f){ flashBtn.classList.remove('red'); status.innerText='Ready'; return; }
  if(f.size > maxfw){
    flashBtn.classList.add('red');
    flashBtn.disabled = true;
    status.innerText = '❌ File too large — cannot flash.';
  } else {
    flashBtn.classList.remove('red');
    flashBtn.disabled = false;
    status.innerText = '✅ File OK. Ready to Flash.';
  }
};

function setUploadingState(on){
  if(on){
    flashBtn.disabled = true;
    firmInput.disabled = true;
  } else {
    flashBtn.disabled = false;
    firmInput.disabled = false;
  }
}

function upload(){
  const f = firmInput.files[0];
  if(!f){ status.innerText='Select .bin file'; return; }
  if(f.size > maxfw){ status.innerText='❌ File too large!'; return; }

  const form = new FormData(); form.append('firmware', f);
  const xhr = new XMLHttpRequest();
  xhr.open('POST','/update',true);

  xhr.upload.onprogress = function(e){
    if(e.lengthComputable){
      const pct = Math.round((e.loaded / e.total) * 100);
      bar.style.width = pct + '%';
      status.innerText = 'Uploading: ' + pct + '%';
    }
  };

  xhr.onload = function(){
    const resp = xhr.responseText || '';
    if(resp.trim() == 'FLASH_OK'){
      status.innerText = '✅ Flash Successful — Rebooting...';
      bar.style.width = '100%';
      setTimeout(function(){ location.reload(true); }, 1500);
    } else {
      status.innerText = '❌ Flash Failed';
      setUploadingState(false);
    }
  };

  xhr.onerror = function(){
    status.innerText = '❌ Upload error';
    setUploadingState(false);
  };

  setUploadingState(true);
  status.innerText = 'Starting upload...';
  xhr.send(form);
}
</script>

</body>
</html>
)rawliteral";

  return html;
}

void handleUpdate() {
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    flashing = true;
    digitalWrite(LED_PIN, HIGH);

    // Prefer reported upload size; fall back to free sketch space
    size_t sketchSize = upload.totalSize;
    if(sketchSize == 0) sketchSize = ESP.getFreeSketchSpace();

    Serial.printf("Update start. Size: %u\n", (unsigned)sketchSize);
    if(!Update.begin(sketchSize)){           // start with expected size
      Update.printError(Serial);
      server.send(500, "text/plain", "UPDATE_BEGIN_FAIL");
      flashing = false;
      digitalWrite(LED_PIN, LOW);
    }
  }
  else if(upload.status == UPLOAD_FILE_WRITE){
    // toggle LED occasionally to show activity (do not flood)
    static unsigned long last = 0;
    if(millis() - last > 120){
      last = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    size_t written = Update.write(upload.buf, upload.currentSize);
    if(written != upload.currentSize){
      Update.printError(Serial);
    }
  }
  else if(upload.status == UPLOAD_FILE_END){
    flashing = false;
    digitalWrite(LED_PIN, LOW);
    Serial.printf("Update finished. Total: %u\n", (unsigned)upload.totalSize);

    if(Update.end(true)){
      Serial.println("Update success, restarting.");
      server.send(200, "text/plain", "FLASH_OK");
      delay(700);
      ESP.restart();
    } else {
      Update.printError(Serial);
      server.send(500, "text/plain", "FLASH_FAIL");
    }
  }
  else if(upload.status == UPLOAD_FILE_ABORTED){
    flashing = false;
    Update.end();
    digitalWrite(LED_PIN, LOW);
    Serial.println("Update aborted");
    server.send(500, "text/plain", "FLASH_ABORT");
  }
}

void setup(){
  Serial.begin(115200);
  SPIFFS.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.softAP(apSSID, apPassword);

  server.on("/", HTTP_GET, [](){ server.send(200, "text/html", buildPageHTML(WiFi.softAPIP())); });
  server.on("/update", HTTP_POST, [](){}, handleUpdate);

  server.begin();
  Serial.println("AP started. Connect to SSID: Sarjul (http://192.168.4.1)");
}

void loop(){
  server.handleClient();
  if(!flashing) digitalWrite(LED_PIN, LOW);
}
