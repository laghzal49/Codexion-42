/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 23:05:32 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 12:54:02 by tlaghzal         ###   ########.fr       */
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

int	error_exit(const char *msg)
{
	write(2, msg, (size_t)ft_strlen(msg));
	return (FAIL);
}

int	valdite_number(const char *s)
{
	int	i;

	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (!s[i])
		return (FAIL);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (FAIL);
		i++;
	}
	return (SUCCESS);
}
#include <stdio.h>
long long	ft_atol(const char *str)
{
	long long	res;
	int			sign;
	int			i;
	int			digit;

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
	{
		digit = str[i] - '0';
		if (res > (LLONG_MAX - digit) / 10)
		{
			printf("Overflow %lld", res);
			return (LLONG_MAX);
		}
		res = res * 10 + digit;
		i++;
	}
	return (res * sign);
}
