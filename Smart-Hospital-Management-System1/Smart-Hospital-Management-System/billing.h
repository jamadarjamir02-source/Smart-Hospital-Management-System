/* ============================================================
 * FILE        : billing.h
 * MODULE      : Billing Module
 * PROJECT     : Smart Hospital Management System
 * SUBJECT     : Data Structures Using C (DSC)
 * AUTHOR      : Kunal (Appointment Management & Billing)
 * DESCRIPTION : Header file — declares the Bill structure
 *               and all billing function prototypes.
 *               Total amount is auto-calculated from the two
 *               fee components and stored with the record.
 * ============================================================ */

#ifndef BILLING_H      /* Header guard — prevents double inclusion */
#define BILLING_H      /* if this file is included multiple times  */

/* ── Constants ─────────────────────────────────────────────── */

#define BILL_FILE  "data/bills.dat"   /* Binary data file path */

/* ── Bill Structure ─────────────────────────────────────────── */

/*
 * Bill:
 *   Represents one billing record for a patient's visit.
 *   All numeric fields use fixed-size types (int, float) so
 *   that fread() / fwrite() can handle the struct as raw bytes
 *   with sizeof(Bill) — no padding surprises on a single build.
 *
 *   totalAmount is NOT entered by the user.
 *   It is computed inside generateBill() as:
 *       totalAmount = consultationFee + medicineFee
 *   and then stored with the record for quick retrieval.
 */
typedef struct
{
    int   billId;            /* Unique numeric ID for this bill          */
    int   patientId;         /* Links this bill to a Patient record      */
    float consultationFee;   /* Doctor consultation charges (INR)        */
    float medicineFee;       /* Medicine / pharmacy charges  (INR)       */
    float totalAmount;       /* Auto-calculated: consult + medicine      */
} Bill;

/* ── Function Prototypes ────────────────────────────────────── */

/*
 * generateBill()
 *   Collects bill details from the user, auto-calculates the
 *   total amount, validates for a duplicate bill ID and
 *   non-negative fees, then saves the record to bills.dat.
 */
void generateBill(void);

/*
 * viewBills()
 *   Reads all bill records from bills.dat and displays them
 *   in a formatted table with a running grand total at the end.
 */
void viewBills(void);

#endif /* BILLING_H */