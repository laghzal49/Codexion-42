/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simlition.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 14:06:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 14:48:11 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	should_stop(t_all *all)
{
	int	stop_flag;

	pthread_mutex_lock(&all->req_mu);
	stop_flag = all->stop_flag;
	pthread_mutex_unlock(&all->req_mu);
	return (stop_flag);
}

void	set_stop(t_all *all)
{
	int	index;

	pthread_mutex_lock(&all->req_mu);
	all->stop_flag = 1;
	pthread_cond_broadcast(&all->req_cv);
	index = 0;
	while (index < all->parms.num_coders)
	{
		pthread_cond_broadcast(&all->coder[index].cv);
		index++;
	}
	pthread_mutex_unlock(&all->req_mu);
}

void	mark_coder_finished(t_all *all)
{
	int	is_complete;

	is_complete = 0;
	pthread_mutex_lock(&all->req_mu);
	all->finished_coders++;
	if (all->finished_coders >= all->parms.num_coders)
		is_complete = 1;
	pthread_mutex_unlock(&all->req_mu);
	if (is_complete)
		set_stop(all);
}

static int	has_burned_out(t_all *all, int index, long long current_time)
{
	long long	time_to_die;
	int			compile_count;

	pthread_mutex_lock(&all->coder[index].cv_mu);
	time_to_die = all->coder[index].time_to_die;
	compile_count = all->coder[index].compile_count;
	pthread_mutex_unlock(&all->coder[index].cv_mu);
	if (compile_count >= all->parms.compiles_required)
		return (0);
	if (current_time <= time_to_die + 60)
		return (0);
	set_stop(all);
	pthread_mutex_lock(&all->log_mutex);
	printf("%lld %lld burned out\n", get_time_in_ms() - all->start_time_ms,
		all->coder[index].coder_id);
	pthread_mutex_unlock(&all->log_mutex);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_all		*all;
	long long	current_time;
	int			index;

	all = (t_all *)arg;
	while (!should_stop(all))
	{
		current_time = get_time_in_ms();
		index = 0;
		while (index < all->parms.num_coders)
		{
			if (has_burned_out(all, index, current_time))
				return (NULL);
			index++;
		}
		usleep(1000);
	}
	return (NULL);
}
