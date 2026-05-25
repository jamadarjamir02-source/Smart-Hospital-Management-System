/* ============================================================
 * FILE        : appointment.c
 * MODULE      : Appointment Management Module
 * PROJECT     : Smart Hospital Management System
 * SUBJECT     : Data Structures Using C (DSC)
 * AUTHOR      : Kunal (Appointment Management & Billing)
 * DESCRIPTION : Implementation of all appointment management
 *               functions using a manually built Linear Queue
 *               (array-based). Appointments are persisted via
 *               binary file I/O in data/appointments.dat.
 *
 * ── QUEUE CONCEPT RECAP ────────────────────────────────────
 *
 *  A Queue is a linear data structure that follows FIFO order:
 *  First In → First Out.
 *
 *  Two key operations:
 *    ENQUEUE (bookAppointment):
 *      Add a new item at the REAR of the queue.
 *      rear = rear + 1;
 *      queue[rear] = newItem;
 *
 *    DEQUEUE (serveAppointment):
 *      Remove the item at the FRONT of the queue.
 *      servedItem = queue[front];
 *      front = front + 1;
 *
 *  State checks:
 *    Empty : rear < front  (no items between front and rear)
 *    Full  : rear == MAX_APPOINTMENTS - 1  (array exhausted)
 *
 *  Example with 3 bookings and 1 serve:
 *
 *    After booking A01, A02, A03:
 *      front=0  rear=2
 *      [ A01 | A02 | A03 | ... ]
 *        ↑front        ↑rear
 *
 *    After serving A01:
 *      front=1  rear=2
 *      [ ---- | A02 | A03 | ... ]
 *               ↑front  ↑rear
 *
 * ============================================================ */

#include <stdio.h>       /* printf, scanf, fopen, fclose, fread, fwrite  */
#include <string.h>      /* strcspn() — strips newline from fgets output  */
#include <stdlib.h>      /* exit()                                         */
#include "appointment.h" /* Appointment, AppointmentQueue, prototypes      */
#include "patient.h"     /* patientExists()                                */
#include "doctor.h"      /* doctorExists()                                 */

/* ── Module-Level Queue Instance ────────────────────────────── */
/*
 * 'aq' is the single AppointmentQueue instance for this module.
 * Declared at file scope but NOT as a global — it is 'static',
 * meaning it is invisible outside this .c file.
 * Other modules cannot accidentally access or corrupt it.
 */
static AppointmentQueue aq;

/* ── Internal Helper Prototypes ─────────────────────────────── */
static void printAppointmentDivider(void);
static void printAppointmentTableHeader(void);
static void printAppointmentRow(const Appointment *a);
static int  appointmentIdExists(int id);
static void saveAppointmentsToFile(void);
static void loadAppointmentsFromFile(void);
static void clearApptInputBuffer(void);

/* ============================================================
 *  SECTION 1 — QUEUE INITIALISATION
 * ============================================================ */

/*
 * initializeQueue()
 * -----------------
 * Resets the queue to its empty state and then loads any
 * previously saved appointments from disk into the queue array.
 *
 * Must be called once at program startup before any other
 * appointment function is used.
 *
 * Initial state:
 *   front = 0    → next item to serve is at index 0
 *   rear  = -1   → no item has been added yet
 *   (rear < front means the queue is empty)
 */
void initializeQueue(void)
{
    aq.front = 0;
    aq.rear  = -1;

    /*
     * Reload appointments saved from a previous session.
     * This gives the queue data persistence — appointments
     * survive program restarts just like patient/doctor records.
     */
    loadAppointmentsFromFile();
}

/* ============================================================
 *  SECTION 2 — QUEUE STATE CHECKS
 * ============================================================ */

/*
 * isQueueEmpty()
 * --------------
 * The queue is empty when rear is less than front.
 * This happens in two situations:
 *   1. After initializeQueue() — rear=-1, front=0 → -1 < 0.
 *   2. After every element has been dequeued (served).
 *
 * Returns: 1 if empty, 0 if at least one appointment exists.
 */
