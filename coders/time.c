/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 08:56:47 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/12 15:45:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

long long	get_time_in_ms(void)
{
	long long		time;
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (1);
	time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (time);
}

void	ft_usleep(long long time_to_sleep)
{
	long long	start_time;

	start_time = get_time_in_ms();
	while ((get_time_in_ms() - start_time) < time_to_sleep)
		usleep(50);
}

void	get_target_timespec(struct timespec *ts, long long wait_ms)
{
	struct timeval	tv;
	long long		total_ns;

	gettimeofday(&tv, NULL);
	total_ns = ((long long)tv.tv_sec * 1000000000LL
			+ (long long)tv.tv_usec * 1000LL
			+ (wait_ms * 1000000LL));
	ts->tv_sec = total_ns / 1000000000LL;
	ts->tv_nsec = total_ns % 1000000000LL;
}

void	ft_smart_sleep(t_coder *coder, long long time_ms)
{
	struct timespec	ts;

	if (time_ms <= 0)
		return ;
	get_target_timespec(&ts, time_ms);
	pthread_mutex_lock(&coder->time_mutex);
	pthread_cond_timedwait(&coder->time_cond, &coder->time_mutex, &ts);
	pthread_mutex_unlock(&coder->time_mutex);
}
