/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 19:16:10 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 13:56:05 by tlaghzal         ###   ########.fr       */
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
# define FIFO 0
# define EDF 1
# define ERROREDF "Error: Invalid scheduler. Must be 'fifo' or 'edf'.\n"
# define NOFLOAT "Error: Invalid u need number not float"
# define CHECK "Usage: ./Codexion num_coders time_to_burno time_to_compile time_to_debug \
	time_to_refactor num_compiles_req dongle_cooldown scheduler\n"



typedef struct s_coder	t_coder;
typedef struct s_heap	t_heap;

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

typedef struct s_dongle
{
	pthread_cond_t	cond;
	pthread_mutex_t	mutex;
	int				in_use;
	long long		cooldown;
	int				id;
}	t_dongle;

typedef struct s_heap
{
	t_coder	**items;
	int		max_size;
	int		size;
	int		is_edf;
}	t_heap;

typedef struct s_all
{
	t_params		parms;
	t_dongle		*dongles;
	t_coder			*coder;
	t_heap			*heap;
	pthread_t		manager_thread;
	pthread_t		monitor_thread;
	long long		start_time_ms;
	int				stop_flag;
	int				finished_coders;
	long long		request_seq;
	pthread_mutex_t	req_mu;
	pthread_cond_t	req_cv;
	pthread_mutex_t	log_mutex;
	int				req_mu_ready;
	int				req_cv_ready;
	int				log_mutex_ready;
}	t_all;

typedef struct s_coder
{
	long long		coder_id;
	long long		time_to_die;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_dongle		*target_dongle;
	int				granted;
	int				has_first_dongle;
	long long		request_seq;
	pthread_cond_t	cv;
	pthread_t		thread;
	pthread_mutex_t	cv_mu;
	int				compile_count;
	long long		last_compile;
	t_all			*all;
}	t_coder;

void		coder_compile(t_coder *coder);
int			check_parsing(int argc);
void		parsing(t_params *params, char **argv);
void		coder_other(t_coder *coder);
int			ft_strlen(const char *string);
void		error_exit(const char *message);
void		log_print(t_coder *coder, const char *action);
void		print_state(t_coder *coder, const char *state);
long long	ft_atol(const char *str);
long long	get_time_in_ms(void);
void		ft_usleep(long long time_to_sleep);
int			init_all(t_all *all);
int			start_and_join_coders(t_all *all);
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
int			should_stop(t_all *all);
void		set_stop(t_all *all);
void		mark_coder_finished(t_all *all);
void		*manager_routine(void *arg);

void		cleanup_all(t_all *all);
void		cleanup_dongles(t_all *all, int num_coders);
void		take_dongles(t_coder *coder);
void		put_dongle(t_coder *coder);
void		smart_sleep(long long time_to_sleep, t_all *all);
int			request_second_dongle(t_coder *coder, t_dongle *first,
				t_dongle *second);

t_heap		*heap_init(int max_size, int is_edf);
void		heap_destroy(t_heap *heap);
int			heap_is_empty(t_heap *heap);
int			heap_is_full(t_heap *heap);
int			heap_insert(t_heap *heap, t_coder *coder);
int			heap_pop(t_heap *heap, t_coder **out);
void		bubble_up(t_heap *heap, int index);
void		bubble_down(t_heap *heap, int index, int size);
void		heap_remove_at(t_heap *heap, int index);

void		from_ms_to_timespec(struct timespec *time_spec, long long time_ms);
uint64_t	from_timeval_to_ms(struct timeval time_value);
uint64_t	get_current_time_ms(void);
int	valdite_number(const char *s);


#endif
