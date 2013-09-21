import models
from datetime import timedelta

def proc_satalite():
    for log in models.LogTempLive.objects.filter(processed = False):
        delta = -15
        for reading in reverse(log.data):
            delta = delta - 30
            temp = models.Temp(
                sensor = log.sensor,
                time = log.time + timedelta(minutes = delta),
                temp = convert_rtd(reading)
            );
            temp.save()
    #TODO: should we fillin back temps here?

def convert_rtd(value):
    #TODO
    return value
