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
	long long	start_time;

	start_time = get_time_in_ms();
	while ((get_time_in_ms() - start_time) < time_to_sleep)
	{
		if (should_stop(all))
			break ;
		usleep(100);
	}
}
