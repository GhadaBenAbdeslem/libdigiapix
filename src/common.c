/*
 * Copyright (c) 2017 Digi International Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * =======================================================================
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "_log.h"
#include "_common.h"
#include "common.h"

#define DEFAULT_DIGIAPIX_CFG_FILE	"/etc/libdigiapix.conf"

static void __attribute__ ((constructor(101))) digiapix_init(void);
static void __attribute__ ((destructor(101))) digiapix_fini(void);

static int config_load(void);
static void config_free(void);

static board_config *config;

/**
 * digiapix_init() - Initializes the library
 *
 * Executed at the startup of the program, i.e before 'main()' function.
 */
static void digiapix_init(void)
{
	init_logger(LOG_ERR, LOG_CONS | LOG_NDELAY | LOG_PID | LOG_PERROR);

	config_load();
}

/**
 * digiapix_fini() - Releases the library resources
 *
 * Executed just before the program terminates through _exit, i.e after 'main()'
 * function.
 */
static void digiapix_fini(void)
{
	config_free();

	close_logger();
}

/**
 * config_load() - Load board configuration specific values
 *
 * Use 'config_free()' to free the configuration memory.
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
static int config_load(void)
{
	if (config == NULL) {
		if (access(DEFAULT_DIGIAPIX_CFG_FILE, R_OK) == 0 &&
		    setenv("LIBSOC_CONF", DEFAULT_DIGIAPIX_CFG_FILE, 1) != 0)
			return EXIT_FAILURE;
		config = libsoc_board_init();
	}

	return config == NULL ? EXIT_FAILURE : EXIT_SUCCESS;
}

/**
 * config_free() - Free up memory for the configuration
 */
static void config_free(void)
{
	if (config != NULL) {
		libsoc_board_free(config);
		config = NULL;
	}
}

int config_check_alias(const char * const alias)
{
	if (alias == NULL || strlen(alias) == 0) {
		log_error("%s: Invalid alias, it cannot be %s", __func__,
				alias == NULL ? "NULL" : "empty");
		return EXIT_FAILURE;
	}

	if (config == NULL) {
		log_error("%s: Unable get requested alias ('%s')",
				__func__, alias);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int config_get_gpio_kernel_number(const char * const alias)
{
	return libsoc_board_gpio_id(config, alias);
}

int check_request_mode(request_mode_t request_mode)
{
	switch (request_mode) {
	case REQUEST_SHARED:
	case REQUEST_GREEDY:
	case REQUEST_WEAK:
		return EXIT_SUCCESS;
	default:
		return EXIT_FAILURE;
	}
}