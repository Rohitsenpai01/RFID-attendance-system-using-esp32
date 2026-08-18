# RFID Attendance Management System

Enterprise-ready RFID-based attendance system for educational institutions. This repository includes:

- A lightweight web dashboard (static HTML/CSS/JavaScript) that connects to Firebase Realtime Database and Firebase Auth.
- ESP32 firmware (PlatformIO) for MFRC522-based RFID readers that read tags, display feedback on an OLED, and write attendance records to Firebase.

This README documents the actual, tested functionality present in the repository (software/ + firmware/), how components interact, and detailed setup instructions.

---

## Summary of Functionalities

Frontend (software/)
- Static web dashboard (no React, no Node build required) providing:
  - Dashboard overview (total students, present today, recent scans, scanner status)
  - Live attendance monitor (real-time stream of today’s attendance with toasts)
  - Student management (register new students, edit existing student records)
  - Attendance log browser (load by date, search/filter, resolve student info)
  - CSV export for daily attendance
  - Firebase Authentication (email/password) guard for all pages
- The frontend uses the Firebase Web SDK (Realtime Database + Auth). Files are located under `software/`.

Firmware (firmware/)
- ESP32 firmware (C++ / PlatformIO) that:
  - Reads RFID tags using the MFRC522 module
  - Connects to Wi‑Fi and authenticates to Firebase Realtime Database (using Firebase_ESP_Client)
  - Maintains a device heartbeat under `/devices/<DEVICE_ID>` with `lastSeen` timestamp and `online` flag
  - Supports a registration flow controlled from the web dashboard via `/card_registration`:
    - When registration is requested, hardware writes a `pendingUid` on scans so the dashboard can auto-fill the registration form
    - The dashboard writes the new student record to `/students/<UID>` and clears the registration waiting flags
  - Marks attendance records at `/attendance/<YYYY-MM-DD>/<studentId>` with timestamp, name, rfidUid, device, and status
  - Handles duplicate detection (prevents double-marking for the same day)
  - Displays status and messages on a small SSD1306 OLED and uses LEDs / buzzer for feedback
  - Syncs time via NTP for reliable timestamps

Backend (Firebase Realtime Database + Auth)
- Realtime Database paths used:
  - `/students/<rfidUid>`: student records (rfidUid, studentId, name, rollNumber, active, ...)
  - `/attendance/<YYYY-MM-DD>/<studentId>`: attendance records for each day
  - `/devices/<deviceId>`: device metadata and heartbeat (deviceId, location, online, lastSeen)
  - `/card_registration`: coordination node used to enter/exit registration mode and exchange the scanned `pendingUid`
  - `/registration`: queued unknown-card entries the device may push so the web UI can show pending registrations
  - `/appInitialized`, `/settings` (initialized by the frontend on first run)
- Authentication relies on Firebase Auth (email/password). Protect pages by enabling Auth and applying Realtime Database rules in production.

---

## Project Structure (actual)

rfid-attendance-system/
├── software/                # Static web dashboard (HTML/CSS/JS)
│   ├── index.html           # Dashboard overview
│   ├── login.html           # Auth page
│   ├── live.html            # Live attendance monitor
│   ├── students.html        # Student management & registration flow
│   ├── attendance.html      # Attendance log viewer
│   ├── reports.html         # CSV export
│   ├── css/                 # Styles
│   └── js/                  # firebase-config.js, auth.js
├── firmware/                # ESP32 firmware (PlatformIO project)
│   ├── platformio.ini       # PlatformIO config
│   └── src/
│       ├── config.h         # WiFi / Firebase / device configuration (edit before building)
│       └── main.cpp         # Firmware logic (RFID reading, Firebase integration)
└── README.md                # (this file)

---

## Technology Stack (actual)
- Frontend: Vanilla HTML, CSS, ECMAScript modules (modern browser `type="module"`) using Firebase Web SDK (Realtime Database + Auth)
- Database & Auth: Firebase Realtime Database and Firebase Auth
- Embedded firmware: ESP32, PlatformIO, MFRC522 RFID library, Firebase_ESP_Client, Adafruit_SSD1306 (OLED), Arduino core

---

## Quickstart — Frontend (software/)

1. Create a Firebase project with Realtime Database (Native mode) and enable Email/Password Authentication.
2. Copy your Firebase config into `software/js/firebase-config.js` (the file currently contains example/demo keys — replace them with your own). The file exports `db` (Realtime DB) and `auth` used by the pages.

   Example config fields to replace:
   - apiKey, authDomain, databaseURL, projectId, storageBucket, messagingSenderId, appId

3. Serve the `software/` folder over HTTP. ES module imports (Firebase CDN modules) require a web server; opening files via `file://` will fail.

   Options:
   - Python: `python -m http.server 8000` from inside the `software/` folder (Python 3)
   - Node: `npx http-server -c-1 .` (install http-server if needed)
   - Use Firebase Hosting or any static hosting service

