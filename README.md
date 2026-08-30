# 🚗 SlotSpot — IoT-Enabled Smart Parking System

<div align="center">

### Smart Parking Management using ESP32, RFID, Firebase & Android

<p>
  <img src="https://img.shields.io/badge/ESP32-IoT-blue?style=for-the-badge" alt="ESP32"/>
  <img src="https://img.shields.io/badge/Android-Java-green?style=for-the-badge&logo=android" alt="Android"/>
  <img src="https://img.shields.io/badge/Firebase-Backend-orange?style=for-the-badge&logo=firebase" alt="Firebase"/>
  <img src="https://img.shields.io/badge/RFID-Access%20Control-purple?style=for-the-badge" alt="RFID"/>
</p>

</div>

---

## 📌 Overview

**SlotSpot** is an IoT-enabled smart parking system developed as a
Final Year Project to provide a more efficient approach to parking
space management.

The system combines **ESP32, RFID, sensors, an Android application,
and Firebase** to monitor parking slots and manage parking activity.

The system is designed to support:

- Real-time parking-slot monitoring
- Automated slot detection
- Vehicle entry and exit tracking
- RFID-based identification
- Mobile-based monitoring and management
- Cloud-connected data management

---

## 🎯 Problem Statement

Traditional parking systems can make it difficult to determine the
availability of parking spaces and efficiently manage vehicle entry,
exit, and parking information.

SlotSpot addresses these challenges by integrating **IoT hardware
with a mobile application and cloud-based backend** to provide
centralized and real-time parking management.

---

## 💡 Proposed Solution

SlotSpot uses an IoT-based architecture in which sensors and
controllers monitor the physical parking environment.

The collected information is communicated through the system's
software and cloud components, allowing parking information to be
managed and accessed through the Android application.

### System Concept

```text
Parking Sensors / RFID
          │
          ▼
        ESP32
          │
          ▼
     Data / Internet
          │
          ▼
       Firebase
          │
          ▼
    Android Application
          │
          ▼
 Parking Monitoring & Management
