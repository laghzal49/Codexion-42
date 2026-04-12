/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 15:21:54 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/11 18:38:42 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	push_request_to_queue(t_coder *coder, t_dongle *dongle)
{
	t_request	req;

	req.coder_id = coder;
	if (coder->sim->params.is_edf == 1)
		req.key = coder->last_compile + coder->sim->params.time_to_burnout;
	else
		req.key = coder->last_compile;
	pq_push(&dongle->queue, req);
}

static void	wait_for_availability(t_dongle *dongle, int coder_id)
{
	int	cond;

	cond = 1;
	while (cond)
	{
		if (dongle->available_flag == 0)
			cond = 1;
		else if (pq_peek(&dongle->queue)->coder_id->id != coder_id)
			cond = 1;
		else
			cond = 0;
		if (cond)
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
}

static void	wait_for_cooldown(t_dongle *dongle)
{
	while (get_time_in_ms() < dongle->cooldown_timestamp)
	{
		pthread_mutex_unlock(&dongle->mutex);
		usleep(50);
		pthread_mutex_lock(&dongle->mutex);
	}
}

void	take_dongles(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	push_request_to_queue(coder, dongle);
	wait_for_availability(dongle, coder->id);
	wait_for_cooldown(dongle);
	dongle->available_flag = 0;
	pq_pop(&dongle->queue);
	print_state(coder, "has taken a dongle");
	pthread_mutex_unlock(&dongle->mutex);
}

void	put_dongles(t_coder *coder, t_dongle *dongle)
{
	long long	cooldown;

	pthread_mutex_lock(&dongle->mutex);
	dongle->available_flag = 1;
	cooldown = coder->sim->params.dongle_cooldown;
	dongle->cooldown_timestamp = get_time_in_ms() + cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