int isQueueEmpty(void)
{
    return (aq.rear < aq.front);
}

/*
 * isQueueFull()
 * -------------
 * The queue is full when rear has reached the last valid index
 * of the backing array (MAX_APPOINTMENTS - 1).
 * No more enqueue operations are possible at that point.
 *
 * Returns: 1 if full, 0 if space is available.
 */
int isQueueFull(void)
{
    return (aq.rear == MAX_APPOINTMENTS - 1);
}

/* ============================================================
 *  SECTION 3 — BOOK APPOINTMENT (ENQUEUE)
 * ============================================================ */

/*
 * bookAppointment()
 * -----------------
 * Collects a new appointment's details, validates the input,
 * checks for a duplicate appointment ID, then ENQUEUEs the
 * record by incrementing rear and storing the data there.
 *
 * ENQUEUE steps:
 *   1. Check that the queue is not full (overflow guard).
 *   2. Increment rear: aq.rear = aq.rear + 1
 *   3. Store the new record: aq.queue[aq.rear] = newAppointment
 *   4. Persist the updated queue to disk.
 */
void bookAppointment(void)
{
    Appointment newAppt; /* Local variable for the new record */

    printf("  +--------------------------------------+\n");
    printf("  |        BOOK NEW APPOINTMENT          |\n");
    printf("  +--------------------------------------+\n");

    /* ── Shifting active elements to the left to resolve false overflow if there is space ── */
    if (aq.rear == MAX_APPOINTMENTS - 1 && aq.front > 0)
    {
        int shift = aq.front;
        int i;
        for (i = aq.front; i <= aq.rear; i++)
        {
            aq.queue[i - shift] = aq.queue[i];
        }
        aq.rear -= shift;
        aq.front = 0;
    }

    /* ── Queue Overflow Guard ────────────────────────────────── */
    /*
     * OVERFLOW: Attempting to enqueue when the queue is already
     * full. If we allowed this, we would write beyond the bounds
     * of the array — undefined behaviour / memory corruption.
     */
    if (isQueueFull())
    {
        printf("\n  [ERROR] Queue Overflow! The appointment queue is full.\n");
        printf("  Maximum capacity of %d appointments has been reached.\n",
               MAX_APPOINTMENTS);
        printf("  Please serve pending appointments before booking new ones.\n");
        return;
    }

    /* ── Step 1: Appointment ID ──────────────────────────────── */
    printf("\n  Enter Appointment ID  : ");
    if (scanf("%d", &newAppt.appointmentId) != 1)
    {
        printf("  [ERROR] Invalid input. Appointment ID must be a number.\n");
        clearApptInputBuffer();
        return;
    }
    clearApptInputBuffer();

    /* Duplicate ID check before accepting the record */
    if (appointmentIdExists(newAppt.appointmentId))
    {
        printf("  [ERROR] Appointment ID %d already exists in the queue.\n",
               newAppt.appointmentId);
        printf("  Please use a unique Appointment ID.\n");
        return;
    }

    /* ── Step 2: Patient ID ──────────────────────────────────── */
    printf("  Enter Patient ID      : ");
    if (scanf("%d", &newAppt.patientId) != 1)
    {
        printf("  [ERROR] Invalid input. Patient ID must be a number.\n");
        clearApptInputBuffer();
        return;
    }
    clearApptInputBuffer();

    /* Validate Patient existence */
    if (!patientExists(newAppt.patientId))
    {
        printf("  [ERROR] Patient ID %d does not exist in the system.\n", newAppt.patientId);
        printf("  Please register the patient first.\n");
        return;
    }

    /* ── Step 3: Doctor ID ───────────────────────────────────── */
    printf("  Enter Doctor ID       : ");
    if (scanf("%d", &newAppt.doctorId) != 1)
    {
        printf("  [ERROR] Invalid input. Doctor ID must be a number.\n");
        clearApptInputBuffer();
        return;
    }
    clearApptInputBuffer();

    /* Validate Doctor existence */
    if (!doctorExists(newAppt.doctorId))
    {
        printf("  [ERROR] Doctor ID %d does not exist in the system.\n", newAppt.doctorId);
        printf("  Please register the doctor first.\n");
        return;
    }

    /* ── Step 4: Appointment Date ────────────────────────────── */
    printf("  Enter Date (DD-MM-YYYY): ");
    fgets(newAppt.date, MAX_DATE_LEN, stdin);
    /*
     * fgets stores the trailing '\n' — replace it with '\0'
     * to get a clean null-terminated date string.
     */
    newAppt.date[strcspn(newAppt.date, "\n")] = '\0';

    if (newAppt.date[0] == '\0')
    {
        printf("  [ERROR] Date cannot be empty.\n");
        return;
    }

    /* ── Step 5: ENQUEUE ─────────────────────────────────────── */
    /*
     * Move rear one position to the right, then store the new
     * appointment at that position.
     *
     * Before: rear = 2  →  queue[0..2] occupied
     * After:  rear = 3  →  queue[3] = newAppt
     */
    aq.rear++;                      /* Advance rear pointer   */
    aq.queue[aq.rear] = newAppt;    /* Store the new record   */

    /* ── Step 6: Persist to File ─────────────────────────────── */
    saveAppointmentsToFile();

    printf("\n  [SUCCESS] Appointment ID %d booked successfully!\n",
           newAppt.appointmentId);
    printf("  Patient ID %d is queued with Doctor ID %d on %s.\n",
           newAppt.patientId, newAppt.doctorId, newAppt.date);
    printf("  Queue position: %d of %d\n",
           aq.rear - aq.front + 1,
           aq.rear - aq.front + 1);
}

