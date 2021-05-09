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
 *
 * Credits go to Evgueni Oulianov (eoulianov@hotbox.ru) for his
 * excellent document about the different Pronto IR formats,
 * which can be found e.g at:
 * http://www.hifi-remote.com/infrared/prontoirformats.pdf
 */

#ifndef PRONTOCODES_H
#define PRONTOCODES_H

#ifndef uint32_t
#define uint32_t unsigned int
#endif

/*
 * return codes:
 */
#define PCO_RETURN_OK         0
#define PCO_ERROR_BAD_CODE    1  /* could not find all required values */
#define PCO_ERROR_OUTOFMEM    2  /* failed to malloc returned arrays */
#define PCO_ERROR_NOT_IMPL   11  /* not yet implemented */

/*
 * Translate a return value into an actual error message. Pass 
 * in the int you received, get back a string.
 */
const char *pco_strerror(int err);

/*
 * Native Pronto code data structure. For raw format, once/repeat_signal
 * contain word pairs for mark/space durations (in carrier clock cycles).
 *
 * Note: the internal Pronto format header counts mark/space pairs; for
 *       convenience, we count total marks + spaces instead , i.e. twice
 *       the Pronto counts.
 * 
 * For decoded formats, format_id determines the encoding and once/
 * repeat_signal contain the corresponding address/command codes 
 * (or whatever data is needed for the specific encoding):
 * For reference, the original names from Evgueni's document have
 * been added in brackets <>:
 *
 * format_id             <wFmtId>:
 *                       determines format (raw or several decoded)
 * carrier_clock_divider <wFrqDiv>:
 *                       IR carrier clock in internal units
 * once_signal_length    <nOnceSeq>:
 *                       length of once_signal (total of marks+spaces -> even number)
 * repeat_signal_length  <nRepeatSeq>:
 *                       length of repeat_signal (total of marks+spaces -> even number)
 * *once_signal          *<OncePulses>:
 *                       sequence to send once for command execution
 * *repeat_signal        *<RepeatPulses>:
 *                       sequence to send repeatedly for command repetition
 */

struct pco_pronto_code {
	uint32_t format_id;
	uint32_t carrier_clock_divider;
	uint32_t once_signal_length;
	uint32_t repeat_signal_length;
	uint32_t *once_signal;
	uint32_t *repeat_signal;
};

/*
 * Provide interface to deallocate unused allocated memory:
 */
void pco_delete_ir_signal(uint32_t* ir_signal);
void pco_delete_pronto_code(struct pco_pronto_code pronto_code);

/*
 * Read from string:
 * Allocates memory for once/repeat_signal in result, caller is responsible
 * for deallocation.
 */
int pco_sscanf_pronto_code(char *source, struct pco_pronto_code *result);

/*
 * Decode from Pronto format to IR signal mark/space stream and carrier
 * clock. repetitions determines the repetitions of repeat_signal part of
 * the pronto code - 0 means that repeat_signal is not included at all.
 * Allocates memory for the returned ir_signal, caller is responsible 
 * for deallocation.
 */
int pco_pronto_to_ir_signal(
	struct pco_pronto_code source, uint32_t repetitions,
	uint32_t *carrier_freq,
	uint32_t **ir_signal, uint32_t *ir_signal_length);

#endif
