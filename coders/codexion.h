/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 23:07:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/12 14:22:19 by tlaghzal         ###   ########.fr       */
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

typedef struct s_sim	t_sim;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;

typedef struct s_params {
	long long	num_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	long long	compiles_required;
	long long	dongle_cooldown;
	int			is_edf;
}	t_params;

typedef struct s_request {
	long long	key;
	t_coder		*coder_id;
}	t_request;

typedef struct s_pq {
	t_request	heap[300];
	int			size;
}	t_pq;

struct s_dongle {
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				available_flag;
	long long		cooldown_timestamp;
	t_pq			queue;
};

struct s_coder {
	int				id;
	int				compile_count;
	long long		last_compile;
	pthread_t		thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_sim			*sim;
	pthread_mutex_t	time_mutex;
	pthread_cond_t	time_cond;
};

struct s_sim {
	t_params		params;
	long long		start_time_ms;
	int				stop_flag;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
};

int			init_sim(t_sim *sim);
int			init_sim_core(t_sim *sim);
int			init_alloc(t_sim *sim);
int			init_dongles(t_sim *sim);
int			init_coders(t_sim *sim);

long long	get_time_in_ms(void);
void		ft_usleep(long long time_to_sleep);
void		get_target_timespec(struct timespec *ts, long long wait_ms);
void		ft_smart_sleep(t_coder *coder, long long time_ms);

int			ft_strlen(const char *s);
void		error_exit(const char *msg);
long long	ft_atol(const char *str);
void		*ft_malloc(size_t size);

void		take_dongles(t_coder *coder, t_dongle *dongle);
void		put_dongles(t_coder *coder, t_dongle *dongle);

void		print_state(t_coder *coder, const char *state);
void		*coder_routine(void *arg);

int			start_sim(t_sim *sim);
void		join_sim(t_sim *sim);
void		cleanup_sim(t_sim *sim);

void		*monitor_routine(void *arg);

void		pq_init(t_pq *pq);
void		pq_push(t_pq *pq, t_request req);
t_request	*pq_peek(t_pq *pq);
t_request	pq_pop(t_pq *pq);

#endif
