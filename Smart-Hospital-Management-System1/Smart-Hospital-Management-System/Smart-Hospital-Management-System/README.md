# 🏥 Smart Hospital Management System

<div align="center">

![C Language](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Subject](https://img.shields.io/badge/Subject-Data%20Structures%20Using%20C-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen?style=for-the-badge)
![Team Project](https://img.shields.io/badge/Type-Team%20Project-orange?style=for-the-badge)

*A console-based Hospital Management System built in C, demonstrating core Data Structure concepts through real-world application.*

</div>

---

## 📋 Table of Contents

- [Project Description](#-project-description)
- [Objectives](#-objectives)
- [Features](#-features)
- [Technologies Used](#-technologies-used)
- [Data Structures Used](#-data-structures-used)
- [Project Modules](#-project-modules)
- [Folder Structure](#-folder-structure)
- [Team Members & Roles](#-team-members--roles)
- [Future Enhancements](#-future-enhancements)
- [Conclusion](#-conclusion)

---

## 📖 Project Description

The **Smart Hospital Management System** is a console-based application developed in the **C programming language** as part of the **Data Structures Using C (DSC)** subject curriculum. It simulates a real-world hospital environment by digitising the management of patients, doctors, appointments, and billing — all within a structured, menu-driven interface.

The system demonstrates practical application of fundamental data structures such as **Queues**, **Structures**, and **Arrays**, combined with **File Handling** to ensure persistent data storage across sessions. The project follows a modular programming approach, with each module independently developed and integrated into a unified system.

This project was collaboratively built by a four-member team, with each member owning a dedicated module — reflecting real-world software development practices like role separation, version control, and documentation.

---

## 🎯 Objectives

- ✅ Apply core **Data Structure** concepts in a practical, real-world scenario
- ✅ Implement **Queue-based** appointment scheduling for efficient management
- ✅ Use **File Handling** for persistent storage of patients, doctors, and appointments
- ✅ Design a **modular system** with separate, reusable components for each hospital function
- ✅ Understand and implement **Structures** to represent real-world entities like Patient and Doctor
- ✅ Practise **team collaboration** using GitHub for version control and integration
- ✅ Produce clean, well-documented C code following good programming principles

---

## ✨ Features

### 👤 Patient Management
| Feature | Description |
|---|---|
| ➕ Add Patient | Register a new patient with ID, name, age, and diagnosis |
| 📋 View Patients | Display all registered patient records |
| 🔍 Search Patient | Search for a patient by ID or name |
| 🗑️ Delete Patient | Remove a patient record from the system |

### 🩺 Doctor Management
| Feature | Description |
|---|---|
| ➕ Add Doctor | Register a new doctor with ID, name, and specialisation |
| 📋 View Doctors | Display all registered doctor records |
| 🔍 Search Doctor | Search for a doctor by ID or specialisation |
| 🗑️ Delete Doctor | Remove a doctor record from the system |

### 📅 Appointment & Billing
| Feature | Description |
|---|---|
| 📌 Book Appointment | Queue a patient-doctor appointment using Queue data structure |
| 📋 View Appointments | Display all pending and scheduled appointments |
| 💰 Generate Bill | Auto-generate a billing summary for a patient's consultation |

### 💾 File Handling
- All records (patients, doctors, appointments) are **saved to `.dat` files**
- Data **persists across program runs** — no data loss on exit
- Records are **read from files on startup** to restore the previous state

---

## 🛠️ Technologies Used

| Technology | Purpose |
|---|---|
| **C Language** | Core programming language for the entire system |
| **GCC Compiler** | Compiling the C source files |
| **File I/O (stdio.h)** | Persistent data storage using binary/text files |
| **Standard Libraries** | `stdlib.h`, `string.h`, `ctype.h` for utility functions |
| **GitHub** | Version control, collaboration, and repository management |
| **VS Code / Code::Blocks** | IDE used for development and testing |

---

## 🧠 Data Structures Used

### 1. 🏗️ Structures (`struct`)
Used to represent real-world entities:
- `struct Patient` — stores patient ID, name, age, gender, and diagnosis
- `struct Doctor` — stores doctor ID, name, specialisation, and availability
- `struct Appointment` — stores appointment ID, patient ID, doctor ID, and date

### 2. 📦 Arrays
- Arrays of structures are used to store multiple patient and doctor records in memory
- Used for iterating, searching, and displaying records efficiently

### 3. 🔁 Queue (Linear Queue)
- Implemented manually using an array with `front` and `rear` pointers
- Used for **Appointment Management** — appointments are queued and served in FIFO order
- Demonstrates the real-world analogy of a hospital's appointment waiting list

### 4. 📂 File Handling
- Binary file read/write operations (`fread`, `fwrite`) for saving and loading records
- Separate `.dat` files for patients, doctors, and appointments
- Ensures data persistence between program sessions

---

## 🧩 Project Modules

```
┌─────────────────────────────────────────────────────────────┐
│               Smart Hospital Management System               │
├───────────────┬──────────────┬──────────────┬───────────────┤
│    Patient    │    Doctor    │ Appointment  │    Billing    │
│   Management  │  Management  │  & Queue     │   Module      │
│   (Siddharth) │  (Nishant)   │  (Kunal)     │   (Kunal)     │
└───────────────┴──────────────┴──────────────┴───────────────┘
                          │
              ┌───────────┴───────────┐
              │   File Handling &     │
              │   Data Persistence    │
              │   (Shared Utility)    │
              └───────────────────────┘
```

### Module Breakdown

- **`patient.c` / `patient.h`** — All patient-related operations: add, view, search, delete
- **`doctor.c` / `doctor.h`** — All doctor-related operations: add, view, search, delete
- **`appointment.c` / `appointment.h`** — Queue-based appointment booking and viewing
- **`billing.c` / `billing.h`** — Bill generation and display for patients
- **`filehandler.c` / `filehandler.h`** — Centralised file read/write operations
- **`main.c`** — Entry point; main menu loop, module integration

---

## 📁 Folder Structure

```
Smart-Hospital-Management-System/
│
├── 📄 README.md                  # Project documentation
├── 📄 Makefile                   # Build automation (optional)
│
├── 📂 src/                       # Source files (.c)
│   ├── main.c                    # Entry point & main menu
│   ├── patient.c                 # Patient management logic
│   ├── doctor.c                  # Doctor management logic
│   ├── appointment.c             # Appointment & queue logic
│   ├── billing.c                 # Billing generation logic
│   └── filehandler.c             # File I/O utility functions
│
├── 📂 include/                   # Header files (.h)
│   ├── patient.h                 # Patient struct & function declarations
│   ├── doctor.h                  # Doctor struct & function declarations
│   ├── appointment.h             # Appointment struct & queue declarations
│   ├── billing.h                 # Billing function declarations
│   └── filehandler.h             # File handler declarations
│
├── 📂 data/                      # Data files for persistence
│   ├── patients.dat              # Stored patient records
│   ├── doctors.dat               # Stored doctor records
│   └── appointments.dat         # Stored appointment records
│
└── 📂 docs/                      # Additional documentation
    ├── project_report.pdf        # Final project report
    └── flowchart.png             # System flowchart diagram
```

---

## 👨‍💻 Team Members & Roles

<div align="center">

| # | Name | Role | Module Owned |
|---|------|------|--------------|
| 1 | **Siddharth** 👑 | Team Leader | Patient Management Module |
| 2 | **Nishant** | Developer | Doctor Management Module |
| 3 | **Kunal** | Developer | Appointment Management & Billing Module |
| 4 | **Jamir** | DevOps & QA | GitHub Management, Integration, Testing & Documentation |

</div>

### 🔑 Role Details

- **Siddharth** *(Team Leader & Patient Management)*
  - Led project planning, task delegation, and milestone tracking
  - Implemented `patient.c` — add, view, search, and delete patient records
  - Designed the `Patient` structure and related file I/O logic

- **Nishant** *(Doctor Management)*
  - Implemented `doctor.c` — add, view, search, and delete doctor records
  - Designed the `Doctor` structure and specialisation-based search
  - Coordinated with Kunal for doctor-appointment linking

- **Kunal** *(Appointment Management & Billing)*
  - Implemented the Queue data structure for appointment scheduling (`appointment.c`)
  - Developed the billing module (`billing.c`) for generating patient invoices
  - Integrated patient and doctor data into the appointment and billing flow

- **Jamir** *(GitHub Management, Integration, Testing & Documentation)*
  - Managed the GitHub repository — branches, pull requests, and merge reviews
  - Integrated all individual modules into the final working system (`main.c`)
  - Conducted end-to-end testing and bug fixes across all modules
  - Authored the README, project report, and code comments

---

## 🚀 Getting Started

### Prerequisites
- GCC Compiler installed (`gcc --version` to verify)
- Any terminal or command prompt (Linux / Windows / macOS)

### Compilation & Run

```bash
# Clone the repository
git clone https://github.com/your-username/Smart-Hospital-Management-System.git

# Navigate to the project directory
cd Smart-Hospital-Management-System

# Compile all source files
gcc src/main.c src/patient.c src/doctor.c src/appointment.c src/billing.c src/filehandler.c -I include/ -o hospital

# Run the program
./hospital
```

> **Note:** On Windows, replace `./hospital` with `hospital.exe`

---

## 🔮 Future Enhancements

- [ ] 🖥️ **Graphical User Interface (GUI)** — Migrate to a GUI using GTK or a web-based front end
- [ ] 🔐 **User Authentication** — Add login system with Admin and Staff roles
- [ ] 🗄️ **Database Integration** — Replace file handling with SQLite for scalable storage
- [ ] 📊 **Analytics Dashboard** — View patient trends, most consulted doctors, and revenue reports
- [ ] 💊 **Pharmacy Module** — Track medicine inventory and prescriptions per patient
- [ ] 🔔 **Appointment Reminders** — SMS or email notifications via API integration
- [ ] 🏥 **Ward & Bed Management** — Track bed availability and patient ward assignments
- [ ] 📱 **Cross-platform Build** — Makefile / CMake setup for Linux, Windows, and macOS

---

## ✅ Conclusion

The **Smart Hospital Management System** successfully demonstrates how fundamental **Data Structures** concepts — Queues, Structures, Arrays, and File Handling — can be applied to solve a real-world problem in a structured and modular way.

Through this project, our team gained hands-on experience in:
- Designing and implementing **custom data structures** in C
- Applying **FIFO Queue logic** to model real appointment workflows
- Using **File I/O** for persistent, session-independent data management
- Practising **modular programming** by separating concerns across multiple `.c` and `.h` files
- Collaborating as a team using **GitHub** for version control and code integration

This project reflects both our academic learning and our ability to work as a cohesive development team — from planning and coding to testing and documentation.

---

<div align="center">

**📚 Data Structures Using C (DSC) — Team Project**

Made with ❤️ by **Siddharth, Nishant, Kunal & Jamir**

*© 2025 Smart Hospital Management System. All rights reserved.*

</div>