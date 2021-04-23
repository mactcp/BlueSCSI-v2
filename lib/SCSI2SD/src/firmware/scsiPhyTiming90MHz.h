//	Copyright (C) 2021 Michael McMaster <michael@codesrc.com>
//
//	This file is part of SCSI2SD.
//
//	SCSI2SD is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	SCSI2SD is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with SCSI2SD.  If not, see <http://www.gnu.org/licenses/>.

#ifndef S2S_SCSIPHYTIMING

// Timing at a 90Hz clock.

static uint8_t asyncTimings[][4] =
{
/* Speed,    Assert,    Deskew,    Hold,    Glitch */
{/*1.5MB/s*/ 23,        15,        6,      12},
{/*3.3MB/s*/ 11,        5,         5,      11},
{/*5MB/s*/   8,         5,         5,       5}, // 80ns
{/*safe*/    2,         5,         5,       5}, // Probably safe
{/*turbo*/   2,         2,         2,       2}
};

// 5MB/s synchronous timing
#define SCSI_FAST5_DESKEW 5 // 55ns
#define SCSI_FAST5_HOLD 5 // 53ns

// 10MB/s synchronous timing
// 2:0 Deskew count, 25ns
// 6:4 Hold count, 33ns
// 3:0 Assertion count, 30ns
// We want deskew + hold + assert + 3 to add up to 100ns
// the fpga code has 1 clock of overhead when transitioning from deskew to
// assert to hold

#define SCSI_FAST10_DESKEW 2 // 25ns
#define SCSI_FAST10_HOLD 2 // 33ns
#define SCSI_FAST10_WRITE_ASSERT 2 // 30ns. Overall clocks only works if fpga overhead is 3.

// Slow down the cycle to be valid. 2x assert period is TOO FAST when
// reading data. It's ok when writing due to the deskew.
// 50ns. ie. 100ns / 2. Rounded down because there's likely a few extra cycles
// here and there.
#define SCSI_FAST10_READ_ASSERT 4

// Fastest possible timing, probably not 20MB/s
#define SCSI_FAST20_DESKEW 1
#define SCSI_FAST20_HOLD 2
#define SCSI_FAST20_ASSERT 2


#define syncDeskew(period) ((period) < 35 ? \
	SCSI_FAST10_DESKEW : SCSI_FAST5_DESKEW)

#define syncHold(period) ((period) < 35 ? \
	((period) == 25 ? SCSI_FAST10_HOLD : 3) /* 25ns/33ns */\
	: SCSI_FAST5_HOLD)


// Number of overhead cycles per period.
#define FPGA_OVERHEAD 2
#define FPGA_CYCLES_PER_NS 8
#define SCSI_PERIOD_CLKS(period) ((((int)period * 4) + (FPGA_CYCLES_PER_NS/2)) / FPGA_CYCLES_PER_NS)

// 3.125MB/s (80 period) to < 10MB/s sync
// 3:0 Assertion count, variable
#define syncAssertionWrite(period,deskew) ((SCSI_PERIOD_CLKS(period) - deskew - FPGA_OVERHEAD + 1) / 2)
#define syncAssertionRead(period) syncAssertionWrite(period,0)


// Time until we consider ourselves selected
// 400ns
#define SCSI_DEFAULT_SELECTION 36
#define SCSI_FAST_SELECTION 4


#endif // S2S_SCSIPHYTIMING
