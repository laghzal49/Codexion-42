/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 17:45:23 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 13:59:41 by tlaghzal         ###   ########.fr       */
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
	if (coder->left_dongle == coder->right_dongle)
		return ;
	if (request_dongles(coder) != SUCCESS)
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

void	*coder_routine(void *arg)
{
	t_dev		*coder;

	coder = (t_dev *)arg;
	wait_for_start(coder);
	if (coder->coder_id % 2 == 0)
		smart_sleep((coder->all->parms.time_to_compile / 4), coder->all);
	while (!should_stop(coder->all))
		run_cycle(coder);
	return (NULL);
}
