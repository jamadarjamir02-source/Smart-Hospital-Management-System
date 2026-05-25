/* ============================================================
 * FILE        : patient.h
 * MODULE      : Patient Management Module
 * PROJECT     : Smart Hospital Management System
 * SUBJECT     : Data Structures Using C (DSC)
 * AUTHOR      : Siddharth (Team Leader & Patient Management)
 * DESCRIPTION : Header file — declares the Patient structure
 *               and all patient management function prototypes.
 * ============================================================ */

#ifndef PATIENT_H      /* Header guard — prevents double inclusion */
#define PATIENT_H      /* if this file is included multiple times  */

/* ── Constants ─────────────────────────────────────────────── */

#define MAX_NAME_LEN       50    /* Maximum length for patient name      */
#define MAX_GENDER_LEN     10    /* Maximum length for gender field       */
#define MAX_DIAGNOSIS_LEN  100   /* Maximum length for diagnosis field    */
#define PATIENT_FILE       "data/patients.dat"  /* Binary data file path */

/* ── Patient Structure ──────────────────────────────────────── */

/*
 * Patient:
 *   Represents a single patient record stored in the system.
 *   All fields are fixed-size to allow reliable binary file I/O
 *   using fread() and fwrite() with sizeof(Patient).
 */
typedef struct
{
    int  patientId;                    /* Unique numeric ID for the patient  */
    char name[MAX_NAME_LEN];           /* Full name of the patient           */
    int  age;                          /* Age of the patient (in years)      */
    char gender[MAX_GENDER_LEN];       /* Gender: Male / Female / Other      */
    char diagnosis[MAX_DIAGNOSIS_LEN]; /* Medical diagnosis or chief complaint*/
} Patient;

/* ── Function Prototypes ────────────────────────────────────── */

/*
 * addPatient()
 *   Prompts the user to enter patient details and saves the new
 *   record to patients.dat. Validates for duplicate patient IDs.
 */
void addPatient(void);

/*
 * viewPatients()
 *   Reads all patient records from patients.dat and displays
 *   them in a formatted table on the console.
 */
void viewPatients(void);

/*
 * searchPatient()
 *   Prompts for a patient ID and searches patients.dat for a
 *   matching record. Displays the full record if found.
 */
void searchPatient(void);

/*
 * deletePatient()
 *   Prompts for a patient ID and removes the matching record
 *   from patients.dat using the temporary-file swap method.
 */
void deletePatient(void);

/*
 * patientExists()
 *   Checks if a patient record with the given ID exists in patients.dat.
 *   Returns 1 if exists, 0 otherwise.
 */
int patientExists(int id);

#endif /* PATIENT_H */