/* ============================================================
 * FILE        : doctor.h
 * MODULE      : Doctor Management Module
 * PROJECT     : Smart Hospital Management System
 * SUBJECT     : Data Structures Using C (DSC)
 * AUTHOR      : Nishant (Doctor Management)
 * DESCRIPTION : Header file — declares the Doctor structure
 *               and all doctor management function prototypes.
 * ============================================================ */

#ifndef DOCTOR_H       /* Header guard — prevents double inclusion */
#define DOCTOR_H       /* if this file is included multiple times  */

/* ── Constants ─────────────────────────────────────────────── */

#define MAX_DOCTOR_NAME_LEN   50   /* Maximum length for doctor name          */
#define MAX_SPECIALIZATION_LEN 50  /* Maximum length for specialization field  */
#define DOCTOR_FILE  "data/doctors.dat"  /* Binary data file path             */

/* ── Doctor Structure ───────────────────────────────────────── */

/*
 * Doctor:
 *   Represents a single doctor record stored in the system.
 *   All fields are fixed-size to allow reliable binary file I/O
 *   using fread() and fwrite() with sizeof(Doctor).
 */
typedef struct
{
    int  doctorId;                          /* Unique numeric ID for the doctor   */
    char name[MAX_DOCTOR_NAME_LEN];         /* Full name of the doctor            */
    char specialization[MAX_SPECIALIZATION_LEN]; /* Medical specialization field  */
} Doctor;

/* ── Function Prototypes ────────────────────────────────────── */

/*
 * addDoctor()
 *   Prompts the user to enter doctor details and saves the new
 *   record to doctors.dat. Validates for duplicate doctor IDs.
 */
void addDoctor(void);

/*
 * viewDoctors()
 *   Reads all doctor records from doctors.dat and displays
 *   them in a formatted table on the console.
 */
void viewDoctors(void);

/*
 * searchDoctor()
 *   Prompts for a doctor ID and searches doctors.dat for a
 *   matching record. Displays the full record if found.
 */
void searchDoctor(void);

/*
 * deleteDoctor()
 *   Prompts for a doctor ID and removes the matching record
 *   from doctors.dat using the temporary-file swap method.
 */
void deleteDoctor(void);

/*
 * doctorExists()
 *   Checks if a doctor record with the given ID exists in doctors.dat.
 *   Returns 1 if exists, 0 otherwise.
 */
int doctorExists(int id);

#endif /* DOCTOR_H */