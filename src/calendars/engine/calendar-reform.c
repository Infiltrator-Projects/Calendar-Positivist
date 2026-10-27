// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 1993-2026 Shannon Smith

/*
 * Reform-calendar conversion.
 *
 * The resource uses the arithmetic (Romme-style) continuation of the French
 * Republican calendar: the historical epoch is fixed at JDN 2375840 and leap
 * years follow the shifted Gregorian 400-year rule.  That choice is deliberate
 * because several incompatible post-historical proposals exist; callers must
 * get one deterministic rule for dates outside the original republican era.
 *
 * The Positivist calendar is year-relative to Gregorian chronology and is
 * handled in the same module because both systems are regular reform calendars
 * with explicit complementary days.
 */

#include "calendar-reform.h"

#include "calendar-helpers.h"
#include "julian-day.h"

#include <math.h>

#define floor_divide calendar_floor_divide
#define positive_modulo calendar_positive_modulo
#define gregorian_is_leap calendar_gregorian_is_leap
#define gregorian_to_jdn calendar_gregorian_to_jdn
#define jdn_to_gregorian calendar_jdn_to_gregorian
#define gregorian_day_of_year calendar_gregorian_day_of_year

#define FRENCH_EPOCH_JDN ((gint64)2375840)


gint
calendar_positivist_month_length(gint64 year,
                                      gint month)
{
    const gint64 gregorian_year = calendar_i64_add_saturating(year, 1788);

    if (month == 13)
        return gregorian_is_leap(gregorian_year) ? 30 : 29;
    return 28;
}

void
calendar_positivist_fields_from_jdn(
    gint64 jdn,
    CalendarCalendarFields *fields)
{
    gint year;
    gint gregorian_month;
    gint gregorian_day;
    gint ordinal;

    g_return_if_fail(fields != NULL);

    jdn_to_gregorian(jdn, &year, &gregorian_month, &gregorian_day);
    ordinal = gregorian_day_of_year(year,
                                    gregorian_month,
                                    gregorian_day);

    /* Comte's Great Crisis era: Gregorian 1789 is Positivist year 1. */
    fields->year = calendar_i64_subtract_saturating(year, 1788);
    if (ordinal <= 364)
    {
        fields->month = (ordinal - 1) / 28 + 1;
        fields->day = (ordinal - 1) % 28 + 1;
    }
    else
    {
        fields->month = 13;
        fields->day = ordinal - 336;
    }

    fields->auxiliary = 0;
    fields->special = fields->day > 28;
}

gint64
calendar_positivist_fields_to_jdn(
    const CalendarCalendarFields *fields)
{
    g_return_val_if_fail(fields != NULL,
                         gregorian_to_jdn(1970, 1, 1));

    const gint64 gregorian_year =
        calendar_i64_add_saturating(fields->year, 1788);

    g_return_val_if_fail(gregorian_year >= G_MININT &&
                         gregorian_year <= G_MAXINT,
                         gregorian_to_jdn(1970, 1, 1));
    return gregorian_to_jdn((gint)gregorian_year, 1, 1) +
           (gint64)(fields->month - 1) * 28 +
           fields->day - 1;
}
