#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

/* Forward Declarations to fix the circular dependency */
typedef struct s_sim t_sim;
typedef struct s_coder t_coder;
typedef struct s_dongle t_dongle;

/* ------------------------------------------------ */

typedef struct s_params {
    long long   num_coders;
    long long   time_to_burnout;
    long long   time_to_compile;
    long long   time_to_debug;
    long long   time_to_refactor;
    long long   compiles_required;
    long long   dongle_cooldown;
    int         is_edf;
} t_params;

typedef struct s_dongle {
    pthread_mutex_t mutex;              // FIXED: Must be pthread_mutex_t
    pthread_cond_t  cond;               // FIXED: Must be pthread_cond_t
    int             available_flag;     // Changed to int (0 or 1)
    long long       cooldown_timestamp;
    // We will build the actual wait_queue struct in Phase 4 
} t_dongle;

typedef struct s_coder {
    int         id;
    int         state;
    long long   last_compile;
    int         compile_count;
    pthread_t   thread;
    t_dongle    *left_dongle;
    t_dongle    *right_dongle;
    t_sim       *sim;
} t_coder;

typedef struct s_sim {
    t_coder         *coders;        // Pointer to array of coders 
    t_dongle        *dongles;       // Pointer to array of dongles 
    t_params        params;         // The struct holding our parsed arguments
    
    int             stop_flag;      // 1 if sim should stop, 0 otherwise
    pthread_mutex_t stop_mutex;     // Protects the stop_flag 
    pthread_mutex_t log_mutex;      // Protects printf 
    
    long long       start_time_ms;  // Tracks when the simulation officially started
} t_sim;

/* ------------------------------------------------ */
/* Function Prototypes                              */
/* ------------------------------------------------ */

long long   get_time_in_ms(void);
void        error_exit(const char *msg);
long long   ft_atol(const char *str);
int         ft_strcmp(const char *s1, const char *s2);
int         ft_strlen(const char *s);

#endif