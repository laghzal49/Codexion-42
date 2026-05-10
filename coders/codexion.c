/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 21:56:33 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:18:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_parsing(int argc)
{
	if (argc != 9)
	{
		error_exit(ERR_USAGE_1);
		error_exit(ERR_USAGE_2);
		return (error_exit(ERR_USAGE_3));
	}
	return (0);
}

static int	prepare_app(t_app *all, int argc, char **argv)
{
	if (check_parsing(argc) != 0)
		return (1);
	if (parsing(&all->parms, argv) != 0)
		return (1);
	if (init_all(all) != 0)
		return (1);
	return (0);
}

static int	run_app(int argc, char **argv)
{
	t_app	*all;

	all = malloc(sizeof(t_app));
	if (!all)
		return (1);
	memset(all, 0, sizeof(t_app));
	if (prepare_app(all, argc, argv) != 0)
	{
		cleanup_all(all);
		return (1);
	}
	if (start_and_join_coders(all) != 0)
	{
		cleanup_all(all);
		return (1);
	}
	cleanup_all(all);
	return (0);
}

int	main(int argc, char **argv)
{
	if (!run_app(argc, argv))
		return (0);
	return (1);
}
