/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 17:45:23 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:18:42 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

void	log_print(t_dev *coder, const char *action)
{
	long long	timestamp;

	timestamp = get_time_in_ms() - coder->all->start_time_ms;
	pthread_mutex_lock(&coder->all->log_mutex);
	printf("%lld %lld %s\n", timestamp, coder->coder_id, action);
	pthread_mutex_unlock(&coder->all->log_mutex);
}


static void	run_cycle(t_dev *coder)
{
	coder_compile(coder);
	if (!should_stop(coder->all))
		coder_other(coder);
}

void	*coder_routine(void *arg)
{
	long long	current_start_time;
	t_dev		*coder;
	int			is_done;

	coder = (t_dev *)arg;
	while (!should_stop(coder->all))
	{
		pthread_mutex_lock(&coder->cv_mu);
		current_start_time = coder->all->start_time_ms;
		pthread_mutex_unlock(&coder->cv_mu);
		if (current_start_time != 0)
			break ;
		smart_sleep(50, coder->all);
	}
	while (!should_stop(coder->all))
	{
		pthread_mutex_lock(&coder->cv_mu);
		is_done = (coder->compile_count >= \
			coder->all->parms.compiles_required);
		pthread_mutex_unlock(&coder->cv_mu);
		if (is_done)
			break ;
		run_cycle(coder);
	}
	mark_coder_finished(coder->all);
	return (NULL);
}
