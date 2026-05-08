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

void	log_print(t_dev *coder, const char *action)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->all->log_mutex);
	timestamp = get_time_in_ms() - coder->all->start_time_ms;
	if (!should_stop(coder->all))
		printf("%lld %lld %s\n", timestamp, coder->coder_id, action);
	pthread_mutex_unlock(&coder->all->log_mutex);
}

static void	run_cycle(t_dev *coder)
{
	if (!request_dongles(coder))
		return ;
	coder_compile(coder);
	if (!should_stop(coder->all))
		coder_other(coder);
}

static void	wait_for_start(t_dev *coder)
{
	pthread_mutex_lock(&coder->all->req_mu);
	while (!coder->all->stop_requested && coder->all->start_time_ms == 0)
		pthread_cond_wait(&coder->all->req_cv, &coder->all->req_mu);
	pthread_mutex_unlock(&coder->all->req_mu);
}

int	is_it_even(t_dev *coder)
{
	return (coder->coder_id % 2 == 0);
}

void	*coder_routine(void *arg)
{
	t_dev		*coder;
	int			is_done;

	coder = (t_dev *)arg;
	wait_for_start(coder);
	if (!is_it_even(coder))
		smart_sleep((coder->all->parms.time_to_compile / 4), coder->all);
	while (!should_stop(coder->all))
	{
		pthread_mutex_lock(&coder->cv_mu);
		is_done = (coder->compile_count >= \
			coder->all->parms.compiles_required);
		pthread_mutex_unlock(&coder->cv_mu);
		if (is_done)
			mark_coder_finished(coder->all);
		run_cycle(coder);
	}
	return (NULL);
}
