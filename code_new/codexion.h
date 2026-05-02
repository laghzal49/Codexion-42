/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 19:16:10 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:07:45 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <errno.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>
# define FIFO 0
# define EDF 1
# define ERR_USAGE \
	"Usage: ./codexion num_coders time_to_burnout time_to_compile" \
	" time_to_debug time_to_refactor num_compiles_req" \
	" dongle_cooldown scheduler\n"
# define ERR_SCHEDULER "Error: Invalid scheduler. Use 'fifo' or 'edf'.\n"
# define ERR_NOT_NUMBER "Error: Arguments must be integers (no floats, letters or signs).\n"
# define ERR_OVERFLOW "Error: Value overflows long long maximum.\n"
# define ERR_TOO_LARGE "Error: Value exceeds maximum allowed (2147483647).\n"
# define ERR_POSITIVE "Error: num_coders, time_to_burnout and time_to_compile must be > 0.\n"
# define ERR_NON_NEG "Error: time_to_debug, refactor, num_compiles and cooldown must be >= 0.\n"

typedef struct s_dev			t_dev;
typedef struct s_scheduler	t_scheduler;

typedef struct s_config
{
	long long	num_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	long long	compiles_required;
	long long	dongle_cooldown;
	int			is_edf;
}	t_config;

typedef struct s_tool
{
	int				in_use;
	long long		cooldown;
	int				id;
	pthread_mutex_t		mutex;
}	t_tool;

typedef struct s_scheduler
{
	t_dev	**items;
	int		max_size;
	int		size;
	int		is_edf;
}	t_scheduler;

typedef struct s_manger
{
	pthread_t	thread;
	long long	request_seq;
	t_dev		**coders;
	int			num_coders;
	t_scheduler		*heap;
}	t_manger;

typedef struct s_app
{
	t_config		parms;
	t_tool			*dongles;
	t_dev				*coder;
	t_scheduler		*heap;
	pthread_t		monitor_thread;
	t_manger		manger;
	long long		start_time_ms;
	int				stop_flag;
	int				finished_coders;
	long long		request_seq;
	int				stop_requested;
	pthread_mutex_t	req_mu;
	pthread_cond_t	req_cv;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	heap_mutex;
	int				req_mu_ready;
	int				req_cv_ready;
	int				log_mutex_ready;
	int				heap_mutex_ready;
}	t_app;

typedef struct s_dev
{
	long long		coder_id;
	long long		time_to_die;
	t_tool			*left_dongle;
	t_tool			*right_dongle;
	t_tool			*target_dongle;
	int				granted;
	int				has_first_dongle;
	long long		request_seq;
	pthread_cond_t	cv;
	pthread_t		thread;
	pthread_mutex_t	cv_mu;
	int				compile_count;
	t_app				*all;
}	t_dev;

typedef t_config	t_params;
typedef t_tool		t_dongle;
typedef t_scheduler	t_heap;
typedef t_app		t_all;
typedef t_dev		t_coder;

void		coder_compile(t_dev *coder);
int			check_parsing(int argc);
void		parsing(t_config *params, char **argv);
void		coder_other(t_dev *coder);
int			request_dongles(t_dev *coder);
int			wait_for_dongle(t_coder *coder);
int			ft_strlen(const char *string);
void		error_exit(const char *message);
void		log_print(t_dev *coder, const char *action);
void		print_state(t_dev *coder, const char *state);
long long	ft_atol(const char *str);
long long	get_time_in_ms(void);
void		ft_usleep(long long time_to_sleep);
int			init_all(t_app *all);
int			start_and_join_coders(t_app *all);
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
int			should_stop(t_app *all);
void		set_stop(t_app *all);
void		mark_coder_finished(t_app *all);
void		*manager_routine(void *arg);

void		cleanup_all(t_app *all);
void		cleanup_dongles(t_app *all, int num_coders);
void		take_dongles(t_dev *coder);
void		put_dongle(t_dev *coder);
void		smart_sleep(long long time_to_sleep, t_app *all);
int			request_second_dongle(t_dev *coder, t_tool *first,
				t_tool *second);
int	should_stop(t_app *all);
t_scheduler	*heap_init(int max_size, int is_edf);
void		heap_destroy(t_scheduler *heap);
int			heap_is_empty(t_scheduler *heap);
int			heap_is_full(t_scheduler *heap);
int			heap_insert(t_scheduler *heap, t_dev *coder);
int			heap_pop(t_scheduler *heap, t_dev **out);
void		bubble_up(t_scheduler *heap, int index);
void		bubble_down(t_scheduler *heap, int index, int size);
void		heap_remove_at(t_scheduler *heap, int index);

void		from_ms_to_timespec(struct timespec *time_spec, long long time_ms);
uint64_t	from_timeval_to_ms(struct timeval time_value);
uint64_t	get_current_time_ms(void);
int	valdite_number(const char *s);
int	check_avalible(t_tool *first, t_tool *second);

#endif
