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

int	main(int argc, char **argv)
{
	t_app	*all;

	all = malloc(sizeof(t_app));
	if (!all)
		return (1);
	memset(all, 0, sizeof(t_app));
	check_parsing(argc);
	parsing(&all->parms, argv);
	if (init_all(all))
	{
		cleanup_all(all);
		return (1);
	}
	if (start_and_join_coders(all))
	{
		cleanup_all(all);
		return (1);
	}
	cleanup_all(all);
	return (0);
}
