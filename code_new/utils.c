/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 23:05:32 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 13:53:46 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_strlen(const char *s)
{
	int	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

void	error_exit(const char *msg)
{
	write(2, msg, (size_t)ft_strlen(msg));
	exit(1);
}

void	print_state(t_coder *coder, const char *state)
{
	long long	now;

	pthread_mutex_lock(&coder->all->log_mutex);
	now = get_time_in_ms() - coder->all->start_time_ms;
	printf("%lld %lld %s\n", now, coder->coder_id, state);
	pthread_mutex_unlock(&coder->all->log_mutex);
}

long long	ft_atol(const char *str)
{
	long long	res;
	int			sign;
	int			i;

	i = 0;
	res = 0;
	sign = 1;
	while (str[i] == ' ' || (str[i] <= 13 && str[i] >= 9))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		res = res * 10 + (str[i++] - '0');
	return (res * sign);
}
