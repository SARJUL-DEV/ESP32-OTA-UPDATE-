# 🚀 Universal OTA Web Updater (ESP32 + ESP8266)

A professional wireless firmware updater with a modern web interface compatible with both ESP32 and ESP8266 boards.

This project allows you to upload firmware files directly from your browser and flash them instantly without USB.

---

## ✅ Supported Boards

✔ ESP32  
✔ ESP8266  

Both versions use the **same UI design**, same workflow, same flashing process.  
Only firmware files differ depending on board.

---

## 🌐 Web Interface Preview

![UI Preview](esp32.ota.jpg)

---

## 📌 Overview

The device creates its own WiFi access point and hosts a built-in update dashboard.

After uploading firmware:

- firmware writes to flash
- verification runs
- device reboots automatically

No cables required after first upload.

---

## ✨ Features

- Wireless OTA flashing  
- Premium modern UI  
- Real-time progress indicator  
- Storage usage display  
- Maximum firmware size indicator  
- Safe flashing system  
- Auto restart after update  
- Lightweight code  

---

## 📡 Device Access

After boot device creates WiFi:

```
SSID: sarjul
Password: sarjul123
```

Open browser and visit:

```
192.168.4.1
```

---

## 📂 Project Structure

```
Universal-OTA/
│
├── ESP32-Version.ino
├── ESP8266-Version.ino
├── README.md
└── assets/
     └── ui.png
```

---

## ⚙️ Installation

### First Upload (Required once)

1. Open Arduino IDE  
2. Install board package  
3. Select correct board  
4. Upload code via USB  

After that all updates can be done wirelessly.

---

## 🔄 OTA Update Steps

1. Connect to device WiFi  
2. Open browser  
3. Enter IP address  
4. Choose firmware file  
5. Click **Upload & Flash**  
6. Wait for reboot  

Done.

---

## 🧠 Working Logic

```
Boot → AP Mode → Web Server → Upload → Flash → Restart
```

---

## 🛡 Safety Guidelines

- Do not power off during update  
- Only upload correct firmware file  
- Ensure stable connection  

---

## 🔧 Customization

| Setting | Location |
|--------|----------|
WiFi name | setup() |
Password | setup() |
UI text | HTML section |
LED pin | define section |

---

## 📜 License

Free for personal and commercial use.

---

## 👨‍💻 Developer

Sarjul
