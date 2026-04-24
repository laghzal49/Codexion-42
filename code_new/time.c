/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:42:22 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 17:17:26 by otahiri-         ###   ########.fr       */
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
	long long	start_time;

	start_time = get_time_in_ms();
	while ((get_time_in_ms() - start_time) < time_to_sleep)
	{
		if (should_stop(all))
			break ;

		usleep(100);
	}
}

void	from_ms_to_timespec(struct timespec *time_spec, long long time_ms)
{
	time_spec->tv_sec = time_ms / 1000;
	time_spec->tv_nsec = (time_ms % 1000) * 1000000L;
}

uint64_t	from_timeval_to_ms(struct timeval time_value)
{
	uint64_t	time_ms;

	time_ms = time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
	return (time_ms);
}

uint64_t	get_current_time_ms(void)
{
	uint64_t		time_ms;
	struct timeval	time_value;

	gettimeofday(&time_value, NULL);
	time_ms = from_timeval_to_ms(time_value);
	return (time_ms);
}
