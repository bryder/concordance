/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  (C) Copyright Andreas Schulz 2008
 */

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
/* avoid warnings about insecure *scanf*'s by Visual C++ */
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <io.h>
#else
#include <sys/stat.h>
#endif
#include <errno.h>

#include "prontocodes.h"

#define PRONTO_CLOCK 4145146
/*
 * Pronto IR formats:
 * - raw formats:
 */
#define PRONTO_FMTID_RAW_MOD    0x0000 /* – raw oscillated code (with carrier) */
#define PRONTO_FMTID_RAW_UNMOD  0x0100 /* – raw unmodulated code (no carrier) */
/*
 * - predefined formats:
 */
#define PRONTO_FMTID_UDB_INDEX  0x8000 /* – index to UDB */
	/* - variable length: */
#define PRONTO_FMTID_VAR_PREDEF 0x7000 /* – predefined code of variable length */
	/* - fixed length: */
#define PRONTO_FMTID_RC5        0x5000 /* – RC5 */
#define PRONTO_FMTID_RC5X       0x5001 /* – RC5x */
#define PRONTO_FMTID_RC6M0      0x6000 /* – RC6 Mode 0 */
#define PRONTO_FMTID_NEC_0      0x9000 /* - NEC */
#define PRONTO_FMTID_NEC_A      0x900A /* - NEC */
#define PRONTO_FMTID_NEC_B      0x900B /* - NEC */
#define PRONTO_FMTID_NEC_C      0x900C /* - NEC */
#define PRONTO_FMTID_NEC_D      0x900D /* - NEC */
#define PRONTO_FMTID_NEC_E      0x900E /* - NEC */
#define PRONTO_FMTID_NEC_YAM    0x9001 /* – basic mode YAMAHA NEC code */

/*
 * Translate a return value into an actual error message. Pass in the int
 * you received, get back a string.
 */

const char *pco_strerror(int err)
{
	switch (err) {
		case PCO_RETURN_OK:
		return "Success";
		break;
		case PCO_ERROR_BAD_CODE:
		return "Bad Pronto Hex format";
		break;
		case PCO_ERROR_OUTOFMEM:
		return "Memory allocation failed";
		break;
		case PCO_ERROR_NOT_IMPL:
		return "Function not yet implemented";
		break;
	}
	return "Unknown error";
}

/*
 * Provide interface to deallocate unused allocated memory:
 */
void pco_delete_ir_signal(uint32_t* ir_signal)
{
	free(ir_signal);
}

void pco_delete_pronto_code(struct pco_pronto_code pronto_code)
{
	free(pronto_code.once_signal);
	free(pronto_code.repeat_signal);
}

/*
 * read a hex word from source string:
 */

int _get_hex_word(char **a_string, char *string_end, uint32_t *result)
{
	int chars_read;
	int err = 0;
	if (*a_string <= string_end) {
		err = sscanf(*a_string, "%4x%n", result, &chars_read);
		if (err == 1) {
			*a_string += chars_read;
		}
	}
	return err;
}

/*
 * convert Hex code from string:
 */

/* 
 * read from string:
 */
int pco_sscanf_pronto_code(char *hex_string,
	       struct pco_pronto_code *pronto_code)
{
	char *hs_cursor = hex_string;
	char *hs_end    = hex_string + strlen(hex_string) - 1;
	uint32_t next_word;
	uint32_t index = 0;

	/* hex_string contains blank-separated 4-digit hex numbers */
	if (_get_hex_word(&hs_cursor, hs_end, &next_word) != 1 ) {
		return PCO_ERROR_BAD_CODE;
	}
	pronto_code->format_id = next_word;
	
	if (_get_hex_word(&hs_cursor, hs_end, &next_word) != 1 ) {
		return PCO_ERROR_BAD_CODE;
	}
	pronto_code->carrier_clock_divider = next_word;

	/*
	 * following are lengths of once_signal and repeat_signal, counting
	 * mark/space duration pairs. We multiply them by 2 to store the total
	 * number of durations (marks + spaces) for each code:
	 */
	if (_get_hex_word(&hs_cursor, hs_end, &next_word) != 1 ) {
		return PCO_ERROR_BAD_CODE;
	}
	pronto_code->once_signal_length = next_word * 2;
	
	if (_get_hex_word(&hs_cursor, hs_end, &next_word) != 1 ) {
		return PCO_ERROR_BAD_CODE;
	}
	pronto_code->repeat_signal_length = next_word * 2;
	
	/* allocate memory for sequences: */
	if (pronto_code->once_signal_length > 0) {
		pronto_code->once_signal   =
		       calloc(pronto_code->once_signal_length, sizeof(uint32_t));
	}
	if (pronto_code->repeat_signal_length > 0) {
		pronto_code->repeat_signal =
		       calloc(pronto_code->repeat_signal_length, sizeof(uint32_t));
	}
	for (index = 0; index < pronto_code->once_signal_length; index ++) {
		if (_get_hex_word(&hs_cursor, hs_end, &next_word) != 1 ) {
		return PCO_ERROR_BAD_CODE;
		}
		pronto_code->once_signal[index] = next_word;
	}
	
	for (index = 0; index < pronto_code->repeat_signal_length; index ++) {
		if (_get_hex_word(&hs_cursor, hs_end, &next_word) != 1 ) {
		return PCO_ERROR_BAD_CODE;
		}
		pronto_code->repeat_signal[index] = next_word;
	}
	return PCO_RETURN_OK;
}

