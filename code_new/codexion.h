/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 19:16:10 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/07 12:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>

# define FIFO 0
# define EDF 1
# define ERR_USAGE_1 "Usage: ./codexion number_of_coders time_to_burnout"
# define ERR_USAGE_2 " time_to_compile time_to_debug time_to_refactor"
# define ERR_USAGE_3 " number_of_compiles_required dongle_cooldown scheduler\n"
# define ERR_SCHEDULER "Error: Invalid scheduler. Use 'fifo' or 'edf'.\n"
# define ERR_NOT_NUMBER "Error: only integers allowed (no floats or letters).\n"
# define ERR_OVERFLOW "Error: Value overflows long long maximum.\n"
# define ERR_TOO_LARGE "Error: Value exceeds maximum allowed (2147483647).\n"
# define ERR_POSITIVE "Error: args 1-3 (coders/burnout/compile) must be > 0.\n"
# define ERR_NON_NEG "Error: args 4-7 must be >= 0.\n"

typedef struct s_dev		t_dev;
typedef struct s_scheduler	t_scheduler;

typedef struct s_config
{
	long long		num_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	long long		compiles_required;
	long long		dongle_cooldown;
	int				is_edf;
}t_config;

typedef struct s_tool
{
	int				in_use;
	long long		cooldown;
	pthread_mutex_t	mutex;
	t_scheduler		*heap;
	pthread_mutex_t	heap_mutex;
	int				heap_mutex_ready;
}t_tool;

typedef struct s_scheduler
{
	t_dev			**items;
	int				max_size;
	int				size;
	int				is_edf;
}t_scheduler;

typedef struct s_app
{
	t_config		parms;
	t_tool			*dongles;
	t_dev			*coder;
	pthread_t		monitor_thread;
	long long		start_time_ms;
	int				finished_coders;
	long long		request_seq;
	int				stop_requested;
	pthread_mutex_t	req_mu;
	pthread_cond_t	req_cv;
	pthread_mutex_t	log_mutex;
	int				req_mu_ready;
	int				req_cv_ready;
	int				log_mutex_ready;
}t_app;

typedef struct s_dev
{
	long long		coder_id;
	long long		time_to_die;
	t_tool			*left_dongle;
	t_tool			*right_dongle;
	long long		request_seq;
	pthread_t		thread;
	pthread_mutex_t	cv_mu;
	int				compile_count;
	t_app			*all;
	pthread_mutex_t	data_mutex;
}t_dev;

typedef t_app				t_all;
typedef t_dev				t_coder;

int				parsing(t_config *params, char **argv);
int				init_all(t_app *all);
int				start_and_join_coders(t_app *all);
void			*monitor_routine(void *arg);
void			*coder_routine(void *arg);

void			coder_compile(t_dev *coder);
void			coder_other(t_dev *coder);
int				request_dongles(t_dev *coder);
void			put_dongle(t_dev *coder);

int				is_top_of_heap(t_tool *dongle, t_dev *coder);
void			lock_order(t_dev *coder, t_tool **first, t_tool **second);
long long		next_seq(t_dev *coder);
void			cleanup_heaps_locked(t_dev *coder);

int				should_stop(t_app *all);
void			set_stop(t_app *all);
void			mark_coder_finished(t_app *all);

void			cleanup_all(t_app *all);
void			cleanup_dongles(t_app *all, int num_coders);

long long		get_time_in_ms(void);
void			smart_sleep(long long time_to_sleep, t_app *all);

int				error_exit(const char *message);
int				ft_strlen(const char *string);
long long		ft_atol(const char *str);
int				valdite_number(const char *s);
void			log_print(t_dev *coder, const char *action);

t_scheduler		*heap_init(int max_size, int is_edf);
void			heap_destroy(t_scheduler *heap);
int				heap_is_empty(t_scheduler *heap);
int				heap_is_full(t_scheduler *heap);
int				heap_insert(t_scheduler *heap, t_dev *coder);
void			bubble_up(t_scheduler *heap, int index);
void			bubble_down(t_scheduler *heap, int index, int size);
void			heap_remove_at(t_scheduler *heap, int index);
int				heap_find_index(t_scheduler *heap, t_dev *coder);
void			lock_mutex(t_tool *first, t_tool *second, int lock, int heap);
#endif