4. Open `http://localhost:8000/login.html` (or the server URL) and sign in with a Firebase user (create one via Firebase Console or implement a signup flow).

Notes:
- The frontend initializes a minimal DB structure on first load (appInitialized & settings). If the client lacks permission to write to the DB, this initialization will be skipped and logged to console.
- The dashboard requires authenticated access; make sure your Firebase DB rules permit the authenticated user to read/write the expected paths.

---

## Quickstart — Firmware (firmware/)

1. Edit firmware/src/config.h to set the following values for your environment (DO NOT commit secrets to a public repo):
   - WIFI_SSID / WIFI_PASSWORD
   - API_KEY (Firebase REST/API key for your project)
   - DATABASE_URL (Realtime Database URL)
   - USER_EMAIL / USER_PASSWORD (a service user or device user — alternatively create a service token workflow)
   - DEVICE_ID / DEVICE_LOCATION
   - Pin definitions (if your hardware wiring differs)

2. Install PlatformIO (recommended: VSCode with PlatformIO extension) or use PlatformIO Core CLI.

3. Build & upload to the ESP32
   - From VSCode PlatformIO: Build and Upload project
   - From CLI inside `firmware/`: `platformio run --target upload`

4. Open serial monitor to view logs: `platformio device monitor` or use VSCode Serial Monitor. The firmware prints Wi‑Fi, Firebase and scan events.

Firmware behaviors to be aware of:
- On start the device syncs time from NTP — time is used for date/time fields in attendance records.
- Every ~60 seconds the device updates `/devices/<DEVICE_ID>` with `online` and `lastSeen` to support online/offline display in the dashboard.
- On RFID scan:
  - If `/card_registration/waiting` is true, device writes `/card_registration/pendingUid = <UID>` and does not mark attendance. The web UI is expected to read pendingUid and create the `/students/<UID>` record.
  - If not in registration mode, device checks `/students/<UID>`:
    - If no student found: write a pending registration object to `/registration` and show 'Card Not Found' on OLED
    - If student exists and `active` is true: check `/attendance/<YYYY-MM-DD>/<studentId>` to avoid duplicates, then write the attendance record

---

## Database Schema (Realtime Database keys)
- students/{rfidUid}
  - rfidUid, studentId, name, rollNumber, active (true/false), ...
- attendance/{YYYY-MM-DD}/{studentId}
  - studentId, rfidUid, name, rollNumber, date, time, timestamp, deviceId, location, status
- devices/{deviceId}
  - deviceId, location, online (bool), lastSeen (ms since epoch)
- card_registration
  - waiting (bool), pendingUid (string|null), textSize (int)
- registration (queued unknown cards) — object containing uid + timestamp
- appInitialized / settings

---

## Security & Production Recommendations
- Do NOT commit API keys, user passwords, or service account credentials to the repository. The repo currently contains example keys; rotate them and replace with your own.
- Configure strict Realtime Database rules to allow only authenticated users to read/write the expected paths and enforce least privilege. Example:

  - Allow the device service user to write to `/attendance`, `/devices`, and `/registration` only.
  - Allow admin users (dashboard) to read/write `/students`, `/attendance`, `/card_registration`, and `/devices`.

- Use HTTPS when hosting the dashboard.
- Consider using Firebase Cloud Functions or a backend service for advanced validation, server-side rules, or to avoid embedding writable keys in devices.

---

## Troubleshooting

Common issues and fixes:

- Wi‑Fi not connecting:
  - Verify SSID/password in `firmware/src/config.h` and ensure device is in Wi‑Fi range.
  - Serial logs show connection attempts.

- Firebase authentication or DB errors on device:
  - Check USER_EMAIL / USER_PASSWORD in `config.h`. Ensure the user exists and can sign in via the Realtime Database client.
  - Confirm `DATABASE_URL` matches the Firebase project (no trailing slash).
  - Review serial output; `fbdo.errorReason()` is logged on failures.

- Dashboard fails to load or modules fail:
  - Serve `software/` over HTTP (modules don't work over file://).
  - Open browser console for CORS/Firebase errors.

- Duplicate attendance entries:
  - Firmware checks existing path `/attendance/<date>/<studentId>` and avoids duplicate writes. If duplicates exist, confirm device/system clock and timestamp handling.

- Time sync failures:
  - Device attempts NTP sync at startup and falls back after several retries. Ensure device can reach NTP servers and DNS is working.

- Build/upload firmware errors:
  - Use PlatformIO in VSCode for easiest experience. Check `platformio.ini` for correct board settings.

---

## Development & Contribution
- Make changes on a feature branch and open a pull request describing the change.
- For frontend changes, edit the files under `software/` and test by serving the directory.
- For firmware changes, edit `firmware/src/` and test by building & uploading to an ESP32.

---

## License
- License: To be determined. Replace with your project license before publishing.

---

## Support
- For issues: open a GitHub issue in this repository (or contact the development team listed in your internal docs).

---

**Last Updated:** 2026-08-18
**Status:** Active development