/*
 * Conversion between different kinds of Pronto codes and IR signal streams:
 */

int _convert_raw_mod(struct pco_pronto_code source, uint32_t carrier_cycle,
	uint32_t repetitions, uint32_t **ir_signal, uint32_t *ir_signal_length)
{
	uint32_t source_index = 0;
	uint32_t ir_signal_index = 0;
	uint32_t repeat_count = 0;

	/* basic validity check : */
	if (((source.once_signal_length > 0) && (source.once_signal == NULL)) || 
		((source.repeat_signal_length > 0) && (source.repeat_signal == NULL)) ||
		(carrier_cycle == 0)) {
		return PCO_ERROR_BAD_CODE;
	}

	/*
	 * calculate total number of mark/space durations to be returned
	 * and allocate array to return:
	 */
	*ir_signal_length = source.once_signal_length 
			+ source.repeat_signal_length*(repetitions);
	*ir_signal  = calloc(*ir_signal_length, sizeof(uint32_t));

	/*
	 * alternating pairs of uint32_t wLEDflash, uint32_t wLEDoff in units
	 * of carrier cycles: just convert to microseconds and copy to ir_signal:
	 */

	/*
	 * add OnceSeq:
	 */
	for ( source_index = 0; source_index < source.once_signal_length; source_index++ ) {
		(*ir_signal)[ir_signal_index++] = 
			carrier_cycle * source.once_signal[source_index];
	}

	/* 
	 * add RepeatSeq as many times as requested:
	 */
	for ( repeat_count = repetitions; repeat_count > 0; repeat_count-- ) {
		for ( source_index = 0; source_index < source.repeat_signal_length; source_index++ ) {
			(*ir_signal)[ir_signal_index++] = 
				carrier_cycle * source.repeat_signal[source_index];
		}
	}
	return PCO_RETURN_OK;
} /* _convert_raw_mod */


int _convert_nec_0(struct pco_pronto_code source, uint32_t carrier_cycle,
	uint32_t repetitions, uint32_t **ir_signal, uint32_t *ir_signal_length)
{
/*	uint32_t mark_cycles    = 0;
	uint32_t space_cycles    = 0;
*/
	return PCO_ERROR_BAD_CODE;
} /* convert_nec_0 */


/*
 * decode Pronto format to ir signal stream and carrier clock.
 * repetitions determines the repetitions of repeat_signal
 * part of the pronto code - 0 means that repeat_signal is
 * not included at all.
 */
int pco_pronto_to_ir_signal(
	struct pco_pronto_code source, uint32_t repetitions,
	uint32_t *carrier_freq, 
	uint32_t **ir_signal, uint32_t *ir_signal_length)
{
	int err = PCO_RETURN_OK;
	uint32_t carrier_cycle = 0;
	/*
	 * convert carrier freq from Pronto cycles to Hz and µs:
	 */
	if (source.carrier_clock_divider != 0 ) {
		*carrier_freq = PRONTO_CLOCK/source.carrier_clock_divider;
		carrier_cycle = 1000000/(*carrier_freq);    /* in microseconds */
	}

	switch (source.format_id) {
	case PRONTO_FMTID_RAW_MOD: /* learned (raw) cmds */
		err = _convert_raw_mod(source, carrier_cycle, repetitions, ir_signal, ir_signal_length);
		break;

	case PRONTO_FMTID_RAW_UNMOD:
		/*
		 * basically the same as raw_mod, just no modulation with carrier.
		 * Replacing dummy carrier from Pronto format ; on/off times are 
		 * still multiples of (dummy) carrier cycles in pronto code:
		 */
		*carrier_freq = 0; 
		err = _convert_raw_mod(source, carrier_cycle, repetitions, ir_signal, ir_signal_length);
		break;
	/*
	 * Predefined formats have different structure, but for the reason of 
	 * compatibility with format 0000, fields wFrqDiv, nOnceSeq, nRepeatSeq 
	 * leave as dummy, so that code “looks” the same. 
	 * aOnceSeq and aRepeatSeq are replaced with sCode, that consist real 
	 * code info. Also, nOnceSeq and nRepeatSeq must meet the condition 
	 * (nOnceSeq + nRepeatSeq) * 2 = sizeOf(sCode):
	 * Carrier clock is usually determined by the encoding, so it has to
	 * be returned by the encoding functions.
	 */
	/*
	 * encoded formats not (yet?) handled:
	 */
	case PRONTO_FMTID_RC5:
	case PRONTO_FMTID_RC5X:
	case PRONTO_FMTID_RC6M0:
	case PRONTO_FMTID_VAR_PREDEF:
	case PRONTO_FMTID_UDB_INDEX:
	case PRONTO_FMTID_NEC_0:
	case PRONTO_FMTID_NEC_A:
	case PRONTO_FMTID_NEC_B:
	case PRONTO_FMTID_NEC_C:
	case PRONTO_FMTID_NEC_D:
	case PRONTO_FMTID_NEC_E:
	case PRONTO_FMTID_NEC_YAM:
	default:
		/* not yet implemented Pronto format: */
		err = PCO_ERROR_NOT_IMPL;
		break;
	} /* switch source.format_id */
	return err;
} /* pco_pronto_to_ir_signal */

