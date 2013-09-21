from django.db import models

class LogTempLive(models.Model):
    sensor = models.ForeignKey(Sensor)
    processed = models.BooleanField()
    data = models.CharField(maxlength=50)
    time = models.DateTimeField()

class LogTempBackup(models.Model):
    sensor = models.ForeignKey(Sensor)
    byyy = models.CharField(maxlength=4)
    bm =models.CharField(maxlength=2)
    bd =models.CharField(maxlength=2)
    bh =models.CharField(maxlength=2)
    eyyy = models.CharField(maxlength=4)
    em =models.CharField(maxlength=2)
    ed=models.CharField(maxlength=2)
    eh=models.CharField(maxlength=2)
    sst=models.FloatField()
    sstanom=models.FloatField()
    hotspot=models.FloatField()
    dhw=models.FloatField()
    lat=models.FloatField()
    long=models.FloatField()    
