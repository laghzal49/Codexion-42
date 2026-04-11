/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 23:07:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/10 08:00:04 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include "garbge_colle/ft_malloc.h"
# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

typedef struct s_sim	t_sim;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;


typedef struct s_params
{
	long long	num_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	long long	compiles_required;
	long long	dongle_cooldown;
	int			is_edf;
}	t_params;

typedef struct s_request
{
	t_coder		*coder_id;
	long long	key;
}	t_request;

typedef struct s_pq_node
{
	t_request	arr[2];
	int			size;
}	t_pq;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	t_pq			queue;
	pthread_cond_t	cond;
	int				available_flag;
	long long		cooldown_timestamp;
}	t_dongle;

typedef struct s_coder
{
	int			id;
	int			state;
	long long	last_compile;
	int			compile_count;
	pthread_t	thread;
	t_dongle	*left_dongle;
	t_dongle	*right_dongle;
	t_sim		*sim;
}	t_coder;

typedef struct s_sim
{
	t_coder			*coders;
	t_dongle		*dongles;
	t_params		params;
	int				stop_flag;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	long long		start_time_ms;
}	t_sim;

/* Function prototypes */
void		*coder_routine(void *arg);
long long	get_time_in_ms(void);
void		error_exit(const char *msg);
long long	ft_atol(const char *str);
int			ft_strlen(const char *s);
int			init_sim(t_sim *sim);
int			init_sim_core(t_sim *sim);
int			init_alloc(t_sim *sim);
int			init_dongles(t_sim *sim);
int			init_coders(t_sim *sim);
int     pq_push(t_pq *pq, t_request req);
t_request pq_pop(t_pq *pq);
t_request *pq_peek(t_pq *pq);
void	swap(t_request *a, t_request *b);

#endif
