/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:09:20 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:17:57 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	validate_numeric_args(char **argv)
{
	int	index;

	index = 1;
	while (index <= 7)
	{
		if (valdite_number(argv[index]))
			error_exit(NOFLOAT);
		if (index <= 5 && ft_atol(argv[index]) <= 0)
			error_exit("Error: Invalid numeric arguments. Must be positive.\n");
		if (index >= 6 && ft_atol(argv[index]) < 0)
			error_exit("Error: Invalid numeric arguments. Must be positive.\n");
		index++;
	}
}

static void	parse_values(t_config *params, char **argv)
{
	params->num_coders = ft_atol(argv[1]);
	params->time_to_burnout = ft_atol(argv[2]);
	params->time_to_compile = ft_atol(argv[3]);
	params->time_to_debug = ft_atol(argv[4]);
	params->time_to_refactor = ft_atol(argv[5]);
	params->compiles_required = ft_atol(argv[6]);
	params->dongle_cooldown = ft_atol(argv[7]);
}

static void	parse_scheduler(t_config *params, char **argv)
{
	if (strcmp(argv[8], "fifo") == 0)
		params->is_edf = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		params->is_edf = EDF;
	else
		error_exit(ERROREDF);
}

void	parsing(t_config *params, char **argv)
{
	validate_numeric_args(argv);
	parse_values(params, argv);
	parse_scheduler(params, argv);
}

int	check_parsing(int argc)
{

	if (argc != 9)
		error_exit(CHECK);
	return (0);
}
