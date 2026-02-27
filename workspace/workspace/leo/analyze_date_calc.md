# Date Calculation Bug Analysis

## The Fine-Tuning Logic

```cpp
// Step 1: Rough calculation
int days_to_subtract = minutes_to_from / (24 * 60);
Date start_date = departure_date;
// Subtract full days...

// Step 2: Fine-tune
DateTime leaving_at_from(start_date, train->startTime);
leaving_at_from.addMinutes(minutes_to_from);

// Step 3: Adjust if needed
while (leaving_at_from.date < departure_date) {
    start_date.day++;
    // ... handle month boundaries ...
    leaving_at_from = DateTime(start_date, train->startTime);
    leaving_at_from.addMinutes(minutes_to_from);
}

while (leaving_at_from.date > departure_date) {
    start_date.day--;
    // ... handle month boundaries ...
    leaving_at_from = DateTime(start_date, train->startTime);
    leaving_at_from.addMinutes(minutes_to_from);
}
```

## Potential Bug

The loops compare `leaving_at_from.date` with `departure_date` using DATE comparison only.
This means the TIME component is ignored!

For a train that reaches a station at 23:45, and another passenger wants to board
at the next station at 01:30 the next day, they might calculate different start_dates
depending on the exact timing.

But wait - both passengers SHOULD calculate the same start_date because they're on 
the same physical train run. The issue is that the comparison only checks DATES, not TIMES.

Actually, this should be fine because we're comparing the DATE the passenger wants to
depart with the DATE the train reaches that station. The TIME is embedded in the 
train schedule.

So what could be wrong?