/* ============================================================
 *  SECTION 4 — VIEW ALL APPOINTMENTS
 * ============================================================ */

/*
 * viewAppointments()
 * ------------------
 * Iterates through the queue from front to rear and prints
 * every pending appointment as a formatted table row.
 *
 * The loop runs from index aq.front to aq.rear (inclusive).
 * No element is removed — this is a read-only traversal.
 */
void viewAppointments(void)
{
    int i;        /* Loop index — C89 style declaration at top */
    int count;    /* Number of appointments currently queued   */

    printf("  +--------------------------------------+\n");
    printf("  |       ALL QUEUED APPOINTMENTS        |\n");
    printf("  +--------------------------------------+\n\n");

    /* ── Queue Underflow Guard ───────────────────────────────── */
    if (isQueueEmpty())
    {
        printf("  [INFO] The appointment queue is currently empty.\n");
        printf("  Book an appointment first using the 'Book Appointment' option.\n");
        return;
    }

    printAppointmentTableHeader();

    /*
     * Traverse from front to rear.
     * queue[front] = oldest (to be served next)
     * queue[rear]  = newest (most recently booked)
     */
    for (i = aq.front; i <= aq.rear; i++)
    {
        printAppointmentRow(&aq.queue[i]);
    }

    printAppointmentDivider();

    count = aq.rear - aq.front + 1;
    printf("\n  Appointments in Queue  : %d\n", count);
    printf("  Slots Remaining        : %d\n", MAX_APPOINTMENTS - 1 - aq.rear);
    printf("  Next to be Served      : Appointment ID %d\n",
           aq.queue[aq.front].appointmentId);
}

/* ============================================================
 *  SECTION 5 — SERVE APPOINTMENT (DEQUEUE)
 * ============================================================ */

/*
 * serveAppointment()
 * ------------------
 * Removes and displays the appointment at the FRONT of the
 * queue — implementing the FIFO (First In, First Out) rule.
 *
 * DEQUEUE steps:
 *   1. Check that the queue is not empty (underflow guard).
 *   2. Copy the front element into a local variable.
 *   3. Increment front: aq.front = aq.front + 1
 *      (The element is not physically erased — moving front
 *       past it makes it logically inaccessible, which is
 *       equivalent to removal in an array-based queue.)
 *   4. Display the served appointment's details.
 *   5. Persist the updated queue to disk.
 */
