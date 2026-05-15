/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:42:22 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 17:17:26 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_in_ms(void)
{
	long long		current_time;
	struct timeval	time_value;

	if (gettimeofday(&time_value, NULL) == -1)
		return (1);
	current_time = (time_value.tv_sec * 1000) + (time_value.tv_usec / 1000);
	return (current_time);
}

void	smart_sleep(long long time_to_sleep, t_all *all)
{
	struct timespec	wait_time;
	long long		end_time_ms;

	end_time_ms = get_time_in_ms() + time_to_sleep;
	wait_time.tv_sec = end_time_ms / 1000;
	wait_time.tv_nsec = (end_time_ms % 1000) * 1000000;
	pthread_mutex_lock(&all->req_mu);
	pthread_cond_timedwait(&all->req_cv, &all->req_mu, &wait_time);
	pthread_mutex_unlock(&all->req_mu);
}
