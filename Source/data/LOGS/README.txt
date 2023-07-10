There are only two logs that are kept, the current logs and the previous boot
logs. This may seem odd that all logs aren't retained, but there is a reason
for this. If all logs were to be retained, there would need to be a way
differentiate between the times that they were recorded from an organizational
perspective. The typical way this would be done would be naming the log file
based on the date it was recorded. There is a problem with this, however.
The RTC clock may not be able to get the time from the time server due to
connection issues. In addition, these logs may take significant space.

So this is what happens at boot. If LOGS_PREVIOUS exists, it is deleted. If
LOGS_CURRENT exists, it is moved to LOGS_PREVIOUS and LOGS_CURRENT is cleared
of the original content. Then, logging begins at LOGS_CURRENT. Time stamps
will appear in the logs once the RTC clock is set.