void serveAppointment(void)
{
    Appointment served; /* Local copy of the appointment being served */

    printf("  +--------------------------------------+\n");
    printf("  |         SERVE NEXT APPOINTMENT       |\n");
    printf("  +--------------------------------------+\n");

    /* ── Queue Underflow Guard ───────────────────────────────── */
    /*
     * UNDERFLOW: Attempting to dequeue from an empty queue.
     * There is nothing at the front to serve — must guard first.
     */
    if (isQueueEmpty())
    {
        printf("\n  [ERROR] Queue Underflow! No appointments in the queue.\n");
        printf("  There are no pending appointments to serve.\n");
        return;
    }

    /* ── Step 1: Copy the front record ───────────────────────── */
    served = aq.queue[aq.front];

    /* ── Step 2: DEQUEUE — advance front pointer ─────────────── */
    /*
     * Moving front one position to the right logically removes
     * the served record. The slot at the old front index still
     * holds the data in memory, but it will never be read again
     * since all traversals and state checks use aq.front as the
     * lower bound.
     *
     * Before: front=0  queue[0]=A01, queue[1]=A02
     * After:  front=1  A01 is logically gone, queue[1]=A02 is next
     */
    aq.front++;

    /* ── Step 3: Display served appointment details ──────────── */
    printf("  +----------------------------------------+\n");
    printf("  |  SERVED APPOINTMENT DETAILS            |\n");
    printf("  +----------------------------------------+\n");
    printf("  |  Appointment ID : %-20d |\n", served.appointmentId);
    printf("  |  Patient ID     : %-20d |\n", served.patientId);
    printf("  |  Doctor ID      : %-20d |\n", served.doctorId);
    printf("  |  Date           : %-20s |\n", served.date);
    printf("  +----------------------------------------+\n");

    /* Show how many appointments remain after serving */
    if (isQueueEmpty())
    {
        printf("\n  [INFO] The queue is now empty. No more pending appointments.\n");
    }
    else
    {
        printf("\n  Appointments remaining in queue : %d\n",
               aq.rear - aq.front + 1);
        printf("  Next appointment ID to serve    : %d\n",
               aq.queue[aq.front].appointmentId);
    }

    /* ── Step 4: Persist updated queue (without served item) ─── */
    saveAppointmentsToFile();
}

/* ============================================================
 *  SECTION 6 — FILE HANDLING
 * ============================================================ */

/*
 * saveAppointmentsToFile()
 * ------------------------
 * Writes the ACTIVE portion of the queue (from front to rear)
 * to appointments.dat in binary mode.
 *
 * Why write only front..rear?
 *   Slots 0..(front-1) have been dequeued (already served).
 *   They still occupy the array in memory but are logically
 *   gone. We must not save them or they'd reappear on reload.
 *
 * Strategy: overwrite the entire file each time ("wb" mode).
 *   This is simpler and safer than partial updates for a
 *   queue of this size.
 */
static void saveAppointmentsToFile(void)
{
    FILE *fp;
    int   i;
    int   savedCount = 0;

    fp = fopen(APPOINTMENT_FILE, "wb");
    if (fp == NULL)
    {
        printf("  [WARNING] Could not save appointments to '%s'.\n",
               APPOINTMENT_FILE);
        printf("  Ensure the 'data/' directory exists.\n");
        return;
    }

    /*
     * Write each active appointment (front to rear) as raw bytes.
     * fwrite(&item, sizeof(Appointment), 1, fp) writes one record.
     */
    for (i = aq.front; i <= aq.rear; i++)
    {
        if (fwrite(&aq.queue[i], sizeof(Appointment), 1, fp) != 1)
        {
            printf("  [WARNING] Failed to write appointment ID %d to file.\n",
                   aq.queue[i].appointmentId);
        }
        else
        {
            savedCount++;
        }
    }

    fclose(fp);
    /* Uncomment the line below for debugging save operations: */
    /* printf("  [DEBUG] %d appointment(s) saved to file.\n", savedCount); */
}

