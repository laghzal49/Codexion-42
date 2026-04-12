/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 23:07:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/11 18:18:17 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include "garbge_colle/ft_malloc.h"

/* ========================================================================== */
/* FORWARD DECLARATIONS                          */
/* ========================================================================== */

typedef struct s_sim	t_sim;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;

/* ========================================================================== */
/* STRUCTURES                                 */
/* ========================================================================== */

// The user's command line arguments
typedef struct s_params {
	long long	num_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	long long	compiles_required;
	long long	dongle_cooldown;
	int			is_edf; // 0 for FIFO, 1 for Earliest Deadline First
}	t_params;

// A ticket for the Priority Queue
typedef struct s_request {
	long long	key; // Used to sort the queue (either burnout time or wait time)
	t_coder		*coder_id;
}	t_request;

// The Priority Queue itself
typedef struct s_pq {
	t_request	heap[300]; // Assuming a max of ~250 coders for safety
	int			size;
}	t_pq;

// The Dongle (Fork)
struct s_dongle {
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				available_flag;
	long long		cooldown_timestamp;
	t_pq			queue;
};

// The Coder (Philosopher)
struct s_coder {
	int			id;
	int			compile_count;
	long long	last_compile;
	pthread_t	thread;
	t_dongle	*left_dongle;
	t_dongle	*right_dongle;
	t_sim		*sim; // Pointer back to the main simulation
};

// The Main Simulation Manager
struct s_sim {
	t_params		params;
	long long		start_time_ms;
	int				stop_flag;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
};

/* ========================================================================== */
/* FUNCTION PROTOTYPES                           */
/* ========================================================================== */

// --- init.c ---
int			init_sim(t_sim *sim);
int			init_sim_core(t_sim *sim);
int			init_alloc(t_sim *sim);
int			init_dongles(t_sim *sim);
int			init_coders(t_sim *sim);

// --- time.c ---
long long	get_time_in_ms(void);
void		ft_usleep(long long time_to_sleep);

// --- utils.c ---
int			ft_strlen(const char *s);
void		error_exit(const char *msg);
long long	ft_atol(const char *str);
void		*ft_malloc(size_t size); // Assuming you wrote this wrapper!

// --- dongles.c ---
void		take_dongles(t_coder *coder, t_dongle *dongle);
void		put_dongles(t_coder *coder, t_dongle *dongle);

// --- coder_routine.c ---
void		print_state(t_coder *coder, const char *state);
void		*coder_routine(void *arg);

// --- simulation.c ---
int			start_sim(t_sim *sim);
void		join_sim(t_sim *sim);

// --- monitor.c ---
void		*monitor_routine(void *arg);


// --- pq.c (Priority Queue Tools) ---
void		pq_init(t_pq *pq);
void		pq_push(t_pq *pq, t_request req);
t_request	*pq_peek(t_pq *pq);
t_request   pq_pop(t_pq *pq);

#endif
