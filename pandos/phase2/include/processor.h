#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <print.h>
#include <umps/libumps.h>
#include <umps/types.h>

// wrappers for the HALT, WAIT and PANIC macros

#define IT_INTERVAL 100000

/**
 * @brief Halts the processor, printing a formatted message
 */
#define halt(x, ...)                    \
    {                                   \
        PRINT_PANDOS(x, ##__VA_ARGS__); \
        HALT();                         \
    }

/**
 * @brief Puts the processor in wait state, printing a formatted message
 */
#define wait(x, ...)                    \
    {                                   \
        PRINT_PANDOS(x, ##__VA_ARGS__); \
        WAIT();                         \
    }

/**
 * @brief Kernel panic, but with a funny formatted message!
 */
#define panic(x, ...)                   \
    {                                   \
        PRINT_PANDOS(x, ##__VA_ARGS__); \
        PANIC();                        \
    }

/**
 * @brief Saves the current state into st.
 *
 * @param st The state to save the current state into
 */
static inline void set_state(state_t *st)
{
    STST(st);
}

/**
 * @brief Loads st into the current state.
 *
 * @param st The state to be loaded into the current state
 */
static inline void load_state(state_t *st)
{
    LDST(st);
}

/**
 * @brief Makes ctx the current context.
*/
static inline void load_context(context_t* cxt) {
    LDCXT(cxt->stackPtr, cxt->pc, cxt->status);
}

/**
 * @brief Stores the time of day into the parameter.
 * 
 * @param time where to store the time of day.
*/
static inline void store_tod(cpu_t* time) {
    STCK(*time);
}

/**
 * @brief Loads the interval timer with the given value.
 * 
 * @param time The amount of time to be loaded.
*/
static inline void load_interval_timer(cpu_t time) {
    LDIT(time);
}

/**
 * @brief Loads the processor local timer (PLT) with the given value.
 * 
 * @param time The amount of time to be loaded.
*/
static inline void load_processor_local_timer(cpu_t time) {
    setTIMER(TRANSLATE_TIME(time));
}

static inline void reset_plt()
{
    load_processor_local_timer(TIMESLICE);
}

static inline void reset_interval_timer() {
    load_interval_timer(IT_INTERVAL);
}



#endif