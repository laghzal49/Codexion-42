/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simlition.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 14:06:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:02:09 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	should_stop(t_app *all)
{
	int	stop_flag;

	pthread_mutex_lock(&all->req_mu);
	stop_flag = all->stop_requested;
	pthread_mutex_unlock(&all->req_mu);
	return (stop_flag);
}

void	set_stop(t_app *all)
{
	pthread_mutex_lock(&all->req_mu);
	all->stop_requested = 1;
	pthread_cond_broadcast(&all->req_cv);
	pthread_mutex_unlock(&all->req_mu);
}

void	mark_coder_finished(t_app *all)
{
	pthread_mutex_lock(&all->req_mu);
	all->finished_coders++;
	if (all->finished_coders >= all->parms.num_coders)
	{
		all->stop_requested = 1;
		pthread_cond_broadcast(&all->req_cv);
	}
	pthread_mutex_unlock(&all->req_mu);
}

static int	has_burned_out(t_app *all, int index, long long current_time, int *all_finish)
{
	long long	time_to_die;
	int			compile_count;

	pthread_mutex_lock(&all->coder[index].cv_mu);
	time_to_die = all->coder[index].time_to_die;
	compile_count = all->coder[index].compile_count;
	pthread_mutex_unlock(&all->coder[index].cv_mu);
	if (compile_count >= all->parms.compiles_required)
	{
		*all_finish += 1;
		return (0);
	}
	if (current_time < time_to_die)
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
	int			all_finish;

	all = (t_app *)arg;
	while (!should_stop(all))
	{
		current_time = get_time_in_ms();
		index = 0;
		all_finish = 0;
		while (index < all->parms.num_coders)
		{
			if (has_burned_out(all, index, current_time, &all_finish))
				return (NULL);
			index++;
		}
		if (all_finish >= all->parms.num_coders)
		{
			set_stop(all);
			return (NULL);
		}
		usleep(50);
	}
	return (NULL);
}