/*
 * loadAppointmentsFromFile()
 * --------------------------
 * Reads all appointment records from appointments.dat and
 * re-enqueues them into the queue array at startup.
 *
 * Called ONCE by initializeQueue() at program launch.
 * If the file does not exist (first run), the function returns
 * silently — this is normal and not an error.
 *
 * Each record is read via:
 *   fread(&temp, sizeof(Appointment), 1, fp)
 * which fills 'temp' with the raw bytes of one Appointment.
 */
static void loadAppointmentsFromFile(void)
{
    FILE        *fp;
    Appointment  temp;  /* Temporary holder for each record read */

    fp = fopen(APPOINTMENT_FILE, "rb");
    if (fp == NULL)
    {
        /*
         * File doesn't exist yet — this is the first program run.
         * The queue stays empty (front=0, rear=-1). Not an error.
         */
        return;
    }

    /*
     * Read records one at a time until EOF or queue is full.
     * Each successful fread() returns 1; EOF/error returns 0.
     */
    while (fread(&temp, sizeof(Appointment), 1, fp) == 1)
    {
        if (isQueueFull())
        {
            printf("  [WARNING] Queue full during file load. "
                   "Some records were not loaded.\n");
            break;
        }
        /* Enqueue the loaded record */
        aq.rear++;
        aq.queue[aq.rear] = temp;
    }

    fclose(fp);

    if (!isQueueEmpty())
    {
        printf("  [INFO] %d appointment(s) loaded from '%s'.\n",
               aq.rear - aq.front + 1, APPOINTMENT_FILE);
    }
}

/* ============================================================
 *  SECTION 7 — INTERNAL HELPER FUNCTIONS (static)
 * ============================================================ */

/* ──────────────────────────────────────────────────────────── */
/*  appointmentIdExists()                                        */
/*  Scans the active queue (front to rear) for a duplicate ID. */
/*  Returns 1 if found, 0 if the ID is unique.                 */
/* ──────────────────────────────────────────────────────────── */
static int appointmentIdExists(int id)
{
    int i;
    for (i = aq.front; i <= aq.rear; i++)
    {
        if (aq.queue[i].appointmentId == id)
        {
            return 1; /* Duplicate found */
        }
    }
    return 0; /* ID is unique */
}

/* ──────────────────────────────────────────────────────────── */
/*  printAppointmentDivider()                                   */
/*  Prints a horizontal separator line for table formatting.   */
/* ──────────────────────────────────────────────────────────── */
static void printAppointmentDivider(void)
{
    printf("  +---------+------------+------------+---------------+\n");
}

/* ──────────────────────────────────────────────────────────── */
/*  printAppointmentTableHeader()                               */
/*  Prints the column header row of the appointments table.    */
/* ──────────────────────────────────────────────────────────── */
static void printAppointmentTableHeader(void)
{
    printAppointmentDivider();
    printf("  | %-7s | %-10s | %-10s | %-13s |\n",
           "Appt ID", "Patient ID", "Doctor ID", "Date");
    printAppointmentDivider();
}

/* ──────────────────────────────────────────────────────────── */
/*  printAppointmentRow()                                       */
/*  Prints one Appointment record as a single formatted row.   */
/*  Takes a const pointer — read-only, no full struct copy.    */
/* ──────────────────────────────────────────────────────────── */
static void printAppointmentRow(const Appointment *a)
{
    printf("  | %-7d | %-10d | %-10d | %-13s |\n",
           a->appointmentId,
           a->patientId,
           a->doctorId,
           a->date);
}

/* ──────────────────────────────────────────────────────────── */
/*  clearApptInputBuffer()                                      */
/*  Discards all characters left in stdin after a scanf() call */
/*  up to and including the '\n'. Prevents the next fgets()    */
/*  from reading a stale newline and returning an empty string. */
/* ──────────────────────────────────────────────────────────── */
static void clearApptInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* Intentionally empty — consuming leftover characters */
    }
}