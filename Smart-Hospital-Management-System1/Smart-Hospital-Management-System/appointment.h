/* ============================================================
 * FILE        : appointment.h
 * MODULE      : Appointment Management Module
 * PROJECT     : Smart Hospital Management System
 * SUBJECT     : Data Structures Using C (DSC)
 * AUTHOR      : Kunal (Appointment Management & Billing)
 * DESCRIPTION : Header file — declares the Appointment
 *               structure, the Queue data structure, and all
 *               appointment management function prototypes.
 *
 * DATA STRUCTURE USED: Linear Queue (Array Implementation)
 *   A Queue follows the FIFO principle — First In, First Out.
 *   The first appointment booked is the first one to be served.
 *   This mirrors a real hospital waiting list perfectly.
 *
 *   Visual representation of the Queue:
 *
 *   front                          rear
 *     │                              │
 *     ▼                              ▼
 *   ┌─────┬─────┬─────┬─────┬─────┬─────┐
 *   │ A01 │ A02 │ A03 │ A04 │ A05 │ ... │
 *   └─────┴─────┴─────┴─────┴─────┴─────┘
 *     ↑                              ↑
 *   served                        booked
 *   first                          last
 *
 *   bookAppointment() → enqueue → rear moves right
 *   serveAppointment() → dequeue → front moves right
 * ============================================================ */

#ifndef APPOINTMENT_H    /* Header guard — prevents double inclusion */
#define APPOINTMENT_H    /* if this file is included multiple times  */

/* ── Constants ─────────────────────────────────────────────── */

#define MAX_APPOINTMENTS   100    /* Maximum queue capacity             */
#define MAX_DATE_LEN        20    /* Maximum length for date string     */
#define APPOINTMENT_FILE  "data/appointments.dat" /* Binary file path  */

/* ── Appointment Structure ──────────────────────────────────── */

/*
 * Appointment:
 *   Represents one appointment record stored in the Queue
 *   and persisted to disk via binary file I/O.
 *   Fixed-size fields ensure fread()/fwrite() work reliably.
 */
typedef struct
{
    int  appointmentId;       /* Unique numeric ID for this appointment  */
    int  patientId;           /* Foreign key — links to a Patient record */
    int  doctorId;            /* Foreign key — links to a Doctor record  */
    char date[MAX_DATE_LEN];  /* Appointment date, e.g. "25-07-2025"    */
} Appointment;

/* ── Queue Structure ────────────────────────────────────────── */

/*
 * AppointmentQueue:
 *   Encapsulates the entire Queue in one structure.
 *   Keeping 'queue', 'front', and 'rear' together avoids
 *   global variables and makes the module self-contained.
 *
 *   Fields:
 *     queue[MAX_APPOINTMENTS] — the fixed-size array of records
 *     front  — index of the element to be served next (dequeue end)
 *     rear   — index of the last element added    (enqueue end)
 *
 *   Initial state: front = 0, rear = -1
 *   Queue is empty when rear < front.
 *   Queue is full  when rear == MAX_APPOINTMENTS - 1.
 */
typedef struct
{
    Appointment queue[MAX_APPOINTMENTS]; /* Array backing the queue     */
    int         front;                   /* Points to next item to serve*/
    int         rear;                    /* Points to last booked item  */
} AppointmentQueue;

/* ── Function Prototypes ────────────────────────────────────── */

/*
 * initializeQueue()
 *   Sets front = 0 and rear = -1 to mark the queue as empty.
 *   Also loads any previously saved appointments from the
 *   binary file so records survive program restarts.
 *   Must be called ONCE before any other queue function.
 */
void initializeQueue(void);

/*
 * isQueueEmpty()
 *   Returns 1 (true) if there are no appointments in the queue.
 *   Returns 0 (false) if at least one appointment exists.
 *   Condition: rear < front
 */
int isQueueEmpty(void);

/*
 * isQueueFull()
 *   Returns 1 (true) if the queue has reached MAX_APPOINTMENTS.
 *   Returns 0 (false) if space remains for new appointments.
 *   Condition: rear == MAX_APPOINTMENTS - 1
 */
int isQueueFull(void);

/*
 * bookAppointment()
 *   Collects appointment details from the user and enqueues
 *   the new record (adds to rear). Validates for a duplicate
 *   appointment ID. Saves all records to appointments.dat.
 */
void bookAppointment(void);

/*
 * viewAppointments()
 *   Displays all current appointments in the queue as a
 *   formatted table, from front to rear (oldest to newest).
 */
void viewAppointments(void);

/*
 * serveAppointment()
 *   Dequeues the appointment at the front (FIFO rule).
 *   Displays its details, then saves the updated queue
 *   (with that record removed) back to appointments.dat.
 */
void serveAppointment(void);

#endif /* APPOINTMENT_H */